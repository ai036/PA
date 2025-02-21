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

#include <common.h>

void init_monitor(int, char *[]);
void am_init_monitor();
void engine_start();
int is_exit_status_bad();
word_t expr(char *e, bool *success);
void test()//测试表达式
{
  FILE *fp= fopen("input.txt", "r");
  char a[100];
  char r[10];
  int count=0;
  if(fp== NULL)
    printf("Error: Could not open input.txt file.\n");
  while(fgets(a,100,fp) != NULL)
 {
  int len = strlen(a);
  a[len-1] = '\0';  /*去掉换行符*/
  printf("%s\n",a);
  for(int i=0;i<len;i++)
  { if(a[i] != ' ')
      {r[i]=a[i];
      a[i]=' ';}
    else if(a[i]==' ')
    { r[i]='\0';
      break;}
    }
  int res=atoi(r);
  bool success;
  if(res!=expr(a,&success))
    count++;
  printf("number of error: %d\n",count);
 }
}

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
#ifdef CONFIG_TARGET_AM
  am_init_monitor();
#else
  init_monitor(argc, argv);
#endif
 // test();  测试表达式用
  /* Start engine. */
  engine_start();
  
  return is_exit_status_bad();
}
