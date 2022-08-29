#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  int parent[1], child[1];
  pipe(parent);
  pipe(child);
  char buf[64];

  if (fork()) {
    // for Parent
    write(parent[1], "ping", strlen("ping"));
    read(child[0], buf, 4);
    printf("%d: received %s\n", getpid(), buf);
  } else {
    // for Child
    read(parent[0], buf, 4);
    printf("%d: received %s\n", getpid(), buf);
    write(child[1], "pong", strlen("pong"));
  }

  exit(0);
}