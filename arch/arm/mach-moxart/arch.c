/*
 *  linux/arch/arm/mach-cpe/arch.c
 *  modified by ivan wang 2004/8/18 01:35pm
 */
#include <linux/tty.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/init.h>
#include <linux/serial_core.h>
#include <linux/serial_8250.h>

#include <asm/io.h>
#include <asm/elf.h>
#include <asm/setup.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <mach/hardware.h>
#include <asm/sizes.h>
#include <asm/memory.h>
#include <asm/mach/map.h>
#include <mach/time.h>	/*Calling moxa_timer_init*/
#include <mach/irq.h>	/*Calling moxa_irq_init*/
#include <mach/moxa.h>
#include <linux/serial.h>
#include <linux/serial_core.h>
#include <mach/moxa.h>

#if 0	// mask by Victor Yu. 02-12-2007
static struct uart_port moxa_serial_ports[] = {
	{
		.iobase		= CPE_UART1_BASE,
		.mapbase	= CPE_UART1_BASE,
		.irq		= IRQ_UART,
		.flags		= UPF_SKIP_TEST|UPF_SHARE_IRQ,
		.iotype		= UPIO_PORT,	
		.regshift	= 2,
		.uartclk	= CONFIG_UART_CLK,
		.line		= 0,
		.type		= PORT_16550A,
		.fifosize	= 16
	},
	{
		.mapbase	= CPE_UART2_BASE,
		.iobase		= CPE_UART2_BASE,
		.irq		= IRQ_UART,
		.flags		= UPF_SKIP_TEST|UPF_SHARE_IRQ,
		.iotype		= UPIO_PORT,	
		.regshift	= 2,
		.uartclk	= CONFIG_UART_CLK,
		.line		= 1,
		.type		= PORT_16550A,
		.fifosize	= 16
	}
};
#else
static struct uart_port moxa_serial_ports[2];
#endif

#if 0	// mask by Victor Yu. 02-08-2007
static int __init moxa_serial_init(void)
{
	early_serial_setup(&moxa_serial_ports[0]);
	early_serial_setup(&moxa_serial_ports[1]);
	return 0;
}
__initcall(moxa_serial_init);
#endif

#if 1	// add by Victor Yu, 03-15-2007
#include <linux/irq.h>
#include <linux/interrupt.h>
extern void cpe_mask_irq(struct irq_data *d);
extern void cpe_unmask_irq(struct irq_data *d);
extern void cpe_mask_ack_irq(struct irq_data *d);
extern void cpe_clear_irq(unsigned int intNum);
extern void cpe_int_init(void);
static struct irq_chip	cpe_irq_chip;

static u64 eth_dmamask = 0xffffffffUL;

static struct resource ftmac100_eth0_resources[] = {
	[0] = {
		.start	= CPE_FTMAC_BASE,
		.end	= CPE_FTMAC_BASE + 0x00ff,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= IRQ_MAC,
		.end	= IRQ_MAC,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct resource ftmac100_eth1_resources[] = {
	[0] = {
		.start	= CPE_FTMAC2_BASE,
		.end	= CPE_FTMAC2_BASE + 0x00ff,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= IRQ_MAC2,
		.end	= IRQ_MAC2,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device ftmac100_eth0_device = {
	.name		= "ftmac100",
	.id		= 0,
	.dev		= {
				.dma_mask		= &eth_dmamask,
				.coherent_dma_mask	= 0xffffffff,
	},
	.resource	= ftmac100_eth0_resources,
	.num_resources	= ARRAY_SIZE(ftmac100_eth0_resources),
};

static struct platform_device ftmac100_eth1_device = {
	.name		= "ftmac100",
	.id		= 1,
	.dev		= {
				.dma_mask		= &eth_dmamask,
				.coherent_dma_mask	= 0xffffffff,
	},
	.resource	= ftmac100_eth1_resources,
	.num_resources	= ARRAY_SIZE(ftmac100_eth1_resources),
};

static struct platform_device dsa_device = {
	.name		= "dsa",
	.id		= 2,
};

static void __init uc711x_init(void)
{
	printk(KERN_INFO "MOXA NPort 6610-32 initializing\n");
#if 0
	ftmac100_plat_eth[0].hwaddr[0] = 00;
	ftmac100_plat_eth[0].hwaddr[1] = 90;
	ftmac100_plat_eth[0].hwaddr[2] = e8;
	ftmac100_plat_eth[0].hwaddr[3] = 28;
	ftmac100_plat_eth[0].hwaddr[4] = 38;
	ftmac100_plat_eth[0].hwaddr[5] = bd;
#endif
	platform_device_register(&ftmac100_eth0_device);
	platform_device_register(&ftmac100_eth1_device);
	platform_device_register(&dsa_device);
}

static void irq_init_irq(void)
{
	unsigned long flags;
	int irq;

	local_irq_save(flags);
	cpe_int_init();
	local_irq_restore(flags);
	cpe_irq_chip.irq_ack = cpe_mask_ack_irq;
	cpe_irq_chip.irq_mask = cpe_mask_irq;
	cpe_irq_chip.irq_unmask = cpe_unmask_irq;
	for (irq = 0; irq < NR_IRQS; irq++) {
		irq_set_chip_and_handler(irq, &cpe_irq_chip, handle_level_irq);
		set_irq_flags(irq, IRQF_VALID);
	}
}
#endif

static void __init
fixup_uc711x(struct machine_desc *desc, struct tag *tags, char **cmdline, struct meminfo *mi)
{
	mi->nr_banks      = 1;
	mi->bank[0].start = CONFIG_DRAM_BASE;
	mi->bank[0].size  = CONFIG_DRAM_SIZE;

#if 1	// add by Victor Yu. 02-12-2007
	moxa_serial_ports[0].iobase	= CPE_UART1_BASE;
	moxa_serial_ports[0].mapbase	= CPE_UART1_BASE;
	moxa_serial_ports[0].irq	= IRQ_UART;
	moxa_serial_ports[0].flags	= UPF_SKIP_TEST|UPF_SHARE_IRQ;
	moxa_serial_ports[0].iotype	= UPIO_PORT;
	moxa_serial_ports[0].regshift	= 2;
	moxa_serial_ports[0].uartclk	= CONFIG_UART_CLK;
	moxa_serial_ports[0].line	= 0;
	moxa_serial_ports[0].type	= PORT_16550A;
	moxa_serial_ports[0].fifosize	= 16;

	moxa_serial_ports[1].iobase	= CPE_UART2_BASE;
	moxa_serial_ports[1].mapbase	= CPE_UART2_BASE;
	moxa_serial_ports[1].irq	= IRQ_UART;
	moxa_serial_ports[1].flags	= UPF_SKIP_TEST|UPF_SHARE_IRQ;
	moxa_serial_ports[1].iotype	= UPIO_PORT;
	moxa_serial_ports[1].regshift	= 2;
	moxa_serial_ports[1].uartclk	= CONFIG_UART_CLK;
	moxa_serial_ports[1].line	= 1;
	moxa_serial_ports[1].type	= PORT_16550A;
	moxa_serial_ports[1].fifosize	= 16;

	early_serial_setup(&moxa_serial_ports[0]);
	early_serial_setup(&moxa_serial_ports[1]);
#endif
}

extern struct sys_timer	moxa_timer;

#ifdef CONFIG_ARCH_UC7110
MACHINE_START(MOXART, "UC7110")
	.fixup = fixup_uc711x,
	.init_irq = irq_init_irq,
	.init_machine = uc711x_init,
	.timer = &moxa_timer,
MACHINE_END
#endif

#ifdef CONFIG_ARCH_UC7112
MACHINE_START(MOXART, "UC7112")
	.fixup = fixup_uc711x,
	.init_irq = irq_init_irq,
	.init_machine = uc711x_init,
	.timer = &moxa_timer,
MACHINE_END
#endif

#ifdef CONFIG_ARCH_EM1220
MACHINE_START(MOXART, "EM1220")
	.fixup = fixup_uc711x,
	.init_irq = irq_init_irq,
	.timer = &moxa_timer,
MACHINE_END
#endif

#ifdef CONFIG_ARCH_EM1220_DLIN
MACHINE_START(MOXART, "EM1220 DLIN")
	.fixup = fixup_uc711x,
	.init_irq = irq_init_irq,
	.timer = &moxa_timer,
MACHINE_END
#endif

#ifdef CONFIG_ARCH_EM1220_APIT
MACHINE_START(MOXART, "EM1220 APIT")
	.fixup = fixup_uc711x,
	.init_irq = irq_init_irq,
	.timer = &moxa_timer,
MACHINE_END
#endif

#ifdef CONFIG_ARCH_EM1240
MACHINE_START(MOXART, "EM1240")
	.fixup = fixup_uc711x,
	.init_irq = irq_init_irq,
	.timer = &moxa_timer,
MACHINE_END
#endif

#ifdef CONFIG_ARCH_EM1240_IVTC
MACHINE_START(MOXART, "EM1240 IVTC")
	.fixup = fixup_uc711x,
	.init_irq = irq_init_irq,
	.timer = &moxa_timer,
MACHINE_END
#endif

#ifdef CONFIG_ARCH_EM1240_MT
MACHINE_START(MOXART, "EM1240 MT")
	.fixup = fixup_uc711x,
	.init_irq = irq_init_irq,
	.timer = &moxa_timer,
MACHINE_END
#endif

#ifdef CONFIG_ARCH_W321_GL2
MACHINE_START(MOXART, "W321 GL2")
	.fixup = fixup_uc711x,
	.init_irq = irq_init_irq,
	.timer = &moxa_timer,
MACHINE_END
#endif

#ifdef CONFIG_ARCH_W315_EC
MACHINE_START(MOXART, "W315_EC")
	.fixup = fixup_uc711x,
	.init_irq = irq_init_irq,
	.timer = &moxa_timer,
MACHINE_END
#endif

#ifdef CONFIG_ARCH_UC7101
MACHINE_START(MOXART, "UC7101")
	.fixup = fixup_uc711x,
	.init_irq = irq_init_irq,
	.timer = &moxa_timer,
MACHINE_END
#endif

#ifdef CONFIG_ARCH_W311_TEST
MACHINE_START(MOXART, "W311_TEST")
	.fixup = fixup_uc711x,
	.init_irq = irq_init_irq,
	.timer = &moxa_timer,
MACHINE_END
#endif

#ifdef CONFIG_ARCH_EM1110
MACHINE_START(MOXART, "EM1110")
	.fixup = fixup_uc711x,
	.init_irq = irq_init_irq,
	.timer = &moxa_timer,
MACHINE_END
#endif
