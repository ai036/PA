#include <proc.h>

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void naive_uload(PCB *pcb, const char *filename);
void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]);

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  
  while (1) {
    if(j%5000==0)
      Log("Hello World from Nanos-lite with arg '%s' for the %dth time!", (char*)arg, j);
    j ++;
    yield();
  }
}

void context_kload(PCB* p,void (*entry)(void *), void *arg)
{
  printf("context_kload: %p\n", entry);
  Area kstack;
  kstack.start=&p->cp;
  kstack.end=(&p->cp)+STACK_SIZE;//这里好像出现了点问题 native上会提示下标越界

  p->cp=kcontext(kstack,entry,arg);
}

void init_proc() {

  char *v[]={"1234",NULL};
  context_uload(&pcb[0], "/bin/nslider",v,NULL);
  
  switch_boot_pcb();

  Log("Initializing processes...");

  // load program here

}

Context* schedule(Context *prev) {
  // save the context pointer
  current->cp = prev;

  // 在两个进程之间切换
  current = (&pcb[0]);

  // then return the new context
  return current->cp;
}


int execve(const char *filename,char* const argv[],char* const envp[])
{
  context_uload(&pcb[1],filename,argv,envp);
  switch_boot_pcb();
  yield();
  return -1;
}//出现了PC与diff不同的问题 4字节对齐后修复