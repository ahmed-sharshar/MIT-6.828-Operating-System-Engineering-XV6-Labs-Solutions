#include "kernel/types.h"
#include "user/user.h"

//generate the numbers between 2 and 35
void generate() {
  int i;
  for (i = 2; i < 36; i++) {
    write(1, &i, sizeof(i));
  }
}

//check if the number is prime or not
void is_prime(int p) {
  int n;
  while (read(0, &n, sizeof(n))) {
    if (n % p != 0) {
      write(1, &n, sizeof(n));
    }
  }
}

//open pipes
void redirect(int k, int pd[]) {
  close(k);
  dup(pd[k]);
  close(pd[0]);
  close(pd[1]);
}

//shows the prime numbers gradually
void primezz() {
  int pd[2];
  int p;

  if (read(0, &p, sizeof(p))) {
    printf("primes %d\n", p);
    pipe(pd);
    if (fork()) {
      redirect(0, pd);
      primezz();
    } else {
      redirect(1, pd);
      is_prime(p);
    }
  }
}

//main function th continue running 
int main(int argc, char *argv[]) {

  int pd[2];
  pipe(pd);

  if (fork()) {
    redirect(0, pd);
    primezz();
  } else {
    redirect(1, pd);
    generate();
  }

  exit(0);
}