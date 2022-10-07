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

#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  char *expr;
  int value;
  /* TODO: Add more members if necessary */

} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

int new_wp(char *str)
{ bool a;
  if(free_==NULL)
    assert(0);

  WP *p=NULL;
  if(free_->next!=NULL)
    {p=free_;
     free_=free_->next;
    }
  else
    p=free_;
  //p->NO=num;
  if(head==NULL)
    head=p;
     
  else 
    { WP *q=head;
      while(q->next!=NULL)
        q=q->next;
      q->next=p;
    }
 // num++;
  p->expr=str;
  p->value=expr(str,&a);
  printf("Hardware watchpoint %d: %s\n",p->NO,str);
  p->next=NULL;
  return 0;
    
};
/* TODO: Implement the functionality of watchpoint */

void free_wp(int num)
{
  bool find=false;
  if(num>31||num<0)
    assert(0);  
  WP* wp=head;
  WP* pre=head;
  if(head->NO==num)
  { find=true;
    head=head->next;}   
  else if(head->next!=NULL)
  {
    wp=head->next;
    while(wp!=NULL)
    { if(wp->NO==num)       //找到num号监视点
      { find=true;
        break;}
      wp=wp->next;               
      pre=pre->next;
    }
  }
  if(!find)
    assert(0);
  pre->next=wp->next;
  if (free_==NULL)
    {free_=wp;
     return ;}
  WP *tail=free_;
  while(tail->next != NULL)
    tail=tail->next;
  tail->next=wp;
  printf("free watchpoint %d: %s\n", wp->NO,wp->expr);
  wp->expr=NULL;

}

