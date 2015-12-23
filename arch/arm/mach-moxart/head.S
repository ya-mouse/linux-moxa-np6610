/*
 *  uClinux kernel startup code for s5c7375
 *  which has no proper bootloader for linux startup
 *  because of XIP.
 */
#include <linux/config.h>
#include <linux/linkage.h>
#include <linux/init.h>

#include <asm/assembler.h>
#include <asm/mach-types.h>
#include <asm/procinfo.h>
#include <asm/ptrace.h>
#include <asm/mach/arch.h>

/*
 * Kernel startup entry point.
 */

ENTRY(led1)
@		bl	led
@led1:
		ldr	r2, =0x98700000
		mov	r3, #0x0000000f
		strb	r3,[r2]
		mov	pc,lr
ENTRY(led2)
@led2:
		ldr	r2, =0x98700000
		mov	r3, #0x0000000e
		strb	r3,[r2]
		mov	pc,lr
ENTRY(led3)
@led3:
		ldr	r2, =0x98700000
		mov	r3, #0x0000000d
		strb	r3,[r2]
		mov	pc,lr
ENTRY(led4)
@led4:
		ldr	r2, =0x902ffffc
		mov	r3, #4
		strb	r3,[r2]
		mov	pc,lr
ENTRY(led5)
@led5:
		ldr	r2, =0x902ffffc
		mov	r3, #5
		strb	r3,[r2]
		mov	pc,lr
ENTRY(led6)
@led6:
		ldr	r2, =0x902ffffc
		mov	r3, #6
		strb	r3,[r2]
		mov	pc,lr

ENTRY(led7)
@led7:
		ldr	r2, =0x902ffffc
		mov	r3, #7
		strb	r3,[r2]
		mov	pc,lr

ENTRY(led8)
@led8:
		ldr	r2, =0x902ffffc
		mov	r3, #8
		strb	r3,[r2]
		mov	pc,lr

ENTRY(led9)
@led9:
		ldr	r2, =0x902ffffc
		mov	r3, #9
		strb	r3,[r2]
		mov	pc,lr

ENTRY(msg)
@msg:
		ldr	r2, =0xB0900000
		mov	r3, #0x70
		strb	r3,[r2]
		mov	pc,lr
ENTRY(hank)
@msg:
		ldr	r2, =0x98700000
		mov	r3, #0x05
		strb	r3,[r2]
		mov	pc,lr
#include "../kernel/head.S"
