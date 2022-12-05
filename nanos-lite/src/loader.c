#include <proc.h>
#include <elf.h>
#include <fs.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

size_t ramdisk_read(void *buf, size_t offset, size_t len);
int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
Elf_Ehdr elf_head;

static uintptr_t loader(PCB *pcb, const char *filename) {
  //TODO把用户程序加载到正确的内存位置
  int fd=fs_open(filename,0,0);
  fs_read(fd,&elf_head,sizeof(Elf_Ehdr));
  assert(*(uint32_t *)elf_head.e_ident == 0x464c457f);
  Elf_Phdr *phdr=(Elf_Phdr*)malloc(sizeof(Elf_Phdr)*elf_head.e_phnum);
  assert(phdr!=NULL);

  fs_lseek(fd,elf_head.e_phoff,SEEK_SET);
  fs_read(fd,phdr,sizeof(Elf_Phdr)*elf_head.e_phnum);

  for(int i=0;i<elf_head.e_phnum;i++)
    if(phdr[i].p_type==PT_LOAD)
    { printf("load\n");
      printf("%p\n",phdr[i].p_offset); 
      printf("%p\n",phdr[i].p_vaddr);
      printf("%d\n",phdr[i].p_memsz);

      fs_lseek(fd,phdr[i].p_offset,SEEK_SET);
      fs_read(fd,(void*)phdr[i].p_vaddr,phdr[i].p_memsz);


      memset((void*)(phdr[i].p_vaddr+phdr[i].p_filesz),0,phdr[i].p_memsz-phdr[i].p_filesz);
    }
  printf("load end\n");
  printf("entry %p\n",elf_head.e_entry);
  return elf_head.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, "/bin/hello");
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}