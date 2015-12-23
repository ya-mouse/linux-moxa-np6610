/*
 * 	UC-7110-LX Version 2 Flash mapping driver
 */

#include <mach/moxa.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/ioport.h>

#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/partitions.h>

#include <asm/io.h>

#define WINDOW_ADDR CONFIG_FLASH_MEM_BASE
#define WINDOW_SIZE CONFIG_FLASH_SIZE
#define UC7110_FLASH_SIZE 0x01000000

#define RW_PART0_OF	0x0
#define RW_PART0_SZ	0x40000		/* Bootloader */
#define RW_PART1_OF	(RW_PART0_OF+RW_PART0_SZ+0x100000)
#define RW_PART1_SZ	0xc0000	/* MoxaKernel */
#define RW_PART2_OF	(RW_PART1_OF+RW_PART1_SZ)
#define RW_PART2_SZ	0x220000	/* Linux */
#define RW_PART3_OF	(RW_PART2_OF+RW_PART2_SZ)
#define RW_PART3_SZ	0x200000	/* user disk - JFFS2 */
#define RW_PART4_OF	(RW_PART3_OF+RW_PART3_SZ)
#define RW_PART4_SZ	(UC7110_FLASH_SIZE-RW_PART4_OF)	/* user disk - Ext2 */

static struct mtd_partition moxart_flash_partitions[] = {
	{
		.name = "Bootloader",
		.offset = RW_PART0_OF,
		.size = RW_PART0_SZ
	},
	{
		.name = "MoxaKernel",
		.offset = RW_PART1_OF,
		.size = RW_PART1_SZ,
		//.mask_flags = MTD_WRITEABLE
	},
	{
		.name = "LinuxKernel",
		.offset = RW_PART2_OF,
		.size = RW_PART2_SZ,
		//.mask_flags = MTD_WRITEABLE
	},
	{
		.name = "rootfs2m",
		.offset = RW_PART3_OF,
		.size = RW_PART3_SZ,
		//.mask_flags = MTD_WRITEABLE
	},
	{
		.name = "rootfs",
		.offset = RW_PART4_OF,
		.size = RW_PART4_SZ,
		//.mask_flags = MTD_WRITEABLE
	}
};
struct map_info moxart_flash_map = {
	.name = "UC7112 FLASH",
	.size = WINDOW_SIZE,
	.bankwidth = 2,
	.phys = WINDOW_ADDR,
	.virt = IO_ADDRESS(WINDOW_ADDR),
};


#define NUM_MOXART_FLASH_PARTITIONS ARRAY_SIZE(moxart_flash_partitions)

static struct mtd_info *moxart_mtd;

int __init init_moxart_flash(void)
{
	printk(KERN_NOTICE "moxart: flash mapping: 0x%x at 0x%x\n",
			WINDOW_SIZE, WINDOW_ADDR);

	simple_map_init(&moxart_flash_map);

	moxart_mtd = do_map_probe("cfi_probe",&moxart_flash_map);

	if (moxart_mtd) {
		moxart_mtd->owner = THIS_MODULE;
		return add_mtd_partitions(moxart_mtd,
				moxart_flash_partitions,
				NUM_MOXART_FLASH_PARTITIONS);
	}

	return -ENXIO;
}

static void __exit cleanup_moxart_flash(void)
{
	if (moxart_mtd) {
		del_mtd_partitions(moxart_mtd);
		/* moved iounmap after map_destroy - armin */
		map_destroy(moxart_mtd);
	}
}

module_init(init_moxart_flash);
module_exit(cleanup_moxart_flash);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jimmy_chen@moxa.com.tw");
MODULE_DESCRIPTION("MTD map driver for the UC-7112");
