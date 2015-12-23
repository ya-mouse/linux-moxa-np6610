
#ifndef _MOXASD_H
#define _MOXAAD_H

// register
#define MSD_CMD_REG		0
#define MSD_ARG_REG		4
#define MSD_RESP0_REG		8
#define MSD_RESP1_REG		0x0c
#define MSD_RESP2_REG		0x10
#define MSD_RESP3_REG		0x14
#define MSD_RESP_CMD_REG	0x18
#define MSD_DATA_CTRL_REG	0x1c
#define MSD_DATA_TIMER_REG	0x20
#define MSD_DATA_LEN_REG	0x24
#define MSD_STATUS_REG		0x28
#define MSD_CLEAR_REG		0x2c
#define MSD_INT_MASK_REG	0x30
#define MSD_POWER_CTRL_REG	0x34
#define MSD_CLOCK_CTRL_REG	0x38
#define MSD_BUS_WIDTH_REG	0x3c
#define MSD_DATA_WIN_REG	0x40
#define MSD_FEATURE_REG		0x44
#define MSD_REVISION_REG	0x48

typedef struct _moxasd_reg {
	unsigned int	command;
#define MSD_SDC_RST			(1<<10)
#define MSD_CMD_EN			(1<<9)
#define MSD_APP_CMD			(1<<8)
#define MSD_LONG_RSP			(1<<7)
#define MSD_NEED_RSP			(1<<6)
#define MSD_CMD_IDX_MASK		0x3f
	unsigned int	argument;
	unsigned int	response0;
	unsigned int	response1;
	unsigned int	response2;
	unsigned int	response3;
	unsigned int	response_command;
#define MSD_RSP_CMD_APP			(1<<6)
#define MSD_RSP_CMD_IDX_MASK		0x3f
	unsigned int	data_control;
#define MSD_DATA_EN			(1<<6)
#define MSD_DMA_EN			(1<<5)
#define MSD_DATA_WRITE			(1<<4)
#define MSD_BLK_SIZE_MASK		0x0f
	unsigned int	data_timer;
	unsigned int	data_length;
#define MSD_DATA_LEN_MASK		0xffffff
	unsigned int	status;
#define MSD_WRITE_PROT			(1<<12)
#define MSD_CARD_DETECT			(1<<11)
#define MSD_CARD_CHANGE			(1<<10)
#define MSD_FIFO_ORUN			(1<<9)
#define MSD_FIFO_URUN			(1<<8)
#define MSD_DATA_END			(1<<7)
#define MSD_CMD_SENT			(1<<6)
#define MSD_DATA_CRC_OK			(1<<5)
#define MSD_RSP_CRC_OK			(1<<4)
#define MSD_DATA_TIMEOUT		(1<<3)
#define MSD_RSP_TIMEOUT			(1<<2)
#define MSD_DATA_CRC_FAIL		(1<<1)
#define MSD_RSP_CRC_FAIL		(1<<0)
	unsigned int	clear;
#define MSD_CLR_CARD_CHANGE		(1<<10)
#define MSD_CLR_FIFO_ORUN		(1<<9)
#define MSD_CLR_FIFO_URUN		(1<<8)
#define MSD_CLR_DATA_END		(1<<7)
#define MSD_CLR_CMD_SENT		(1<<6)
#define MSD_CLR_DATA_CRC_OK		(1<<5)
#define MSD_CLR_RSP_CRC_OK		(1<<4)
#define MSD_CLR_DATA_TIMEOUT		(1<<3)
#define MSD_CLR_RSP_TIMEOUT		(1<<2)
#define MSD_CLR_DATA_CRC_FAIL		(1<<1)
#define MSD_CLR_RSP_CRC_FAIL		(1<<0)
	unsigned int	interrupt_mask;
#define MSD_INT_CARD_CHANGE		(1<<10)
#define MSD_INT_FIFO_ORUN		(1<<9)
#define MSD_INT_FIFO_URUN		(1<<8)
#define MSD_INT_DATA_END		(1<<7)
#define MSD_INT_CMD_SENT		(1<<6)
#define MSD_INT_DATA_CRC_OK		(1<<5)
#define MSD_INT_RSP_CRC_OK		(1<<4)
#define MSD_INT_DATA_TIMEOUT		(1<<3)
#define MSD_INT_RSP_TIMEOUT		(1<<2)
#define MSD_INT_DATA_CRC_FAIL		(1<<1)
#define MSD_INT_RSP_CRC_FAIL		(1<<0)
	unsigned int	power_control;
#define MSD_SD_POWER_ON			(1<<4)
#define MSD_SD_POWER_MASK		0x0f
	unsigned int	clock_control;
#define MSD_CLK_DIS			(1<<8)
#define MSD_CLK_SD			(1<<7)
#define MSD_CLK_DIV_MASK		0x7f
	unsigned int	bus_width;
#define MSD_WIDE_BUS_SUPPORT		(1<<3)
#define MSD_WIDE_BUS			(1<<2)	// bus width=4
#define MSD_SINGLE_BUS			(1<<0)	// bus width=1
	unsigned int	data_window;
	unsigned int	feature;
#define MSD_CPRM_FUNCTION		(1<<8)
	unsigned int	revision;
} moxasd_reg;

#define MSD_FIFO_LENW	4	// 4 words, total 4 * 4 = 16 bytes
#define MSD_FIFO_LENB	16	// 16 bytes

#endif	// _MOXASD_H
