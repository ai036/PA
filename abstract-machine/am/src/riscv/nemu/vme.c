#include <am.h>
#include <nemu.h>
#include <klib.h>
#include <riscv/riscv.h>

static AddrSpace kas = {};
static void* (*pgalloc_usr)(int) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;

static Area segments[] = {      // Kernel memory mappings
  NEMU_PADDR_SPACE
};

#define USER_SPACE RANGE(0x40000000, 0x80000000)

static inline void set_satp(void *pdir) {
  uintptr_t mode = 1ul << (__riscv_xlen - 1);
  asm volatile("csrw satp, %0" : : "r"(mode | ((uintptr_t)pdir >> 12)));
}

static inline uintptr_t get_satp() {
  uintptr_t satp;
  asm volatile("csrr %0, satp" : "=r"(satp));
  return satp << 12;    //读取PTE的基地址
}

bool vme_init(void* (*pgalloc_f)(int), void (*pgfree_f)(void*)) {
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  kas.ptr = pgalloc_f(PGSIZE);

  int i;
  for (i = 0; i < LENGTH(segments); i ++) {
    void *va = segments[i].start;  
    for (; va < segments[i].end; va += PGSIZE) {
      map(&kas, va, va, 0);
    }
    
  }

  set_satp(kas.ptr);
  vme_enable = 1;


  return true;
}

void protect(AddrSpace *as) {
  PTE *updir = (PTE*)(pgalloc_usr(PGSIZE));
  as->ptr = updir;
  as->area = USER_SPACE;
  as->pgsize = PGSIZE;
  // map kernel space
  memcpy(updir, kas.ptr, PGSIZE);
}

void unprotect(AddrSpace *as) {
}

void __am_get_cur_as(Context *c) {
  c->pdir = (vme_enable ? (void *)get_satp() : NULL);
}

void __am_switch(Context *c) {
  if (vme_enable && c->pdir != NULL) {
    set_satp(c->pdir);
  }
}

static inline uintptr_t VPN1(uintptr_t addr)
{
  return (addr & 0xffc00000u)>>22;
}

static inline uintptr_t VPN0(uintptr_t addr)
{
  return (addr & 0x003ff000u)>>12;
}

#define OFFSET 0x00000fffu
#define PPN 0xfffffc00u
#define PTE_SIGN 0x000003ffu

//TODO: 实现map 将地址空间as中虚拟地址va所在的虚拟页,映射到pa所在的物理页
//目前有点bug
void map(AddrSpace *as, void *va, void *pa, int prot) {
  uintptr_t vaddr=(uintptr_t)va;
  PTE* pte=(PTE*)(as->ptr+ VPN1(vaddr)*4); //找到二级页表项 page_table_entry

  if((*pte & PTE_V)==0) //V位为0说明页表项无效
  {
    void* page_table=pgalloc_usr(PGSIZE);
    *pte=(((uintptr_t)page_table >> 2) & PPN) | (*pte & PTE_SIGN);
    *pte+=1;//将V位置为1
  }

  PTE* leaf_pte=(PTE*)(((*pte & PPN)<<2) + VPN0(vaddr)*4);//找到叶节点页表项
  uintptr_t paddr=(uintptr_t)pa;
  *leaf_pte=((paddr >> 2) & PPN) | 0xf;
}

Context *ucontext(AddrSpace *as, Area kstack, void *entry) {
  Context *c =kstack.end-sizeof(Context);
  
  c->pdir=NULL;
  c->mstatus=0x1800;
  c->mepc=(uintptr_t)entry;


  return c;
}
