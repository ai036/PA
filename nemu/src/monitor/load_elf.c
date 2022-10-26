#include "load_elf.h"
extern FILE *elfp;
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

    uint8_t *strtab=(uint8_t*)malloc(sizeof(uint8_t)*shdr[strtab_index].sh_size);
    ret=fseek(elfp,shdr[strtab_index].sh_offset,SEEK_SET);
    ret=fread(strtab,sizeof(char)*shdr[strtab_index].sh_size,1,elfp);//读取strtab

		// 显示读取的内容
	printf("%s",strtab);

    char* symtab=(char*)malloc(sizeof(char)*shdr[symtab_index].sh_size);
    ret=fseek(elfp,shdr[symtab_index].sh_offset,SEEK_SET);
    ret=fread(symtab,sizeof(char)*shdr[symtab_index].sh_size,1,elfp);//读取symtab
    
    free(strtab);
    free(symtab);
    free(shdr);

}