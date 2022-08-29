#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  backtrace();
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

//added
uint64
sys_sigalarm(void)
{
  int interval;
  uint64 handler;

  if (argint(0, &interval) < 0)
    return -1;
  if (argaddr(1, &handler) < 0)
    return -1;
  
  struct proc *proc = myproc();
  proc->alarm_interval = interval;
  proc->handler = handler;
  return 0;
}

//  restore registers state from struct proc context when the timer goes off 
uint64
sys_sigreturn(void)
{
  struct proc *p = myproc();

  p->trapframe->epc = p->savereg_alarm.epc;
  p->trapframe->ra = p->savereg_alarm.ra;
  p->trapframe->sp = p->savereg_alarm.sp;
  p->trapframe->gp = p->savereg_alarm.gp;
  p->trapframe->tp = p->savereg_alarm.tp;
  
  p->trapframe->s0 = p->savereg_alarm.s0;
  p->trapframe->s1 = p->savereg_alarm.s1;
  p->trapframe->s2 = p->savereg_alarm.s2;
  p->trapframe->s3 = p->savereg_alarm.s3;
  p->trapframe->s4 = p->savereg_alarm.s4;
  p->trapframe->s5 = p->savereg_alarm.s5;
  p->trapframe->s6 = p->savereg_alarm.s6;
  p->trapframe->s7 = p->savereg_alarm.s7;
  p->trapframe->s8 = p->savereg_alarm.s8;
  p->trapframe->s9 = p->savereg_alarm.s9;
  p->trapframe->s10 = p->savereg_alarm.s10;
  p->trapframe->s11 = p->savereg_alarm.s11;
  
  p->trapframe->t0 = p->savereg_alarm.t0;
  p->trapframe->t1 = p->savereg_alarm.t1;
  p->trapframe->t2 = p->savereg_alarm.t2;
  p->trapframe->t3 = p->savereg_alarm.t3;
  p->trapframe->t4 = p->savereg_alarm.t4;
  p->trapframe->t5 = p->savereg_alarm.t5;
  p->trapframe->t6 = p->savereg_alarm.t6;

  p->trapframe->a0 = p->savereg_alarm.a0;
  p->trapframe->a1 = p->savereg_alarm.a1;
  p->trapframe->a2 = p->savereg_alarm.a2;
  p->trapframe->a3 = p->savereg_alarm.a3;
  p->trapframe->a4 = p->savereg_alarm.a4;
  p->trapframe->a5 = p->savereg_alarm.a5;
  p->trapframe->a6 = p->savereg_alarm.a6;
  p->trapframe->a7 = p->savereg_alarm.a6;
  
  p->alarm_handler = 0;
  return 0;
}