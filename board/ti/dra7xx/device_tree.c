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
//#include <configs/dra7xx_evm.h>
#include <malloc.h>

//#include <boot_settings.h>
#include <device_tree.h>
#include <usb/fastboot.h>
#include <mmc.h>
//#include <alloc.h>

#define MEMORY_BASE			0x80000000
#define CONFIG_ADDR_ATAGS		(MEMORY_BASE + 0x100)
#define CONFIG_ADDR_DOWNLOAD		(MEMORY_BASE + 0x02000000)

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
 
void printPacketBuffer3(void *buffer, unsigned int length)
{
    printf("*************************************************\n");
    unsigned int i=0;
    char *c;
    unsigned int limit = length;
	int line = 0;
    for (i=0; i<limit; i++) {
        c = (char *)(buffer+i);
        if ( *c != '\0' ) {
            printf("%c  ", *c,i);
			line++;
			if(line == 100) {
				line = 0;
				printf("\n");
			}
        }else {
        //printf("null\n");
        }
    }
}
static int find_dev_tree()
{

	struct fastboot_ptentry *pte;
	enviro_img_hdr *env_hdr;
	boot_img_hdr *boot_hdr;
	int ret = 0;
	u64 num_sectors = 0;
	int sector_sz = 0;
	u32 addr = CONFIG_ADDR_DOWNLOAD;
	char *dev[3] = { "mmc", "dev", "1" };
	char source[32], dest[32], length[32];

	char *mmc_read[5]  = {"mmc", "read", NULL, NULL, NULL};
	char *mmc_init[2] = {"mmc", "rescan",};

	sector_sz = 512;

	dt_data = (void *) malloc(sizeof(struct device_tree_data));
	if (dt_data == NULL) {
		printf("unable to allocate memory requested: dt_data\n");
		return -1;
	}

	pte = fastboot_flash_find_ptn("environment");
	if (pte) {
		printf("found2\n");
		dt_data->pte = pte;
		dt_data->page_size = pte->length;
		dt_data->dev_tree_sz = pte->length;
		dt_data->dev_tree_load_addr = DEVICE_TREE;
		goto out;
	}

	printf("Find partition environment\n");
	pte = fastboot_flash_find_ptn("environment");
	if (pte) {
		printf("found\n");
		env_hdr = (enviro_img_hdr *)addr;
		num_sectors =  sizeof(enviro_img_hdr) / sector_sz;
		if (num_sectors <= 0)
			num_sectors = 1;

		if(do_mmcops(NULL, 0, 3, dev)) {
			printf("Unable to set MMC device\n");
			return -1;
		}
		
		if (do_mmcops(NULL, 0, 2, mmc_init)) {
			printf("FAIL:Init of MMC card..");
			return -1;
		}

		mmc_read[2] = source;
		mmc_read[3] = dest;
		mmc_read[4] = length;

		sprintf(source, "0x%x", env_hdr);
		sprintf(dest, "0x%x", pte->start);
		sprintf(length, "0x%x", num_sectors);

		if (do_mmcops(NULL, 0, 5, mmc_read)) {
			printf("Reading boot magic FAILED!\n");
			return -1;
		}
		printPacketBuffer3(env_hdr,512);

	//		ret = boot_ops->storage_ops->read(pte->start, num_sectors,
	//							(void *) env_hdr);


		ret = memcmp(env_hdr->magic, ENVIRO_MAGIC, ENVIRO_MAGIC_SIZE);
		if (ret != 0) {
			printf("%s: bad enviroment magic\n", __func__);
			goto out;
		}

		if (env_hdr->dev_tree_size) {
			printf("Found device tree\n");
			dt_data->pte = pte;
			dt_data->page_size = env_hdr->page_size;
			dt_data->dev_tree_sz = env_hdr->dev_tree_size;
			dt_data->dev_tree_load_addr = env_hdr->dev_tree_addr;
			goto out;
		}
		ret = -1;
		goto out;
	}

out:
	return ret;

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
	u32 dt_load_addr;
	char *dev[3] = { "mmc", "dev", "1" };
	struct mmc* mmc;


	printf("%s\n",__func__);
	ret = find_dev_tree();
	if (ret < 0) {
		printf("%s: Device tree not supported\n", __func__);
		dt_data->dev_tree_load_addr = atag_load_addr;
		goto out;
	}
	sector_sz = 512;
	sector = dt_data->pte->start + (dt_data->page_size / sector_sz);

	num_sectors = (dt_data->dev_tree_sz, sector_sz);
	if (num_sectors <= (dt_data->dev_tree_sz / sector_sz))
		num_sectors = (dt_data->dev_tree_sz / sector_sz);

	mmc = find_mmc_device(1);
	mmc_init(mmc);
	mmc->block_dev.block_read(1,sector,num_sectors,(void *)dt_data->dev_tree_load_addr);

//	ret = boot_ops->storage_ops->read(sector, num_sectors,
//					(void *)dt_data->dev_tree_load_addr);

	printf("dev_tree @ %08x (%d)\n",
		dt_data->dev_tree_load_addr,
		dt_data->dev_tree_sz);

out:
	if (dt_data->dev_tree_load_addr) {		
		dt_load_addr = dt_data->dev_tree_load_addr;
		printf("dt_load_addr=======%u 0x%x\n",dt_load_addr,dt_load_addr);
	}
	else
		return -1;

	free(dt_data);

	return dt_load_addr;

}
