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
  kstack.end=&p->cp+STACK_SIZE-4;//这里好像出现了点问题 不-4参数会错误

  p->cp=kcontext(kstack,entry,arg);
}



void init_proc() {
  context_kload(&pcb[0], hello_fun, "hhr");
  char *v[]={"123",NULL};
  context_uload(&pcb[1], "/bin/pal",v,NULL);
  
  switch_boot_pcb();

  Log("Initializing processes...");

  // load program here

}

Context* schedule(Context *prev) {
  // save the context pointer
  current->cp = prev;

  // 在两个进程之间切换
  current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);

  // then return the new context
  return current->cp;
}

void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]);

int execve(const char *filename,char* const argv[],char* const envp[])
{

  context_uload(&pcb[1],filename,argv,envp);
  switch_boot_pcb();
  yield();
  return -1;
}//出现了PC与diff不同的问题