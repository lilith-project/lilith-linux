/*
 * linux/arch/arm/mach-at91/board-lilith.c
 *
 *  Copyright (C) 2005 SAN People
 *  Copyright (C) 2006 Atmel
 *  Copyright (C) 2007 Olimex Ltd
 *  Copyright (C) 2012 Julien Rouviere
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

#include <linux/types.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/uio_driver.h>

#include <linux/spi/spi.h>

#include <mach/hardware.h>
#include <asm/setup.h>
#include <asm/mach-types.h>
#include <asm/irq.h>

#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/irq.h>

#include <mach/board.h>
#include <mach/at91sam9_smc.h>
#include <mach/at91sam9260.h>

#include "sam9_smc.h"
#include "generic.h"


static void __init ek_init_early(void)
{
	/* Initialize processor: 18.432 MHz crystal */
	at91_initialize(18432000);

	/* Setup the LEDs */
	at91_init_leds(AT91_PIN_PA9, AT91_PIN_PA6);

	/* DBGU on ttyS0. (Rx & Tx only) */
	at91_register_uart(0, 0, 0);

	/* USART0 on ttyS1. (Rx, Tx, CTS, RTS, DTR, DSR, DCD, RI) */
	at91_register_uart(AT91SAM9260_ID_US0, 1, ATMEL_UART_CTS | ATMEL_UART_RTS
			   | ATMEL_UART_DTR | ATMEL_UART_DSR | ATMEL_UART_DCD
			   | ATMEL_UART_RI);

	/* USART1 on ttyS2. (Rx, Tx, CTS, RTS) */
	at91_register_uart(AT91SAM9260_ID_US1, 2, ATMEL_UART_CTS | ATMEL_UART_RTS);

	/* set serial console to ttyS0 (ie, DBGU) */
	at91_set_serial_console(0);
}

/*
 * USB Host port
 */
static struct at91_usbh_data __initdata ek_usbh_data = {
	.ports		= 2,
};

/*
 * USB Device port
 */
static struct at91_udc_data __initdata ek_udc_data = {
	.vbus_pin	= AT91_PIN_PC5,
	.pullup_pin	= 0,		/* pull-up driven by UDC */
};


/*
 * MACB Ethernet device
 */
static struct at91_eth_data __initdata ek_macb_data = {
	.phy_irq_pin	= AT91_PIN_PA7,
	.is_rmii	= 0,
};


/*
 * MCI (SD/MMC)
 */
static struct at91_mmc_data __initdata ek_mmc_data = {
	.slot_b		= 1,
	.wire4		= 1,
	.det_pin	= AT91_PIN_PC8,
	.wp_pin		= AT91_PIN_PC4,
//	.vcc_pin	= ... not connected
};

/*
 * Regs via UIO
 */
static struct uio_info at91_uio_platform_data = {
        .name = "AT91 Regs",
        .version = "0",
};

static struct resource at91_regs_resource[] = {
        [0] = {
                .name   = "TC 0-2 regs",
                .start  = AT91SAM9260_BASE_TC0,
                .end    = AT91SAM9260_BASE_TC0 + 0xC0,
                .flags  = IORESOURCE_MEM,
        },

        [1] = {
                .name   = "TC 3-6 regs",
                .start  = AT91SAM9260_BASE_TC3,
                .end    = AT91SAM9260_BASE_TC3 + 0xC0,
                .flags  = IORESOURCE_MEM,
        },


        [2] = {
                .name   = "GPIO regs",
                .start  = AT91_PIOA + AT91_BASE_SYS,
                .end    = AT91_PIOA + AT91_BASE_SYS + 0x600,
                .flags  = IORESOURCE_MEM,
        },

/*	[3] = {
                .name   = "USART 0-2 regs",
                .start  = AT91SAM9260_BASE_US0,
                .end    = AT91SAM9260_BASE_US0 + 0xC000,
                .flags  = IORESOURCE_MEM,
        },

	[4] = {
                .name   = "USART 3-6 regs",
                .start  = AT91SAM9260_BASE_US3,
                .end    = AT91SAM9260_BASE_US3 + 0xC000,
                .flags  = IORESOURCE_MEM,
        },
*/
};

static struct platform_device at91_uio_regs = {
        .name           = "uio_pdrv_genirq",
        .id             = 0,
        .dev = {
                .platform_data  = &at91_uio_platform_data,
        },

        .resource       = at91_regs_resource,
        .num_resources  = ARRAY_SIZE(at91_regs_resource),
};



/*
 * Board init
 */
static void __init ek_board_init(void)
{
	/* Serial */
	at91_add_device_serial();
	/* MMC */
	at91_add_device_mmc(0, &ek_mmc_data);	
	/* USB Host */
	at91_add_device_usbh(&ek_usbh_data);
	/* USB Device */
	at91_add_device_udc(&ek_udc_data);
	/* Ethernet */
	at91_add_device_eth(&ek_macb_data);
	/* I2C */
	at91_add_device_i2c(NULL, 0);


	/* UIO */
	platform_device_register(&at91_uio_regs);
}

MACHINE_START(LILITH, "Lilith")
	/* Maintainer: JR */
	.timer		= &at91sam926x_timer,
	.map_io		= at91_map_io,
	.init_early	= ek_init_early,
	.init_irq	= at91_init_irq_default,
	.init_machine	= ek_board_init,
MACHINE_END

