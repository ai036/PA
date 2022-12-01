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
    case SYS_exit: halt(c->GPR2); break;
    case SYS_yield: yield(); c->GPRx=0; break;
    case SYS_write: 
    int fd=c->GPR2,count=0;
    char *start=(char *)c->GPR3;
    if(fd==1||fd==2)
      {for(size_t i=0;i<c->GPR4;i++)
        {
          putch(start[i]);
          count++;
        }
       c->GPRx=count;
      }
    break;
    case SYS_brk:   //简易版，直接返回0
      c->GPRx=0;
      break;
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
