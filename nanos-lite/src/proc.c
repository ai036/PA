#include <proc.h>

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void naive_uload(PCB *pcb, const char *filename);

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
  kstack.end=(&p->cp)+STACK_SIZE;

  p->cp=kcontext(kstack,entry,arg);
}

void init_proc() {
  context_kload(&pcb[0], hello_fun, "NULL");

  
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
