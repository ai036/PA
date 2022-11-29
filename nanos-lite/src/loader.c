#include <proc.h>
#include <elf.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

size_t ramdisk_read(void *buf, size_t offset, size_t len);
Elf_Ehdr elf;

static uintptr_t loader(PCB *pcb, const char *filename) {
  TODO();             //把用户程序加载到正确的内存位置

  Elf32_Ehdr elf_head;
  ramdisk_read(&elf_head,0, sizeof(Elf32_Ehdr));
  
  return 0;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

