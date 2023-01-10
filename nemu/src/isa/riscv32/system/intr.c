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

struct EXCEPTION_TRACE
{
  word_t cause;
  vaddr_t epc;
};

#ifdef CONFIG_ETRACE
static struct EXCEPTION_TRACE exception_buf[10];
static int exception_idx=0;
#endif

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
  csr.mcause=NO;
  csr.mepc=epc;
csr.mstatus.status_bit.MPP = 3;
#ifdef CONFIG_ETRACE   //etrace
  if(exception_idx<10)
    {exception_buf[exception_idx].cause=NO;
    exception_buf[exception_idx].epc=epc;
    exception_idx++;}
  else
    printf("error_buf overflow!\n");
#endif

  return csr.mtvec;
}

word_t isa_query_intr() {
  return INTR_EMPTY;
}
