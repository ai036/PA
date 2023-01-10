#include <proc.h>
#include <elf.h>
#include <fs.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
# define ELF_Off  Elf64_Off

#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
# define ELF_Off  Elf32_Off
#endif

int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);

static void read(int fd, void *buf, size_t offset, size_t len){
  fs_lseek(fd, offset, SEEK_SET);
  fs_read(fd, buf, len);
}


#define NR_PAGE 8
#define PAGESIZE 4096

 __attribute__ ((__used__)) static void * alloc_section_space(AddrSpace *as, uintptr_t vaddr, size_t p_memsz){
  //size_t page_n = p_memsz % PAGESIZE == 0 ? p_memsz / 4096 : (p_memsz / 4096 + 1);
  size_t page_n = ((vaddr + p_memsz - 1) >> 12) - (vaddr >> 12) + 1;
  void *page_start = new_page(page_n);

  printf("Loaded Segment from [%x to %x)\n", vaddr, vaddr + p_memsz);
  
  for (int i = 0; i < page_n; ++i){
    // TODO: 这里prot参数不规范
    map(as, (void *)((vaddr & ~0xfff) + i * PAGESIZE), (void *)(page_start + i * PAGESIZE), 1);
  }

  return page_start;
}

#define MAX(a, b)((a) > (b) ? (a) : (b))

static uintptr_t loader(PCB *pcb, const char *filename) {
  int fd = fs_open(filename, 0, 0);
  if (fd == -1){ 
    assert(0); //filename指向文件不存在
  }
  
  AddrSpace *as = &pcb->as;
  
  Elf_Ehdr elf_header;
  read(fd, &elf_header, 0, sizeof(elf_header));
  //根据小端法 0x7F E L F
  assert(*(uint32_t *)elf_header.e_ident == 0x464c457f);
  
  ELF_Off program_header_offset = elf_header.e_phoff;
  size_t headers_entry_size = elf_header.e_phentsize;
  int headers_entry_num = elf_header.e_phnum;

  for (int i = 0; i < headers_entry_num; ++i){
    Elf_Phdr section_entry;
    read(fd, &section_entry, 
      i * headers_entry_size + program_header_offset, sizeof(elf_header));
    void *phys_addr;
    uintptr_t virt_addr;
    switch (section_entry.p_type) {
    case PT_LOAD:
      //virt_addr = (void *)section_entry.p_vaddr; 
      // phys_addr = (void *)alloced_page_start + (section_entry.p_vaddr - 0x40000000); // 这里是把0x40000000加载到他对应的实际地址
      virt_addr = section_entry.p_vaddr;
      phys_addr = alloc_section_space(as, virt_addr, section_entry.p_memsz);

      // printf("Load to %x with offset %x\n", phys_addr, section_entry.p_offset);
      //做一个偏移
      read(fd, phys_addr + (virt_addr & 0xfff), section_entry.p_offset, section_entry.p_filesz);
      //同样做一个偏移
      memset(phys_addr + (virt_addr & 0xfff) + section_entry.p_filesz, 0, 
        section_entry.p_memsz - section_entry.p_filesz);
      
      if (section_entry.p_filesz < section_entry.p_memsz){// 应该是.bss节
        //做一个向上的4kb取整数
        // if (pcb->max_brk == 0){
        printf("Setting .bss end %x\n", section_entry.p_vaddr + section_entry.p_memsz);
        // 我们虽然用max_brk记录了最高达到的位置，但是在新的PCB中，我们并未在页表目录中更新这些信息，oH，所以就会失效啦。
        // 于是我们就做了一些权衡。
        //pcb->max_brk = MAX(pcb->max_brk, ROUNDUP(section_entry.p_vaddr + section_entry.p_memsz, 0xfff));
        //TODO: Trade-off
        pcb->max_brk = ROUNDUP(section_entry.p_vaddr + section_entry.p_memsz, 0xfff);
        
        // }
      }
      
      break;
    
    default:
      break;
    }

  }
  
  printf("Entry: %p\n", elf_header.e_entry);
  return elf_header.e_entry;
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

static int str_byte(int len)//根据字符串长度进行四字节对齐，不然会出BUG
{
  int bytes=len+1;
  if(bytes%4==0)
    return bytes;
  else
    return ((bytes>>2)<<2)+4;
}

void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[])
{
  printf("context_uload: %s\n", filename);
  protect(&pcb->as);

  Area kstack;
  kstack.start=&pcb->cp;
  kstack.end=&pcb->cp+STACK_SIZE;

  void* npage=new_page(8) + (8 << 12); //分到的页面栈顶

  for(int i=1;i<=8;i++)
  {
    map(&pcb->as, pcb->as.area.end - i*PAGESIZE, npage - i*PAGESIZE,1);
  }
  int envc=0,argc=0;
  char* brk=(char*)npage;
  
  if(argv)
    for(;argv[argc]!=NULL;argc++)
    { 
      int len=str_byte(strlen(argv[argc]));
      brk-=len;
    }

  if(envp)
    for(;envp[envc]!=NULL;envc++)
    {
      int len=str_byte(strlen(envp[envc]));
      brk-=len;
    }

  printf("argc:%d  envc:%d\n",argc,envc);
  char* str=brk;
  char** ptr=(char**)brk;

  ptr-=1;
  *ptr=NULL;
  printf("HHHHHH%p\n",ptr);

  ptr-=1;
  for(int i=envc-1;i>=0;i--)
  {
    strcpy(str,envp[i]);
    int len=strlen(envp[i]);
    *ptr=str;
    ptr-=1;
    str[len]='\0';
    str+=str_byte(len);
  }

  *ptr=NULL;  
  ptr-=1;


  for(int i=argc-1;i>=0;i--)
  {
    strcpy(str,argv[i]);
    int len=strlen(argv[i]);
    *ptr=str;
    ptr-=1;
    str[len]='\0';
    str+=str_byte(len);
  }

  
  int* iptr=(int*)ptr;
  *iptr=argc;

  uintptr_t entry = loader(pcb, filename);
  Context* c=ucontext(&pcb->as,kstack,(void*)entry);
  pcb->cp=c;
  c->GPRx=(uintptr_t)ptr;

}

