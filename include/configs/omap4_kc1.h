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

#include <configs/omap4_common.h>

/* Not an SBL build */
#undef CONFIG_SPL

/* SMB347 */
#ifndef CONFIG_SPL_BUILD
#define CONFIG_SMB347_POWER		1
#endif

/* Environment information */
#undef CONFIG_BOOTDELAY
#define CONFIG_BOOTDELAY        	1

/* Use local mmc.c file */
#define CONFIG_SKIP_COMMON_MMC		1

/* Battery Charger */
#ifndef CONFIG_SPL_BUILD
#define CONFIG_CMD_BAT			1
#endif

/* Defines for SDRAM init */
#undef CONFIG_SYS_EMIF_PRECALCULATED_TIMING_REGS
#define CONFIG_SYS_AUTOMATIC_SDRAM_DETECTION
#define CONFIG_SYS_DEFAULT_LPDDR2_TIMINGS

/* ENV related config options */
#define CONFIG_ENV_IS_NOWHERE		1

#define CONFIG_SYS_PROMPT		"OMAPKC1 # "

/* undef to save memory */
#undef CONFIG_SYS_LONGHELP
#undef CONFIG_SYS_HUSH_PARSER

#undef CONFIG_SYS_MEMTEST_END
#define CONFIG_SYS_MEMTEST_END		(0x80000000 + (0x00100000 * 31))

#undef CONFIG_EXTRA_ENV_SETTINGS
#define CONFIG_EXTRA_ENV_SETTINGS \
	"loadaddr=0x82000000\0" \
	"console=ttyO2,115200n8\0" \
	"fdt_high=0xffffffff\0" \
	"fdtaddr=0x80f80000\0" \
	"bootpart=0:2\0" \
	"bootdir=/boot\0" \
	"bootfile=zImage\0" \
	"usbtty=cdc_acm\0" \
	"vram=16M\0" \
	"mmcdev=0\0" \
	"mmcroot=/dev/mmcblk0p2 rw\0" \
	"mmcrootfstype=ext3 rootwait\0" \
	"mmcargs=setenv bootargs console=${console} " \
		"vram=${vram} " \
		"root=${mmcroot} " \
		"rootfstype=${mmcrootfstype}\0" \
	"loadbootscript=fatload mmc ${mmcdev} ${loadaddr} boot.scr\0" \
	"bootscript=echo Running bootscript from mmc${mmcdev} ...; " \
		"source ${loadaddr}\0" \
	"loadbootenv=fatload mmc ${mmcdev} ${loadaddr} uEnv.txt\0" \
	"importbootenv=echo Importing environment from mmc${mmcdev} ...; " \
		"env import -t ${loadaddr} ${filesize}\0" \
	"loadimage=load mmc ${bootpart} ${loadaddr} ${bootdir}/${bootfile}\0" \
	"mmcboot=echo Booting from mmc${mmcdev} ...; " \
		"run mmcargs; " \
		"bootz ${loadaddr} - ${fdtaddr}\0" \
	"findfdt="\
		"if test $board_name = sdp4430; then " \
			"setenv fdtfile omap4-sdp.dtb; fi; " \
	"loadfdt=load mmc ${bootpart} ${fdtaddr} ${bootdir}/${fdtfile}\0" \

#undef CONFIG_BOOTCOMMAND
#define CONFIG_BOOTCOMMAND \
	"run findfdt; " \
	"mmc dev ${mmcdev}; if mmc rescan; then " \
		"echo SD/MMC found on device ${mmcdev};" \
		"if run loadbootscript; then " \
			"run bootscript; " \
		"else " \
			"if run loadbootenv; then " \
				"run importbootenv; " \
			"fi;" \
			"if test -n ${uenvcmd}; then " \
				"echo Running uenvcmd ...;" \
				"run uenvcmd;" \
			"fi;" \
		"fi;" \
		"if run loadimage; then " \
			"run loadfdt;" \
			"run mmcboot; " \
		"fi; " \
	"fi"

#endif /* __CONFIG_OMAP4KC1_H */
