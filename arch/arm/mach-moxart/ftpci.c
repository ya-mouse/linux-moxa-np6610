/* 
    ftpci.h 
    maintened by ivan wang 2004/8/18 11:25 
*/

#include <linux/config.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/ptrace.h>
#include <linux/slab.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>
#include <linux/init.h>
#include <asm/hardware.h>
#include <asm/irq.h>
#include <asm/system.h>
#include <asm/mach/pci.h>
#include <asm/sizes.h>
#include <asm/arch/moxa.h>
#include <asm/arch/irq.h>
#include <asm/arch/ftpci.h>


#ifdef CONFIG_CPE120_PLATFORM
#include <asm/arch/cpe/cpe_ext_int.h>
#endif

#define DEBUG_FTPCI			1

#define CONFIG_CMD(bus, device_fn, where)   (0x80000000 | (bus << 16) | (device_fn << 8) | (where & ~3) )
static spinlock_t ftpci_lock;
struct pci_dev *pci_bridge=NULL;
static unsigned int pci_config_addr;
static unsigned int pci_config_data;
int ftpci_probed=0;

static int ftpci_read_config_byte(struct pci_dev *dev, int where, u8 *val)
{
	unsigned long   flags;
	u32             v;
	unsigned int    shift;

	spin_lock_irqsave(&ftpci_lock, flags);
    *(volatile unsigned int *)pci_config_addr=CONFIG_CMD(dev->bus->number,dev->devfn,where);	
    v=*(volatile unsigned int *)pci_config_data;
	spin_unlock_irqrestore(&ftpci_lock, flags);
	shift = (where&0x3)*8;
	*val = (v>>shift)&0xff;
	return PCIBIOS_SUCCESSFUL;
}

static int ftpci_read_config_word(struct pci_dev *dev, int where, u16 *val)
{
	unsigned long   flags;
	u32             v;
	unsigned int    shift;

	spin_lock_irqsave(&ftpci_lock, flags);
	*(volatile unsigned int *)pci_config_addr=CONFIG_CMD(dev->bus->number,dev->devfn,where);
    v=*(volatile unsigned int *)pci_config_data;
	spin_unlock_irqrestore(&ftpci_lock, flags);
	shift = (where&0x3)*8;
	*val = (v>>shift)&0xffff;
	return PCIBIOS_SUCCESSFUL;
}

static int ftpci_read_config_dword(struct pci_dev *dev, int where, u32 *val)
{
	unsigned long   flags;
	u32             v;

	spin_lock_irqsave(&ftpci_lock, flags);
	*(volatile unsigned int *)pci_config_addr=CONFIG_CMD(dev->bus->number,dev->devfn,where);
    v=*(volatile unsigned int *)pci_config_data;
	spin_unlock_irqrestore(&ftpci_lock, flags);
	*val = v;
	return PCIBIOS_SUCCESSFUL;
}

static int ftpci_write_config_byte(struct pci_dev *dev, int where, u8 val)
{
	u32             org_val;
	unsigned long   flags;
	unsigned int    shift;

	shift = (where&0x3)*8;
	spin_lock_irqsave(&ftpci_lock, flags);
    *(volatile unsigned int *)pci_config_addr=CONFIG_CMD(dev->bus->number,dev->devfn,where);
    org_val=*(volatile unsigned int *)pci_config_data;
    org_val=(org_val&~(0xff<<shift))|((u32)val<<shift);
    *(volatile unsigned int *)pci_config_data=org_val;
	spin_unlock_irqrestore(&ftpci_lock, flags);
	return PCIBIOS_SUCCESSFUL;
}

static int ftpci_write_config_word(struct pci_dev *dev, int where, u16 val)
{
    u32             org_val;
	unsigned long   flags;
	unsigned int    shift;

	shift = (where&0x3)*8;
	spin_lock_irqsave(&ftpci_lock, flags);
	*(volatile unsigned int *)pci_config_addr=CONFIG_CMD(dev->bus->number, dev->devfn, where);
	org_val=*(volatile unsigned int *)pci_config_data;
	org_val=(org_val&~(0xffff<<shift))|((u32)val<<shift);
	*(volatile unsigned int *)pci_config_data=org_val;
	spin_unlock_irqrestore(&ftpci_lock, flags);
	return PCIBIOS_SUCCESSFUL;
}

static int ftpci_write_config_dword(struct pci_dev *dev, int where, u32 val)
{
	unsigned long flags;
	spin_lock_irqsave(&ftpci_lock, flags);
    *(volatile unsigned int *)pci_config_addr=CONFIG_CMD(dev->bus->number, dev->devfn, where);
    *(volatile unsigned int *)pci_config_data=val;
	spin_unlock_irqrestore(&ftpci_lock, flags);
	return PCIBIOS_SUCCESSFUL;
}

static struct pci_ops ftpci_ops = {
	.read	= ftpci_read_config_byte,
	//.read_word	= ftpci_read_config_word,
//	.read_dword	= ftpci_read_config_dword,
	.write	= ftpci_write_config_byte,
//	.write_word	= ftpci_write_config_word,
//	.write_dword= ftpci_write_config_dword,
};

/* using virtual address for pci_resource_start() function*/
static struct resource pci_io = {
	.name	= "PCI io",
	.start	= (CPE_PCI_BASE+SZ_4K),
	.end	= (CPE_PCI_BASE+SZ_1M),
	.flags	= IORESOURCE_IO,
};
/* using physical address for memory resource*/
static struct resource pci_mem = {
	.name	= "PCI non-prefetchable",
	.start	= PCI_MEM_BASE,
	.end	= PCI_MEM_END,
	.flags	= IORESOURCE_MEM,
};

//int __init ftpci_setup_resource(struct resource **resource)
void __init ftpci_setup_resource(struct resource **resource)
{
	if (request_resource(&ioport_resource, &pci_io)) {
		printk(KERN_ERR "PCI: unable to allocate io region\n");
//		return -EBUSY;
	}
	if (request_resource(&iomem_resource, &pci_mem)) {
		printk(KERN_ERR "PCI: unable to allocate non-prefetchable "
		       "memory region\n");
//		return -EBUSY;
	}

	/*
	 * bus->resource[0] is the IO resource for this bus
	 * bus->resource[1] is the mem resource for this bus
	 * bus->resource[2] is the prefetch mem resource for this bus
	 */

	resource[0] = &pci_io;
	resource[1] = &pci_mem;

//	return 1;
}

inline int ftpci_get_irq(void)
{
    unsigned int     status;
    ftpci_read_config_dword(pci_bridge, 0x4c, &status);
//printk("ftpci_get_irq,status=0x%x\n",status);
    status=(status>>28);
    if(status&0x1)
        return 0;
    if(status&0x2)
        return 1;
    if(status&0x4)
        return 2;
    if(status&0x8)
        return 3;
    return -1;
}

void ftpci_clear_irq(unsigned int irq)
{
    //int             i;
	unsigned int     status;
	ftpci_read_config_dword(pci_bridge, 0x4c, &status);
    if(irq==0)
	    status=(status&0xfffffff)|((0x1)<<28);
	else if(irq==1)
	    status=(status&0xfffffff)|((0x2)<<28);
	else if(irq==2)
	    status=(status&0xfffffff)|((0x4)<<28);
	else if(irq==3)
	    status=(status&0xfffffff)|((0x8)<<28);
	ftpci_write_config_dword(pci_bridge, 0x4c, status);	
}

static int ftpci_probe(unsigned int addr_p)
{
    unsigned int *addr=(unsigned int*)addr_p;
    *(volatile unsigned int *)addr=0x80000000;
    if(*(volatile unsigned int *)addr==0x80000000)
        ftpci_probed=1;
    else
        ftpci_probed=0;
    *(volatile unsigned int *)addr=0x0;
    return ftpci_probed;
}


void __init ftpci_init(void *sysdata)
{
    u16             val;

    pci_config_addr=CPE_PCI_BASE+FTPCI_CFG_ADR_REG;
    pci_config_data=CPE_PCI_BASE+FTPCI_CFG_DATA_REG;

    if(!ftpci_probe(pci_config_addr))
        return;
        
    pci_scan_bus(0, &ftpci_ops, sysdata);
    pci_bridge=pci_find_device(PCI_BRIDGE_VENID,PCI_BRIDGE_DEVID,NULL);
    if (pci_bridge == NULL)
        return;

    spin_lock_init(&ftpci_lock);
        
	// Enable the Interrupt Mask (INTA/INTB/INTC/INTD)
    ftpci_read_config_word(pci_bridge,PCI_INT_MASK,&val);
    val|=(PCI_INTA_ENABLE|PCI_INTB_ENABLE|PCI_INTC_ENABLE|PCI_INTD_ENABLE);
    ftpci_write_config_word(pci_bridge,PCI_INT_MASK,val);
	
	// Write DMA Start Address/Size Data to the Bridge configuration space 
	ftpci_write_config_dword(pci_bridge, PCI_MEM_BASE_SIZE1, FTPCI_BASE_ADR_SIZE_1GB);
}

/*
 * This routine handles multiple bridges.
 */
static u8 __init cpe_swizzle(struct pci_dev *dev, u8 *pinp)
{
	return 0;
}

static int __init cpe_map_irq(struct pci_dev *dev, u8 slot, u8 pin)
{
    //printk("cpe_map_irq,slot=%d pin=%d\n",PCI_SLOT(dev->devfn),pin);
    switch(PCI_SLOT(dev->devfn))
    {
        case 8:
            return VIRQ_PCI_A;
        case 9:
            return VIRQ_PCI_B;
        case 10:
            return VIRQ_PCI_C;
        case 11:
            return VIRQ_PCI_D;
        default:
            //printk("Not Support Slot %d\n",slot);
            break;
    }
    return -1;
}

struct hw_pci cpe_pci __initdata = {
//	.setup_resources	= ftpci_setup_resource,
//	.init			= ftpci_init,
	.swizzle		= cpe_swizzle,
	.map_irq		= cpe_map_irq,
};
