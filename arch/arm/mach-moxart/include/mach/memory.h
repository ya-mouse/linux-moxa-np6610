
/*
 * linux/include/asm-armnommu/arch-p52/memory.h
 *
 * Copyright (c) 1999 Nicolas Pitre <nico@cam.org>
 * 2001 Mindspeed
 */

#ifndef __ASM_ARCH_MEMORY_H
#define __ASM_ARCH_MEMORY_H

/* #include <linux/config.h> */
#include	<asm/sizes.h>
#define CONFIG_SDRAM_SIZE	SZ_64M
#define CONFIG_DRAM_BASE 0x00000000
#define CONFIG_DRAM_SIZE CONFIG_SDRAM_SIZE

#define DRAM_BASE       0x00000000
#define DRAM_SIZE       CONFIG_SDRAM_SIZE
#define MEM_SIZE        DRAM_SIZE

#define PHYS_OFFSET     (DRAM_BASE)
#define END_MEM         (DRAM_BASE + DRAM_SIZE)
#define DMA_SIZE        0xffffffff

#define __pfn_to_bus(x) __pfn_to_phys(x)
#define __bus_to_pfn(x) __phys_to_pfn(x)

#define __virt_to_phys__is_a_macro
#define __virt_to_phys(vpage) ((vpage) - PAGE_OFFSET)
#define __phys_to_virt__is_a_macro
#define __phys_to_virt(ppage) ((ppage) + PAGE_OFFSET)

#define __virt_to_bus__is_a_macro
#define __virt_to_bus(x)	(x - PAGE_OFFSET)
#define __bus_to_virt__is_a_macro
#define __bus_to_virt(x)	(x + PAGE_OFFSET)

#if 0

#if 0	/* mask by Victor Yu. 11-17-2005 */
#define TASK_SIZE       (0x01a00000UL)
#define TASK_SIZE_26    TASK_SIZE

#define DRAM_BASE       0x00000000
#define DRAM_SIZE       CONFIG_DRAM_SIZE
#define MEM_SIZE        DRAM_SIZE

#define PHYS_OFFSET     (DRAM_BASE)
#define PAGE_OFFSET     (0xc0000000UL)
#define PAGE_OFFSET     (DRAM_BASE)
#define END_MEM         (DRAM_BASE + DRAM_SIZE)
#define DMA_SIZE        0xffffffff
#endif

//#define TASK_SIZE	(0x01a00000UL)
//#define TASK_SIZE_26	TASK_SIZE
#define	PHYS_OFFSET	CONFIG_DRAM_BASE
//#define	PAGE_OFFSET	CONFIG_DRAM_BASE
#define END_MEM		(CONFIG_DRAM_BASE + CONFIG_DRAM_SIZE)

#if 0
#define __virt_to_phys(vpage) ((unsigned long) (vpage))
#define __phys_to_virt(ppage) ((void *) (ppage))
#define __virt_to_bus(vpage) ((unsigned long) (vpage))
#define __bus_to_virt(ppage) ((void *) (ppage))
#define dma_to_virt(dev, addr)		((void *)__bus_to_virt(addr))
#define virt_to_dma(dev, addr)		((dma_addr_t)__virt_to_bus((unsigned long)(addr)))
#define page_to_dma(dev, page)		((dma_addr_t)__virt_to_bus((unsigned long)page_address(page)))
#endif

#define PLAT_PHYS_OFFSET UL(0x00000000)

#define __virt_to_bus(x) __virt_to_phys(x)
#define __bus_to_virt(x) __phys_to_virt(x)
#define __pfn_to_bus(x) __pfn_to_phys(x)
#define __bus_to_pfn(x)	__phys_to_pfn(x)


#if 0	/* mask by Victor Yu. */
#define __virt_to_phys__is_a_macro
#define __virt_to_phys(vpage) ((vpage) - PAGE_OFFSET)
#define __phys_to_virt__is_a_macro
#define __phys_to_virt(ppage) ((ppage) + PAGE_OFFSET)

#define __virt_to_bus__is_a_macro
#define __virt_to_bus(x)	(x - PAGE_OFFSET)
#define __bus_to_virt__is_a_macro
#define __bus_to_virt(x)	(x + PAGE_OFFSET)

#define TASK_UNMAPPED_BASE (TASK_SIZE / 3)
#endif

#endif

#endif
