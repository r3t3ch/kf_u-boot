#include <common.h>
#include <usb/fastboot.h>
#include <asm/io.h>

#define CONTROL_ID_CODE		0x4A002204

char serialno[100];

static struct usb_string def_usb_fb_strings[] = {
	{ FB_STR_SERIAL_IDX,            &serialno },
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
#define SZ_16M                          0x01000000
#define SZ_128M                         0x08000000
#define CFG_FASTBOOT_TRANSFER_BUFFER (void *)(gd->bd->bi_dram[0].start + SZ_16M)
#define CFG_FASTBOOT_TRANSFER_BUFFER_SIZE (SZ_128M - SZ_16M)

int fastboot_board_init(struct fastboot_config *interface,
		struct usb_gadget_strings **str) {

	u32 val[4] = { 0 };
	u32 reg;

	interface->transfer_buffer = CFG_FASTBOOT_TRANSFER_BUFFER;
	interface->transfer_buffer_size = CFG_FASTBOOT_TRANSFER_BUFFER_SIZE;

	/* Determine the serial number */
	reg = CONTROL_ID_CODE;
	val[2] = readl(reg + 0xC);
	val[3] = readl(reg + 0x10);
	printf("Device Serial Number: %08X%08X\n", val[3], val[2]);
	sprintf(serialno, "%08X%08X", val[3], val[2]);

	*str = &def_fb_strings;
	return 0;
}
