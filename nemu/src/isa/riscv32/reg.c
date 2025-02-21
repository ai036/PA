/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>
#include "local-include/reg.h"

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

void isa_reg_display() {
  printf("The states of all General Purpose Registers:\n");
  for(int i = 0; i <32; i++)
    {if(i==26||i==27)
       printf("%s  %p   %d     ",regs[i],&cpu.gpr[i],cpu.gpr[i]);
     else
       printf("%s   %p   %d     ",regs[i],&cpu.gpr[i],cpu.gpr[i]);
     if((i+1)%2==0)
        printf("\n");
    }
  printf("\n");
  printf("The state of PC:\n");
  printf("pc   %p   %d\n",&cpu.pc,cpu.pc);

}

word_t isa_reg_str2val(const char *s, bool *success) {
  if(strcmp("$0",s)==0)
    return cpu.gpr[0];
  int i=1;
  char str[5];//去掉$
  while(i<=2)
    {str[i-1]=s[i];
     i++;
     }
  str[2]= '\0';
  if(s[3]=='1'||s[3]=='2')//识别s10和s11
    {str[2]=s[3];
     str[3]='\0';}
  for(int i = 0; i <32; i++)
    {if(strcmp(regs[i],str)==0)
        { if(success!=NULL)
            *success=true;
          return cpu.gpr[i];
          }}
    if(strcmp("pc",str)==0)
      return cpu.pc;
  printf("%s\n",str);
  printf("Wrong register name!\n");
  assert(0);
}
