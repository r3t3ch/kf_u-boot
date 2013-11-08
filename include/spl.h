/*
 * (C) Copyright 2012
 * Texas Instruments, <www.ti.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#ifndef	_SPL_H_
#define	_SPL_H_

/* Platform-specific defines */
#include <linux/compiler.h>
#include <asm/spl.h>
#include <mmc.h>
#include <android_image.h>

/* Boot type */
#define MMCSD_MODE_UNDEFINED	0
#define MMCSD_MODE_RAW		1
#define MMCSD_MODE_FAT		2

#if 0
struct spl_kernel_boot_info {
	u8 magic[BOOT_MAGIC_SIZE];
	u32 ramdisk_size; /* size in bytes */
	u32 ramdisk_addr; /* physical load addr */

	u32 second_size;  /* size in bytes */
	u32 second_addr;  /* physical load addr */

	u32 tags_addr;    /* physical addr for kernel tags */
	u32 page_size;    /* flash page size we assume */
	u32 unused[2];    /* future expansion: should be 0 */

	u8 name[ANDR_BOOT_NAME_SIZE]; /* asciiz product name */

	u8 cmdline[ANDR_BOOT_ARGS_SIZE];

	u32 id[8]; /* timestamp / checksum / sha1 / etc */
};
#endif

struct spl_image_info {
	const char *name;
	u8 os;
	u32 load_addr;
	u32 entry_point;
	u32 size;
	u32 flags;
};

#define SPL_COPY_PAYLOAD_ONLY	1

extern struct spl_image_info spl_image;
extern struct boot_img_hdr spl_kernel_boot;

/* SPL common functions */
void preloader_console_init(void);
u32 spl_boot_device(void);
u32 spl_boot_mode(void);
void spl_parse_image_header(const struct image_header *header);
void spl_board_prepare_for_linux(void);
void __noreturn jump_to_image_linux(void *arg);
int spl_start_uboot(void);
void spl_display_print(void);

/* NAND SPL functions */
void spl_nand_load_image(void);

/* OneNAND SPL functions */
void spl_onenand_load_image(void);

/* NOR SPL functions */
void spl_nor_load_image(void);

/* MMC SPL functions */
void spl_mmc_init(struct mmc **mmc);
void spl_mmc_load_image(void);
void spl_mmc_load_image_raw(struct mmc *mmc, u8 image_type);

/* YMODEM SPL functions */
void spl_ymodem_load_image(void);

/* SPI SPL functions */
void spl_spi_load_image(void);

/* Ethernet SPL functions */
void spl_net_load_image(const char *device);
#ifdef CONFIG_SPL_BOARD_INIT
void spl_board_init(void);
#endif
#endif
