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
    
    for (int i = 0; i < elf_head.e_shnum; i++)
	{
		temp = shstrtab;
		temp = temp + shdr[i].sh_name;
        if (strcmp(temp, ".dynsym") != 0) continue;//该section名称
		printf("节的名称: %s\n", temp);
		printf("节首的偏移: %x\n", shdr[i].sh_offset);
		printf("节的大小: %x\n", shdr[i].sh_size);
        uint8_t *sign_data=(uint8_t*)malloc(sizeof(uint8_t)*shdr[i].sh_size);
		// 依据此段在文件中的偏移读取出
		ret=fseek(elfp, shdr[i].sh_offset, SEEK_SET);
		ret=fread(sign_data, sizeof(uint8_t)*shdr[i].sh_size, 1, elfp);
		// 显示读取的内容
		uint8_t *p = sign_data;
		int j = 0;
		for (j=0; j<shdr[i].sh_size; j++)
		{
		    printf("%x", *p);
            p++;
		}
	 }


    free(shdr);

}