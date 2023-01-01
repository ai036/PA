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
    { printf("load %s\n",filename);

      fs_lseek(fd,phdr[i].p_offset,SEEK_SET);
      fs_read(fd,(void*)phdr[i].p_vaddr,phdr[i].p_memsz);
      memset((void*)(phdr[i].p_vaddr+phdr[i].p_filesz),0,phdr[i].p_memsz-phdr[i].p_filesz);
    }
  fs_close(fd);
  printf("entry: %p\n",elf_head.e_entry);
  return elf_head.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry;
  if(filename!=NULL)
    entry = loader(pcb, filename);
  else
    entry = loader(pcb, "/bin/nterm");
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

int execve(const char *filename,char* const argv[],char* const envp[])
{
  printf("execve %s\n",filename);
  naive_uload(NULL,filename);
  return -1;
}

void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[])
{
  printf("context_uload: %s\n", filename);
  Area kstack;
  kstack.start=&pcb->cp;
  kstack.end=&pcb->cp+STACK_SIZE;

  uintptr_t entry = loader(pcb, filename);
  Context* c=ucontext(NULL,kstack,(void*)entry);
  pcb->cp=c;

  printf("ustack:%p\n",heap.end);

  c->GPRx=(uintptr_t)heap.end;
}