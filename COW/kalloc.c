// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

//edited or added

#define PA2IDX(pa) (((uint64)pa) >> 12)
struct {
  struct spinlock lock;
  int count[PGROUNDUP(PHYSTOP) / PGSIZE];
} refcnt;

//index the array with the page's physical address divided by 4096, and give the array a number of 
//elements equal to highest physical address of any page placed on the free list by kinit() in kalloc.c. 

//Initiate pages, set the reference count = 0 
void
init_pc()
{
  initlock(&refcnt.lock, "refcnt");
  acquire(&kmem.lock);
  for (int i = 0; i < PGROUNDUP(PHYSTOP) / PGSIZE; i++)
    refcnt.count[i] = 0;
  release(&kmem.lock);
}

void
kinit()
{
  init_pc();
  initlock(&kmem.lock, "kmem");
  freerange(end, (void*)PHYSTOP);
}
 //increment a page's reference count when fork causes a child to share the page,
void
increase_pc(void *pa)
{
  acquire(&refcnt.lock);
  refcnt.count[PA2IDX(pa)]++;
  release(&refcnt.lock);
}

//decrement a page's count each time any process drops the page from its page table
void
decrease_pc(void *pa)
{
  acquire(&refcnt.lock);
  refcnt.count[PA2IDX(pa)]--;
  release(&refcnt.lock);
}
int
get_pc(void *pa)
{
  acquire(&refcnt.lock);
  int rc = refcnt.count[PA2IDX(pa)];
  release(&refcnt.lock);
  return rc;
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
  {
    // init_pc set refcnt = 0, kfree will decrease 1, so increase first
    increase_pc((void*)p);
    kfree(p);
  }
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)

//kfree determines whether the reference count is 0 to determine whether the page is free
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  decrease_pc(pa);
  if (get_pc(pa) > 0)
    return;
  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.

//Add count 
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r)
  {
    memset((char*)r, 5, PGSIZE); // fill with junk
    increase_pc((void*)r);
  }
  return (void*)r;
}
