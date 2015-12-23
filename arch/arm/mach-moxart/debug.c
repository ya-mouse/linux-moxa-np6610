
#include <asm/arch/cpe/cpe.h>

#define SERIAL_THR                     	0x00
#define SERIAL_LSR                      0x14
#define SERIAL_LSR_THRE                 0x20

void debug_puts(const char *s)
{
	while (*s) 
	{
	    volatile unsigned int status=0;
	    do
	    {
	        status = *(unsigned char *)(IO_ADDRESS(CPE_UART1_BASE)+SERIAL_LSR);
	    }
		while (!((status & SERIAL_LSR_THRE)==SERIAL_LSR_THRE) );

		*(unsigned char *)(IO_ADDRESS(CPE_UART1_BASE)+SERIAL_THR) = *s;

		if (*s == '\n') 
		{

    	    do
    	    {
    	        status = *(unsigned char *)(IO_ADDRESS(CPE_UART1_BASE)+SERIAL_LSR);
    	    }
    		while (!((status & SERIAL_LSR_THRE)==SERIAL_LSR_THRE) );

			*(unsigned char *)(IO_ADDRESS(CPE_UART1_BASE)+SERIAL_THR) = '\r';
		}
		s++;
	}
}
