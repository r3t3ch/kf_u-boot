#ifndef _DWC3_PHY_H_
#define _DWC3_PHY_H_

#include <linux/types.h>	/* __u8 etc */
#include "asm/arch/dwc3.h"
#include "io.h"

struct usb_dpll_params {
	u16	m;
	u8	n;
	u8	freq:3;
	u8	sd;
	u32	mf;
};

void omap_usb_dpll_relock(void);

void omap_usb_dpll_lock(void);

void usb3_phy_partial_powerup(void);

void usb3_phy_power(bool on);

void omap_usb3_suspend(int suspend);

void omap_usb2_suspend(int suspend);

void dwc_usb3_phy_init(void);

#endif /* _DWC3_PHY_H */
