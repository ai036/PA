#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>


static Context* (*user_handler)(Event, Context*) = NULL;
//事件分发
Context* __am_irq_handle(Context *c) {
  if (user_handler) {
    Event ev = {0};
    switch (c->mcause) {
      //mcause=11 为Machine external interrupt
      case 11: 
      if(c->GPR1==-1)
        ev.event = EVENT_YIELD;
      else
        ev.event = EVENT_SYSCALL;
      c->mepc+=4;
      break;

      default: ev.event = EVENT_ERROR; break;
    }

    c = user_handler(ev, c);
    
    assert(c != NULL);  
  }

  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));

  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  //TODO: 实现kcontext
  Context *c=kstack.end-sizeof(Context);

  c->mstatus=0x1800;
  c->mepc=(uintptr_t)entry;
  c->pdir=NULL;



  return c;
}

void yield() {
  asm volatile("li a7, -1; ecall");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
