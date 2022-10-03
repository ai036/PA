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

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ,TK_NUM,TK_LEFT,TK_RIGHT,TK_MINUS

  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"\\-", '-'},         // 减  
  {"\\*", '*'},         // 乘
  {"\\/", '/'},         // 除
  {"==", TK_EQ},        // equal
  {"[0-9]+" , TK_NUM},       // 数字
  {"[(]", TK_LEFT},      // 左括号
  {"[)]", TK_RIGHT}     // 右括号
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

int re_nr_token()//这是一个奇怪的函数,因为不知道怎样调用nr_token
{
  return nr_token;
}


static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case '+': tokens[nr_token].type='+';break;
          case '-': tokens[nr_token].type='-';
                    if(nr_token==0||(tokens[nr_token-1].type!=TK_NUM&&tokens[nr_token-1].type!=TK_RIGHT))
                      tokens[nr_token].type=TK_MINUS;//判断为负号
                    break;
          case '*': tokens[nr_token].type='*';break;
          case '/': tokens[nr_token].type='/';break;
          case TK_EQ: tokens[nr_token].type=TK_EQ;break;
          case TK_NUM: tokens[nr_token].type=TK_NUM;
            for(int k=0;k<substr_len;k++)
              tokens[nr_token].str[k]=substr_start[k];
            break;
          case TK_LEFT: tokens[nr_token].type=TK_LEFT;break;
          case TK_RIGHT: tokens[nr_token].type=TK_RIGHT;break;
          case TK_NOTYPE: break;

          default: TODO();
        }
        if(rules[i].token_type!=TK_NOTYPE)
          nr_token++;
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}


word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  //TODO();
  for(int i=0;i<nr_token;i++) 
    printf("%d\n",tokens[i].type);
  return 0;
}

bool check_parentheses(int p, int q)
{ int count=0;
  if(tokens[p].type!=TK_LEFT||tokens[q].type!=TK_RIGHT)
    return false;
    
  for(int i=p;i<=q;i++)
    {
      if(tokens[i].type==TK_LEFT)
        count++;
      else if(tokens[i].type==TK_RIGHT)
        count--;
      if(count<0)
        return false;      
    }
  if(count!=0)
    return false;
  return true;
}

/*bool check_parentheses(int p, int q)
{
  if(tokens[p].type==TK_LEFT&&tokens[q].type==TK_RIGHT)
    return true;
  return false;
}*/


int eval(int p, int q) {
  if (p > q) {
    /* Bad expression */
    printf("Wrong expression.\n");
    return -1;
  }
  else if (p == q) {
    /* Single token.
     * For now this token should be a number.
     * Return the value of the number.
     */
    return atoi(tokens[p].str);
  }
  else if (check_parentheses(p, q) == true) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    printf("YES");
    return eval(p + 1, q - 1);
  }
  else {
    
    int op = p;           //the position of 主运算符 in the token expression;
    int index=p;
    int flag=0;
    while(index<=q)
    {
      if(tokens[index].type==TK_NUM);
      
      else if(tokens[index].type==TK_LEFT)
        flag=1;
      else if(tokens[index].type==TK_RIGHT)
        flag=0;
      else if(tokens[index].type=='+'||tokens[index].type=='-')
        {if(flag==0)
          op=index;}
      else if(tokens[index].type=='*'||tokens[index].type=='/')
        {if(tokens[op].type!='+'&&tokens[op].type!='-'&&flag==0)
          op=index;}
      else if(tokens[index].type==TK_MINUS)
        {if(tokens[op].type!='+'&&tokens[op].type!='-'&&tokens[op].type!='*'&&tokens[op].type!='/'&&flag==0)
          op=index;}
      index++;
    }
    int val1 = eval(p, op - 1);
    int val2 = eval(op + 1, q);
  
    switch (tokens[op].type) {
      case '+': printf("add");return val1 + val2;
      case '-': printf("sub");return val1 - val2;
      case '*': printf("mul");return val1 * val2;
      case '/': printf("div");return val1 / val2;
      case TK_MINUS: printf("minus");return -val2;
      default: assert(0);
    }
  }
}