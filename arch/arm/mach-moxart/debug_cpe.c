/*
 *  linux/include/asm-arm/arch-integrator/debug-cpe.c
 *
 *  Copyright (C) 1999 ARM Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
 
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
//	while (AMBA_UART_FR & (1 << 3));
}

