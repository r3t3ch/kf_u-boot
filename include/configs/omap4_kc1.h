/*
 * (C) Copyright 2010
 * Texas Instruments Incorporated.
 * Aneesh V       <aneesh@ti.com>
 * Steve Sakoman  <steve@sakoman.com>
 *
 * Configuration settings for the TI SDP4430 board.
 * See omap4_common.h for OMAP4 common part
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

#ifndef __CONFIG_OMAP4KC1_H
#define __CONFIG_OMAP4KC1_H

/*
 * High Level Configuration Options
 */
#define CONFIG_OMAP4KC1			1
#define CONFIG_OMAP4KC1_VERSION		"v2.00"

#include <configs/omap4_common.h>

#define CONFIG_ARCH_OMAP4               1

#undef CONFIG_SYS_PTV
#define CONFIG_SYS_PTV                  7

/* DEBUG OUTPUT */
#define DEBUG				1

/* MEMORY ENV + 2M */
#undef CONFIG_SYS_MALLOC_LEN
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + 0x01000000)

/* Not an SBL build */
#undef CONFIG_SPL

/* SMB347 */
#undef CONFIG_TWL6030_POWER
#define CONFIG_POWER_FG_BQ27541		1
#define CONFIG_POWER_SMB347		1

/* SPI */
#define CONFIG_OMAP4_SPI		1

/* DSS */
#define CONFIG_VIDEO_OMAP4		1

/* I2C */
//#undef CONFIG_SYS_I2C_SPEED
//#define CONFIG_SYS_I2C_SPEED		400000

/* SPLASH SCREEN */
#define CONFIG_LCD			1
#define CONFIG_CMD_BMP			1
#define CONFIG_BMP_16BPP		1
#define CONFIG_BMP_32BPP		1
#define CONFIG_VIDEO_BMP_GZIP		1
#define CONFIG_SPLASH_SCREEN		1
#define CONFIG_SYS_VIDEO_LOGO_MAX_SIZE  0x00200000
#define CONFIG_SYS_WHITE_ON_BLACK	1
#define LCD_BPP				LCD_COLOR16

/* BOOTMENU */
//#define CONFIG_CFB_CONSOLE		1
//#define CONFIG_CFB_CONSOLE_ANSI	1
//#define CONFIG_VGA_AS_SINGLE_DEVICE	1
//#define CONFIG_CMD_BOOTMENU		1
//#define CONFIG_MENU			1

/* Environment information */
#undef CONFIG_BOOTDELAY
#define CONFIG_BOOTDELAY        	1

/* Use local mmc.c file */
#define CONFIG_SKIP_COMMON_MMC		1
#define CONFIG_BOARD_LATE_INIT		1

/* Enhance our eMMC support / experience. */
#define CONFIG_CMD_GPT
#define CONFIG_EFI_PARTITION
#define CONFIG_PARTITION_UUIDS
#define CONFIG_CMD_PART
#define CONFIG_DONT_USE_ADMA2

/* Defines for SDRAM init */
#undef CONFIG_SYS_EMIF_PRECALCULATED_TIMING_REGS
#define CONFIG_SYS_AUTOMATIC_SDRAM_DETECTION
#define CONFIG_SYS_DEFAULT_LPDDR2_TIMINGS

/* ENV related config options */
#define CONFIG_ENV_IS_NOWHERE            1
#define CONFIG_VERSION_VARIABLE          1

#define CONFIG_SYS_PROMPT                "OMAPKC1 # "

/* undef to save memory */
#undef CONFIG_SYS_LONGHELP
#undef CONFIG_SYS_HUSH_PARSER

#undef CONFIG_SYS_MEMTEST_END
#define CONFIG_SYS_MEMTEST_END           (0x80000000 + (0x00100000 * 31))

/* fastboot */
#undef CONFIG_MUSB_UDC
#undef CONFIG_USB_OMAP3
#undef CONFIG_USB_TTY
#undef CONFIG_USB_DEVICE

#define CONFIG_CMD_FASTBOOT              1
#define CONFIG_CUSTOM_COMMON_FASTBOOT    1
#define CONFIG_MUSB_GADGET               1
#define CONFIG_USB_MUSB_OMAP2PLUS        1
#define CONFIG_MUSB_PIO_ONLY             1
#define CONFIG_USB_GADGET_DUALSPEED      1
#define CONFIG_TWL6030_USB               1

/* long press power to turn on (in seconds) */
#define CONFIG_LONGPRESS_POWERON         2

// 5 second fastboot / button press countdown
#define CONFIG_FASTBOOT_COUNTDOWN        5
// poll times per second
#define CONFIG_FASTBOOT_COUNTDOWN_POLL   5

// charger mode
#define CONFIG_CHARGERMODE_ENABLE        1

#define MEMORY_BASE                      0x80000000
#define CONFIG_ADDR_ATAGS                (MEMORY_BASE + 0x100)
#define CONFIG_ADDR_DOWNLOAD             (MEMORY_BASE + 0x02000000)
#define CONFIG_FB_ADDR                   (MEMORY_BASE + 0x01b00000)
#define CONFIG_BOARD_MACH_TYPE           2160
#define DEVICE_TREE                      0x80f80000
#define CONFIG_ANDROID_BOOT_IMAGE        1

#define CUSTOM_DEVICE_VENDOR_ID          0x18d1
#define CUSTOM_DEVICE_PRODUCT_ID         0x0100


/* env */
#undef CONFIG_EXTRA_ENV_SETTINGS
#define CONFIG_EXTRA_ENV_SETTINGS \
	"console=ttyO2,115200n8\0" \
	"fdt_high=0xffffffff\0" \
	"fdtaddr=0x80f80000\0" \
	"initrd_high=no\0" \
	"wifimac=FFEEDDCCBBAA\0" \
	"serialno=0123456789ABCDEF\0" \
	"mmcdev=1\0" \
	"mmcargs_old=setenv bootargs console=${console} \
		mem=456M@0x80000000 \
		init=/init vram=5M omapfb.vram=0:5M \
		androidboot.console=ttyO2\0" \
	"mmcargs_new=setenv dtbootargs console=${console} \
		${chargermode}androidboot.wifimac=${wifimac} \
		androidboot.serialno=${serialno} \
		androidboot.hardware=otterx\0"

#undef CONFIG_BOOTCOMMAND
#define CONFIG_BOOTCOMMAND		"run mmcargs_old; run mmcargs_new; booti mmc1"

// legacy ATAGS config
#define CONFIG_SETUP_MEMORY_TAGS	1
#define CONFIG_CMDLINE_TAG		1
#define CONFIG_INITRD_TAG		1
#define CONFIG_REVISION_TAG		1

/* if not usb_detect: */
   /* check long_power_press(1 second) */
   /* if not: shutdown */

/* check_batt_low: */
/* if yes: */
   /* show_battlow */
   /* start charging */
   /* charge_loop() */

/* load all idme values */
/* set android.boot cmdline params (serial, mac, etc) */

/* if run check_fastboot_cable; then */
   /* show_fastboot */
   /* start fastboot_mode */

/* check recovery_boot */
/* if yes: */
   /* bootm recovery */

/* show logo */
/* show menu start */
/* run fastboot loop / menu button check (bootdelay=sec): */

/* if fastboot_detect: */
   /* show_fastboot */
   /* start fastboot_mode */

/* if button_press: */
   /* show bootmenu */
   /* enter bootmenu loop */

/* bootm boot */

#endif /* __CONFIG_OMAP4KC1_H */
