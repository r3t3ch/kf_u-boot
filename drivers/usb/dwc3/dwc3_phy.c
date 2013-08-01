#include "dwc3_phy.h"

u32 get_sys_clk_freq(void);
void udelay(unsigned long usec);
void mdelay(unsigned long msec);
u32 get_sys_clk_index(void);;



static struct usb_dpll_params omap_usb3_dpll_params[7] = {
	{1250, 5, 4, 20, 0},		/* 12 MHz */
	{1000, 7, 4, 10, 0},            /* 20 MHz */
	{3125, 20, 4, 20, 0},		/* 16.8 MHz */
	{1172, 8, 4, 20, 65537},	/* 19.2 MHz */
	{1250, 12, 4, 20, 0},		/* 26 MHz */
	{0, 0, 0, 0, 0},		/* 27 MHz TBD */
	{3125, 47, 4, 20, 92843},	/* 38.4 MHz */
};

void omap_usb_dpll_relock()
{
	u32 val;
	writel(USB3_PHY_SET_PLL_GO, USB3_PHY_PLL_GO);
	do {
		val = readl(USB3_PHY_PLL_STATUS);
			if (val & USB3_PHY_PLL_LOCK)
				break;
	} while (1);
}

void omap_usb_dpll_lock() {

	u32 clk_index = get_sys_clk_index();
	u32 val;

	val = readl(USB3_PHY_PLL_CONFIGURATION1);
	val &= ~USB3_PHY_PLL_REGN_MASK;
	val |= omap_usb3_dpll_params[clk_index].n << USB3_PHY_PLL_REGN_SHIFT;
	writel(val, USB3_PHY_PLL_CONFIGURATION1);

	val = readl(USB3_PHY_PLL_CONFIGURATION2);
	val &= ~USB3_PHY_PLL_SELFREQDCO_MASK;
	val |= omap_usb3_dpll_params[clk_index].freq <<  USB3_PHY_PLL_SELFREQDCO_SHIFT;
	writel(val, USB3_PHY_PLL_CONFIGURATION2);

	val = readl(USB3_PHY_PLL_CONFIGURATION1);
	val &= ~USB3_PHY_PLL_REGM_MASK;
	val |= omap_usb3_dpll_params[clk_index].m << USB3_PHY_PLL_REGM_SHIFT;
	writel(val, USB3_PHY_PLL_CONFIGURATION1);

	val = readl(USB3_PHY_PLL_CONFIGURATION4);
	val &= ~USB3_PHY_PLL_REGM_F_MASK;
	val |= omap_usb3_dpll_params[clk_index].mf << USB3_PHY_PLL_REGM_F_SHIFT;
	writel(val,USB3_PHY_PLL_CONFIGURATION4);

	val = readl(USB3_PHY_PLL_CONFIGURATION3);
	val &= ~USB3_PHY_PLL_SD_MASK;
	val |= omap_usb3_dpll_params[clk_index].sd << USB3_PHY_PLL_SD_SHIFT;
	writel(val,USB3_PHY_PLL_CONFIGURATION3);

 	omap_usb_dpll_relock();  	
}

void usb3_phy_partial_powerup() {
	u32 rate = get_sys_clk_freq()/1000000;
	u32 val;
	
	val = readl(USB3_PHY_CONTROL_PHY_POWER_USB);

	val &= ~(USB3_PWRCTL_CLK_CMD_MASK | USB3_PWRCTL_CLK_FREQ_MASK);
	val |= (USB3_PHY_PARTIAL_RX_POWERON | USB3_PHY_TX_RX_POWERON)
		<< USB3_PWRCTL_CLK_CMD_SHIFT;
	val |= rate << USB3_PWRCTL_CLK_FREQ_SHIFT;

	writel(val, USB3_PHY_CONTROL_PHY_POWER_USB);
}

void usb3_phy_power(bool on) {
	u32 val;
	val = readl(USB3_PHY_CONTROL_PHY_POWER_USB);
	
	if (on) {
		val &= ~USB3_PWRCTL_CLK_CMD_MASK;
		val |= USB3_PHY_TX_RX_POWERON << USB3_PWRCTL_CLK_CMD_SHIFT;
	} else {
	//	val &= ~USB3_PWRCTL_CLK_CMD_MASK;
	//	val |= USB3_PHY_TX_RX_POWEROFF << USB3_PWRCTL_CLK_CMD_SHIFT;
	}

	writel(val, USB3_PHY_CONTROL_PHY_POWER_USB);
}

void omap_usb3_suspend(int suspend)
{
	volatile u32 val = 0;
	if (!suspend) {
		val = readl(USB3_PHY_PLL_CONFIGURATION2);
		val &= ~USB3_PHY_PLL_IDLE;
		writel(val, USB3_PHY_PLL_CONFIGURATION2);
		udelay(200);
		val = readl(USB3_PHY_PLL_STATUS);
	}
}

void omap_usb2_suspend(int suspend)
{
	if (!suspend) {
		writel(0, CONTROL_DEV_CONF);
		mdelay(200);	
	}	
}

void dwc_usb3_phy_init() {
	omap_usb_dpll_lock();
	usb3_phy_partial_powerup();
	/*
	 * Give enough time for the PHY to partially power-up before
	 * powering it up completely. delay value suggested by the HW
	 * team.
	 */
	mdelay(100);
	usb3_phy_power(1);

}

