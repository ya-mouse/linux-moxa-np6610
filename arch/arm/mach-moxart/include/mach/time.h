/* 
    time.h 
    maintened by ivan wang 2004/8/18 11:25 
*/


#ifndef __ASM_ARCH_TIME_H__
#define __ASM_ARCH_TIME_H__

#include <asm/uaccess.h>
#include <asm/io.h>
#include <mach/hardware.h>
#include "cpe_time.h"
#include <mach/timex.h>
#include <mach/irq.h>


#endif /* __ASM_ARCH_TIME_H__ */
#if 0	// mask by Victor Yu. 11-17-2005
#include <asm/arch/cpe_int.h>


#define TIMER1_COUNT                0x0
#define TIMER1_LOAD                 0x4
#define TIMER1_MATCH1               0x8
#define TIMER1_MATCH2               0xC
#define TIMER2_COUNT                0x10
#define TIMER2_LOAD                 0x14
#define TIMER2_MATCH1               0x18
#define TIMER2_MATCH2               0x1C
#define TIMER3_COUNT                0x20
#define TIMER3_LOAD                 0x24
#define TIMER3_MATCH1               0x28
#define TIMER3_MATCH2               0x2C 
#define TIMER_CR                    0x30
#endif
