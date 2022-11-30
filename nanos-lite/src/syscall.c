#include <common.h>
#include "syscall.h"

struct SYS_TRACE{
    char *name;
    int perimeter[4];
    int ret;
  };
#ifdef STRACE
  SYS_TRACE strace[20];
  int sys_index=0;
#endif

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
    case 0: halt(c->GPR2); break;
    case 1: yield(); c->GPRx=0; break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
  
#ifdef STRACE
  strace[sys_index].name="syscall";
  strace[sys_index].perimeter[0]=c->GPR1;
  strace[sys_index].perimeter[1]=c->GPR2;
  strace[sys_index].perimeter[2]=c->GPR3;
  strace[sys_index].perimeter[3]=c->GPR4;
  strace[sys_index].ret=c->GPRx;
#endif
}
