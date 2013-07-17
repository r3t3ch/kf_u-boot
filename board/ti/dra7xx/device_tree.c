/*
* Copyright (C) 2012 Texas Instruments, Inc.
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


#include <bootimg.h>
#include <configs/dra7xx_evm.h>
#include <malloc.h>
#include <device_tree.h>
#include <usb/fastboot.h>
#include <mmc.h>

#ifdef DEBUG
#define DBG(x...) printf(x)
#else
#define DBG(x...)
#endif /* DEBUG */

struct device_tree_data {
	struct fastboot_ptentry *pte;
	int dev_tree_sz;
	u32 dev_tree_load_addr;
	int page_size;
};

static struct device_tree_data *dt_data;
static int find_dev_tree();

/**
 * DOC: Introduction
 * device_tree.c - Device tree support
**/

/**
 * find_dev_tree(void) - Find the device tree.
 *
 * This API will search multiple partitions looking for a device tree.
 * The search order is:
 * 1.  Look for a partition called device tree.  This would just be the
 *     device tree blob and will not have any associated header.  So whatever
 *     the contents is here is assumed to be a device tree.
 *
 * 2.  Look for an enviroment partition.  This partition may contain a device
 *     tree.  So if we find the partition there may or may not be a device tree
 *     contained within this partition.  Only if the device tree size is
 *     populated in the enviroment structure do we actually say we have a device
 *     tree.
 *
 * 3.  Finally look in the boot partition at the second data section.  The
 *     boot image may have contain a second data section.  Like the enviroment
 *     partition is the size is valid then we assume it to be device tree.
 *
 * Of course the code is not smart enough to actually tell if the data that
 * exists is a device tree.  But patches welcome for differentiation.
 *
 * Returns 0 if the device tree is found or -1 if no device tree is found.
 **/
 
static int find_dev_tree()
{
	struct fastboot_ptentry *pte;
	dt_data = (void *) malloc(sizeof(struct device_tree_data));
	if (dt_data == NULL) {
		printf("unable to allocate memory requested: dt_data\n");
		return -1;
	}

	pte = fastboot_flash_find_ptn("environment");
	if (pte) {
		dt_data->pte = pte;
		dt_data->page_size = pte->length;
		dt_data->dev_tree_sz = pte->length;
		dt_data->dev_tree_load_addr = DEVICE_TREE;
		return 1;
	}
	return -1;
}

/**
 * load_dev_tree(void) - Load the device tree if found.
 *
 * If a device tree is found within a partition then it is loaded into
 * the device tree load address.
 *
 * The load address depends on the partition it was found in:
 * Default is #define DEVICE_TREE for device_tree partition
 * where the device tree is just the compiled binary or undefined.
 * Otherwise the address is read from the associated header.
 *
 * Returns the load addres in memory of the device tree.
 **/
u32 load_dev_tree(u32 atag_load_addr)
{
	int ret = 0;
	int sector;
	int num_sectors;
	int sector_sz = 0;
	u32 dt_load_addr = 0;
	struct mmc* mmc = NULL;
	int status;

	ret = find_dev_tree();
	if (ret < 0) {
		printf("%s: Device tree not supported\n", __func__);
		dt_data->dev_tree_load_addr = atag_load_addr;
		goto out;
	}
	sector_sz = 512;
	sector = dt_data->pte->start;

	num_sectors = (dt_data->dev_tree_sz/sector_sz);
	if (num_sectors <= (dt_data->dev_tree_sz / sector_sz))
		num_sectors = (dt_data->dev_tree_sz / sector_sz);

	mmc = find_mmc_device(1);
	if(mmc == NULL) {
		goto out;
	}
	
	status = mmc_init(mmc);
	if(status != 0) {
		printf("mmc init failed\n");
		goto out;
	}
	
	status = mmc->block_dev.block_read(1,sector,num_sectors,(void *)dt_data->dev_tree_load_addr);
	if(status < 0) {
		printf("mmc read failed\n");
		goto out;
	}

	DBG("dev_tree @ %08x (%d)\n",
		dt_data->dev_tree_load_addr,
		dt_data->dev_tree_sz);

out:
	if (dt_data->dev_tree_load_addr) {		
		dt_load_addr = dt_data->dev_tree_load_addr;
	}

	free(dt_data);
	return dt_load_addr;

}
