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
#include <command.h>
#include <asm/io.h>

#include "kc1_board.h"
#include "kc1_twl6030.h"
#include "fg_bq27541.h"


void kc1_twl6030_start_usb_charging(void)
{
    debug("*** KC1_TWL6030: start_usb_charging\n");
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_CHARGER, CHARGERUSB_VICHRG_500, CHARGERUSB_VICHRG);
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_CHARGER, CHARGERUSB_CIN_LIMIT_500, CHARGERUSB_CINLIMIT);
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_CHARGER, MBAT_TEMP, CONTROLLER_INT_MASK);
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_CHARGER, MASK_MCHARGERUSB_THMREG, CHARGERUSB_INT_MASK);
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_CHARGER, CHARGERUSB_VOREG_4P76, CHARGERUSB_VOREG);
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_CHARGER, CHARGERUSB_CTRL2_VITERM_100, CHARGERUSB_CTRL2);
    /* Enable USB charging */
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_CHARGER, CONTROLLER_CTRL1_EN_CHARGER, CONTROLLER_CTRL1);
}

void kc1_twl6030_reset_wd(void)
{
    debug("*** KC1_TWL6030: reset_wd\n");
    get_bat_voltage();
    get_bat_current();
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_CHARGER, 0xa0, CONTROLLER_WDG);
}

void kc1_twl6030_shutdown(void)
{
    debug("*** KC1_TWL6030: shutdown\n");
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_PM, 7, TWL6030_PHONIX_DEV_ON);
}

int kc1_twl6030_get_vbus_status(void)
{   
    u8   data;
    debug("*** KC1_TWL6030: get_vbus_status\n");
    kc1_twl6030_i2c_read_u8(TWL6030_CHIP_CHARGER, &data, CONTROLLER_STAT1);//49,
    return ((data & 0x4) >> 2);
}

void kc1_twl6030_init_battery_charging(void)
{
    debug("*** KC1_TWL6030: init_battery_charging\n");
    get_bat_voltage();
    get_bat_current();
    kc1_twl6030_start_usb_charging();
}

void kc1_twl6030_init_vusb(void)
{
    u8 data;
    debug("*** KC1_TWL6030: init_vusb\n");
    /* Select APP Group and set state to ON */
    kc1_twl6030_i2c_read_u8(TWL6030_CHIP_PM, &data, MISC2);
    data |= 0x10;
    /* Select the input supply for VBUS regulator */
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_PM, data, MISC2);

    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x18, VUSB_CFG_VOLTAGE);
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x00, VUSB_CFG_GRP);
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x00, VUSB_CFG_TRANS);
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x01, VUSB_CFG_STATE);
}

void kc1_twl6030_disable_vusb(void)
{
    u8   data;
    debug("*** KC1_TWL6030: disable_vusb\n");
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x18,VUSB_CFG_VOLTAGE);
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x00,VUSB_CFG_GRP);
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x00,VUSB_CFG_TRANS);
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x00,VUSB_CFG_STATE);

    /* Select APP Group and set state to ON */
    kc1_twl6030_i2c_read_u8(TWL6030_CHIP_PM, &data, MISC2);
    data &= ~0x18;
    /* Select the input supply for VBUS regulator */
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_PM, data, MISC2);
}

void kc1_twl6030_kc1_settings(void)
{
    debug("*** KC1_TWL6030: kc1_settings\n");
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x00, VMEM_CFG_GRP);
    //rtc off mode low power,BBSPOR_CFG,VRTC_EN_OFF_STS
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x72, 0xE6);
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x00, VMEM_CFG_GRP);
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x00, VMEM_CFG_STATE);
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x00, VMEM_CFG_TRANS);
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0xC0, PHOENIX_MSK_TRANSITION);
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x01, VCXIO_CFG_TRANS);
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x00, VDAC_CFG_GRP);
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x00, VDAC_CFG_TRANS);
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x00, VDAC_CFG_STATE);
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x00, VMMC_CFG_GRP);
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x00, VMMC_CFG_TRANS);
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x00, VMMC_CFG_STATE);
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x00, VAUX3_CFG_GRP);
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x00, VAUX3_CFG_STATE);
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x00, VAUX2_CFG_GRP);
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x00, VAUX2_CFG_STATE);
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x09, VAUX2_CFG_VOLTAGE);
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x10, 0xEC);
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x00, VUSIM_CFG_GRP);
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x00, VUSIM_CFG_STATE);
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x14, VUSIM_CFG_VOLTAGE);
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x3C, V2V1_CFG_VOLTAGE);
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x01, CLK32KG_CFG_STATE);
}

void kc1_tw6030_kc1_clk32kg(void)
{
    //32k
    debug("*** KC1_TWL6030: kc1_clk32kg\n");
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x0, CLK32KG_CFG_GRP);
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x1, CLK32KG_CFG_STATE);
}

void kc1_twl6030_power_mmc_init(void)
{
    debug("*** KC1_TWL6030: kc1_twl6030_power_mmc_init\n");
    /* set voltage to 3.0 and turnon for APP */
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x15, VMMC_CFG_VOLTAGE);
    kc1_twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x21, VMMC_CFG_STATE);
}

void kc1_twl6030_usb_device_settings(void)
{
    debug("*** KC1_TWL6030: usb_device_settings\n");
    kc1_twl6030_init_vusb();
}

#define PHOENIX_START_CONDITION		0x1F
#define PHOENIX_MSK_TRANSITION 		0x20
#define PHOENIX_STS_HW_CONDITIONS 	0x21
#define PHOENIX_LAST_TURNOFF_STS 	0x22

int kc1_twl6030_get_power_button_status(void)
{   
    volatile u8 data;
    kc1_twl6030_i2c_read_u8 (TWL6030_CHIP_PM, &data, PHOENIX_STS_HW_CONDITIONS);
    return (data & 0x1);
}

int kc1_twl6030_print_boot_reason(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    volatile u8 data1 = 0, data2 = 0, data3 = 0;
    volatile u32 data4 = 0;

    printf ("PMIC TWL 6030 start conditions\n");
    kc1_twl6030_i2c_read_u8 (TWL6030_CHIP_PM, &data1, PHOENIX_START_CONDITION);
    kc1_twl6030_i2c_read_u8 (TWL6030_CHIP_PM, &data2, PHOENIX_LAST_TURNOFF_STS);
    kc1_twl6030_i2c_read_u8 (TWL6030_CHIP_PM, &data3, PHOENIX_STS_HW_CONDITIONS);
    printf ("PHOENIX_START_CONDITION  :0x%02x\n", data1);
    printf ("PHOENIX_LAST_TURNOFF_STS :0x%02x\n", data2);
    printf ("PHOENIX_STS_HW_CONDITIONS:0x%02x\n", data3);
    data4 = __raw_readl(PRM_RSTST);
    /* PRM_RSTST 10:31 are reserved. Mask them off */
    data4 = (0x3FF & data4);
    printf ("OMAP4 PRM_RSTST          :0x%08x\n", data4);
    return 0;
}

U_BOOT_CMD( print_boot_reason, 1, 0, kc1_twl6030_print_boot_reason, "print_boot_reason - display TWL6030 boot registers.\n", NULL );

