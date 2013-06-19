/*
 * OMAP DWC3 support
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com
 * Author: Ruchika Kharwar <ruchika@ti.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2  of
 * the License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _OMAP5_DWC3_H_
#define _OMAP5_DWC3_H_

/* CLKCTRL and OPT clocks */
#define CM_L3INIT_USB_OTG_SS_CLKCTRL		0x4A0096F0UL

#define USB_OTG_SS_OPTFCLKEN_REFCLK960M 	(1 << 8)
#define USB_OTG_SS_MODULEMODE_HW		(1 << 0)

/* IRQS0 BITS */
#define USBOTGSS_IRQO_COREIRQ_ST		(1 << 0)

/* IRQ1 BITS */
#define USBOTGSS_IRQ1_DMADISABLECLR		(1 << 17)
#define USBOTGSS_IRQ1_OEVT			(1 << 16)
#define USBOTGSS_IRQ1_DRVVBUS_RISE		(1 << 13)
#define USBOTGSS_IRQ1_CHRGVBUS_RISE		(1 << 12)
#define USBOTGSS_IRQ1_DISCHRGVBUS_RISE		(1 << 11)
#define USBOTGSS_IRQ1_IDPULLUP_RISE		(1 << 8)
#define USBOTGSS_IRQ1_DRVVBUS_FALL		(1 << 5)
#define USBOTGSS_IRQ1_CHRGVBUS_FALL		(1 << 4)
#define USBOTGSS_IRQ1_DISCHRGVBUS_FALL		(1 << 3)
#define USBOTGSS_IRQ1_IDPULLUP_FALL		(1 << 0)


#define USBOTGSS_IRQENABLE_SET_0	0x4A02002c	
#define USBOTGSS_IRQENABLE_SET_1	0x4A02003c	
#define USBOTGSS_SYSCONFIG		0x4A020010
#define SYSCONFIG_DMADISABLE		(1<<16)
#define SYSCONFIG_STANDBY_SMART		(0x3<<4)
#define SYSCONFIG_IDLEMODE		(0x2<<2)

#define USBOTGSS_IRQSTATUS_0		0x4A020028
#define USBOTGSS_IRQSTATUS_1		0x4A020038
#define USBOTGSS_UTMI_OTG_STATUS 	0x4A020084

#define USB3_PHY_PLL_CONFIGURATION1	0x4A084C0C
#define USB3_PHY_PLL_REGN_MASK		0xFE
#define USB3_PHY_PLL_REGN_SHIFT		1
#define USB3_PHY_PLL_REGM_MASK		0x1FFE00 
#define USB3_PHY_PLL_REGM_SHIFT		9 
#define USB3_PHY_PLL_CONFIGURATION2	0x4A084C10
#define USB3_PHY_PLL_SELFREQDCO_MASK	0xE
#define USB3_PHY_PLL_SELFREQDCO_SHIFT	1
#define USB3_PHY_PLL_CONFIGURATION4     0x4A084C20
#define USB3_PHY_PLL_REGM_F_MASK	0x3FFFF
#define USB3_PHY_PLL_REGM_F_SHIFT	0
#define USB3_PHY_PLL_CONFIGURATION3	0x4A084C14
#define USB3_PHY_PLL_SD_MASK		0x3FC00
#define USB3_PHY_PLL_SD_SHIFT		9
#define USB3_PHY_CONTROL_PHY_POWER_USB	0x4A002370 
#define USB3_PWRCTL_CLK_CMD_MASK	0x3FE000
#define USB3_PWRCTL_CLK_FREQ_MASK	0xFFC
#define USB3_PHY_PARTIAL_RX_POWERON     (1<<6)
#define USB3_PHY_TX_RX_POWERON		0x3
#define USB3_PWRCTL_CLK_CMD_SHIFT	14	
#define USB3_PWRCTL_CLK_FREQ_SHIFT 	22
#define USB3_PHY_PLL_IDLE		1

#define USB3_PHY_PLL_STATUS	0x4A084C04
#define USB3_PHY_PLL_TICOPWDN   0x10000
#define USB3_PHY_PLL_LOCK	0x2
#define CONTROL_DEV_CONF	0x4A002300
#define CONTROL_DEV_CONF_USBPHY_PD	1

#define USB3_PHY_PLL_GO		0x4A084C08
#define USB3_PHY_SET_PLL_GO	1	

#define DWC3_GCTL_DISSCRAMBLE		(1 << 3)
#define DWC3_GCTL_SCALEDOWN_MASK	DWC3_GCTL_SCALEDOWN(3)

#define CM_L3INIT_OCP2SCP1_CLKCTRL	0x4A0096E0
#define OCP2SCP1_MODULEMODE_HW		(1 << 0)

#define CM_COREAON_USB_PHY_CORE_CLKCTRL 0x4A008640
#define USB_PHY_CORE_OPTFCLKEN_CLK32K   (1<<8)

#define CM_L3INIT_CLKSTCTRL		0x4A009600
#define L3INIT_CLKSTCTRL_NOSLEEP	0
#endif /* _OMAP5_DWC3_H_ */
