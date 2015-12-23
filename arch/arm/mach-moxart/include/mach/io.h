#ifndef __ASM_ARM_ARCH_IO_H
#define __ASM_ARM_ARCH_IO_H

#define IO_SPACE_LIMIT      0xffffffff
#define __io(a)             (a)
#define __mem_pci(a)        ((unsigned long)(a))

#if 0	// mask by Victor Yu. 11-16-2005
#define __arch_getw(a) 	    (*(volatile unsigned short *)(a))
#define __arch_putw(v,a)    (*(volatile unsigned short *)(a) = (v))
#endif

#define iomem_valid_addr(iomem,sz)  (1)
#define iomem_to_phys(iomem)        (iomem)

#endif
