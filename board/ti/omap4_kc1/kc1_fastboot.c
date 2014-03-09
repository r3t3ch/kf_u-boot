#include <common.h>
#include <asm/arch/sys_proto.h>
#include <usb/fastboot.h>
#include <asm/io.h>
#include <asm-generic/gpio.h>
#include <config.h>
#include <kc1_board.h>

#include "pmic_smb347.h"

char serialno[100];
char manufacturer[100];
char product[100];

/* To support the Android-style naming of flash */
#define MAX_PTN 16

static fastboot_ptentry ptable[MAX_PTN];
static unsigned int pcount = 0;

/*
 * Hardcoded memory region to stash data which comes over USB before it is
 * stored on media
 */
DECLARE_GLOBAL_DATA_PTR;
#define CFG_FASTBOOT_TRANSFER_BUFFER      CONFIG_ADDR_DOWNLOAD
#define CFG_FASTBOOT_TRANSFER_BUFFER_SIZE 0x16000000

int fastboot_board_init(struct cmd_fastboot_interface *interface, char **device_strings)
{
	interface->transfer_buffer = (void *)CFG_FASTBOOT_TRANSFER_BUFFER;
	interface->transfer_buffer_size = CFG_FASTBOOT_TRANSFER_BUFFER_SIZE;
	interface->storage_medium = EMMC;
	debug("*** %s:: transfer_buffer==0x%08x, buffer_size==%u\n", __func__, (unsigned int)interface->transfer_buffer, interface->transfer_buffer_size);

	board_mmc_ftbtptn_init();

	sprintf(serialno, "%s", getenv("idme_serialno"));
	printf("Device Serial Number: %s\n", serialno);
	sprintf(manufacturer, "%s", "Amazon");
	sprintf(product, "%s", "kindle");

	device_strings[DEVICE_STRING_PRODUCT_INDEX] = product;
	device_strings[DEVICE_STRING_SERIAL_NUMBER_INDEX] = serialno;
	device_strings[DEVICE_STRING_MANUFACTURER_INDEX] = manufacturer;

	return 0;
}

fastboot_ptentry *fastboot_flash_find_ptn(const char *name)
{
	unsigned int n;

	for (n = 0; n < MAX_PTN; n++) {
		/* Make sure a substring is not accepted */
		if (strlen(name) == strlen(ptable[n].name)) {
			if (0 == strcmp(ptable[n].name, name))
				return ptable + n;
		}
	}

	return NULL;
}
void fastboot_flash_reset_ptn(void)
{
	pcount = 0;
}

void fastboot_flash_add_ptn(fastboot_ptentry *ptn, int count)
{
	if(pcount < MAX_PTN) {
	    memcpy(ptable + pcount, ptn, sizeof(*ptn));
	    pcount++;
	}
}

int fastboot_getvar(const char *rx_buffer, char *tx_buffer)
{
	/* Place board specific variables here */
	return 0;
}

