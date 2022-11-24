#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>


static Context* (*user_handler)(Event, Context*) = NULL;

Context* __am_irq_handle(Context *c) {
  if (user_handler) {
    Event ev = {0};
    switch (c->mcause) {
      case 0: ev.event = EVENT_NULL; break;
      case 1: ev.event = EVENT_YIELD; break;
      case 2: ev.event = EVENT_SYSCALL; break;
      case 3: ev.event = EVENT_PAGEFAULT; break;
      case 4: ev.event = EVENT_ERROR; break;
      case 5: ev.event = EVENT_IRQ_TIMER; break;
      case 6: ev.event = EVENT_IRQ_IODEV; break;
      default: ev.event = EVENT_ERROR; break;
    }
  printf("\n%d\n\n",c->gpr[10]);

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
  return NULL;
}

void yield() {
  asm volatile("li a7, -1; ecall");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
