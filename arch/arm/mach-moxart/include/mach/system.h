/*
 * linux/include/asm-armnommu/arch-cpe/system.h
 *
 * Copyright (c) 1999 Nicolas Pitre <nico@cam.org>
 * Copyright (c) 2001 RidgeRun, Inc (http://www.ridgerun.org)
 *
 */
#include	<mach/moxa.h>

static inline void arch_idle(void)
{
	cpu_do_idle();
//	while (!current->need_resched && !hlt_counter);
}

extern inline void arch_reset(char mode, const char *cmd)
{
#if 1	// add by Victor Yu. 11-23-2005
	if ( mode == 's' ) {
		// Jump to ROM address 0
		cpu_reset(0);
	} else {
		// reset the CPU
		// frist set the counter to 1
		*(unsigned int *)(CPE_WATCHDOG_BASE+4) = 1;

		// to start the watch dog
		*(unsigned int *)(CPE_WATCHDOG_BASE+8) = 0x5ab9;

		// set to reset the CPU
		*(unsigned int *)(CPE_WATCHDOG_BASE+12) = 0x03;
	}
#endif
	/* REVISIT --gmcnutt */
}

