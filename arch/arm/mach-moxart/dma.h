#ifndef ARCH_CPE_RTC_H
#define ARCH_CPE_RTC_H
 
 /* registers */
#define DMA_INT						0x0
#define DMA_INT_TC					0x4
#define DMA_INT_TC_CLR				0x8
#define DMA_INT_ERR					0xC
#define DMA_INT_ERR_CLR				0x10
#define DMA_TC						0x14
#define DMA_ERR						0x18
#define DMA_CH_EN					0x1C
#define DMA_CH_BUSY					0x20
#define DMA_CSR						0x24
#define DMA_SYNC					0x28

#define DMA_C0_DevRegBase			0x40
#define DMA_C0_DevDtBase			0x80

#define DMA_CH_CFG_REG_OFFSET		0x20
#define DMA_C0_CSR					0x100
#define DMA_C0_CFG					0x104
#define DMA_C0_SrcAddr				0x108
#define DMA_C0_DstAddr				0x10C
#define DMA_C0_LLP					0x110
#define DMA_C0_SIZE					0x114

/* bit mapping of main configuration status register(CSR) */
#define DMA_CSR_M1ENDIAN			0x00000004
#define DMA_CSR_M0ENDIAN			0x00000002
#define DMA_CSR_DMACEN				0x00000001

/* bit mapping of channel control register */
#define DMA_CSR_TC_MSK				0x80000000
#define DMA_CSR_CHPRJ_HIGHEST		0x00C00000
#define DMA_CSR_CHPRJ_2ND			0x00800000
#define DMA_CSR_CHPRJ_3RD			0x00400000
#define DMA_CSR_PRTO3				0x00200000
#define DMA_CSR_PRTO2				0x00100000
#define DMA_CSR_PRTO1				0x00080000
#define DMA_CSR_SRC_BURST_SIZE_1	0x00000000
#define DMA_CSR_SRC_BURST_SIZE_4	0x00010000
#define DMA_CSR_SRC_BURST_SIZE_8	0x00020000
#define DMA_CSR_SRC_BURST_SIZE_16	0x00030000
#define DMA_CSR_SRC_BURST_SIZE_32	0x00040000
#define DMA_CSR_SRC_BURST_SIZE_64	0x00050000
#define DMA_CSR_SRC_BURST_SIZE_128	0x00060000
#define DMA_CSR_SRC_BURST_SIZE_256	0x00070000

#define DMA_CSR_ABT					0x00008000
#define DMA_CSR_SRC_WIDTH_8			0x00000000
#define DMA_CSR_SRC_WIDTH_16		0x00000800
#define DMA_CSR_SRC_WIDTH_32		0x00001000

#define DMA_CSR_DST_WIDTH_8			0x00000000
#define DMA_CSR_DST_WIDTH_16		0x00000100
#define DMA_CSR_DST_WIDTH_32		0x00000200

#define DMA_CSR_MODE_NORMAL			0x00000000
#define DMA_CSR_MODE_HANDSHAKE		0x00000080

#define DMA_CSR_SRC_INCREMENT		0x00000000
#define DMA_CSR_SRC_DECREMENT		0x00000020
#define DMA_CSR_SRC_FIX				0x00000040

#define DMA_CSR_DST_INCREMENT		0x00000000
#define DMA_CSR_DST_DECREMENT		0x00000008
#define DMA_CSR_DST_FIX				0x00000010

#define DMA_CSR_SRC_SEL				0x00000004
#define DMA_CSR_DST_SEL				0x00000002
#define DMA_CSR_CH_ENABLE			0x00000001	


#define DMA_MAX_SIZE				0x10000
#define DAM_CHANNEL_NUMBER			8


typedef struct
{
	UINT32 enable:1;
	UINT32 dst_sel:1;
	UINT32 src_sel:1;
	UINT32 dst_ctrl:2;
	UINT32 src_ctrl:2;
	UINT32 mode:1;
	UINT32 dst_width:3;		
	UINT32 src_width:3;
	UINT32 reserved1:1;
	UINT32 abt:1;
	UINT32 src_size:3;
	UINT32 prot:3;
	UINT32 priority:2;
	UINT32 reserved0:7;
	UINT32 tc_msk:1;	
}fLib_DMA_CH_CSR_t;

typedef struct
{
	UINT32 int_tc_msk:1;
	UINT32 int_err_msk:1;
	UINT32 reserved0:30;
}fLib_DMA_CH_CFG_t;
	
typedef struct
{
	UINT32 master_id:1;
	UINT32 reserved:1;
	UINT32 link_list_addr:30;
}fLib_DMA_CH_LLP_t;

typedef struct
{
	UINT32 size:12;
	UINT32 reserved:4;
	
	UINT32 dst_sel:1;
	UINT32 src_sel:1;
	UINT32 dst_ctrl:2;
	UINT32 src_ctrl:2;
	UINT32 dst_width:3;
	UINT32 src_width:3;
	UINT32 tc_msk:1;
	UINT32 reserved1:3;
}fLib_DMA_LLP_CTRL_t;

typedef struct
{
	fLib_DMA_CH_CSR_t csr;
	fLib_DMA_CH_CFG_t cfg;
	UINT32 src_addr;
	UINT32 dst_addr;
	fLib_DMA_CH_LLP_t llp;
	UINT32 size;
	UINT32 dummy[2];
}fLib_DMA_CH_t;

typedef struct
{
	UINT32 src_addr;
	UINT32 dst_addr;
	fLib_DMA_CH_LLP_t llp;
	fLib_DMA_LLP_CTRL_t llp_ctrl;
}fLib_DMA_LLD_t;


typedef struct
{
	UINT32 dma_int;
	UINT32 dma_int_tc;
	UINT32 dma_int_tc_clr;
	UINT32 dma_int_err;
	UINT32 dma_int_err_clr;
	UINT32 dma_tc;
	UINT32 dma_err;
	UINT32 dma_ch_enable;
	UINT32 dma_ch_busy;
	UINT32 dma_csr;
	UINT32 dma_sync;
	UINT32 dummy0[5];
	
	UINT32 dma_ch_dev_reg_base[8];
	UINT32 dummy1[8];
	
	UINT32 dma_ch_dev_dt_base[8];
	
	UINT32 dummy2[24];
	
	fLib_DMA_CH_t dma_ch[7];
}fLib_DMA_Reg_t;


#ifdef not_complete_yet
/*  -------------------------------------------------------------------------------
 *   API
 *  -------------------------------------------------------------------------------
 */
 
extern int    fLib_IsDMAChannelBusy(INT32 Channel);
extern int    fLib_IsDMAChannelEnable(INT32 Channel);
extern UINT32 fLib_GetDMAIntStatus(void);
extern UINT32 fLib_GetDMAChannelIntStatus(INT32 Channel);
extern int    fLib_GetDMABusyStatus(void);
extern int    fLib_GetDMAEnableStatus(void);

extern void   fLib_InitDMA(UINT32 M0_BigEndian, UINT32 M1_BigEndian, UINT32 Sync);
extern void   fLib_EnableDMAChannel(INT32 Channel);
extern void   fLib_ClearDMAChannelIntStatus(INT32 Channel);

extern void   fLib_SetDMAChannelCfg(INT32 Channel, fLib_DMA_CH_CSR_t Csr);
extern fLib_DMA_CH_CSR_t fLib_GetDMAChannelCfg(INT32 Channel);
extern void   fLib_DMA_CHIntMask(INT32 Channel, fLib_DMA_CH_CFG_t Mask);
extern void   fLib_DMA_CHLinkList(INT32 Channel, fLib_DMA_CH_LLP_t LLP);
extern void   fLib_DMA_CHDataCtrl(INT32 Channel, UINT32 SrcAddr, UINT32 DstAddr, UINT32 Size);
#endif /* end_of_not */

#endif