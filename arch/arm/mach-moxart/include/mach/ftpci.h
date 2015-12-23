#ifndef ARCH_CPE_FTPCI_H
#define ARCH_CPE_FTPCI_H
#include <asm/sizes.h>
#include <asm/arch/moxa.h>
// --------------------------------------------------------------------
//		AHB Control Register
// --------------------------------------------------------------------
#define FTPCI_IOSIZE_REG			0x0
#define FTPCI_PROT_REG				0x4
#define	FTPCI_CTRL_REG				0x8
#define FTPCI_ERREN_REG				0xc
#define FTPCI_SOFTRST_REG			0x10
#define FTPCI_EN64_REG				0x14
#define FTPCI_ADDRH32_REG			0x18
#define FTPCI_CFG_ADR_REG			0x28
#define FTPCI_CFG_DATA_REG			0x2c


	// --------------------------------------------------------------------
	//		FTPCI_IOSIZE_REG 的內容
	// --------------------------------------------------------------------
#define FTPCI_BASE_IO_SIZE_1M		0x0
#define FTPCI_BASE_IO_SIZE_2M		0x1
#define FTPCI_BASE_IO_SIZE_4M		0x2
#define FTPCI_BASE_IO_SIZE_8M		0x3
#define FTPCI_BASE_IO_SIZE_16M		0x4
#define FTPCI_BASE_IO_SIZE_32M		0x5
#define FTPCI_BASE_IO_SIZE_64M		0x6
#define FTPCI_BASE_IO_SIZE_128M		0x7
#define FTPCI_BASE_IO_SIZE_256M		0x8
#define FTPCI_BASE_IO_SIZE_512M		0x9
#define FTPCI_BASE_IO_SIZE_1G		0xa
#define FTPCI_BASE_IO_SIZE_2G		0xb



// --------------------------------------------------------------------
//		PCI Configuration Register
// --------------------------------------------------------------------
#define PCI_INT_MASK				0x4e
#define PCI_MEM_BASE_SIZE1			0x50
#define PCI_MEM_BASE_SIZE2			0x54
#define PCI_MEM_BASE_SIZE3			0x58


	// --------------------------------------------------------------------
	//		PCI_INT_MASK 的內容
	// --------------------------------------------------------------------
#define PCI_INTA_ENABLE				(1U<<6)
#define PCI_INTB_ENABLE				(1U<<7)
#define PCI_INTC_ENABLE				(1U<<8)
#define PCI_INTD_ENABLE				(1U<<9)
	

	// --------------------------------------------------------------------
	//		PCI_MEM_BASE_SIZE1... 的內容
	// --------------------------------------------------------------------
#define FTPCI_BASE_ADR_SIZE_1MB		(0x0<<16)
#define FTPCI_BASE_ADR_SIZE_2MB		(0x1<<16)
#define FTPCI_BASE_ADR_SIZE_4MB		(0x2<<16)
#define FTPCI_BASE_ADR_SIZE_8MB		(0x3<<16)
#define FTPCI_BASE_ADR_SIZE_16MB	(0x4<<16)
#define FTPCI_BASE_ADR_SIZE_32MB	(0x5<<16)
#define FTPCI_BASE_ADR_SIZE_64MB	(0x6<<16)
#define FTPCI_BASE_ADR_SIZE_128MB	(0x7<<16)
#define FTPCI_BASE_ADR_SIZE_256MB	(0x8<<16)
#define FTPCI_BASE_ADR_SIZE_512MB	(0x9<<16)
#define FTPCI_BASE_ADR_SIZE_1GB		(0xa<<16)
#define FTPCI_BASE_ADR_SIZE_2GB		(0xb<<16)

void ftpci_clear_irq(unsigned int irq);
inline int ftpci_get_irq(void);
extern int ftpci_probed;
#endif
