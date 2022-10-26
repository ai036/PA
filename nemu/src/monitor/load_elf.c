#include "load_elf.h"
extern FILE *elfp;
char* strtab=NULL;
Elf32_Sym *symtab=NULL;
int nr_symtab_entry;
int func_count=0;

struct Func_Info{
    char func_name[64];
    paddr_t start;
    size_t size;
}elf_func[1024];


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
			 printf("0x%08x\n",symtab[i].st_value);
             printf("func_name: %s  ",strtab+symtab[i].st_name);

             strcpy(elf_func[func_count].func_name,strtab+symtab[i].st_name);
             elf_func[func_count].start=symtab[i].st_value;
             elf_func[func_count].size=symtab[i].st_size;
		}
	}
    
    free(shdr);
    fclose(elfp);
}



