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
#include <memory/vaddr.h>
enum {
  TK_NOTYPE = 256, TK_NUM=10,TK_HEX=16,TK_REG=32,TK_EQ,TK_NEQ,TK_LEFT,TK_RIGHT,TK_MINUS,TK_AND,DEREF

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
  {"0[xX][0-9a-fA-F]+" , TK_HEX},//十六进制数字 
  {"[0-9]+" , TK_NUM},       // 十进制数字
  {"(\\$0)|(\\$[a-z]+)|(\\$[a-z0-9]+)",TK_REG}, //寄存器(可能有点问题)
  {"\\+", '+'},         // plus
  {"\\-", '-'},         // 减
  {"\\*", '*'},         // 乘
  {"\\/", '/'},         // 除
  {"==", TK_EQ},        // equal
  {"!=", TK_NEQ},       // not equal
  {"[(]", TK_LEFT},      // 左括号
  {"[)]", TK_RIGHT},     // 右括号
  {"&&", TK_AND}        // and
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
  int priority;
} Token;

static Token tokens[10000] __attribute__((used)) = {};
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
          case '+': tokens[nr_token].type='+';tokens[nr_token].priority=4;break;
          case '-': tokens[nr_token].type='-';tokens[nr_token].priority=4;
                    if(nr_token==0||(tokens[nr_token-1].type!=TK_NUM&&tokens[nr_token-1].type!=TK_HEX \
                    &&tokens[nr_token-1].type!=TK_REG&&tokens[nr_token-1].type!=TK_RIGHT))
                      {tokens[nr_token].type=TK_MINUS;//判断为负号
                       tokens[nr_token].priority=2;}
                    break;
          case '*': tokens[nr_token].type='*';tokens[nr_token].priority=3;
                    if(nr_token==0||(tokens[nr_token-1].type!=TK_NUM&&tokens[nr_token-1].type!=TK_HEX \
                    &&tokens[nr_token-1].type!=TK_REG&&tokens[nr_token-1].type!=TK_RIGHT))
                      {tokens[nr_token].type=DEREF;
                       tokens[nr_token].priority=2;}
                    break;
          case '/': tokens[nr_token].type='/';tokens[nr_token].priority=3;break;
          case TK_EQ: tokens[nr_token].type=TK_EQ;tokens[nr_token].priority=7;break;
          case TK_NEQ: tokens[nr_token].type=TK_NEQ;tokens[nr_token].priority=7;break;
          case TK_HEX: tokens[nr_token].type=TK_HEX;tokens[nr_token].priority=0;
            for(int k=0;k<substr_len;k++)
              tokens[nr_token].str[k]=substr_start[k];
            tokens[nr_token].str[substr_len]='\0';
            break;
          case TK_NUM: tokens[nr_token].type=TK_NUM;tokens[nr_token].priority=0;
            for(int k=0;k<substr_len;k++)
              tokens[nr_token].str[k]=substr_start[k];
            tokens[nr_token].str[substr_len]='\0';
            break;
          case TK_LEFT: tokens[nr_token].type=TK_LEFT;tokens[nr_token].priority=1;break;
          case TK_RIGHT: tokens[nr_token].type=TK_RIGHT;tokens[nr_token].priority=1;break;
          case TK_NOTYPE: break;
          case TK_AND: tokens[nr_token].type=TK_AND;tokens[nr_token].priority=11;break;
          case TK_REG: tokens[nr_token].type=TK_REG;tokens[nr_token].priority=0;
            for(int k=0;k<substr_len;k++)
              tokens[nr_token].str[k]=substr_start[k];
            tokens[nr_token].str[substr_len]='\0';
            break;

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

int eval(int p, int q);

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  //TODO();
//  for(int i=0;i<nr_token;i++) 
//    printf("%d\n",tokens[i].type);
  return eval(0,nr_token-1);
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
      if(i!=q&&count==0)
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
    printf("%d  %d\n",p,q);
    return -1;
  }
  else if (p == q) {
    /* Single token.
     * For now this token should be a number.
     * Return the value of the number.
     */
    if(tokens[p].type==TK_NUM)
      return atoi(tokens[p].str);
    else if(tokens[p].type==TK_HEX)
      return strtol(tokens[p].str,NULL,16);
    else
      return isa_reg_str2val(tokens[p].str,NULL);
  }
  else if (check_parentheses(p, q) == true) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    //printf("YES");
    return eval(p + 1, q - 1);
  }
  else {
    
    int op = p;           //the position of 主运算符 in the token expression;
    int index=p;
    int flag=0;
    int priority=0;
    while(index<=q)
    {
      if(tokens[index].type==TK_NUM);
      
      else if(tokens[index].type==TK_LEFT)
        flag++;
      else if(tokens[index].type==TK_RIGHT)
        flag--;
      else if(tokens[index].priority>=2)
        if(flag==0&&tokens[index].priority>=priority)
          {op=index;
          priority=tokens[index].priority;
          }

      index++;
    }
    if(priority==2)
    { index=q;
      flag=0;
      while(index>=p)
        {
          if(tokens[index].type==TK_NUM);
          else if(tokens[index].type==TK_RIGHT)
            flag++;
          else if(tokens[index].type==TK_LEFT)
            flag--;
          else if(tokens[index].priority==2)
            {if(flag==0)
              op=index;}
          index--;
        }
    
    }
    int val1=0; 
    if(tokens[op].type!=TK_MINUS&&tokens[op].type!=DEREF)
      val1= eval(p, op - 1);
    int val2 = eval(op + 1, q);

    switch (tokens[op].type) {
      case '+': return val1 + val2;
      case '-': return val1 - val2;
      case '*': return val1 * val2;
      case '/': return val1 / val2;
      case TK_MINUS: return -val2;
      case TK_EQ: return val1 == val2;
      case TK_NEQ: return val1 != val2;
      case TK_AND: return val1 && val2;
      case DEREF: if(tokens[op+1].type==TK_LEFT&&tokens[q].type==TK_RIGHT)
                {
                 int addr=eval(op+1,q);
                 return vaddr_read(addr,4);
                 }
      default: assert(0);
    }
  }
}