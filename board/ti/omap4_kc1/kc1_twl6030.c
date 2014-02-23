/*
 * (C) Copyright 2009
 * Texas Instruments, <www.ti.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
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
 */
#include <config.h>
#include <asm/omap_common.h>
#include <command.h>
#include <asm/io.h>
#include <twl6030.h>

#include <kc1_board.h>

#include "kc1_twl6030.h"
#include "fg_bq27541.h"


void twl6030_start_usb_charging(void)
{
	debug("[TWL6030] %s:: ENTER\n", __func__);
	twl6030_i2c_write_u8(TWL6030_CHIP_CHARGER, CHARGERUSB_VICHRG, CHARGERUSB_VICHRG_500);
	twl6030_i2c_write_u8(TWL6030_CHIP_CHARGER, CHARGERUSB_CINLIMIT, CHARGERUSB_CIN_LIMIT_500);
	twl6030_i2c_write_u8(TWL6030_CHIP_CHARGER, CONTROLLER_INT_MASK, MBAT_TEMP);
	twl6030_i2c_write_u8(TWL6030_CHIP_CHARGER, CHARGERUSB_INT_MASK, MASK_MCHARGERUSB_THMREG);
	twl6030_i2c_write_u8(TWL6030_CHIP_CHARGER, CHARGERUSB_VOREG, CHARGERUSB_VOREG_4P76);
	twl6030_i2c_write_u8(TWL6030_CHIP_CHARGER, CHARGERUSB_CTRL2, CHARGERUSB_CTRL2_VITERM_100);
	/* Enable USB charging */
	twl6030_i2c_write_u8(TWL6030_CHIP_CHARGER, CONTROLLER_CTRL1, CONTROLLER_CTRL1_EN_CHARGER);
}

void twl6030_shutdown(void)
{
	debug("[TWL6030] %s:: ENTER\n", __func__);
	twl6030_i2c_write_u8(TWL6030_CHIP_PM, TWL6030_PHONIX_DEV_ON, 7);
}

int twl6030_get_vbus_status(void)
{   
	u8   data;
	debug("[TWL6030] %s:: ENTER\n", __func__);
	twl6030_i2c_read_u8(TWL6030_CHIP_CHARGER, CONTROLLER_STAT1, &data);//49,
	return ((data & 0x4) >> 2);
}

void twl6030_init_battery_charging(void)
{
	debug("[TWL6030] %s:: ENTER\n", __func__);
	get_bat_voltage();
	get_bat_current();
	twl6030_start_usb_charging();
}

void twl6030_init_vusb(void)
{
	u8 data;
	debug("[TWL6030] %s:: ENTER\n", __func__);
	/* Select APP Group and set state to ON */
	twl6030_i2c_read_u8(TWL6030_CHIP_PM, MISC2, &data);
	data |= 0x10;
	/* Select the input supply for VBUS regulator */
	twl6030_i2c_write_u8(TWL6030_CHIP_PM, MISC2, data);

	twl6030_i2c_write_u8(TWL6030_CHIP_PM, VUSB_CFG_VOLTAGE, 0x18);
	twl6030_i2c_write_u8(TWL6030_CHIP_PM, VUSB_CFG_GRP, 0x00);
	twl6030_i2c_write_u8(TWL6030_CHIP_PM, VUSB_CFG_TRANS, 0x00);
	twl6030_i2c_write_u8(TWL6030_CHIP_PM, VUSB_CFG_STATE, 0x01);
}

void twl6030_disable_vusb(void)
{
	u8   data;
	debug("[TWL6030] %s:: ENTER\n", __func__);
	twl6030_i2c_write_u8(TWL6030_CHIP_PM, VUSB_CFG_VOLTAGE, 0x18);
	twl6030_i2c_write_u8(TWL6030_CHIP_PM, VUSB_CFG_GRP, 0x00);
	twl6030_i2c_write_u8(TWL6030_CHIP_PM, VUSB_CFG_TRANS, 0x00);
	twl6030_i2c_write_u8(TWL6030_CHIP_PM, VUSB_CFG_STATE, 0x00);

	/* Select APP Group and set state to ON */
	twl6030_i2c_read_u8(TWL6030_CHIP_PM, MISC2, &data);
	data &= ~0x18;
	/* Select the input supply for VBUS regulator */
	twl6030_i2c_write_u8(TWL6030_CHIP_PM, MISC2, data);
}

void twl6030_usb_device_settings(void)
{
	debug("[TWL6030] %s:: ENTER\n", __func__);
	twl6030_init_vusb();
}


int twl6030_get_power_button_status(void)
{   
	volatile u8 data;
	twl6030_i2c_read_u8 (TWL6030_CHIP_PM, PHOENIX_STS_HW_CONDITIONS, &data);
	return (data & 0x1);
}

u32 twl6030_print_boot_reason(void)
{
	volatile u8 data1 = 0, data2 = 0, data3 = 0;
	volatile u32 data4 = 0;

	printf ("PMIC TWL 6030 start conditions\n");
	twl6030_i2c_read_u8 (TWL6030_CHIP_PM, PHOENIX_START_CONDITION, &data1);
	twl6030_i2c_read_u8 (TWL6030_CHIP_PM, PHOENIX_LAST_TURNOFF_STS, &data2);
	twl6030_i2c_read_u8 (TWL6030_CHIP_PM, PHOENIX_STS_HW_CONDITIONS, &data3);
	printf ("PHOENIX_START_CONDITION  :0x%02x\n", data1);
	printf ("PHOENIX_LAST_TURNOFF_STS :0x%02x\n", data2);
	printf ("PHOENIX_STS_HW_CONDITIONS:0x%02x\n", data3);
	data4 = __raw_readl(PRM_RSTST);
	/* PRM_RSTST 10:31 are reserved. Mask them off */
	data4 = (0x3FF & data4);
	printf ("OMAP4 PRM_RSTST          :0x%08x\n", data4);

	return data4;
}

#define OTG_INTERFSEL		0x4A0AB40C
#define USBOTGHS_CONTROL	0x4A00233C
#define OMAP34XX_USB_DEVCTL	(0x4A000000 + 0xAB000 + 0x060)
#define OTG_SYSCONFIG		(0x4A000000 + 0xAB000 + 0x404)
//#define OTG_SYSSTATUS		(0x4A000000 + 0xAB000 + 0x408)

static volatile u32 *otg_sysconfig = (volatile u32  *)OTG_SYSCONFIG;
static volatile u32 *otg_interfsel = (volatile u32  *)OTG_INTERFSEL;
static volatile u32 *otghs_control = (volatile u32  *)USBOTGHS_CONTROL;
//static volatile u32 *otg_sysstatus = (volatile u32  *)OTG_SYSSTATUS;

void twl6030_usb_ulpi_init(void)
{
	debug("[TWL6030] %s:: ENTER\n", __func__);
	int ms = 1;

	twl6030_disable_vusb();
	twl6030_init_vusb();

	__raw_writel(0x101, 0x4A0093E0);    //enable ocp2scp_usb_phy_ick
	__raw_writel(0x100, 0x4A008640);    //enable ocp2scp_usb_phy_phy_48m
	for (ms=0;ms<100;ms++)
		udelay(100);//10ms
	__raw_writel(~(1), 0x4A002300);          //power up the usb phy

	/* Reset Mentor USB block */
	/* 1)soft reset */
	*otg_sysconfig |= (1<<1);

	/* 2)Power Management Configuration .now set better defaults MIDLEMODE=No standby mode ,SIDLEMODE=No idle mode*/
	*otg_sysconfig = (0x1008);

	/* 3)PHY interface is 8-bit, UTMI+ level 3*/
	*otg_interfsel &= 0;

	/* 4)*Enable functional PHY clock (OTG_60M_FCLK)*/
	__raw_writel(0x101, 0x4A0093E0);	//Enable ocp2scp_usb_phy_ick
	__raw_writel(0x100, 0x4A008640);	//Enable ocp2scp_usb_phy_phy_48m

	__raw_writel(~(1), 0x4A002300);		//power up the usb phy

	if (*otghs_control != 0x15) {
		debug("*** %s::ENTER OTG_CONTROL != 0x15\n", __func__);
#if 0
		fastboot_reset();
		*otg_interfsel &= 0;
		/* Program Phoenix registers VUSB_CFG_STATE and MISC2 */
		twl6030_usb_device_settings();
		/* Program the control module register */
		*otghs_control = 0x15;
#endif
	}
	else {
		/* HACK */
		debug("*** %s::ENTER HACK\n", __func__);
// HASH: TODO
//		fastboot_bulk_endpoint_reset();
		*otg_interfsel &= 0;
		/* Keeping USB cable attached and booting causes
		* ROM code to reconfigure USB, and then
		* re-enumeration never happens
		* Setting this SRP bit helps - Cannot see why !!
		* MUSB spec says : Session bit is used only for SRP
		*/
		__raw_writeb(0x1, OMAP34XX_USB_DEVCTL);
	}
}

