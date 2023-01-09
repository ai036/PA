#include <common.h>
#include "syscall.h"
#include <fs.h>
#include <sys/time.h>

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

int execve(const char *filename,char* const argv[],char* const envp[]);

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
    case SYS_exit: halt(1); break;
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
        ret=fs_read(fd,(void*)c->GPR3,c->GPR4);
        c->GPRx=ret;
      }
      break;
    case SYS_write: 
    fd=c->GPR2,ret=0;
    if(fd!=0)
      { 
        ret=fs_write(fd,(void*)c->GPR3,c->GPR4);
        c->GPRx=ret;
      }
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
    case SYS_gettimeofday:
      struct timeval* tv=(struct timeval*)c->GPR2;
      __uint64_t time=io_read(AM_TIMER_UPTIME).us;
      tv->tv_usec=time%1000000;
      tv->tv_sec=time/1000000;
      c->GPRx=0;
      break;
    case SYS_execve:
      char* filename=(char*)c->GPR2;
      if(fs_open(filename,0,0)==-1)
        {
          c->GPRx=-2;
          break;
        };
      ret=execve(filename,(char**)c->GPR3,NULL);
      c->GPRx=ret;
      break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

void get_name(int fd,char *buf);
#ifdef STRACE
  get_name(fd,strace[sys_index].name);
  strace[sys_index].perimeter[0]=c->GPR1;
  strace[sys_index].perimeter[1]=c->GPR2;
  strace[sys_index].perimeter[2]=c->GPR3;
  strace[sys_index].perimeter[3]=c->GPR4;
  strace[sys_index].ret=c->GPRx;
#endif
}
