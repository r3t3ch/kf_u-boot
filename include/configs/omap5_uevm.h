/*
 * (C) Copyright 2013
 * Texas Instruments Incorporated.
 * Sricharan R	  <r.sricharan@ti.com>
 *
 * Configuration settings for the TI EVM5430 board.
 * See omap5_common.h for omap5 common settings.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __CONFIG_OMAP5_EVM_H
#define __CONFIG_OMAP5_EVM_H

/* Define the default GPT table for eMMC */
#define PARTS_DEFAULT \
	"uuid_disk=${uuid_gpt_disk};" \
	"name=rootfs,start=2MiB,size=-,uuid=${uuid_gpt_rootfs}"

#include <configs/omap5_common.h>

#define CONFIG_CONS_INDEX		3
#define CONFIG_SYS_NS16550_COM3		UART3_BASE
#define CONFIG_BAUDRATE			115200

/* MMC ENV related defines */
#define CONFIG_ENV_IS_IN_MMC
#define CONFIG_SYS_MMC_ENV_DEV		1	/* SLOT2: eMMC(1) */
#define CONFIG_ENV_OFFSET		0xE0000
#define CONFIG_CMD_SAVEENV

/* Enhance our eMMC support / experience. */
#define CONFIG_CMD_GPT
#define CONFIG_EFI_PARTITION
#define CONFIG_PARTITION_UUIDS
#define CONFIG_CMD_PART

#define CONFIG_SYS_PROMPT		"OMAP5430 EVM # "

/* Clock Defines */
#define V_OSCK			19200000	/* Clock output from T2 */

#define NON_SECURE_SRAM_START	0x40300000
#define NON_SECURE_SRAM_END	0x40320000	/* Not inclusive */

#define CONFIG_OMAP_PLATFORM_RESET_TIME_MAX_USEC	16296
/*
 * DWC3 requires caches to be off. This is done by default once you uncomment
 * the option below. Since the majority of users probably prefer caches over
 * DWC3, DWC3 is disabled by default. If you need fastboot (and therefore DWC3)
 * feel free to enable it.
*/

#define CONFIG_USB_DWC3		1
#define CONFIG_USB_DWC3_UDC_REGS		((void *)0x4a030000)
#define CONFIG_USB_DWC3_UDC_REGS_END		((void *)0x4a03ffff)
#define CONFIG_USB_DWC3_WRAP_REGS		((void *)0x4a020000)
#define CONFIG_USB_DWC3_WRAP_REGS_END		((void *)0x4a02ffff)
#ifdef CONFIG_USB_DWC3
#define CONFIG_SYS_DCACHE_OFF	1
#define CONFIG_SYS_L2CACHE_OFF	1
#endif
#define CONFIG_ANDROID_BOOT_IMAGE	1

#endif /* __CONFIG_OMAP5_EVM_H */
