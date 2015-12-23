/*
 *  
 *	jimmy_chen@moxa.com.tw
 */

//#include <asm/arch/irq.h>
#include <asm/arch/moxa.h>
#if defined(CONFIG_ARCH_MOXART)
		.macro	disable_fiq
		.endm

		.macro  get_irqnr_preamble, base, tmp
		.endm

		.macro get_fiqnr_and_base, irqnr, irqstat, base, tmp
		ldr		\base, =(IO_ADDRESS(CPE_IC_BASE)+FIQ_STATUS_REG)
		ldr 		\irqstat, [\base]
		mov		\irqnr, #32
2001:
		tst		\irqstat, #1
		bne		2002f
		add 		\irqnr, \irqnr, #1
		mov		\irqstat, \irqstat, lsr #1
		cmp		\irqnr, #64
		bcc		2001b
2002:
		.endm

		.macro get_irqnr_and_base, irqnr, irqstat, base, tmp
		ldr		\base, =(CPE_IC_BASE+IRQ_STATUS_REG) 
		ldr		\irqstat, [\base]
		mov		\irqnr, #0
2003:
		tst		\irqstat, #1
		bne		2004f
		add		\irqnr, \irqnr, #1
		mov		\irqstat, \irqstat, lsr #1
		cmp		\irqnr, #32
		bcc		2003b
2004:
		.endm

		.macro irq_prio_table
		.endm
#endif
