/* for MOXA CPU embedded UART */
#define MOXA_EMBEDDED_UART_IRQ				31
#define MOXA_EMBEDDED_UART_BASE_REG			0x98200000L
#define MOXA_EMBEDDED_UART_BASE_INT			0x982000C0L
#define MOXA_EMBEDDED_UART_BASE_MODE			0x982000E0L
/*
#define MOXA_EMBEDDED_UART_IRQ				31
#define MOXA_EMBEDDED_UART_BASE_REG			0xB0900000L
#define MOXA_EMBEDDED_UART_BASE_INT			0xB09000C0L
#define MOXA_EMBEDDED_UART_BASE_MODE			0xB09000E0L
*/

#define MOXA_EMBEDDED_UART_CLK				14745600
/* for register offset */
#define MOXA_EMBEDDED_UART_TX				(UART_TX*4)	
#define MOXA_EMBEDDED_UART_RX				(UART_RX*4)	
#define MOXA_EMBEDDED_UART_LCR				(UART_LCR*4)	
#define MOXA_EMBEDDED_UART_FCR				(UART_FCR*4)	
#define MOXA_EMBEDDED_UART_IIR				(UART_IIR*4)	
#define MOXA_EMBEDDED_UART_IER				(UART_IER*4)	
#define MOXA_EMBEDDED_UART_MCR				(UART_MCR*4)	
#define MOXA_EMBEDDED_UART_LSR				(UART_LSR*4)	
#define MOXA_EMBEDDED_UART_MSR				(UART_MSR*4)	
#define MOXA_EMBEDDED_UART_DLL				(UART_DLL*4)	
#define MOXA_EMBEDDED_UART_DLM				(UART_DLM*4)	

/* for register mask */
#define MOXA_EMBEDDED_MASK_IIR				0x3E

/* Enhance mode */
/* good data mode enable*/
#define MOXA_FCR_GDA_MODE_ENABLE	0x20
/* only good data put into RxFIFO */
#define MOXA_FCR_GDA_ONLY_ENABLE	0x10
/* enable CTS interrupt */
#define MOXA_IER_ECTSI		0x80
/* eanble RTS interrupt */
#define MOXA_IER_ERTSI		0x40
/* enable Xon/Xoff interrupt */
#define MOXA_IER_XINT		0x20
/* enable GDA interrupt */
#define MOXA_IER_EGDAI		0x10

#define MOXA_RECV_ISR		(UART_IER_RDI | MOXA_IER_EGDAI)

/* GDA interrupt pending */
#define MOXA_IIR_GDA		0x1C
#define MOXA_IIR_RDA		0x04
#define MOXA_IIR_RTO		0x0C
#define MOXA_IIR_LSR		0x06

/* recieved Xon/Xoff or specical interrupt pending */
#define MOXA_IIR_XSC		0x10

/* RTS/CTS change state interrupt pending */
#define MOXA_IIR_RTSCTS		0x20
#define MOXA_IIR_MASK		0x3E
#define MOXA_MCR_XON_FLAG	0x40
#define MOXA_MCR_XON_ANY	0x80
#define MOXA_MCR_TX_XON		0x08


/* software flow control on chip mask value */
#define MOXA_EFR_SF_MASK	0x0F
/* send Xon1/Xoff1 */
#define MOXA_EFR_SF_TX2		0x04
/* send Xon1,Xon2/Xoff1,Xoff2 */
#define MOXA_EFR_SF_TX12	0x0C
/* don't send Xon/Xoff */
#define MOXA_EFR_SF_TX_NO	0x00
/* Tx software flow control mask */
#define MOXA_EFR_SF_TX_MASK	0x0C
/* don't receive Xon/Xoff */
#define MOXA_EFR_SF_RX_NO	0x00
/* receive Xon1/Xoff1 */
#define MOXA_EFR_SF_RX1		0x02
/* receive Xon2/Xoff2 */
#define MOXA_EFR_SF_RX2		0x01
/* receive Xon1,Xon2/Xoff1,Xoff2 */
#define MOXA_EFR_SF_RX12	0x03
/* Rx software flow control mask */
#define MOXA_EFR_SF_RX_MASK	0x03

