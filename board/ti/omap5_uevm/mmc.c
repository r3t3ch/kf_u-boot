/*
 * Copyright (c) 2010, The Android Open Source Project.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Neither the name of The Android Open Source Project nor the names
 *    of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written
 *    permission.
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
 *
 */

#include <common.h>
#include <mmc.h>
#include <malloc.h>
#include <usb/fastboot.h>
#include <asm/arch/mmc_host_def.h>

#define EFI_VERSION 0x00010000
#define EFI_ENTRIES 128
#define EFI_NAMELEN 36

#ifdef DEBUG
#define DBG(x...) printf(x)
#else
#define DBG(x...)
#endif /* DEBUG */

int load_ptbl();

static const u8 partition_type[16] = {
	0xa2, 0xa0, 0xd0, 0xeb, 0xe5, 0xb9, 0x33, 0x44,
	0x87, 0xc0, 0x68, 0xb6, 0xb7, 0x26, 0x99, 0xc7,
};

static const u8 random_uuid[16] = {
	0xff, 0x1f, 0xf2, 0xf9, 0xd4, 0xa8, 0x0e, 0x5f,
	0x97, 0x46, 0x59, 0x48, 0x69, 0xae, 0xc3, 0x4e,
};

	
struct efi_entry {
	u8 type_uuid[16];
	u8 uniq_uuid[16];
	u64 first_lba;
	u64 last_lba;
	u64 attr;
	u16 name[EFI_NAMELEN];
};

struct efi_header {
	u8 magic[8];

	u32 version;
	u32 header_sz;

	u32 crc32;
	u32 reserved;

	u64 header_lba;
	u64 backup_lba;
	u64 first_lba;
	u64 last_lba;

	u8 volume_uuid[16];

	u64 entries_lba;

	u32 entries_count;
	u32 entries_size;
	u32 entries_crc32;
} __attribute__((packed));

struct ptable {
	u8 mbr[512];
	union {
		struct efi_header header;
		u8 block[512];
	};
	struct efi_entry entry[EFI_ENTRIES];	
};

static void init_mbr(u8 *mbr, u32 blocks)
{
	mbr[0x1be] = 0x00; /* nonbootable */
	mbr[0x1bf] = 0xFF; /* bogus CHS */
	mbr[0x1c0] = 0xFF;
	mbr[0x1c1] = 0xFF;

	mbr[0x1c2] = 0xEE; /* GPT partition */
	mbr[0x1c3] = 0xFF; /* bogus CHS */
	mbr[0x1c4] = 0xFF;
	mbr[0x1c5] = 0xFF;

	mbr[0x1c6] = 0x01; /* start */
	mbr[0x1c7] = 0x00;
	mbr[0x1c8] = 0x00;
	mbr[0x1c9] = 0x00;

	blocks = (blocks > 0xFFFFFFFF) ? 0xFFFFFFFF : blocks;
	memcpy(mbr + 0x1ca, &blocks, sizeof(u32));

	mbr[0x1fe] = 0x55;
	mbr[0x1ff] = 0xaa;

    
}
static void start_ptbl(struct ptable *ptbl, u64 blocks)
{
	struct efi_header *hdr = &ptbl->header;

	DBG("start_ptbl\n");

	memset(ptbl, 0, sizeof(*ptbl));

	init_mbr(ptbl->mbr, blocks - 1);

	memcpy(hdr->magic, "EFI PART", 8);
	hdr->version = EFI_VERSION;
	hdr->header_sz = sizeof(struct efi_header);
	hdr->crc32 = 0;
	hdr->reserved = 0;
	hdr->header_lba = 1;
	hdr->backup_lba = blocks - 1;
	hdr->first_lba = 34;
	hdr->last_lba = blocks - 1;
	memcpy(hdr->volume_uuid, random_uuid, 16);
	hdr->entries_lba = 2;
	hdr->entries_count = EFI_ENTRIES;
	hdr->entries_size = sizeof(struct efi_entry);
	hdr->entries_crc32 = 0;

	DBG("magic		= %s \n",  hdr->magic);
	DBG("version		= %u \n",  hdr->version);
	DBG("header_sz	= %u \n",  hdr->header_sz);
	DBG("crc32		= %u \n",  hdr->crc32);
	DBG("reserved	= %u \n",  hdr->reserved);
	DBG("header_lba	= %llu \n",  hdr->header_lba);
	DBG("backup_lba	= %llu \n",  hdr->backup_lba);
	DBG("first_lba	= %llu \n",  hdr->first_lba);
	DBG("last_lba	= %llu \n",  hdr->last_lba);
	DBG("entries_lba	= %llu \n",  hdr->entries_lba);
	DBG("entries_count	= %u \n",  hdr->entries_count);
	DBG("entries_size	= %u \n",  hdr->entries_size);
	DBG("entries_crc32	= %u \n",  hdr->entries_crc32);
}

static void end_ptbl(struct ptable *ptbl)
{
	struct efi_header *hdr = &ptbl->header;
	u32 n;

	n = crc32(0, 0, 0);
	n = crc32(n, (void*) ptbl->entry, sizeof(ptbl->entry));
	hdr->entries_crc32 = n;

	n = crc32(0, 0, 0);
	n = crc32(0, (void*) &ptbl->header, sizeof(ptbl->header));
	hdr->crc32 = n;
}

static int add_ptn(struct ptable *ptbl, u64 first, u64 last, const char *name)
{
	struct efi_header *hdr = &ptbl->header;
	struct efi_entry *entry = ptbl->entry;
	u32 n; int i = 0;

	if (first < 34) {
		printf("partition '%s' overlaps partition table\n", name);
		return -1;
	}

	if (last > hdr->last_lba) {
		printf("partition '%s' does not fit\n", name);
		return -1;
	}

	for (n = 0; n < EFI_ENTRIES; n++, entry++) {
		if (entry->last_lba)
			continue;
		memcpy(entry->type_uuid, partition_type, 16);
		memcpy(entry->uniq_uuid, random_uuid, 16);
		entry->uniq_uuid[0] = n;
		entry->first_lba = first;
		entry->last_lba = last;

		/* Converting partition name to simple unicode
		as expected by the kernel */
		while (i <= EFI_NAMELEN && *name) {
			entry->name[i] = name[i];
			if (name[i] == 0)
				break;
			entry->name[i+1] = '0';
			i++;
		}

		return 0;
	}

	return -1;
}

static void import_efi_partition(struct efi_entry *entry, int count)
{
	struct fastboot_ptentry e;
	int ret = 0;
	int n;

	ret = memcmp(entry->type_uuid, partition_type, sizeof(partition_type));
	if (ret != 0) {
		//DBG("memcmp failed for count=%d, ret = %d. entry->type_uuid "
		//    "and partition_type are mismatched.\n", count, ret);
		return;
	}

	for (n = 0; n < (sizeof(e.name)-1); n++) {
		e.name[n] = entry->name[n];
	}
	e.name[n] = 0;


	e.start = entry->first_lba;
	e.length = (entry->last_lba - entry->first_lba + 1) * MMCSD_SECTOR_SIZE;
	e.flags = 0;

	if (!strcmp(e.name, "environment"))
		e.flags |= FASTBOOT_PTENTRY_FLAGS_WRITE_ENV;
	
	fastboot_flash_add_ptn(&e, count);

	
#ifdef DEBUG
	if (e.length > 0x100000)
		DBG("%8d %7dM %s\n", e.start,
			(u32)(e.length/0x100000), e.name);
	else
		DBG("%8d %7dK %s\n", e.start,
			(u32)(e.length/0x400), e.name);
#endif
	
}

int load_ptbl()
{
	u64 ptbl_sectors = 0;
	int i = 0, r = 0;

	struct ptable *gpt;
	struct mmc* mmc = NULL;
	
	mmc = find_mmc_device(1);
	if(mmc == NULL) {
		printf("No MMC in slot 1\n");
		return -1;
	}
	mmc->has_init = 0;
	mmc_init(mmc);
	if(r!= 0) {
		printf("mmc init failed\n");
		return r;
	}
	
	int gpt_size = sizeof(struct ptable);
	
	gpt =  (struct ptable *) malloc(gpt_size);
	if (!gpt) {
		r = -1;
		goto fail;
	}

	ptbl_sectors = (u64)(gpt_size / MMCSD_SECTOR_SIZE);

	r = mmc->block_dev.block_read(1,0,ptbl_sectors,(void*)gpt);
	if (r == -1) {
		printf("error reading GPT\n");
		goto fail;
	}

	if (memcmp(gpt->header.magic, "EFI PART", 8)) {
		printf("efi partition table not found\n");
		r = -1;
		goto fail;
	}

	for (i = 0; i < EFI_ENTRIES; i++)
		import_efi_partition(&gpt->entry[i], i);

fail:
	free((void *)gpt);
	return r;
}


struct _partition {
	const char *name;
	unsigned size_kb;
};

static struct _partition partitions[] = {
	{ "-", 128 },
	{ "bootloader", 256 },
	{ "environment", 256 },
	/* "misc" partition is required for recovery */
	{ "misc", 128 },
	{ "-", 384 },
	{ "efs", 16384 },
	{ "crypto", 16 },
	{ "recovery", 8*1024 },
	{ "boot", 8*1024 },
	{ "system", 512*1024 },
	{ "cache", 256*1024 },
	{ "userdata", 0},
	{ NULL, 0 },
};

static int do_format(void)
{
	struct ptable *ptbl;
	unsigned sector_sz, blocks;
	unsigned next;
	int n;
	struct mmc* mmc = NULL;
	int status = 0;
	u64 ptbl_sectors = 0;

	mmc = find_mmc_device(1);
	if(mmc == NULL) {
		return -1;
	}
	mmc->has_init = 0;
	status = mmc_init(mmc);
	if(status != 0) {
		printf("mmc init failed\n");
		return status;
	}

	blocks = mmc->block_dev.lba;
	sector_sz = mmc->block_dev.blksz;
		
	
	ptbl = (struct ptable *) malloc(sizeof(struct ptable));

	start_ptbl(ptbl, blocks);    
	n = 0;
	next = 0;
	for (n = 0, next = 0; partitions[n].name; n++) {
		unsigned sz = partitions[n].size_kb * 2;
		if (!strcmp(partitions[n].name,"-")) {
			next += sz;
			continue;
		}
		if (sz == 0)
			sz = blocks - next;

	    if (add_ptn(ptbl, next, next + sz - 1, partitions[n].name)) {
	        printf("Add partition failed\n");
			status = -1;
	        goto fail;	        
		}
		next += sz;
	}
	end_ptbl(ptbl);
    

	fastboot_flash_reset_ptn();     
       
	ptbl_sectors = (sizeof(struct ptable) / sector_sz) + 1;
	mmc->block_dev.block_write(1,0,ptbl_sectors,(void*)ptbl);

	load_ptbl();
    
fail:
	free(ptbl);
	return status;
}


int fastboot_oem(const char *cmd)
{
	if (!strcmp(cmd,"format")) {
		return do_format();
	}
	return -1;
}

int board_mmc_ftbtptn_init(void)
{
	return load_ptbl();
}


