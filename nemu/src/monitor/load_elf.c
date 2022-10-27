#include "load_elf.h"
#include <cpu/decode.h>
extern FILE *elfp;
char* strtab=NULL;
Elf32_Sym *symtab=NULL;
int nr_symtab_entry;
int func_count=0;

struct Func_Info{
    char func_name[64];
    paddr_t start;
    size_t size;
}elf_func[100];


void load_elf(char* filename)
{
    elfp=fopen(filename,"r"); //打开一个elf文件
    assert(elfp!= NULL);

    //解析head
    int ret;
    Elf32_Ehdr elf_head;
    ret=fread(&elf_head,sizeof(Elf32_Ehdr),1,elfp);//读取head到elf_head;
    assert(ret!=0);

    if(elf_head.e_ident[0]!=0x7F || elf_head.e_ident[1]!='E' || elf_head.e_ident[2]!='L' || elf_head.e_ident[3]!='F')
        {printf("Error,file is not a valid elf");
        assert(0);
        }

    Elf32_Shdr *shdr=(Elf32_Shdr*)malloc(sizeof(Elf32_Shdr)*elf_head.e_shnum);//分配内存section*数量
    assert(shdr!=NULL);

    ret=fseek(elfp,elf_head.e_shoff,SEEK_SET);//调整指针的位置,elf_head.e_shoff为偏移量
    assert(ret==0);

    ret=fread(shdr,sizeof(Elf32_Shdr)*elf_head.e_shnum,1,elfp);   //读取section到shdr
    assert(ret!=0);

    rewind(elfp);//重置指针位置到文件开头

    fseek(elfp,shdr[elf_head.e_shstrndx].sh_offset,SEEK_SET);//移动到字符串表位置处

    char shstrtab[shdr[elf_head.e_shstrndx].sh_size];//shstrtab 段字符串表
    char *temp=shstrtab;
    ret=fread(shstrtab,shdr[elf_head.e_shstrndx].sh_size,1,elfp);   //读取内容
    assert(ret!=0);
    
    int strtab_index=0,symtab_index=0,i=0;
    while(i<elf_head.e_shnum)         //循环查找strtab和symtab
    {   
        temp=shstrtab;
        temp=temp+shdr[i].sh_name;
        if(strcmp(temp,".strtab")==0)
            strtab_index=i;
        else if(strcmp(temp,".symtab")==0)
            symtab_index=i;
        i++;
    }

    strtab=malloc(shdr[strtab_index].sh_size);
    ret=fseek(elfp,shdr[strtab_index].sh_offset,SEEK_SET);
    ret=fread(strtab,shdr[strtab_index].sh_size,1,elfp);//读取strtab
    assert(ret==1);
	// 显示读取的内容
	char *p = strtab;
	int j = 0;
	for (j=0; j<shdr[strtab_index].sh_size; j++)
		{
		    printf("%c", *p);
            p++;
		}

    printf("\n");
    symtab = malloc(shdr[symtab_index].sh_size);
	fseek(elfp, shdr[symtab_index].sh_offset, SEEK_SET);
	ret = fread(symtab, shdr[symtab_index].sh_size, 1, elfp);
	assert(ret == 1);
	int nr_symtab_entry = shdr[symtab_index].sh_size / sizeof(symtab[0]);
    for (i = 0; i < nr_symtab_entry; i++){
		if ((symtab[i].st_info & 0xf) == STT_FUNC){
			 printf("\n0x%08x ",symtab[i].st_value);
             printf("func_name: %s  ",strtab+symtab[i].st_name);

            //把所有函数信息存储在elf_func中
             strcpy(elf_func[func_count].func_name,strtab+symtab[i].st_name);
             elf_func[func_count].start=symtab[i].st_value;
             elf_func[func_count].size=symtab[i].st_size;
             func_count++;
		}
	}

    free(shdr);
    fclose(elfp);
}

struct Function_inst
{
    struct Func_Info current;
    struct Func_Info dest;
    paddr_t addr;
    int type;           //1为call，2为ret
}func_inst[200];
int func_inst_count=0;

void check_func(Decode *s, vaddr_t pc)
{   
    assert(func_inst_count<200);
    func_inst[func_inst_count].addr=pc;   
    int i=0;     
    for(;i<func_count;i++)
    {
        if(s->dnpc==elf_func[i].start)
            {
                func_inst[func_inst_count].dest=elf_func[i];
                func_inst[func_inst_count].type=1;
                func_inst_count++;
                return;  //说明是函数调用
            }
    }
    if(s->dnpc<elf_func[i].start || s->dnpc<elf_func[i].start+elf_func[i].size)//既不是函数调用也不是函数内跳转，则为ret
        {
            func_inst[func_inst_count].current=elf_func[i];
            func_inst[func_inst_count].type=2;
            func_inst_count++;
            return; 
        }
}