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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";
int expr_index=0;
int choose(int n)
{
  return rand()%n;
}

void gen_num()
{ int n=choose(2)+1;
  for(int i=0; i<n; i++){
  int x=choose(10);
  if(x==0&&i==0&&n!=1)
    continue;
  char dict[10]={'0','1','2','3','4','5','6','7','8','9'};
  buf[expr_index]=dict[x];
  expr_index++;
}
}

void gen_rand_op()
{
  int x=choose(4);
  char op[4]={'+','-','*','/'};
  buf[expr_index]=op[x]; 
  expr_index++;
}

void gen(char c)
{
  buf[expr_index]=c;
  expr_index++;
}

static void gen_rand_expr() {

  switch (choose(4)) {
    case 0: gen_num(); break;
    case 1: gen('('); gen_rand_expr(); gen(')'); break;
    case 2: gen_rand_expr(); gen_rand_op(); gen_rand_expr(); break;
    case 3: gen(' ');gen_rand_expr();break;
  }
  buf[expr_index] = '\0';
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    expr_index=0;
    gen_rand_expr();

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);

    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    fscanf(fp, "%d", &result);
    if(result<0)
      continue;
    pclose(fp);
    

    printf("%u %s\n", result, buf);
  }
  return 0;
}
