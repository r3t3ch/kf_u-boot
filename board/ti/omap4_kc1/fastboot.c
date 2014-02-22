#include <common.h>
#include <asm/arch/sys_proto.h>
#include <usb/fastboot.h>
#include <asm/io.h>
#include <asm-generic/gpio.h>
#include <config.h>

#include "pmic_smb347.h"
#include "kc1_twl6030.h"

char serialno[100];
char manufacturer[100];
char product[100];

/* To support the Android-style naming of flash */
#define MAX_PTN 16

static fastboot_ptentry ptable[MAX_PTN];
static unsigned int pcount = 0;

static struct usb_string def_usb_fb_strings[] = {
	{ FB_STR_PRODUCT_IDX,      (const char*)&product },
	{ FB_STR_SERIAL_IDX,       (const char*)&serialno },
	{ FB_STR_MANUFACTURER_IDX, (const char*)&manufacturer },
	{  }
};

static struct usb_gadget_strings def_fb_strings = {
	.language       = 0x0409, /* en-us */
	.strings        = def_usb_fb_strings,
};

/*
 * Hardcoded memory region to stash data which comes over USB before it is
 * stored on media
 */
DECLARE_GLOBAL_DATA_PTR;
#define SZ_16M                            0x01000000
#define SZ_128M                           0x10000000
#define CFG_FASTBOOT_TRANSFER_BUFFER      (void *)(gd->bd->bi_dram[0].start + SZ_16M)
#define CFG_FASTBOOT_TRANSFER_BUFFER_SIZE (SZ_128M - SZ_16M)

int fastboot_board_init(struct fastboot_config *interface, struct usb_gadget_strings **str)
{
	debug("*** %s\n", __func__);

	interface->transfer_buffer = CFG_FASTBOOT_TRANSFER_BUFFER;
	interface->transfer_buffer_size = CFG_FASTBOOT_TRANSFER_BUFFER_SIZE;

	board_mmc_ftbtptn_init();

	/* HASH: Determine the serial number */
	sprintf(serialno, "%s", getenv("serialno"));
	printf("Device Serial Number: %s\n", serialno);
	sprintf(manufacturer, "%s", "Amazon");
	sprintf(product, "%s", "kindle");

	*str = &def_fb_strings;
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

int do_powerdown(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	twl6030_shutdown();
	/*NOTREACHED*/
	return 0;
}

U_BOOT_CMD( powerdown, 1, 0, do_powerdown, "powerdown.\n", NULL );

