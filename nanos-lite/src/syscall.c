#include <common.h>
#include "syscall.h"
#include <fs.h>

struct SYS_TRACE{
    char *name;
    int perimeter[4];
    int ret;
  };
#ifdef STRACE
  SYS_TRACE strace[20];
  int sys_index=0;
#endif

static int fd,ret;

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
    case SYS_exit: halt(c->GPR2); break;
    case SYS_yield: yield(); c->GPRx=0; break;
    case SYS_open:
      char* path=(char*)c->GPR2;
      fd=fs_open(path,0,0);
      c->GPRx=fd;
      break;
    case SYS_read:
      fd=c->GPR2,ret=0;
      if(fd>=3)
      {
        printf("%p %d\n",c->GPR3,c->GPR4);
        ret=fs_read(fd,(void*)c->GPR3,c->GPR4);
        c->GPRx=ret;
      }
      break;
    case SYS_write: 
      int ret = fs_write(c->GPR2, (void *)c->GPR3, c->GPR4);
  c->GPRx = ret;
    break;
    case SYS_lseek:
      fd=c->GPR2;
      int offset=c->GPR3,whence=c->GPR4;
      ret=fs_lseek(fd,offset,whence);
      c->GPRx=ret;
      break;
    case SYS_close:
      fd=c->GPR2;
      ret=fs_close(fd);
      c->GPRx=ret;
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
