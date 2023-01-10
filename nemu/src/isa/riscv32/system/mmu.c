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
#include <memory/paddr.h>
#include <memory/vaddr.h>

static inline uintptr_t VPN1(uintptr_t addr)
{
  return (addr & 0xffc00000u)>>22;
}

static inline uintptr_t VPN0(uintptr_t addr)
{
  return (addr & 0x003ff000u)>>12;
}

#define OFFSET 0x00000fffu
#define PPN 0xfffffc00u
#define PTE_SIGN 0x000003ffu

#define PTE_V 0x01

typedef uintptr_t PTE;

//TODO: 实现地址翻译
paddr_t isa_mmu_translate(vaddr_t vaddr, int len, int type) {
  //这里不能直接用指针读取
  paddr_t pte_addr=(csr.satp << 12)+ VPN1(vaddr)*4;
  PTE pte=paddr_read(pte_addr,4);//页目录项
  assert(pte & PTE_V);//检查页目录项的valid位

  paddr_t leaf_pte_addr= ((pte & PPN) << 2) + VPN0(vaddr)*4;
  PTE leaf_pte=paddr_read(leaf_pte_addr,4);//叶节点页表项
  assert(leaf_pte & PTE_V);//检查页表项的valid位
  if (type == 0){//读
    paddr_write(leaf_pte_addr, 4, leaf_pte | 0x40);
  }else if (type == 1){//写
    paddr_write(leaf_pte_addr, 4, leaf_pte | 0x80);
  }
  paddr_t paddr=((leaf_pte & PPN) << 2) | (vaddr & OFFSET);//物理地址

  assert(paddr==vaddr);
  return paddr;
}