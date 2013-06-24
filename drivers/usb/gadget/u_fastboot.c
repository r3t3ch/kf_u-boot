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
#include "g_fastboot.h"

/* The 64 defined bytes plus \0 */
#define RESPONSE_LEN	(64 + 1)

struct fastboot_config fb_cfg;

static unsigned int download_size;
static unsigned int download_bytes;
static void* read_buffer;

#define DEBUG
#ifdef DEBUG
#define DBG(x...) printf(x)
#else
#define DBG(x...)
#endif /* DEBUG */

int do_mmcops(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
static int fastboot_erase(const char *partition);
static int fastboot_flash(const char *cmdbuf);


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

static void cb_getvar(struct usb_ep *ep, struct usb_request *req)
{
	char *cmd = req->buf;
	char response[RESPONSE_LEN];
	const char *s;

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

		s = fb_find_usb_string(FB_STR_PROC_TYPE_IDX);
		if (s)
			strncat(response, s, sizeof(response));
		else
			strcpy(response, "FAILValue not set");
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
	} else
		req->length = rx_bytes_expected();

	if (download_bytes && !(download_bytes % BYTES_PER_DOT)) {
		printf(".");
		if (!(download_bytes % (74 * BYTES_PER_DOT)))
				printf("\n");

	}
	req->actual = 0;
	usb_ep_queue(ep, req, 0);
}

static void cb_download(struct usb_ep *ep, struct usb_request *req)
{
	char *cmd = req->buf;
	char response[RESPONSE_LEN];

	strsep(&cmd, ":");
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
		printf("req->length %d\n",req->length);
	}
	fastboot_tx_write_str(response);
}

static char boot_addr_start[32];
static char *bootm_args[] = { "bootm", boot_addr_start, NULL };

static void do_bootm_on_complete(struct usb_ep *ep, struct usb_request *req)
{
	req->complete = NULL;
	fastboot_shutdown();
	printf("Booting kernel..\n");

	do_bootm(NULL, 0, 2, bootm_args);

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

static void cb_oem(struct usb_ep *ep, struct usb_request *req)
{    
	if(fastboot_oem(req->buf + 4) == 0)
		fastboot_tx_write_str("OKAY");
	else {
		fastboot_tx_write_str("FAIL: Unable to create partitions");
	}
	return;
}

static void cb_flash(struct usb_ep *ep, struct usb_request *req)
{    
    fastboot_flash(req->buf + 6);
    fastboot_tx_write_str("OKAY");
    return;
}

static void cb_erase(struct usb_ep *ep, struct usb_request *req)
{
	if(fastboot_erase(req->buf + 6) != 0) {
		printf("Unable to erase partition\n");
	}
}

static struct cmd_dispatch_info cmd_dispatch_info[] = {
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


static int fastboot_flash(const char *cmdbuf)
{
	int status = 0;
	struct fastboot_ptentry *ptn;	
	char source[32], dest[32], length[32];	
	char *dev[3] = { "mmc", "dev", "1" };
	char *mmc_write[5]  = {"mmc", "write", NULL, NULL, NULL};
	char *mmc_init[2] = {"mmc", "rescan",};

	/* Next is the partition name */
	ptn = fastboot_flash_find_ptn("boot");

	if (ptn == 0) {
		printf("Partition:[%s] does not exist\n", cmdbuf);
		fastboot_tx_write_str("FAIL:Partition does not exist");
	} else if ((download_bytes> ptn->length) &&
					!(ptn->flags & FASTBOOT_PTENTRY_FLAGS_WRITE_ENV)) {
		printf("Image too large for the partition\n");
		fastboot_tx_write_str("FAIL: image too large for partition");
	} else {
		source[0] = '\0';
		dest[0] = '\0';
		length[0] = '\0';

		printf("writing to partition '%s'\n", ptn->name);

		mmc_write[2] = source;
		mmc_write[3] = dest;
		mmc_write[4] = length;

		sprintf(source, "0x%x", fb_cfg.transfer_buffer);
		sprintf(dest, "0x%x", ptn->start);
		sprintf(length, "0x%x", (download_bytes/512) + 1);

		printf("Setting current mmc device to 1\n");
		status = do_mmcops(NULL, 0, 3, dev); 
		if(status) {	
			printf("Unable to set MMC device\n");
			fastboot_tx_write_str("FAIL: init of MMC");
			goto exit;
		}

		printf("Initializing '%s'\n", ptn->name);
		status = do_mmcops(NULL, 0, 2, mmc_init);
		if(status) {
			fastboot_tx_write_str("FAIL:Init of MMC card");
			goto exit;
		}
		else {
			printf("Writing '%s'\n", ptn->name);
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
	char start[32], length[32];
	char *dev[3] = { "mmc", "dev", "1" };
	char *erase[4]	= { "mmc", "erase", NULL, NULL, };
	char *mmc_init[2] = {"mmc", "rescan",};

	/* Find the partition and erase it */
	printf("Finding partition %s\n",partition);
	ptn = fastboot_flash_find_ptn("boot");

	if (ptn == 0) {
		printf("Partition does not exist");
		fastboot_tx_write_str("FAIL: partition doesn't exist");
		status = -1;
	} else {

		erase[2] = start;
		erase[3] = length;

		sprintf(length, "0x%x", ptn->length);
		sprintf(start, "0x%x", ptn->start);

		printf("Setting current mmc device to 1\n");
		status = do_mmcops(NULL, 0, 3, dev); 
		if(status) {	
			printf("Unable to set MMC device\n");
			fastboot_tx_write_str("FAIL: init of MMC");
			goto exit;
		}

		printf("Initializing '%s'\n", ptn->name);
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
	//DBG("cmd buff %s %s\n",__func__,cmdbuf);
    
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
