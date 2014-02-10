#include <common.h>
#include <asm/arch/sys_proto.h>
#include <usb/fastboot.h>
#include <asm/io.h>
#include <asm-generic/gpio.h>
#include <config.h>

#include "pmic_smb347.h"
#include "kc1_twl6030.h"

#define OTG_INTERFSEL 0x4A0AB40C

char serialno[100];
char manufacturer[100];
char product[100];

/* To support the Android-style naming of flash */
#define MAX_PTN 16

static fastboot_ptentry ptable[MAX_PTN];
static unsigned int pcount = 0;

static struct usb_string def_usb_fb_strings[] = {
	{ FB_STR_PRODUCT_IDX,      &product },
	{ FB_STR_SERIAL_IDX,       &serialno },
	{ FB_STR_MANUFACTURER_IDX, &manufacturer },
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

static volatile u32 *otg_interfsel = (volatile u32  *)OTG_INTERFSEL;

int usb_gadget_init_udc(void)
{
	// probe
	return 0;
}

void usb_gadget_exit_udc(void)
{
//	dwc3_remove(NULL);
}

int fastboot_board_init(struct fastboot_config *interface, struct usb_gadget_strings **str)
{
	int ms = 1;
	debug("*** fastboot_board_init\n");

	kc1_twl6030_disable_vusb();
	kc1_twl6030_init_vusb();
	__raw_writel(0x101, 0x4A0093E0);    //enable ocp2scp_usb_phy_ick
	__raw_writel(0x100, 0x4A008640);    //enable ocp2scp_usb_phy_phy_48m
	for (ms=0;ms<100;ms++)
		udelay(100);//10ms
	__raw_writel(~(1), 0x4A002300);          //power up the usb phy

	interface->transfer_buffer = CFG_FASTBOOT_TRANSFER_BUFFER;
	interface->transfer_buffer_size = CFG_FASTBOOT_TRANSFER_BUFFER_SIZE;

	/* HASH: Determine the serial number */
	sprintf(serialno, "0123456789ABCDEF");
	printf("Device Serial Number: %s\n", serialno);
	sprintf(manufacturer, "Amazon");
	sprintf(product, "kindle");

	*str = &def_fb_strings;

	/* 3)PHY interface is 8-bit, UTMI+ level 3*/
	*otg_interfsel &= 0;
	/* 4)*Enable functional PHY clock (OTG_60M_FCLK)*/
	__raw_writel(0x101, 0x4A0093E0);	//Enable ocp2scp_usb_phy_ick
	__raw_writel(0x100, 0x4A008640);	//Enable ocp2scp_usb_phy_phy_48m
	__raw_writel(~(1), 0x4A002300);		//power up the usb phy

	board_mmc_ftbtptn_init();
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

char* fastboot_get_serialno(void)
{
	return serialno;
}

int do_powerdown(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
        unsigned char data = 0x07;
        i2c_write(0x48, 0x25, 1, &data, 1);
	/*NOTREACHED*/
	return 0;
}

U_BOOT_CMD( powerdown, 1, 0, do_powerdown, "powerdown.\n", NULL );

