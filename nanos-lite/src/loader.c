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
  //TODO把用户程序加载到正确的内存位置
  Elf_Ehdr elf_head;
  ramdisk_read(&elf_head,0, sizeof(Elf_Ehdr));
  if(elf_head.e_ident[0]!=0x7F || elf_head.e_ident[1]!='E' || elf_head.e_ident[2]!='L' || elf_head.e_ident[3]!='F')
      {printf("Error,file is not a valid elf");
       assert(0);
       }

  Elf_Phdr *phdr=(Elf_Phdr*)malloc(sizeof(Elf_Phdr)*elf_head.e_phnum);
  assert(phdr!=NULL);
  ramdisk_read(phdr,elf_head.e_phoff,sizeof(Elf_Phdr)*elf_head.e_phnum);
  printf("%d\n",elf_head.e_phnum);
  for(int i=0;i<elf_head.e_phnum;i++)
    if(phdr[i].p_type==PT_LOAD)
    { printf("load\n");
      printf("%p\n",phdr[i].p_offset); 
      printf("%p\n",phdr[i].p_vaddr); 
      printf("%d\n",phdr[i].p_memsz); 

    
    }
  printf("load end\n");
  return 0x83000000;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}