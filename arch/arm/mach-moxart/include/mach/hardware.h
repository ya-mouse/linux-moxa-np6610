/* 
    hardware.h 
    maintened by ivan wang 2004/8/18 11:25 
*/
#ifndef __ASM_ARCH_HARDWARE_H
#define __ASM_ARCH_HARDWARE_H

/* the mini io address is 0x6000,that is IO will allocate from 0-0x6000 offset*/
#define PCIBIOS_MIN_IO  0x6000
/* the mini MEM address is 0x100000,that is MEM will allocate from 0-0x100000 offset*/
#define PCIBIOS_MIN_MEM 0x100000

#define pcibios_assign_all_busses() 1
#endif

#if defined(CONFIG_DRAM_BASE) && defined(CONFIG_DRAM_SIZE)
  #define PA_SDRAM_BASE          (CONFIG_DRAM_BASE)
  #define MEM_SIZE               (CONFIG_DRAM_SIZE)
#endif
