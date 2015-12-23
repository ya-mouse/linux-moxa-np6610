/* 
ahb_dma.c programmed by Ivan Wang
AHB dma program for audio 2004/8/13 06:00pm
*/
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <asm/types.h>
#include <asm/io.h>
#include <asm/arch/ahb_dma.h>


ahb_dma_data_t *ahb_dma_alloc(void)
{
    ahb_dma_data_t *priv;
    priv=(ahb_dma_data_t *)kmalloc(sizeof(ahb_dma_data_t), GFP_KERNEL);
    return priv;
}

void ahb_dma_free(ahb_dma_data_t *priv)
{
    int size;
    size=sizeof(ahb_lld_t)*(priv->llp_count);
    if(priv->ahb_dma_lld)
    {
        consistent_free((void *)priv->ahb_dma_lld,size,(dma_addr_t)priv->ahb_dma_lld_phys);
        //printk("free 0x%x whit 0x%x size 0x%x\n",priv->ahb_dma_lld,priv->ahb_dma_lld_phys,size);
    }       
    ahb_dma_reset(priv);
    kfree(priv);
}

/* 
You must given:
    base:   base address of ahb dma
    llp_master: LLP master channel number
    src_data_master: source data channel number
    dest_data_master: dest data channel number
    llp_count:  LLP count
    channel:    ahb dma channel
    hw      is it hardware handshake?
*/
int ahb_dma_init(ahb_dma_data_t *priv)
{   
    int             i,size;
    ahb_lld_t       *lld=0;
    ahb_lld_t       *lld_phys=0;
    
    if(priv==0)
    {
        printk("NULL priv data!\n");
        return 0;
    }
    
    if(priv->llp_count)
    {
        size=sizeof(ahb_lld_t)*(priv->llp_count);
        priv->ahb_dma_lld=(ahb_lld_t *)consistent_alloc(GFP_DMA|GFP_KERNEL,
            size,(dma_addr_t *)&priv->ahb_dma_lld_phys);
        //printk("priv->ahb_dma_lld=0x%x phys=0x%x size=0x%x\n",(int)priv->ahb_dma_lld,(int)priv->ahb_dma_lld_phys,size);
        if(priv->ahb_dma_lld==0)
        {
            printk("ahb_dma_lld allocate memory fail!\n");
            return 0;
        }
    }
    
    priv->channel_base=(priv->base)+0x100+((priv->channel)*0x20);
    priv->llp_last_idx=0;
    priv->llp_free_idx=0;
    priv->ahb_last_lld=0;
    
    lld=priv->ahb_dma_lld;
    lld_phys=priv->ahb_dma_lld_phys;
    /* initialize value */
    for(i=0;i<priv->llp_count;i++)
    {
        lld[i].source=0;  //source
        lld[i].dest=0;     //dest,16bit
        lld[i].control=0;
        
        if(i==priv->llp_count-1)
            lld[i].llp=0;
        else
            lld[i].llp=(unsigned int)(&lld_phys[i+1])|(priv->llp_master); //use second channel 
        //printk("ahb_dma_lld[%d].llp\n",i,ahb_dma_lld[i].llp);
    }

    *(unsigned int *)(priv->channel_base+AHBDMA_CHANNEL_LLP_0x10)=0;
    *(unsigned int *)(priv->channel_base+AHBDMA_CHANNEL_CFG_0x4)=0x0;    //enabled all interrupt
    *(unsigned int *)(priv->base+AHBDMA_ENABLE_0x24)=0x1;  //enable DMA  controller   
    *(unsigned int *)(priv->channel_base+AHBDMA_CHANNEL_CSR_0x0)=0;
    return 1;
}

/*
LLP count =3 (example)
   0 => 1 => 2 => 3
        ^         |
        +---------+
You must prepared:
    src: source address
    dest:dest address
    sw: source width (0/1/2=>8/16/32)
    dw: dest width (0/1/2=>8/16/32)
    sctl: source control (0/1/2/3=>inc/dec/fix/x)
    dctl: dest coontrol (0/1/2/3=>inc/dec/fix/x)
    size: dma count
    irq: (0/1)==>(disable/enable)
 */
void ahb_dma_add(ahb_dma_data_t *priv,ahb_dma_parm_t *parm)
{
    unsigned int    val;
    ahb_lld_t       *lld;
    ahb_lld_t       *lld_phys;
    //printk("add priv=0x%x\n",priv);
    lld=priv->ahb_dma_lld;
    lld_phys=priv->ahb_dma_lld_phys;
    
    if(priv->llp_free_idx==0) //first to call ahb_dma_add
    {
        *(unsigned int *)(priv->channel_base+AHBDMA_CHANNEL_TXSZ_0x14)=parm->size;
        *(unsigned int *)(priv->channel_base+AHBDMA_CHANNEL_SRC_0x8)=parm->src;
        *(unsigned int *)(priv->channel_base+AHBDMA_CHANNEL_DST_0xC)=parm->dest;

        if (priv->llp_count == 0)
           *(unsigned int *)(priv->channel_base+AHBDMA_CHANNEL_LLP_0x10)= 0;  //john modified, for no LLP
		else
           *(unsigned int *)(priv->channel_base+AHBDMA_CHANNEL_LLP_0x10)=
               (unsigned int)(&lld_phys[0])|priv->llp_master;
        
        val=(((1-(parm->irq))<<31)&0x80000000)|
            ((parm->sw<<11)&0x00003800)|((parm->dw<<8)&0x00000700)|
            ((priv->hw_handshake<<7)&0x00000080)|
            ((parm->sctl<<5)&0x00000060)|((parm->dctl<<3)&0x00000018)|
            ((priv->src_data_master<<2)&0x4)|
            ((priv->dest_data_master<<1)&0x2);
        *(unsigned int *)(priv->channel_base+AHBDMA_CHANNEL_CSR_0x0)=val;
    }
    else
    {
        val=((parm->irq<<28)&0x10000000)|
            ((parm->sw<<25)&0x0e000000)|((parm->dw<<22)&0x01c00000)|
            ((parm->sctl<<20)&0x00300000)|((parm->dctl<<18)&0x000c0000)|
            ((priv->src_data_master<<17)&0x00020000)|
            ((priv->dest_data_master<<16)&0x00010000)|(parm->size);
        lld[priv->llp_free_idx-1].source=parm->src;
        lld[priv->llp_free_idx-1].dest=parm->dest;
        lld[priv->llp_free_idx-1].control=val;
        lld[priv->llp_free_idx-1].llp=0;
        
        if(priv->ahb_last_lld)
            priv->ahb_last_lld->llp=(unsigned int)(&lld_phys[priv->llp_free_idx-1])|priv->llp_master;
        priv->ahb_last_lld=&lld[priv->llp_free_idx-1];
    }
    
    if (priv->llp_count)   //john add check
	{
       if(priv->llp_free_idx==priv->llp_count)
          priv->llp_free_idx=1;
       else
          priv->llp_free_idx++;
	}
}

inline void ahb_dma_start(ahb_dma_data_t *priv)
{
    *(volatile unsigned int *)(priv->channel_base+AHBDMA_CHANNEL_CSR_0x0)|=0x1;
}

void ahb_dma_reset(ahb_dma_data_t *priv)
{
    priv->llp_last_idx=0;
    priv->llp_free_idx=0;
    priv->ahb_last_lld=0;

    //printk("ahb_dma_stop\n");
    *(unsigned int *)(priv->base+AHBDMA_ENABLE_0x24)=0; //disable DMA controller
    *(unsigned int *)(priv->channel_base+AHBDMA_CHANNEL_CSR_0x0)=0;   //disable DMA channel 0         
    *(unsigned int *)(priv->channel_base+AHBDMA_CHANNEL_LLP_0x10)=0;
    *(unsigned int *)(priv->channel_base+AHBDMA_CHANNEL_TXSZ_0x14)=0;  //no transfer size (use LLP)
    *(unsigned int *)(priv->channel_base+AHBDMA_CHANNEL_CFG_0x4)=0;   //disable all interrupt
}

inline void ahbdma_clear_int(ahb_dma_data_t *priv)
{
    *(volatile unsigned int *)(priv->base+AHBDMA_ISR_0x8)=1<<(priv->channel);
}

//john add, get ext DMA address
// so we can get DMA lenth
inline u32 ahbdma_get_dest_addr(ahb_dma_data_t *priv)
{
	return *(volatile u32 *)(priv->channel_base+AHBDMA_CHANNEL_DST_0xC);
}

// John add, to get interrupt status
// Bit 0 indicate interrupt created, 
// bit 1 indicate interrupt error
u32 ahbdma_get_status(ahb_dma_data_t *priv)
{
	u32 status=0;
    u32 temp;

    temp = *(volatile u32 *)(priv->base+AHBDMA_INT_ERR_0x0C);
	if ( temp & (1<<(priv->channel)) ) 
	   status |= INT_ERROR;   //error

    temp = *(volatile u32 *)(priv->base+AHBDMA_INT_TC_0x04);
	if ( temp & (1<<(priv->channel)) )
	   status |= INT_TRIGGER;   //complete

    return status;
}