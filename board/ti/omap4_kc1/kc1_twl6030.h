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

#ifndef __KC1_TWL6030__H__
#define __KC1_TWL6030__H__

#include <common.h>

#define TWL6030_PHONIX_DEV_ON               0x25

#define PHOENIX_START_CONDITION             0x1F
#define PHOENIX_MSK_TRANSITION              0x20
#define PHOENIX_STS_HW_CONDITIONS           0x21
#define PHOENIX_LAST_TURNOFF_STS            0x22

/* START_CONDITION_MASK*/
#define STRT_ON_PLUG_DET                    (1 << 3)
#define STRT_ON_USB_ID                      (1 << 2)
#define STRT_ON_PWRON                       (1)

#define PHOENIX_MSK_TRANSITION              0x20
#define PHOENIX_SENS_TRANSITION             0x2A

/* Config Registers */
#define V1V29_CFG_GRP                       0x40
#define V1V29_CFG_TRANS                     0x41
#define V1V29_CFG_STATE                     0x42
#define V1V29_CFG_VOLTAGE                   0x44

#define V2V1_CFG_GRP                        0x4C
#define V2V1_CFG_TRANS                      0x4D
#define V2V1_CFG_STATE                      0x4E
#define V2V1_CFG_VOLTAGE                    0x50

#define VMEM_CFG_GRP                        0x64
#define VMEM_CFG_TRANS                      0x65
#define VMEM_CFG_STATE                      0x66
#define VMEM_CFG_VOLTAGE                    0x68

#define VAUX1_CFG_GRP                       0x84
#define VAUX1_CFG_TRANS                     0x85
#define VAUX1_CFG_STATE                     0x86
#define VAUX1_CFG_VOLTAGE                   0x87

#define VUSB_CFG_GRP                        0xA0
#define VUSB_CFG_TRANS                      0xA1
#define VUSB_CFG_STATE                      0xA2
#define VUSB_CFG_VOLTAGE                    0xA3

#define VAUX2_CFG_GRP                       0x88
#define VAUX2_CFG_TRANS                     0x89
#define VAUX2_CFG_STATE                     0x8A
#define VAUX2_CFG_VOLTAGE                   0x8B

#define VAUX3_CFG_GRP                       0x8C
#define VAUX3_CFG_TRANS                     0x8D
#define VAUX3_CFG_STATE                     0x8E
#define VAUX3_CFG_VOLTAGE                   0x8f

#define VCXIO_CFG_GRP                       0x90
#define VCXIO_CFG_TRANS                     0x91
#define VCXIO_CFG_STATE                     0x92
#define VCXIO_CFG_VOLTAGE                   0x93

#define VDAC_CFG_GRP                        0x94
#define VDAC_CFG_TRANS                      0x95
#define VDAC_CFG_STATE                      0x96
#define VDAC_CFG_VOLTAGE                    0x97

#define VMMC_CFG_GRP                        0x98
#define VMMC_CFG_TRANS                      0x99
#define VMMC_CFG_STATE                      0x9A
#define VMMC_CFG_VOLTAGE                    0x9B

#define VUSIM_CFG_GRP                       0xA4
#define VUSIM_CFG_TRANS                     0xA5
#define VUSIM_CFG_STATE                     0xA6
#define VUSIM_CFG_VOLTAGE                   0xA7

#define CLK32KG_CFG_GRP                     0xBC
#define CLK32KG_CFG_TRANS                   0xBD
#define CLK32KG_CFG_STATE                   0xBE

#define BBSPOR_CFG                          0xE6

#define PWRBTN_KEY_PRESS      0x1
#define PWRBTN_KEY_LONG_PRESS 0x2

void twl6030_shutdown(void);
int  twl6030_get_vbus_status(void);
void twl6030_init_vusb(void);
void twl6030_disable_vusb(void);
void twl6030_kc1_settings(void);
int  twl6030_get_power_button_status(void);
u32  twl6030_print_boot_reason(void);

/* input driver for power button */
int pwrbutton_getc(void);
int drv_twl6030_pwrbutton_init(void);

#endif /* __KC1_TWL6030__H__ */
