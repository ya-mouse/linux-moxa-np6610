/* asm/arch-cpe/irq.h */

#ifndef __ASM_ARCH_IRQ_H__
#define __ASM_ARCH_IRQ_H__

#include <mach/hardware.h>
#include <asm/io.h>
#include <mach/irq.h>
#ifdef CONFIG_PCI
#include <asm/arch/ftpci.h>
#endif	// CONFIG_PCI

#if 0	// mask by Victor Yu. 03-15-2007
extern void cpe_mask_irq(unsigned int irq);
extern void cpe_unmask_irq(unsigned int irq);
extern void cpe_mask_ack_irq(unsigned int irq);
extern void cpe_clear_irq(unsigned int intNum);
extern void cpe_int_init(void);

#if 1	// add by Victor Yu. 05-17-2005
#include <linux/irq.h>
extern struct irq_chip	cpe_irq_chip;
#include <linux/interrupt.h>
#endif

static inline void irq_init_irq(void)
{
	unsigned long flags;
	int irq;

	local_irq_save(flags);
	cpe_int_init();
	local_irq_restore(flags);
#if 1	// add by Victor Yu. 03-15-2007
	cpe_irq_chip.ack = cpe_mask_ack_irq;
	cpe_irq_chip.mask = cpe_mask_irq;
	cpe_irq_chip.unmask = cpe_unmask_irq;
#endif
	for (irq = 0; irq < NR_IRQS; irq++) {
		set_irq_chip(irq, &cpe_irq_chip);
		set_irq_handler(irq, do_level_IRQ);
		set_irq_flags(irq, IRQF_VALID);
	}
}
#endif

static inline int fixup_irq(int irq)
{
#ifdef CONFIG_ARCH_CPE
	unsigned int status;
	unsigned int i;

	if( irq == IRQ_EXT_A321 ) {
		status=*(volatile unsigned int *)(CPE_A321_IC_VA_BASE+IRQ_STATUS_REG);
		if( status & (1<<(IRQ_A321_PCI-CPE_A321_IRQ_START)) ) { //pci irq
#ifdef CONFIG_PCI
		switch ( ftpci_get_irq() ) {
                case 0: return VIRQ_PCI_A;
                case 1: return VIRQ_PCI_B;
                case 2: return VIRQ_PCI_C;
                case 3: return VIRQ_PCI_D;
		}
#endif	// CONFIG_PCI
        	for( i=0; i<32 ;i++ ) {
			if ( status & (1<<i) )
                		return (i+CPE_A321_IRQ_START);
		}
        }
        
    }
#endif	// CONFIG_ARCH_CPE

#ifdef CONFIG_ARCH_IA240
#ifdef CONFIG_PCI
	if ( irq == IRQ_PCI ) {
		switch ( ftpci_get_irq() ) {
                case 0: return VIRQ_PCI_A;
                case 1: return VIRQ_PCI_B;
                case 2: return VIRQ_PCI_C;
                case 3: return VIRQ_PCI_D;
		}
	}
#endif	// CONFIG_PCI
#endif	// CONFIG_ARCH_IA240

    return irq;
}

#endif /* __ASM_ARCH_IRQ_H__ */
