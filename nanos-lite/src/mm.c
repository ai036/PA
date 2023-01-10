#include <memory.h>

static void *pf = NULL;

void* new_page(size_t nr_page) {
  pf+=(nr_page<<12);
  return pf-(nr_page<<12);
}

#ifdef HAS_VME
static void* pg_alloc(int n) {
  assert(n%4096==0);
  int page_num=n/4096;
  void* ptr=new_page(page_num);
  memset(ptr,0,n);
  return ptr;
}
#endif

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {
  Log("mm_brk\n");
  return 0;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
