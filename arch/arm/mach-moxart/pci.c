/* 
    ftpci.h 
    maintened by ivan wang 2004/8/18 11:25 
    Modified by Luke Lee 03/21/2005
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
#include <asm/arch/ftpci.h>
#include <asm/mach-types.h>  // Luke Lee 03/23/2005 ins 1

#ifdef CONFIG_PCI
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

// Luke Lee 03/21/2005 mod begin
static int ftpci_write_config_dlocal(struct pci_bus *bus, unsigned int devfn, int where, u32 val)
{
	unsigned long flags;
	spin_lock_irqsave(&ftpci_lock, flags);
	*(volatile unsigned int *)pci_config_addr=CONFIG_CMD(0, 0, where);
	*(volatile unsigned int *)pci_config_data=val;
	spin_unlock_irqrestore(&ftpci_lock, flags);
	return PCIBIOS_SUCCESSFUL;
}
static int ftpci_write_config_wlocal(struct pci_bus *bus, unsigned int devfn, int where, u16 val)
{
        u32             org_val;
	unsigned long   flags;
	unsigned int    shift;

	shift = (where&0x3)*8;
	spin_lock_irqsave(&ftpci_lock, flags);
	*(volatile unsigned int *)pci_config_addr=CONFIG_CMD(0,0 , where);
	org_val=*(volatile unsigned int *)pci_config_data;
	org_val=(org_val&~(0xffff<<shift))|((u32)val<<shift);
	*(volatile unsigned int *)pci_config_data=org_val;
	spin_unlock_irqrestore(&ftpci_lock, flags);
	return PCIBIOS_SUCCESSFUL;
}
static int ftpci_read_config_wlocal(struct pci_bus *bus, unsigned int devfn, int where, u16 *val)
{
	unsigned long   flags;
	u32             v;
	unsigned int    shift;

	spin_lock_irqsave(&ftpci_lock, flags);
	*(volatile unsigned int *)pci_config_addr=CONFIG_CMD(0,0,where);
	v=*(volatile unsigned int *)pci_config_data;
	spin_unlock_irqrestore(&ftpci_lock, flags);
	shift = (where&0x3)*8;
	*val = (v>>shift)&0xffff;
	return PCIBIOS_SUCCESSFUL;
}
static int ftpci_read_config_byte(struct pci_bus *bus, unsigned int devfn, int where, u8 *val)
{
	unsigned long   flags;
	u32             v;
	unsigned int    shift;

	spin_lock_irqsave(&ftpci_lock, flags);
	*(volatile unsigned int *)pci_config_addr=CONFIG_CMD(bus->number,devfn,where);	
	v=*(volatile unsigned int *)pci_config_data;
	spin_unlock_irqrestore(&ftpci_lock, flags);
	shift = (where&0x3)*8;
	*val = (v>>shift)&0xff;
	return PCIBIOS_SUCCESSFUL;
}

static int ftpci_read_config_word(struct pci_bus *bus, unsigned int devfn, int where, u16 *val)
{
	unsigned long   flags;
	u32             v;
	unsigned int    shift;

	spin_lock_irqsave(&ftpci_lock, flags);
	*(volatile unsigned int *)pci_config_addr=CONFIG_CMD(bus->number,devfn,where);
	v=*(volatile unsigned int *)pci_config_data;
	spin_unlock_irqrestore(&ftpci_lock, flags);
	shift = (where&0x3)*8;
	*val = (v>>shift)&0xffff;
	return PCIBIOS_SUCCESSFUL;
}

static int ftpci_read_config_dword(struct pci_bus *bus, unsigned int devfn, int where, u32 *val)
{
	unsigned long   flags;
	u32             v;

	spin_lock_irqsave(&ftpci_lock, flags);
	*(volatile unsigned int *)pci_config_addr=CONFIG_CMD(bus->number,devfn,where);
	v=*(volatile unsigned int *)pci_config_data;
	spin_unlock_irqrestore(&ftpci_lock, flags);
	*val = v;
	return PCIBIOS_SUCCESSFUL;
}

static int ftpci_write_config_byte(struct pci_bus *bus, unsigned int devfn, int where, u8 val)
{
	u32             org_val;
	unsigned long   flags;
	unsigned int    shift;

	shift = (where&0x3)*8;
	spin_lock_irqsave(&ftpci_lock, flags);
	*(volatile unsigned int *)pci_config_addr=CONFIG_CMD(bus->number,devfn,where);
	org_val=*(volatile unsigned int *)pci_config_data;
	org_val=(org_val&~(0xff<<shift))|((u32)val<<shift);
	*(volatile unsigned int *)pci_config_data=org_val;
	spin_unlock_irqrestore(&ftpci_lock, flags);
	return PCIBIOS_SUCCESSFUL;
}

static int ftpci_write_config_word(struct pci_bus *bus, unsigned int devfn, int where, u16 val)
{
        u32             org_val;
	unsigned long   flags;
	unsigned int    shift;

	shift = (where&0x3)*8;
	spin_lock_irqsave(&ftpci_lock, flags);
	*(volatile unsigned int *)pci_config_addr=CONFIG_CMD(bus->number, devfn, where);
	org_val=*(volatile unsigned int *)pci_config_data;
	org_val=(org_val&~(0xffff<<shift))|((u32)val<<shift);
	*(volatile unsigned int *)pci_config_data=org_val;
	spin_unlock_irqrestore(&ftpci_lock, flags);
	return PCIBIOS_SUCCESSFUL;
}

static int ftpci_write_config_dword(struct pci_bus *bus, unsigned int devfn, int where, u32 val)
{
	unsigned long flags;
	spin_lock_irqsave(&ftpci_lock, flags);
	*(volatile unsigned int *)pci_config_addr=CONFIG_CMD(bus->number, devfn, where);
	*(volatile unsigned int *)pci_config_data=val;
	spin_unlock_irqrestore(&ftpci_lock, flags);
	return PCIBIOS_SUCCESSFUL;
}
// Luke Lee 03/21/2005 mod end

// Luke Lee 03/21/2005 ins begin
static int ftpci_read_config(struct pci_bus *bus, unsigned int devfn, int where,
			  int size, u32 *val)
{
        int r; 
	switch (size) {
	case 1:
	        r = ftpci_read_config_byte(bus,devfn,where,(u8*)val); // Luke Lee TOFIX 03/22/2005 : convert to (u8*) -- beware of endian !
		break;

	case 2:
		r = ftpci_read_config_word(bus,devfn,where,(u16*)val); // Luke Lee TOFIX 03/22/2005 : convert to (u16*) -- beware of endian !
		break;

	default:
		r = ftpci_read_config_dword(bus,devfn,where,val);
		break;
	}

	return r;
}

static int ftpci_write_config(struct pci_bus *bus, unsigned int devfn, int where,
			      int size, u32 val)
{
        int r; 
	switch (size) {
	case 1:
 	        r = ftpci_write_config_byte(bus, devfn, where, val);
		break;

	case 2:
 	        r = ftpci_write_config_word(bus, devfn, where, val);
		break;

	case 4:
 	        r = ftpci_write_config_dword(bus, devfn, where, val);
		break;
	default:
	        printk( "Invalid size for ftpci_write()\n" );
		r = PCIBIOS_FUNC_NOT_SUPPORTED;  // Luke Lee 03/23/2005 ins 1
	}

	return r;
}

// Luke Lee 03/21/2005 ins end

static struct pci_ops ftpci_ops = {
        // Luke Lee 03/21/2005 mod begin
	.read  = ftpci_read_config,
	.write = ftpci_write_config,
	// Luke Lee 03/21/2005 mod end
};

/* using virtual address for pci_resource_start() function*/
static struct resource pci_io = {
	.name	= "PCI io",
	.start	= PCI_IO_VA_BASE,
	.end	= PCI_IO_VA_END,
	.flags	= IORESOURCE_IO,
};
/* using physical address for memory resource*/
static struct resource pci_mem = {
	.name	= "PCI non-prefetchable",
	.start	= PCI_MEM_BASE,
	.end	= PCI_MEM_END,
	.flags	= IORESOURCE_MEM,
};

// Luke Lee 03/23/2005 unrem 1 rem 1
int __init ftpci_setup_resource(struct resource **resource)
//void __init ftpci_setup_resource(struct resource **resource)
{
	if (request_resource(&ioport_resource, &pci_io)) {
		printk(KERN_ERR "PCI: unable to allocate io region\n");
		return -EBUSY;  // Luke Lee 03/23/2005 unrem 1
	}
	if (request_resource(&iomem_resource, &pci_mem)) {
		printk(KERN_ERR "PCI: unable to allocate non-prefetchable "
		       "memory region\n");
		return -EBUSY;  // Luke Lee 03/23/2005 unrem 1
	}

	/*
	 * bus->resource[0] is the IO resource for this bus
	 * bus->resource[1] is the mem resource for this bus
	 * bus->resource[2] is the prefetch mem resource for this bus
	 */

	resource[0] = &pci_io;
	resource[1] = &pci_mem;

	return 1;  // Luke Lee 03/23/2005 unrem 1
}

inline int ftpci_get_irq(void)
{
    unsigned int     status;
    ftpci_read_config_dword(pci_bridge->bus, pci_bridge->devfn, 0x4c, &status); // Luke Lee 03/22/2005 mod 1
printk("ftpci_get_irq,status=0x%x\n",status);
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
     ftpci_read_config_dword(pci_bridge->bus, pci_bridge->devfn, 0x4c, &status); // Luke Lee 03/22/2005 mod 1
    if(irq==0)
       status=(status&0xfffffff)|((0x1)<<28);
    else if(irq==1)
       status=(status&0xfffffff)|((0x2)<<28);
    else if(irq==2)
       status=(status&0xfffffff)|((0x4)<<28);
    else if(irq==3)
       status=(status&0xfffffff)|((0x8)<<28);
    ftpci_write_config_dword(pci_bridge->bus, pci_bridge->devfn, 0x4c, status);	// Luke Lee 03/22/2005 mod 1
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


void __init ftpci_preinit(void /**sysdata*/) // Luke Lee 03/22/2005 mod 1
{
    u16             val;
    printk( "ftpci_init()\n\r" ); // Luke Lee 03/22/2005 ins 1
    pci_config_addr=CPE_PCI_VA_BASE+FTPCI_CFG_ADR_REG;
    pci_config_data=CPE_PCI_VA_BASE+FTPCI_CFG_DATA_REG;
#if 0
    if(!ftpci_probe(pci_config_addr))
        return;
        
    //pci_scan_bus(0, &ftpci_ops, sysdata); // Luke Lee 03/22/2005 rem 1
    pci_bridge=pci_find_device(PCI_BRIDGE_VENID,PCI_BRIDGE_DEVID,NULL);
    if (pci_bridge == NULL)
        return;
#endif
    spin_lock_init(&ftpci_lock);
        
    // Enable the Interrupt Mask (INTA/INTB/INTC/INTD)
    ftpci_read_config_wlocal(pci_bridge->bus,pci_bridge->devfn,PCI_INT_MASK,&val); // Luke Lee 03/22/2005 mod 1
    val|=(PCI_INTA_ENABLE|PCI_INTB_ENABLE|PCI_INTC_ENABLE|PCI_INTD_ENABLE);
    ftpci_write_config_wlocal(pci_bridge->bus,pci_bridge->devfn,PCI_INT_MASK,val);// Luke Lee 03/22/2005 mod 1
	
    // Write DMA Start Address/Size Data to the Bridge configuration space 
    ftpci_write_config_dlocal(pci_bridge->bus, pci_bridge->devfn, PCI_MEM_BASE_SIZE1, FTPCI_BASE_ADR_SIZE_1GB); // Luke Lee 03/22/2005 mod 1
}

/*
 * This routine handles multiple bridges.
 */
static u8 __init cpe_swizzle(struct pci_dev *dev, u8 *pinp)
{
        printk( "cpe_swizzle(%X,%X)\n\r",(unsigned)dev,(unsigned)pinp ); // Luke Lee 03/22/2005 ins 1
	return 0;
}

static int __init cpe_map_irq(struct pci_dev *dev, u8 slot, u8 pin)
{
    printk("cpe_map_irq,slot=%d pin=%d\n",PCI_SLOT(dev->devfn),pin);
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

#if 0 // Luke Lee 03/22/2005 rem block
struct hw_pci cpe_pci __initdata = {
	.setup_resources	= ftpci_setup_resource,
	.init			= ftpci_init,
	.swizzle		= cpe_swizzle,
	.map_irq		= cpe_map_irq,
};
#endif

// Luke Lee 03/22/2005 ins begin

int __init ftpci_setup(int nr, struct pci_sys_data *sys)
{
	int ret = 0;

	if (nr == 0) {
    	        sys->mem_offset = CPE_PCI_MEM;  // Luke Lee TOFIX : 03/22/2005 : or CPE_PCI_BASE  ???
		ret = ftpci_setup_resource(sys->resource);
	}
	return ret;
}

static struct pci_bus *ftpci_scan_bus(int nr, struct pci_sys_data *sys)
{
	return pci_scan_bus(sys->busnr, &ftpci_ops, sys);
}

static struct hw_pci cpe_pci  __initdata = {
	.swizzle		= cpe_swizzle,
	.map_irq		= cpe_map_irq,
	.setup			= ftpci_setup,
	.nr_controllers		= 1,
	.scan			= ftpci_scan_bus,
	// TOFIX
	.preinit		= ftpci_preinit,
	//.postinit		= pci_v3_postinit, // no post init function
};

static int __init a320_pci_init(void)
{
       	pci_common_init(&cpe_pci);
	return 0;
}

subsys_initcall(a320_pci_init);

// Luke Lee 03/22/2005 ins begin
#endif //CONFIG_PCI
