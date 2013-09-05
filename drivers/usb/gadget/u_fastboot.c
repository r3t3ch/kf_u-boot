/*
 * (C) Copyright 2008 - 2009
 * Windriver, <www.windriver.com>
 * Tom Rix <Tom.Rix@windriver.com>
 *
 * Copyright (c) 2011 Sebastian Andrzej Siewior <bigeasy@linutronix.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 * Part of the rx_handler were copied from the Android project.
 * Specifically rx command parsing in the  usb_rx_data_complete
 * function of the file bootable/bootloader/legacy/usbloader/usbloader.c
 *
 * The logical naming of flash comes from the Android project
 * Thse structures and functions that look like fastboot_flash_*
 * They come from bootable/bootloader/legacy/libboot/flash.c
 *
 * This is their Copyright:
 *
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include <common.h>
#include <usb/fastboot.h>
#include <linux/usb/ch9.h>
#include <linux/usb/gadget.h>
#include <sparse.h>
#include <config.h>
#include <bootimg.h>
#include <mmc.h>
#include <malloc.h>
#include <asm/io.h>
#include <asm-generic/gpio.h>
#include "g_fastboot.h"

/* The 64 defined bytes plus \0 */
#define RESPONSE_LEN	(64 + 1)
#define CONTROL_STATUS	0x4A002134
#define DEVICETYPE_MASK	 (0x7 << 6)
#define FORMAT_LEN 7

/* omap-type */
typedef enum {
	OMAP_TYPE_TEST,
	OMAP_TYPE_EMU,
	OMAP_TYPE_SEC,
	OMAP_TYPE_GP,
	OMAP_TYPE_BAD,
} omap_type;



struct fastboot_config fb_cfg;

static unsigned int download_size;
static unsigned int download_bytes;

#ifdef DEBUG
#define DBG(x...) printf(x)
#else
#define DBG(x...)
#endif /* DEBUG */

int do_mmcops(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
static int fastboot_erase(const char *partition);
static int fastboot_flash(const char *cmdbuf);
static int fastboot_update_zimage(void);


fastboot_ptentry *fastboot_flash_find_ptn(const char *name);



static int fastboot_tx_write_str(const char *buffer)
{
	return fastboot_tx_write(buffer, strlen(buffer));
}

static void compl_do_reset(struct usb_ep *ep, struct usb_request *req)
{
	do_reset(NULL, 0, 0, NULL);
}

static void cb_reboot(struct usb_ep *ep, struct usb_request *req)
{
	req_in->complete = compl_do_reset;
	fastboot_tx_write_str("OKAY");
}

static int strcmp_l1(const char *s1, const char *s2)
{
	return strncmp(s1, s2, strlen(s1));
}

static char *get_cpu_type(void)
{
	static char proc_type[8];
	unsigned int value;

	value = readl(CONTROL_STATUS);
	value &= DEVICETYPE_MASK;
	
	switch (value >> 6) {
	case OMAP_TYPE_EMU:
		strcpy(proc_type, "EMU");
		break;
	case OMAP_TYPE_SEC:
		strcpy(proc_type, "HS");
		break;
	case OMAP_TYPE_GP:
		strcpy(proc_type, "GP");
		break;
	default:
		strcpy(proc_type, "unknown");
		break;
	}

	return proc_type;
}

static void cb_getvar(struct usb_ep *ep, struct usb_request *req)
{
	char *cmd = req->buf;
	char response[RESPONSE_LEN];
	const char *s;
	char userdata_sz[64];

	strcpy(response, "OKAY");
	strsep(&cmd, ":");
	if (!cmd) {
		fastboot_tx_write_str("FAILmissing var");
		return;
	}

	if (!strcmp_l1("version", cmd)) {
		strncat(response, FASTBOOT_VERSION, sizeof(response));

	} else if (!strcmp_l1("downloadsize", cmd)) {
		char str_num[12];

		sprintf(str_num, "%08x", fb_cfg.transfer_buffer_size);
		strncat(response, str_num, sizeof(response));

	} else if (!strcmp_l1("product", cmd)) {

		s = fb_find_usb_string(FB_STR_PRODUCT_IDX);
		if (s)
			strncat(response, s, sizeof(response));
		else
			strcpy(response, "FAILValue not set");

	} else if (!strcmp_l1("serialno", cmd)) {

		s = fb_find_usb_string(FB_STR_SERIAL_IDX);
		if (s)
			strncat(response, s, sizeof(response));
		else
			strcpy(response, "FAILValue not set");

	} else if (!strcmp_l1("cpurev", cmd)) {

		s = fb_find_usb_string(FB_STR_PROC_REV_IDX);
		if (s)
			strncat(response, s, sizeof(response));
		else
			strcpy(response, "FAILValue not set");
	} else if (!strcmp_l1("secure", cmd)) {

		s = get_cpu_type();
		if (s)
			strncat(response, s, sizeof(response));
		else
			strcpy(response, "FAILValue not set");
	} else if (!strcmp_l1("userdata_size", cmd)) {
		strncat(response,get_ptn_size(userdata_sz, "userdata"),sizeof(response));

	} else {
		strcpy(response, "FAILVariable not implemented");
	}
	fastboot_tx_write_str(response);
}


static unsigned int rx_bytes_expected(void)
{
	int rx_remain = download_size - download_bytes;
	int rem = 0;
	if (rx_remain < 0) {
		return 0;
	}
	if (rx_remain > EP_BUFFER_SIZE) {
		return EP_BUFFER_SIZE;
	}
	if(rx_remain < 512) {
		rx_remain = 512;
	}else if(rx_remain % 512 != 0) {
		rem = rx_remain % 512;
		rx_remain = rx_remain + (512 - rem);
	}
	return rx_remain;
}

#define BYTES_PER_DOT	32768
static void rx_handler_dl_image(struct usb_ep *ep, struct usb_request *req)
{
	char response[RESPONSE_LEN];
	unsigned int transfer_size = download_size - download_bytes;
	const unsigned char *buffer = req->buf;
	unsigned int buffer_size = req->actual;

	if (req->status != 0) {
		printf("Bad status: %d\n", req->status);
		return;
	}

	if (buffer_size < transfer_size)
		transfer_size = buffer_size;

	memcpy(fb_cfg.transfer_buffer + download_bytes,
			buffer, transfer_size);

	download_bytes += transfer_size;

	/* Check if transfer is done */
	if (download_bytes >= download_size) {
		/*
		 * Reset global transfer variable, keep download_bytes because
		 * it will be used in the next possible flashing command
		 */
		download_size = 0;
		req->complete = rx_handler_command;
		req->length = EP_BUFFER_SIZE;

		sprintf(response, "OKAY");
		fastboot_tx_write_str(response);

		printf("\ndownloading of %d bytes finished\n",
				download_bytes);
	} else {
		req->length = rx_bytes_expected();
		if (req->length < ep->maxpacket)
			req->length = ep->maxpacket;
	}

	/*if (download_bytes && !(download_bytes % BYTES_PER_DOT)) {
		printf(".");
		if (!(download_bytes % (74 * BYTES_PER_DOT)))
				printf("\n");
	
	}*/
	req->actual = 0;
	usb_ep_queue(ep, req, 0);
}

size_t strlcpy(char *dest, const char *src, size_t size)
{
	size_t ret = strlen(src);

	if (size) {
		size_t len = (ret >= size) ? size - 1 : ret;
		memcpy(dest, src, len);
		dest[len] = '\0';
	}
	return ret;
}

static void cb_download(struct usb_ep *ep, struct usb_request *req)
{
	char *cmd = req->buf;
	char response[RESPONSE_LEN];

	strsep(&cmd, ":");

	/*HACK: Zero terminate the command string*/
	strlcpy(cmd,cmd,9);
	download_size = simple_strtoul(cmd, NULL, 16);
	download_bytes = 0;

	printf("Starting download of %d bytes\n",
			download_size);

	if (0 == download_size) {
		sprintf(response, "FAILdata invalid size");
	} else if (download_size >
			fb_cfg.transfer_buffer_size) {
		download_size = 0;
		sprintf(response, "FAILdata too large");
	} else {
		sprintf(response, "DATA%08x", download_size);
		req->complete = rx_handler_dl_image;
		req->length = rx_bytes_expected();
		if (req->length < ep->maxpacket)
			req->length = ep->maxpacket;
	}
	fastboot_tx_write_str(response);
}

static char boot_addr_start[32];

static void do_bootm_on_complete(struct usb_ep *ep, struct usb_request *req)
{
	req->complete = NULL;
	fastboot_shutdown();
	printf("Booting kernel..\n");

	//do_bootm(NULL, 0, 2, bootm_args);

	/* This only happens if image is somehow faulty so we start over */
	do_reset(NULL, 0, 0, NULL);
}

static void cb_boot(struct usb_ep *ep, struct usb_request *req)
{
	sprintf(boot_addr_start, "0x%p", fb_cfg.transfer_buffer);

	req_in->complete = do_bootm_on_complete;
	fastboot_tx_write_str("OKAY");
	return;
}

struct cmd_dispatch_info {
	char *cmd;
	void (*cb)(struct usb_ep *ep, struct usb_request *req);
};

#ifdef CONFIG_SPL_SPI_SUPPORT
int boot_from_spi = 0;
extern int do_spi_flash(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
#endif

static void cb_oem(struct usb_ep *ep, struct usb_request *req)
{    
	char buf[FORMAT_LEN];
#ifdef CONFIG_SPL_SPI_SUPPORT
	char *sf_erase[4] = {"sf", "erase", "0", "10000"};
	int status;
	char *sf_probe[3] = {"sf", "probe", "0"};
	if(strncmp(req->buf + 4,"spi",3) == 0) {
		boot_from_spi = 1;
		status = do_spi_flash(NULL, 0, 3, sf_probe);
		if(status) {
			fastboot_tx_write_str("FAIL:Could not probe SPI");
			return;
		}
		status = do_spi_flash(NULL, 0, 4, sf_erase);
		if(status) {
			fastboot_tx_write_str("FAIL:Could not erase SPI");
			return;
		}
		fastboot_tx_write_str("OKAY");
		return;
	}else if(strncmp(req->buf + 4,"mmc",3) == 0) {
		boot_from_spi = 0;
		fastboot_tx_write_str("OKAY");
		return;
	}
#endif
	strlcpy(buf, req->buf + 4, FORMAT_LEN);
	if (fastboot_oem(buf) == 0)
		fastboot_tx_write_str("OKAY");
	else {
		fastboot_tx_write_str("FAIL: Unable to create partitions");
	}
	return;
}

/*HACK : Fastboot doesn't seem to be null terminating the flash command. Do it ourselves*/
static void format_flash_cmd(char* cmd) 
{
	int i;
	char *parts[] = {"xloader", "bootloader", "boot", "system", "userdata",
		            "cache", "recovery","environment", "zImage", "zimage",
					"misc", "efs", "crypto"};
	for(i = 0;i < 13;i++) {
		if(!strncmp(parts[i],cmd,strlen(parts[i]))) {
			*(cmd + strlen(parts[i])) = '\0';
			break;
		}
	}
	
}

static void cb_flash(struct usb_ep *ep, struct usb_request *req)
{    
	char *cmd = req->buf + 6;
	format_flash_cmd(cmd);
	if (fastboot_flash(cmd) != 0) {
		printf("Unable to flash partition\n");
	}
    return;
}

static void cb_erase(struct usb_ep *ep, struct usb_request *req)
{
	char *cmd = req->buf + 6;
	format_flash_cmd(cmd);
	if(fastboot_erase(cmd) != 0) {
		printf("Unable to erase partition\n");
	}
}

void fastboot_reboot_bootloader(void) {
}

int check_fastboot(void) {
	return 0;
}

static void cb_reboot_bootloader(struct usb_ep *ep, struct usb_request *req)
{
	fastboot_tx_write_str("OKAY");
	fastboot_reboot_bootloader();
}

static struct cmd_dispatch_info cmd_dispatch_info[] = {
	{
		.cmd = "reboot-bootloader",
		.cb  = cb_reboot_bootloader,
	},
	{
		.cmd = "reboot",
		.cb = cb_reboot,
	}, 
	{
		.cmd = "getvar:",
		.cb = cb_getvar,
	}, 
	{
		.cmd = "download:",
		.cb = cb_download,
	}, 
	{
		.cmd = "boot",
		.cb = cb_boot,
	}, 
	{
		.cmd = "flash",
		.cb = cb_flash,
	},
	{
	    .cmd = "oem",
	    .cb = cb_oem,
	},
	{
		.cmd = "erase",
		.cb  = cb_erase,
	}
};

static u32 fastboot_get_boot_ptn(boot_img_hdr *hdr, char *response,
	                                   struct mmc* mmc)
{
	u32 hdr_sectors = 0;
	u32 sector_size;
	struct fastboot_ptentry *pte = NULL;
	int status = 0;
	strcpy(response, "OKAY");

	pte = fastboot_flash_find_ptn("boot");
	if (NULL == pte) {
		strcpy(response, "FAILCannot find boot partition");
		goto out;
	}

	/* Read the boot image header */
	sector_size = mmc->block_dev.blksz;
	hdr_sectors = (sizeof(struct boot_img_hdr)/sector_size) + 1;
	status = mmc->block_dev.block_read(1, pte->start,
			hdr_sectors, (void *)hdr);

	if (status < 0) {
		strcpy(response, "FAILCannot read hdr from boot partition");
		goto out;
	}
	if (memcmp(hdr->magic, BOOT_MAGIC, 8) != 0) {
		printf("bad boot image magic\n");
		strcpy(response, "FAILBoot partition not initialized");
		goto out;
	}

	return hdr_sectors;

out:
	strcpy(response, "INFO");
	fastboot_tx_write_str(response);

	return -1;
}

#define CEIL(a, b) (((a) / (b)) + ((a % b) > 0 ? 1 : 0))

static int fastboot_update_zimage(void)
{
	boot_img_hdr *hdr = NULL;
	u8 *ramdisk_buffer;
	u32 ramdisk_sector_start, ramdisk_sectors;
	u32 kernel_sector_start, kernel_sectors;
	u32 hdr_sectors = 0;
	u32 sectors_per_page = 0;
	int ret = 0;
	struct mmc* mmc = NULL;
	struct fastboot_ptentry *pte = NULL;
	char response[128];
	u32 addr = CONFIG_ADDR_DOWNLOAD;

	strcpy(response, "OKAY");
	printf("Flashing zImage...%d bytes\n", download_bytes);

	mmc = find_mmc_device(1);
	if (mmc == NULL) {
		strcpy(response, "FAILCannot find mmc at slot 1");
		goto out;
	}
	if (mmc_init(mmc) != 0) {
		strcpy(response, "FAILCannot init mmc at slot 1");
		goto out;
	}

	hdr = (boot_img_hdr *) addr;

	hdr_sectors = fastboot_get_boot_ptn(hdr, response, mmc);
	if (hdr_sectors <= 0) {
		sprintf(response + strlen(response),
			"FAILINVALID number of boot sectors %d", hdr_sectors);
		ret = -1;
		goto out;
	}
	pte = fastboot_flash_find_ptn("boot");
	if (pte == NULL) {
		sprintf(response + strlen(response),
			"FAILINVALID partition");
		ret = -1;
		goto out;
	}

	/* Extract ramdisk location and read it into local buffer */
	sectors_per_page = hdr->page_size / mmc->block_dev.blksz;
	ramdisk_sector_start = pte->start + sectors_per_page;
	ramdisk_sector_start += CEIL(hdr->kernel_size, hdr->page_size)*
						sectors_per_page;
	ramdisk_sectors = CEIL(hdr->ramdisk_size, hdr->page_size)*
						sectors_per_page;

	ramdisk_buffer = (u8 *)hdr;
	ramdisk_buffer += (hdr_sectors * mmc->block_dev.blksz);
	ret = mmc->block_dev.block_read(1, ramdisk_sector_start,
		ramdisk_sectors, ramdisk_buffer);
	if (ret < 0) {
		sprintf(response, "FAILCannot read ramdisk from boot "
								"partition");
		ret = -1;
		goto out;
	}

	/* Change the boot img hdr */
	hdr->kernel_size = download_bytes;
	ret = mmc->block_dev.block_write(1, pte->start,
		hdr_sectors, (void *)hdr);
	if (ret < 0) {
		sprintf(response, "FAILCannot writeback boot img hdr");
		ret = -1;
		goto out;
	}

	/* Write the new downloaded kernel*/
	kernel_sector_start = pte->start + sectors_per_page;
	kernel_sectors = CEIL(hdr->kernel_size, hdr->page_size)*
					sectors_per_page;
	ret = mmc->block_dev.block_write(1, kernel_sector_start, kernel_sectors,
			fb_cfg.transfer_buffer);
	if (ret < 0) {
		sprintf(response, "FAILCannot write new kernel");
		ret = -1;
		goto out;
	}

	/* Write the saved Ramdisk back */
	ramdisk_sector_start = pte->start + sectors_per_page;
	ramdisk_sector_start += CEIL(hdr->kernel_size, hdr->page_size)*
						sectors_per_page;
	ret = mmc->block_dev.block_write(1, ramdisk_sector_start, ramdisk_sectors,
						ramdisk_buffer);
	if (ret < 0) {
		sprintf(response, "FAILCannot write back original ramdisk");
		ret = -1;
		goto out;
	}
	fastboot_tx_write_str(response);
	return 0;

out:
	fastboot_tx_write_str(response);
	return ret;
}

static int fastboot_flash(const char *partition)
{
	int status = 0;
	struct fastboot_ptentry *ptn;	
	char source[32], dest[32], length[32];	
	char *dev[3] = { "mmc", "dev", "1" };
	char *mmc_write[5]  = {"mmc", "write", NULL, NULL, NULL};
	char *mmc_init[2] = {"mmc", "rescan",};
	
#ifdef CONFIG_SPL_SPI_SUPPORT
	char *sf_probe[3] = {"sf", "probe", "0"};
	char *sf_write_xloader[5] = {"sf", "write", NULL, "0", "10000"};
	char *sf_write_bootloader[5] = {"sf", "write", NULL, "80000", "80000"};
	char *sf_update_bootloader[5] = {"sf", "update", NULL, "80000", "80000"};


	/*Check if this is for xloader or bootloader. Also, check if we have to flash to SPI*/
	if(strcmp(partition,"xloader") == 0 && boot_from_spi) {
		printf("Flashing %s to SPI\n",partition);
		status = do_spi_flash(NULL, 0, 3, sf_probe);
		if(status) {
			fastboot_tx_write_str("FAIL:Could not probe SPI");
			return status;
		}
		sf_write_xloader[2] = source;
		sprintf(source, "0x%x", (unsigned int)fb_cfg.transfer_buffer);
		status = do_spi_flash(NULL, 0, 5, sf_write_xloader);
		if(status) {
			fastboot_tx_write_str("FAIL:Could not write xloader to SPI");
			return status;
		}
		fastboot_tx_write_str("OKAY");
		return 0;
	}
	if(strcmp(partition,"bootloader") == 0 && boot_from_spi) {
		printf("Flashing %s to SPI\n",partition);
		status = do_spi_flash(NULL, 0, 3, sf_probe);
		if(status) {
			fastboot_tx_write_str("FAIL:Could not probe SPI");
			return status;
		}		
		sf_write_bootloader[2] = source;
		sf_update_bootloader[2] = source;
		sprintf(source, "0x%x", (unsigned int)fb_cfg.transfer_buffer);
		printf("Updating bootloader to SPI\n");
		status = do_spi_flash(NULL, 0, 5, sf_update_bootloader);
		if(status) {
			fastboot_tx_write_str("FAIL:Could not update bootloader to SPI");
			return status;
		}		
		printf("Writing bootloader to SPI\n");
		status = do_spi_flash(NULL, 0, 5, sf_write_bootloader);
		if(status) {
			fastboot_tx_write_str("FAIL:Could not write bootloader to SPI");
			return status;
		}		
		fastboot_tx_write_str("OKAY");
		return 0;
	}
#endif
	if (!strcmp(partition, "zImage") || !strcmp(partition, "zimage")) {
		return fastboot_update_zimage();
	}
	ptn = fastboot_flash_find_ptn(partition);
	
	if (ptn == 0) {
		printf("Partition:[%s] does not exist\n", partition);
		fastboot_tx_write_str("FAIL:Partition does not exist");
	} else if ((download_bytes> ptn->length) &&
					!(ptn->flags & FASTBOOT_PTENTRY_FLAGS_WRITE_ENV)) {
		printf("Image too large for the partition\n");
		fastboot_tx_write_str("FAIL: image too large for partition");
	} else {
		source[0] = '\0';
		dest[0] = '\0';
		length[0] = '\0';
 
		mmc_write[2] = source;
		mmc_write[3] = dest;
		mmc_write[4] = length;

		sprintf(source, "0x%x", (unsigned int)fb_cfg.transfer_buffer);
		sprintf(dest, "0x%x", ptn->start);
		sprintf(length, "0x%x", (download_bytes/512) + 1);

 
		status = do_mmcops(NULL, 0, 3, dev); 
		if(status) {	
			printf("Unable to set MMC device\n");
			fastboot_tx_write_str("FAIL: init of MMC");
			goto exit;
		}
 		status = do_mmcops(NULL, 0, 2, mmc_init);
		if(status) {
			fastboot_tx_write_str("FAIL:Init of MMC card");
			goto exit;
		}
		if ( ((sparse_header_t *)fb_cfg.transfer_buffer)->magic
				== SPARSE_HEADER_MAGIC) {
			printf("fastboot: %s is in sparse format %u %u \n", ptn->name, ptn->start, ptn->length);
			status = do_unsparse(fb_cfg.transfer_buffer,
					ptn->start,
					ptn->length);
			if(status) {
				printf("Writing '%s' FAILED!\n", ptn->name);
				fastboot_tx_write_str("FAIL: Write partition");
			} else {	
				printf("Writing sparsed: '%s' DONE!\n", ptn->name);
				printf("Writing '%s' DONE!\n", ptn->name);
				fastboot_tx_write_str("OKAY");
			}				
		} else {
			printf("Writing non-sparsed format '%s'\n", ptn->name);
			status = do_mmcops(NULL, 0, 5, mmc_write);
			if(status) {
				printf("Writing '%s' FAILED!\n", ptn->name);
				fastboot_tx_write_str("FAIL: Write partition");
				goto exit;
			} else {
				printf("Writing '%s' DONE!\n", ptn->name);
				fastboot_tx_write_str("OKAY");
			}
		}

	}
exit:
	return status;
}

static int fastboot_erase(const char *partition)
{
	struct fastboot_ptentry *ptn;
	int status = 0;
	unsigned int sectors;
	char start[32], length[32];
	char *dev[3] = { "mmc", "dev", "1" };
	char *erase[4]	= { "mmc", "erase", NULL, NULL, };
	char *mmc_init[2] = {"mmc", "rescan",};

	/* Find the partition and erase it */
 	ptn = fastboot_flash_find_ptn(partition);

	if (ptn == 0) {
		printf("Partition does not exist");
		fastboot_tx_write_str("FAIL: partition doesn't exist");
		status = -1;
	} else {
		sectors = ptn->length / 512;
		/*Sectors should be aligned to 1024*/
		if (sectors < 0x400) {
			sectors = 0x400;
		}
		sprintf(length, "0x%x", sectors);
		sprintf(start, "0x%x", (ptn->start & ~(0x400 - 1)));
		erase[2] = start;
		erase[3] = length;

 		status = do_mmcops(NULL, 0, 3, dev); 
		if(status) {	
			printf("Unable to set MMC device\n");
			fastboot_tx_write_str("FAIL: init of MMC");
			goto exit;
		}

 		status = do_mmcops(NULL, 0, 2, mmc_init);
		if(status) {	
			fastboot_tx_write_str("FAIL: init of MMC");
			goto exit;
		}

		printf("Erasing '%s'\n", ptn->name);
		status = do_mmcops(NULL, 0, 4, erase);
		if(status) {
			printf("Erasing '%s' FAILED! %d\n", ptn->name,status);
			fastboot_tx_write_str("FAIL: Eraze of partition");
		} else {
			printf("Erasing '%s' DONE!\n", ptn->name);
			fastboot_tx_write_str("OKAY");
		}
		
	fastboot_tx_write_str("OKAY");

	}
exit:	
	return status;
}


void rx_handler_command(struct usb_ep *ep, struct usb_request *req)
{
	char response[RESPONSE_LEN];
	char *cmdbuf = req->buf;
	void (*func_cb)(struct usb_ep *ep, struct usb_request *req) = NULL;
	int i;

	sprintf(response, "FAIL");
	for (i = 0; i < ARRAY_SIZE(cmd_dispatch_info); i++) {
		if (!strcmp_l1(cmd_dispatch_info[i].cmd, cmdbuf)) {
			func_cb = cmd_dispatch_info[i].cb;
			break;
		}
	}

	if (!func_cb)
		fastboot_tx_write_str("FAILunknown command");
	else
		func_cb(ep, req);

	if (req->status == 0) {
        *cmdbuf = '\0';
		req->actual = 0;
		usb_ep_queue(ep, req, 0);
	}
}
