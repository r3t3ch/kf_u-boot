/*
 * (C) Copyright 2010
 * Texas Instruments, <www.ti.com>
 *
 * Aneesh V <aneesh@ti.com>
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
#include <common.h>
#include <spl.h>
#include <asm/u-boot.h>
#include <asm/utils.h>
#include <mmc.h>
#include <fat.h>
#include <version.h>
#include <bootimg.h>

DECLARE_GLOBAL_DATA_PTR;

#define _ALIGN(n,pagesz) ((n + (pagesz - 1)) & (~(pagesz - 1)))

extern void spl_parse_kernel_image_header(const struct boot_img_hdr *hdr);

void spl_mmc_load_image_raw(struct mmc *mmc, u8 image_type)
{
	u32 image_size_sectors, err, src, dest;
	const struct image_header *header;
	u32 boot_device, sys_boot;
	u8 device;
	const u32 read_address[]= {CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_SECTOR,
					CONFIG_SYS_MMCSD_RAW_MODE_KERNEL,
					CONFIG_SYS_MMCSD_RAW_MODE_RAMDISK,
					CONFIG_SYS_MMCSD_RAW_MODE_DTB,
					CONFIG_SYS_MMCSD_RAW_MODE_IPU };

	boot_device = spl_boot_device();
	sys_boot = spl_boot_mode();

	src = read_address[image_type];

	if ((boot_device == BOOT_DEVICE_MMC1) || (sys_boot == SYS_BOOT_QSPI_PROD)){
		device = 0;
	} else {
		device = 1;
	}

	if ((image_type == LOAD_U_BOOT) || (image_type == LOAD_KERNEL)) {
		header = (struct image_header *)(CONFIG_SYS_TEXT_BASE -
							sizeof(struct image_header));
		/* read image header to find the image size & load address */
		err = mmc->block_dev.block_read(device, src, 1, (void *)header);

		if (err <= 0)
			goto end;

		if (image_type == LOAD_U_BOOT) {
			spl_parse_image_header(header);
			dest = spl_image.load_addr;
			image_size_sectors = (spl_image.size + mmc->read_bl_len - 1) /
								mmc->read_bl_len;
#ifndef CONFIG_SPL_EARLY_BOOT
		}
	}
#else
		} else {
			spl_parse_kernel_image_header((const struct boot_img_hdr *)header);
			dest = spl_image.load_addr - spl_kernel_boot.page_size;
			image_size_sectors = (spl_image.size + spl_kernel_boot.page_size
									+ mmc->read_bl_len - 1) /
								mmc->read_bl_len;
		}
	} else if (image_type == LOAD_DTB) {
		dest = DEVICE_TREE;
		image_size_sectors = (DBT_IMAGE_SIZE + mmc->read_bl_len - 1) / mmc->read_bl_len;
#ifdef CONFIG_BOOTIPU1
	} else if ( image_type == LOAD_IPU) {
		dest = IPU_LOAD_ADDR;
		image_size_sectors = (IPU_IMAGE_SIZE + mmc->read_bl_len - 1) / mmc->read_bl_len;
#endif
	} else {
			src +=(_ALIGN(spl_image.size, spl_kernel_boot.page_size) + spl_kernel_boot.page_size)/
							mmc->block_dev.blksz;

			dest = spl_kernel_boot.ramdisk_addr;
			image_size_sectors = (spl_kernel_boot.ramdisk_size +
							mmc->read_bl_len - 1) /
						mmc->read_bl_len;
	}
#endif
	debug("spl_mmc_load_image_raw: src %x size %x dest %x\n", src, image_size_sectors, dest);
	/* Read the header too to avoid extra memcpy */
	err = mmc->block_dev.block_read(device, src, image_size_sectors, (void *)dest);

end:
	if (err <= 0) {
		printf("spl: mmc blk read err - %d\n", err);
		hang();
	}
}

#ifdef CONFIG_SPL_FAT_SUPPORT
static void mmc_load_image_fat(struct mmc *mmc)
{
	s32 err;
	struct image_header *header;

	header = (struct image_header *)(CONFIG_SYS_TEXT_BASE -
						sizeof(struct image_header));

	err = fat_register_device(&mmc->block_dev,
				CONFIG_SYS_MMC_SD_FAT_BOOT_PARTITION);
	if (err) {
		printf("spl: fat register err - %d\n", err);
		hang();
	}

	err = file_fat_read(CONFIG_SPL_FAT_LOAD_PAYLOAD_NAME,
				(u8 *)header, sizeof(struct image_header));
	if (err <= 0)
		goto end;

	spl_parse_image_header(header);

	err = file_fat_read(CONFIG_SPL_FAT_LOAD_PAYLOAD_NAME,
				(u8 *)spl_image.load_addr, 0);

end:
	if (err <= 0) {
		printf("spl: error reading image %s, err - %d\n",
			CONFIG_SPL_FAT_LOAD_PAYLOAD_NAME, err);
		hang();
	}
}
#endif

void spl_mmc_init(struct mmc **mmc)
{
	int err;
	u32 boot_device;
	u8 device;
	boot_device = spl_boot_device();

	if (boot_device == BOOT_DEVICE_MMC1){
		device = 0;
	} else {
		device = 1;
	}

	mmc_initialize(gd->bd);
	/* We register only one device. So, the dev id is always 0 */
	*mmc = find_mmc_device(device);

	if (!*mmc) {
		puts("spl: mmc device not found!!\n");
		hang();
	}

	err = mmc_init(*mmc);
	if (err) {
		printf("spl: mmc init failed: err - %d\n", err);
		hang();
	}
}

void spl_mmc_load_image(void)
{
	struct mmc *mmc = NULL;
	u32 boot_mode;

	spl_mmc_init(&mmc);
	if (!mmc) {
		puts("spl: mmc device not found!!\n");
		hang();
	}

	boot_mode = spl_boot_mode();
	if (boot_mode == MMCSD_MODE_RAW) {
		debug("boot mode - RAW\n");
		spl_mmc_load_image_raw(mmc, LOAD_U_BOOT);
#ifdef CONFIG_SPL_FAT_SUPPORT
	} else if (boot_mode == MMCSD_MODE_FAT) {
		debug("boot mode - FAT\n");
		mmc_load_image_fat(mmc);
#endif
	} else {
		puts("spl: wrong MMC boot mode\n");
		hang();
	}
}
