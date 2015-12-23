
/*
 * This is Moxa CPU DMA (include APB & AHB) define file.
 * Copyright (C) 2005 Moxa Group All Rights Reserved.
 *
 * History:
 *	Date		Author			Comment
 *	12-01-2005	Victor Yu.		Create it.
 */
#ifndef __ASM_ARCH_DMA_H
#define __ASM_ARCH_DMA_H

#define MAX_DMA_ADDRESS			0xffffffff
#define MAX_DMA_CHANNELS    		0

#define APB_DMA_MAX_CHANNEL		4
#define AHB_DMA_MAX_CHANNEL		8
#define TOTAL_DMA_MAX_CHANNEL		(APB_DMA_MAX_CHANNEL+AHB_DMA_MAX_CHANNEL)

// following define for APB DMA register struct
union _command {
	unsigned int	ul;
#define APB_DMA_ENABLE			(1<<0)
#define APB_DMA_FIN_INT_STS		(1<<1)
#define APB_DMA_FIN_INT_EN		(1<<2)
#define APB_DMA_BURST_MODE		(1<<3)
#define APB_DMA_ERR_INT_STS		(1<<4)
#define APB_DMA_ERR_INT_EN		(1<<5)
#define APB_DMA_SOURCE_AHB		(1<<6)
#define APB_DMA_SOURCE_APB		0
#define APB_DMA_DEST_AHB		(1<<7)
#define APB_DMA_DEST_APB		0
#define APB_DMA_SOURCE_INC_0		0
#define APB_DMA_SOURCE_INC_1_4		(1<<8)
#define APB_DMA_SOURCE_INC_2_8		(2<<8)
#define APB_DMA_SOURCE_INC_4_16		(3<<8)
#define APB_DMA_SOURCE_DEC_1_4		(5<<8)
#define APB_DMA_SOURCE_DEC_2_8		(6<<8)
#define APB_DMA_SOURCE_DEC_4_16		(7<<8)
#define APB_DMA_SOURCE_INC_MASK		(7<<8)
#define APB_DMA_DEST_INC_0		0
#define APB_DMA_DEST_INC_1_4		(1<<12)
#define APB_DMA_DEST_INC_2_8		(2<<12)
#define APB_DMA_DEST_INC_4_16		(3<<12)
#define APB_DMA_DEST_DEC_1_4		(5<<12)
#define APB_DMA_DEST_DEC_2_8		(6<<12)
#define APB_DMA_DEST_DEC_4_16		(7<<12)
#define APB_DMA_DEST_INC_MASK		(7<<12)
#define APB_DMA_DEST_REQ_NO_MASK	(15<<16)
#define APB_DMA_DATA_WIDTH_MASK		(3<<20)
#define APB_DMA_DATA_WIDTH_4		0
#define APB_DMA_DATA_WIDTH_2		(1<<20)
#define APB_DMA_DATA_WIDTH_1		(2<<20)
#define APB_DMA_SOURCE_REQ_NO_MASK	(15<<24)
	struct _bits {
		unsigned int	enable:1;	// enable DMA
#define APB_DMAB_ENABLE			1
		unsigned int	fin_int_sts:1;	// finished interrupt status
#define APB_DMAB_FIN_INT_STS		1
		unsigned int	fin_int_en:1;	// finished interrupt enable
#define APB_DMAB_FIN_INT_EN		1
		unsigned int	burst:1;	// burst mode
#define APB_DMAB_BURST_MODE		1
		unsigned int	err_int_sts:1;	// error interrupt status
#define APB_DMAB_ERR_INT_STS		1
		unsigned int	err_int_en:1;	// error interrupt enable
#define APB_DMAB_ERR_INT_EN		1
		unsigned int	source_sel:1;	// 0:APB (device),1:AHB (RAM)
#define APB_DMAB_SOURCE_AHB		1
#define APB_DMAB_SOURCE_APB		0
		unsigned int	dest_sel:1;	// 0:APB,1:AHB
#define APB_DMAB_DEST_AHB		1
#define APB_DMAB_DEST_APB		0
		unsigned int	source_inc:3;	// 000:no increment
						// 001:+1(busrt=0),+4(burst=1)
						// 010:+2(burst=0),+8(burst=1)
						// 011:+4(burst=0),+16(burst=1)
						// 101:-1(burst=0),-4(burst=1)
						// 110:-2(burst=0),-8(burst=1)
						// 111:-4(burst=0),-16(burst=1)
#define APB_DMAB_SOURCE_INC_0		0
#define APB_DMAB_SOURCE_INC_1_4		1
#define APB_DMAB_SOURCE_INC_2_8		2
#define APB_DMAB_SOURCE_INC_4_16	3
#define APB_DMAB_SOURCE_DEC_1_4		5
#define APB_DMAB_SOURCE_DEC_2_8		6
#define APB_DMAB_SOURCE_DEC_4_16	7
#define APB_DMAB_SOURCE_INC_MASK	7
		unsigned int	reserved1:1;
		unsigned int	dest_inc:3;	// 000:no increment
						// 001:+1(busrt=0),+4(burst=1)
						// 010:+2(burst=0),+8(burst=1)
						// 011:+4(burst=0),+16(burst=1)
						// 101:-1(burst=0),-4(burst=1)
						// 110:-2 (burst=0),-8(burst=1)
						// 111:-4(burst=0),-16(burst=1)
#define APB_DMAB_DEST_INC_0		0
#define APB_DMAB_DEST_INC_1_4		1
#define APB_DMAB_DEST_INC_2_8		2
#define APB_DMAB_DEST_INC_4_16		3
#define APB_DMAB_DEST_DEC_1_4		5
#define APB_DMAB_DEST_DEC_2_8		6
#define APB_DMAB_DEST_DEC_4_16		7
#define APB_DMAB_DEST_INC_MASK		7
		unsigned int	reserved2:1;
		unsigned int	dest_req_no:4;	// request signal select of dest
						// addr for DMA hwd handshake
						// 0:no request/grant signal
						// 1-15:request/grant signal
#define APB_DMAB_DEST_REQ_NO_MASK	15
		unsigned int	data_width:2;	// data width of transfer
						// 00:word, 01:half, 10:byte
#define APB_DMAB_DATA_WIDTH_MASK	3
#define APB_DMAB_DATA_WIDTH_4		0
#define APB_DMAB_DATA_WIDTH_2		1
#define APB_DMAB_DATA_WIDTH_1		2
		unsigned int	reserved3:2;
		unsigned int	source_req_no:4;// request signal select of dest
						// addr for DMA hwd handshake
						// 0:no request/grant signal
						// 1-15:request/grant signal
#define APB_DMAB_SOURCE_REQ_NO_MASK	15
		unsigned int	reserved4:4;
	} bits;
};

typedef struct _apb_dma_reg {
	unsigned int	source_addr;
	unsigned int	dest_addr;
	unsigned int	cycles;	// is depended on burst mode
#define APB_DMA_CYCLES_MASK	0x00ffffff
	union _command	command;
} apb_dma_reg;

#define APB_DMA_SPI_TX_REQ_NO		1
#define APB_DMA_SPI_RX_REQ_NO		2
#define APB_DMA_SD_REQ_NO		5
#define APB_DMA_AC97_TX_REQ_NO		6
#define APB_DMA_AC97_RX_REQ_NO		7
#define APB_DMA_USB_DEVICE_REQ_NO	9

// following APB DMA private data struct define
typedef struct _apb_dma_priv {
	apb_dma_reg	*reg;
	void		(*irq_handler)(void *param);
	void		*irq_handler_param;
	int		used_flag;
	int		error_flag;
	int		req_no;
} apb_dma_priv;

typedef struct _apb_dma_conf_param {
	void		*source_addr;	// need physical address
	void		*dest_addr;	// need physical address
	unsigned int	size;		// total bytes number
	int		data_width;
	int		dest_inc;
	int		source_inc;
	int		dest_sel;	// APB or AHB
	int		source_sel;	// APB or AHB
	int		burst_mode;
} apb_dma_conf_param;

// following APB DMA function call phototype define
extern apb_dma_priv *apb_dma_alloc(int req_no);
extern void apb_dma_release(apb_dma_priv *priv);
extern void apb_dma_set_irq(apb_dma_priv *priv, void (*func)(void *param), void *param);
extern void apb_dma_release_irq(apb_dma_priv *priv);
extern void apb_dma_conf(apb_dma_priv *priv, apb_dma_conf_param *param);
extern void apb_dma_enable(apb_dma_priv *priv);
extern void apb_dma_disable(apb_dma_priv *priv);

#endif /* _ASM_ARCH_DMA_H */
