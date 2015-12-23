
/*
 * Copyright (C) 2005 Moxa Group All Rights Reserved.
 *
 * History:
 *	Date		Author			Comment
 *	12-01-2005	Victor Yu.		Create it.
 */
#include	<linux/version.h>
#include	<mach/moxa.h>
#include	<linux/module.h>
#include	<linux/init.h>
#include	<linux/interrupt.h>
#include	<linux/dma-mapping.h>
#include	<linux/mm.h>
#include	<linux/slab.h>
#include	<mach/cpe_int.h>
#include	<mach/dma.h>
#include	<asm/cacheflush.h>
#include	<asm/irq.h>
#include	<asm/io.h>

//#define MOXA_DEBUG
#ifdef MOXA_DEBUG
#define DBG(x...)	printk(x)
#else
#define DBG(x...)
#endif

static apb_dma_priv	apb_dma_channel[APB_DMA_MAX_CHANNEL];
static spinlock_t	apb_dma_lock;

apb_dma_priv	*apb_dma_alloc(int req_no)
{
	int		i;
	unsigned long	flags;
	apb_dma_priv	*priv=apb_dma_channel;

	spin_lock_irqsave(&apb_dma_lock, flags);
	for ( i=0; i<APB_DMA_MAX_CHANNEL; i++, priv++ ) {
		if ( priv->used_flag == 0 ) {
			priv->used_flag = 1;
			priv->irq_handler = NULL;
			priv->irq_handler_param = NULL;
			priv->error_flag = 0;
			priv->req_no = req_no;
			switch ( req_no ) {
			case APB_DMA_SPI_TX_REQ_NO :
				*(unsigned int *)(CPE_PMU_BASE+0xA0) = 0;
				break;
			case APB_DMA_SPI_RX_REQ_NO :
				*(unsigned int *)(CPE_PMU_BASE+0xA4) = 0;
				break;
			case APB_DMA_SD_REQ_NO :
				*(unsigned int *)(CPE_PMU_BASE+0xB8) = 0;
				break;
			case APB_DMA_AC97_TX_REQ_NO :
				*(unsigned int *)(CPE_PMU_BASE+0xBC) = 0;
				break;
			case APB_DMA_AC97_RX_REQ_NO :
				*(unsigned int *)(CPE_PMU_BASE+0xC0) = 0;
				break;
			case APB_DMA_USB_DEVICE_REQ_NO :
				*(unsigned int *)(CPE_PMU_BASE+0xCC) = 0;
				break;
			}
			spin_unlock_irqrestore(&apb_dma_lock, flags);
			DBG("apb_dma_alloc uses DMA channel %d\n", i);
			return priv;
		}
	}
	spin_unlock_irqrestore(&apb_dma_lock, flags);
	return NULL;
}
EXPORT_SYMBOL(apb_dma_alloc);

void	apb_dma_release(apb_dma_priv *priv)
{
	unsigned long	flags;

	spin_lock_irqsave(&apb_dma_lock, flags);
	if ( priv == NULL ) {
		spin_unlock_irqrestore(&apb_dma_lock, flags);
		return;
	}
	priv->used_flag = 0;
	priv->error_flag = 0;
	priv->irq_handler = NULL;
	priv->irq_handler_param = NULL;
	priv->req_no = 0;
	spin_unlock_irqrestore(&apb_dma_lock, flags);
}
EXPORT_SYMBOL(apb_dma_release);

void	apb_dma_set_irq(apb_dma_priv *priv, void (*func)(void *param), void *param)
{
	unsigned long	flags;
	unsigned int	cmd;

	spin_lock_irqsave(&apb_dma_lock, flags);
	if ( priv == NULL ) {
		spin_unlock_irqrestore(&apb_dma_lock, flags);
		return;
	}
	priv->irq_handler = func;
	priv->irq_handler_param = param;
	priv->error_flag = 0;
	cmd = readl(&priv->reg->command.ul);
	cmd |= (APB_DMA_FIN_INT_EN | APB_DMA_ERR_INT_EN);
	writel(cmd, &priv->reg->command.ul);
	spin_unlock_irqrestore(&apb_dma_lock, flags);
}
EXPORT_SYMBOL(apb_dma_set_irq);

void	apb_dma_release_irq(apb_dma_priv *priv)
{
	unsigned long	flags;
	unsigned int	cmd;

	spin_lock_irqsave(&apb_dma_lock, flags);
	if ( priv == NULL ) {
		spin_unlock_irqrestore(&apb_dma_lock, flags);
		return;
	}
	cmd = readl(&priv->reg->command.ul);
	cmd &= ~(APB_DMA_FIN_INT_EN | APB_DMA_ERR_INT_EN);
	writel(cmd, &priv->reg->command.ul);
	priv->irq_handler = NULL;
	priv->irq_handler_param = NULL;
	priv->error_flag = 0;
	spin_unlock_irqrestore(&apb_dma_lock, flags);
}
EXPORT_SYMBOL(apb_dma_release_irq);

void	apb_dma_conf(apb_dma_priv *priv, apb_dma_conf_param *param)
{
	unsigned long	flags;
	union _command	cmd;
	unsigned int	size;

	spin_lock_irqsave(&apb_dma_lock, flags);
	writel(param->source_addr, &priv->reg->source_addr);
	writel(param->dest_addr, &priv->reg->dest_addr);
	size = param->size;
	switch ( param->data_width ) {
	case APB_DMAB_DATA_WIDTH_1 :
		break;
	case APB_DMAB_DATA_WIDTH_2 :
		size >>= 1;
		break;
	case APB_DMAB_DATA_WIDTH_4 :
	default :
		size >>= 2;
		break;
	}
	if ( param->burst_mode )
		size >>= 2;
	writel(size, &priv->reg->cycles);
	cmd.ul = readl(&priv->reg->command.ul);
	cmd.bits.data_width = param->data_width;
	if ( param->dest_sel ) {	// AHB
#if 0	// add by Victor Yu. 07-12-2007
		dmac_inv_range(param->dest_addr, param->dest_addr+param->size);
#else
		dmac_map_area(param->dest_addr, param->size, DMA_FROM_DEVICE);
#endif
		cmd.bits.dest_req_no = 0;
	} else {	// APB
		cmd.bits.dest_req_no = priv->req_no;
	}
	cmd.bits.dest_sel = param->dest_sel;
	if ( param->source_sel ) {	// AHB
#if 1	// add by Victor Yu. 07-12-2007
		dmac_flush_range(param->source_addr, param->source_addr+param->size);
#endif
		cmd.bits.source_req_no = 0;
	} else {	// APB
		cmd.bits.source_req_no = priv->req_no;
	}
	cmd.bits.source_sel = param->source_sel;
	cmd.bits.burst = param->burst_mode;
	cmd.bits.dest_inc = param->dest_inc;
	cmd.bits.source_inc = param->source_inc;
	writel(cmd.ul, &priv->reg->command.ul);
	spin_unlock_irqrestore(&apb_dma_lock, flags);
}
EXPORT_SYMBOL(apb_dma_conf);

void	apb_dma_enable(apb_dma_priv *priv)
{
	unsigned long	flags;
	unsigned int	cmd;

	spin_lock_irqsave(&apb_dma_lock, flags);
	cmd = readl(&priv->reg->command.ul);
	cmd |= APB_DMA_ENABLE;
	writel(cmd, &priv->reg->command.ul);	
	spin_unlock_irqrestore(&apb_dma_lock, flags);
}
EXPORT_SYMBOL(apb_dma_enable);
EXPORT_SYMBOL(apb_dma_disable);
void	apb_dma_disable(apb_dma_priv *priv)
{
	unsigned long	flags;
	unsigned int	cmd;

	spin_lock_irqsave(&apb_dma_lock, flags);
	cmd = readl(&priv->reg->command.ul);
	cmd &= ~APB_DMA_ENABLE;
	writel(cmd, &priv->reg->command.ul);	
	spin_unlock_irqrestore(&apb_dma_lock, flags);
}

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,9)	// add by Victor Yu. 03-08-2007
static irqreturn_t apb_dma_irq(int irq, void *devid)
#else
static irqreturn_t apb_dma_irq(int irq, void *devid, struct pt_regs *regs)
#endif	// LINUX_VERSION_CODE
{
	int		i;
	unsigned int	cmd;
	apb_dma_priv	*priv=apb_dma_channel;

	DBG("apb_dma_irq test01\n");
	for ( i=0; i<APB_DMA_MAX_CHANNEL; i++, priv++ ) {
		cmd = readl(&priv->reg->command.ul);
		if ( cmd & APB_DMA_FIN_INT_STS ) {
			DBG("apb_dma_irq finish interrupt channel [%d]\n", i);
			cmd &= ~APB_DMA_FIN_INT_STS;
		}
		if ( cmd & APB_DMA_ERR_INT_STS ) {
			DBG("apb_dma_irq error interrupt channel [%d]\n", i);
			cmd &= ~APB_DMA_ERR_INT_STS;
			priv->error_flag = 1;
		}
		if ( priv->used_flag && priv->irq_handler ) {
			priv->irq_handler(priv->irq_handler_param);
		}
		priv->error_flag = 0;
		writel(cmd, &priv->reg->command.ul);
	}

	return IRQ_HANDLED;
}

static int __init	apb_dma_init(void)
{
	int		ret, i;
	apb_dma_priv	*priv=apb_dma_channel;

	printk("Moxa CPU APB DMA Device Driver V1.0 load ");

	memset(apb_dma_channel, 0, sizeof(apb_dma_channel));
	spin_lock_init(&apb_dma_lock);

	for ( i=0; i<APB_DMA_MAX_CHANNEL; i++, priv++ ) {
		priv->reg = (apb_dma_reg *)(CPE_APBDMA_BASE+0x80+i*sizeof(apb_dma_reg));
	}

	cpe_int_set_irq(IRQ_APBDMA, EDGE, H_ACTIVE);
	ret = request_irq(IRQ_APBDMA, apb_dma_irq, IRQF_DISABLED, "APB DMA", NULL);
	if ( ret )
		printk("fail !\n");
	else
		printk("OK.\n");

	return ret;
}

module_init(apb_dma_init);
