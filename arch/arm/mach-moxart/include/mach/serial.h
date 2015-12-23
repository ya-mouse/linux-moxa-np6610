#ifndef CPE_SERIAL_H
#define CPE_SERIAL_H

#include <linux/config.h>
#include <asm/arch/hardware.h>

#define BASE_BAUD (CONFIG_UART_CLK/16)

/* Standard COM flags */
#define EXTRA_SERIAL_PORT_DEFNS
#define STD_SERIAL_PORT_DEFNS
#endif

