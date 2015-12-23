#include <mach/moxa.h>

static void delayputs(void)
{
	volatile unsigned int i=100;
	while(i--)
	   ;
}
/*
 * If we need to do some setup prior to decompression (like initializing the
 * UART if we want to use puts() above) then we define it here. Punt.
 */

#define SERIAL_THR                     	0x00	 		/*  Transmitter Holding Register(Write).*/
#define SERIAL_RBR                     	0x00	 		/*  Receive Buffer register (Read).*/
#define SERIAL_IER                     	0x04	 		/*  Interrupt Enable register.*/
#define SERIAL_IIR                     	0x08	 		/*  Interrupt Identification register(Read).*/
#define SERIAL_FCR                     	0x08	 		/*  FIFO control register(Write).*/
#define SERIAL_EFR			0x08			//  ?????????????????????
#define SERIAL_LCR                     	0x0C	 		/*  Line Control register.*/
#define SERIAL_MCR                     	0x10	 		/*  Modem Control Register.*/
#define SERIAL_LSR                     	0x14	 		/*  Line status register(Read) .*/
#define SERIAL_MSR                     	0x18	 		/*  Modem Status register (Read).*/
#define SERIAL_SPR                     	0x1C     		/*  Scratch pad register */
#define SERIAL_DLL                     	0x0      		/*  Divisor Register LSB */
#define SERIAL_DLM                     	0x4      		/*  Divisor Register MSB */
#define SERIAL_PSR                     	0x8     		/* Prescale Divison Factor */


/* LSR Register */
#define SERIAL_LSR_DR                  0x1      	/* Data Ready */
#define SERIAL_LSR_OE                  0x2      	/* Overrun Error */
#define SERIAL_LSR_PE                  0x4      	/* Parity Error */
#define SERIAL_LSR_FE                  0x8      	/* Framing Error */
#define SERIAL_LSR_BI                  0x10     	/* Break Interrupt */
#define SERIAL_LSR_THRE                0x20     	/* THR Empty */
#define SERIAL_LSR_TE                  0x40     	/* Transmitte Empty */
#define SERIAL_LSR_DE                  0x80     	/* FIFO Data Error */


#define PUTS_UART_BASE CPE_UART1_BASE

#if 0	// mask by Victor Yu. 05-19-2005
static void puts(const char *s)
#else	// add by Victor Yu. 05-19-2005
static void putstr(const char *s)
#endif
{
	while (*s) 
	{
	    volatile unsigned int status=0;
	    do
	    {
	        status = *(unsigned char *)(PUTS_UART_BASE+SERIAL_LSR);
	    }
		while (!((status & SERIAL_LSR_THRE)==SERIAL_LSR_THRE) );

		*(unsigned char *)(PUTS_UART_BASE+SERIAL_THR) = *s;

		if (*s == '\n') 
		{

    	    do
    	    {
    	        status = *(unsigned char *)(PUTS_UART_BASE+SERIAL_LSR);
    	    }
    		while (!((status & SERIAL_LSR_THRE)==SERIAL_LSR_THRE) );

			*(unsigned char *)(PUTS_UART_BASE+SERIAL_THR) = '\r';
		}
		s++;
	}
}

#define arch_decomp_setup()
#define arch_decomp_wdog()
