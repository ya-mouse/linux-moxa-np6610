/***************************************************************************
* Copyright  Faraday Technology Corp 2002-2003.  All rights reserved.      *
*--------------------------------------------------------------------------*
* Name:SD_CNTR.c                                                           *
* Description: SD relative routines                                        *
* Author: Ted Hsu 			                                   *
****************************************************************************
* Porting to Linux on 20030526						   *
* Author: Paul Chiang							   *
* Version: 0.1								   *
* History: 								   *
*          0.1 new creation						   *
*          0.2 Porting to meet the style of linux dma                      *
* Todo: 								   *
****************************************************************************/
#define __NO_VERSION__
#include <linux/module.h>
#include <linux/config.h>
#include <linux/sched.h>
#include <linux/init.h>

#include <asm/dma.h>
#include <asm/fiq.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/hardware.h>
#include <asm/mach-types.h>

#include <asm/mach/dma.h>
#include <asm/arch/apb_dma.h>
#include <linux/ioport.h>
#include <asm/arch/cpe_int.h>
#include "dma.h"

//#define P_DEBUG(a...)      printk("<0> CPE_DMA:" ##a)
#define P_DEBUG(a...)

#ifdef CONFIG_UCLINUX		// uclinux without mmu
//     #error "CONFIG_UCLINUX"
	#define OUTW(port,value)         outl(value,port)
	#define INW(port)                inl(port)
	#define REQUEST_IO_REGION(a,b,c) request_region(a,b,c)
	#define CPUCleanInvalidateDCacheAll()
#else //arm linux with mmu

	#define OUTW(port,value)          outl(value,(IO_ADDRESS((port))))
	#define INW(port)                 inl((IO_ADDRESS((port))))
	#define REQUEST_IO_REGION(a,b,c)  request_region((IO_ADDRESS((a))),b,c)
	#define CPUCleanInvalidateDCacheAll()
#endif

#define  CPE_MAX_DMA_CHANNELS  (CPE_APB_DMA_MAX_CHANNEL+CPE_AHB_DMA_MAX_CHANNEL)
extern dma_t fapb_dma_chan[CPE_MAX_DMA_CHANNELS];	//paulong 20030616 remove static
//========================================================================
struct dma_record
{
  uint active;			//dma is working
  uint base_addr;
  dma_handler_func_t dma_handler;    //(int irq,uint finish_status,uint error_status) paul 20030617
  uint int_nr;
};
static struct dma_record DMA_INTR[CPE_MAX_DMA_CHANNELS];

//===============================================
void APB_set_dma_intr (dmach_t channel, dma_handler_func_t func)
{
  DMA_INTR[channel].dma_handler = func;	// paul 20030617
}

//---------------------------------
static void APB_enable_dma(dmach_t channel, dma_t * dma)
{
	P_DEBUG("APB_enable_dma()\n");
	DMA_INTR[channel].active = 1;
	fLib_APBDMA_Interrupt( dma->dma_base, 1, 1);
	fLib_APBDMA_EnableTrans(dma->dma_base, 1);
	//CPUCleanInvalidateDCacheAll (); // mark by Charles Tsai
}

//---------------------------------
static void APB_disable_dma (dmach_t channel, dma_t * dma)
{
  P_DEBUG ("APB_disable_dma()\n");
  DMA_INTR[channel].active = 0;
  //fLib_APBDMA_Interrupt( dma->dma_base, 0, 0);  //move to init_cpe_dma() run once
  fLib_APBDMA_EnableTrans (dma->dma_base, 0);
}

//---------------------------------
static struct dma_ops APB_dma_ops = {
  enable:			APB_enable_dma,
  disable:			APB_disable_dma,
  type:				"APB_DMA ",
};
//---------------------------------
#define loop_APB_dma_channel()	for (i = 0; i < CPE_APB_DMA_MAX_CHANNEL; i++)
#define loop_AHB_dma_channel()	for (i = 0; i < CPE_AHB_DMA_MAX_CHANNEL; i++)
#define loop_all_dma_channel()	for (i = 0; i < CPE_MAX_DMA_CHANNELS; ++i)
//-----------------------------------------------
#ifdef P_DEBUG
void Dumpdma (void)
{
  int i;
  dma_t *dma = fapb_dma_chan;

  printk ("Dma Channel Informations---\n");
  loop_all_dma_channel ()
  {
    printk (" Channel=%d:\n", i);
    //...........APB DMA Channels
    if (i < CPE_APB_DMA_MAX_CHANNEL)	//Paul: 0~3 APB, 4~12 AHB
      {
		printk ("   APB--");
      }
    //...........AHB DMA Channels
    else
      {
		printk ("   AHB--");
      }

    printk ("active=%d,", dma[i].active);
    printk ("invalid=%d,", dma[i].invalid);
    printk ("using_sg=%d,", dma[i].using_sg);
    printk ("dma_mode=%d,", dma[i].dma_mode);
    printk ("speed=%d,", dma[i].speed);
    printk ("lock=0x%X,", dma[i].lock);
    printk ("device_id=%s,", dma[i].device_id);
    printk ("dma_base=0x%X,", dma[i].dma_base);
    printk ("dma_irq=%d,", dma[i].dma_irq);
    printk ("state=%d,", dma[i].state);
    printk ("d_ops=0x%lX\n",(ulong) dma[i].d_ops);
  }
}
#endif

//--------------------------------
void  AHB_interrupt_handler (int irq, void *dev_id, struct pt_regs *dummy)
{
  P_DEBUG ("AHB_interrupt_handler()\n");
  P_DEBUG ("irq=0x%X 1\n", irq);
}

//-------------------
int APB_Check_Finish_DMA(dmach_t channel)
{

	return (DMA_INTR[channel].active==0);		// return 1 finished, return 0 unfinished
}

//-----------------
void APB_interrupt_handler(int irq, void *dev_id, struct pt_regs *dummy)
{
	uint i;
	uint FinishStatus, ErrorStatus;

	//DMA_INTR[0].dma_handler(irq,1,0); /* special process by Charles tsai */
	//return;
	P_DEBUG("APB_interrupt_handler()\n");
	P_DEBUG("irq=0x%X 1\n", irq);

	loop_APB_dma_channel() {
		FinishStatus = fLib_APBDMA_ChkFinish(DMA_INTR[i].base_addr);
		ErrorStatus = fLib_APBDMA_ChkErr(DMA_INTR[i].base_addr);
		if (FinishStatus || ErrorStatus) {
			DMA_INTR[i].active = 0;
			fLib_APBDMA_ClrStatus(DMA_INTR[i].base_addr); //paul 20030617
			if (DMA_INTR[i].dma_handler != NULL) {
				//dma_isr=(dma_handler_func_t)DMA_INTR[i].dma_handler;
				//(*dma_isr)(irq,FinishStatus,ErrorStatus);       	   	//dispatch
				DMA_INTR[i].dma_handler(irq, FinishStatus, ErrorStatus);
			} else
				printk("APB dma (channels=%d) irq=%d occur, but isr is NULL\n", i, irq);
		}
	}
}

//---------------------------------
void __init arch_dma_init (dma_t * dma)
{
  //init_cpe_dma() cannot request irq /io  here, execute later
}

//--------------------------------------
int __init init_cpe_dma(void)
{
	int i, result;
	dma_t *dma = fapb_dma_chan;

	printk("Faraday CPE DMA drivers, %d channels\n", CPE_MAX_DMA_CHANNELS);

	loop_all_dma_channel() {
		//............ APB DMA Channels
		if (i < CPE_APB_DMA_MAX_CHANNEL) { //Paul: 0~3 APB, 4~12 AHB
			dma[i].d_ops = &APB_dma_ops;
			dma[i].dma_irq = IRQ_APB_BRIDGE; // waiting to do:

			switch (i) {
			case 0:
				dma[i].dma_base = AHB2APB_DMA_A;
				REQUEST_IO_REGION(dma[i].dma_base, 0x10, "AHB2APB_DMA_A");
				break;
			case 1:
				dma[i].dma_base = AHB2APB_DMA_B;
	    		REQUEST_IO_REGION(dma[i].dma_base, 0x10, "AHB2APB_DMA_B");
				break;
			case 2:
				dma[i].dma_base = AHB2APB_DMA_C;
				REQUEST_IO_REGION(dma[i].dma_base, 0x10, "AHB2APB_DMA_C");
				break;
			case 3:
				dma[i].dma_base = AHB2APB_DMA_D;
				REQUEST_IO_REGION(dma[i].dma_base, 0x10, "AHB2APB_DMA_D");
				break;
			}

			if (i == 0) { // do once
				cpe_int_set_irq(dma[i].dma_irq, EDGE, H_ACTIVE);
				result = request_irq(dma[i].dma_irq, APB_interrupt_handler, IRQF_DISABLED, "APB DMA controller", NULL);
				if (result != 0) {
					printk("   APB Dma drivers, %d channels, intr=%d....Failed\n", CPE_APB_DMA_MAX_CHANNEL, dma[i].dma_irq);
					return -1;
				} else
					printk("   APB Dma drivers, %d channels, intr=%d\n",CPE_APB_DMA_MAX_CHANNEL, dma[i].dma_irq);
			}
		}
#if 0 /* chris lee removed */
		//............ AHB DMA Channels
		else			//Paul: not implement
		{
			dma[i].d_ops = NULL;
			dma[i].dma_base = 0;
			dma[i].dma_irq = FIQ_DMA;
			if (i == CPE_APB_DMA_MAX_CHANNEL) {
				printk("   AHB Dma drivers, %X channels, intr=%d\n", CPE_AHB_DMA_MAX_CHANNEL, dma[i].dma_irq);
			}
		}
#endif
		//................general
		dma[i].active = 0;
		dma[i].invalid = 1;
		dma[i].using_sg = 0;

		DMA_INTR[i].base_addr = dma[i].dma_base;
		DMA_INTR[i].dma_handler = NULL;
		DMA_INTR[i].int_nr = dma[i].dma_irq;
		DMA_INTR[i].active = 0;
	}

	return 1;
}

EXPORT_SYMBOL(APB_Check_Finish_DMA);
module_init (init_cpe_dma);
