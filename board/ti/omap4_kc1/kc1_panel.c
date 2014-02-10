// Seven modifies source codes from WinCE EBoot

#include <common.h>
#include <asm/byteorder.h>
#include <asm/io.h>
#include <asm/omap_common.h>
#include <asm/gpio.h>
#include <command.h>
#include <i2c.h>

#include "kc1_panel.h"
#include "omap4_spi.h"

extern struct prcm_regs const omap4_prcm;

extern char const _binary_initlogo_rle_start[];
extern char const _binary_initlogo_rle_end[];
extern char const _binary_lowbattery_rle_start[];
extern char const _binary_lowbattery_rle_end[];
extern char const _binary_multi_download_rle_start[];
extern char const _binary_multi_download_rle_end[];

void turn_off_lcd(void)
{
	/* Turn on backlight */
	set_omap_pwm(0x0);

	/* GPIO_37 (OMAP_RGB_SHTDOWN, switch to LVDS mode) */
	__raw_writew(0x1B,0x4a10005A); // 37 SELECT GPIO_MUX, PULLEN, PULLUP
	gpio_direction_output(37, 0);
	udelay(2000);

	/* GPIO_47 (OMAP_3V_ENABLE, 3.3 V rail) */
	__raw_writel(0x001B0007,0x4a10006C); // 47 SELECT GPIO_MUX, PULLEN, PULLUP, 48 SAFE
	gpio_direction_output(47, 0);

	udelay(dpi_panel_cfg.panel_udelay);

	/* GPIO_47 (OMAP_3V_ENABLE, 3.3 V rail) */
	__raw_writel(0x001B0007,0x4a10006C); // 47 SELECT GPIO_MUX, PULLEN, PULLUP, 48 SAFE
	gpio_direction_output(47, 0);

	/* GPIO_45 (LCD_PWR_ON) */
	__raw_writew(0x1B,0x4a10006A); // 45 SELECT GPIO_MUX, PULLEN, PULLUP
	gpio_direction_output(45, 0);
	gpio_direction_output(119, 0);
	gpio_direction_output(120, 0);
}


/**********************************************************
 * Routine: initialize_lcd
 * Description: Initializes the LCD and displays the
 *              splash logo
 **********************************************************/
void initialize_lcd(int show_lowbattery_fb)
{
	struct dispc_regs *dispc = (struct dispc_regs *) OMAP4_DISPC_BASE;
	struct dsi_regs *dsi2 = (struct dsi_regs *) OMAP4_DSI2_BASE;
	unsigned int val = 0;

	__raw_writel(0x001B001B, 0x4a100110); // 119/120 SELECT GPIO_MUX, PULLEN, PULLUP
	gpio_direction_output(119, 1);
	gpio_direction_output(120, 1);

	spi_init();

	__raw_writew(0x1B, 0x4a10006A); // 45 SELECT GPIO_MUX, PULLEN, PULLUP, 48 SAFE
	gpio_direction_output(45, 1);

	/* Enable GPIO_47 (OMAP_3V_ENABLE, 3.3 V rail) */
	__raw_writel(0x001B0007,0x4a10006C); // 47 SELECT GPIO_MUX, PULLEN, PULLUP, 48 SAFE
	gpio_direction_output(47, 1);

	udelay(dpi_panel_cfg.panel_udelay);

	spi_command();
	udelay(2000);

	/* Enable GPIO_37 (OMAP_RGB_SHTDOWN, switch to LVDS mode) */
	__raw_writew(0x1B, 0x4a10005A); // 37 SELECT GPIO_MUX, PULLEN, PULLUP
	gpio_direction_output(37, 1);

	omap4_fill_black_data(gfx_cfg.frame_buffer, &dpi_panel_cfg);

	// HASH: fastboot image
	if (show_lowbattery_fb == 2)
		show_image((u_int16_t *)_binary_multi_download_rle_start, (u_int16_t *)_binary_multi_download_rle_end);
	else if (show_lowbattery_fb == 1)
		show_image((u_int16_t *)_binary_lowbattery_rle_start, (u_int16_t *)_binary_lowbattery_rle_end);
	else
		show_image((u_int16_t *)_binary_initlogo_rle_start, (u_int16_t *)_binary_initlogo_rle_end);

	omap4_reset_display_controller(PANEL_DELAY_COUNT, PANEL_TIMEOUT);

	// Enable LCD clocks
	__raw_writel(0x00000502, &omap4_prcm.cm_dss_dss_clkctrl);

	// Configure the clock source
	// VP_CLK_POL, DE ACTIVE LOW, HSYNC ACTIVE LOW, NO LINE BUFFER, NO HSYNC START SHORT PACKET
	__raw_writel((1<<8 | 0<<9 | 0<<10 | 0<<12 | 0<<17), &dsi2->ctrl);
	udelay(dpi_panel_cfg.panel_udelay);

	// Configure interconnect parameters

	__raw_writel((1<<0 | 1<<2 | 1<<4 | 1<<13), &dispc->sysconfig);
	udelay(dpi_panel_cfg.panel_udelay);

	// Disable any interrupts
	__raw_writel(0x0, &dsi2->irqenable);
	udelay(dpi_panel_cfg.panel_udelay);

	omap4_graphics_config(&gfx_cfg, &dpi_panel_cfg);

	// CHANNELOUT2
	__raw_writel(__raw_readl(&dispc->gfx_attributes) | (1<<30), &dispc->gfx_attributes);
	udelay(dpi_panel_cfg.panel_udelay);

	// ZOrder (above all)
	__raw_writel(__raw_readl(&dispc->gfx_attributes) | (0x3<<26), &dispc->gfx_attributes);
	udelay(dpi_panel_cfg.panel_udelay);

	// ZOrder Enable
	__raw_writel(__raw_readl(&dispc->gfx_attributes) | (1<<25), &dispc->gfx_attributes);
	udelay(dpi_panel_cfg.panel_udelay);

	// ARBITRATION = High priority
	__raw_writel(__raw_readl(&dispc->gfx_attributes) | (1<<14), &dispc->gfx_attributes);
	udelay(dpi_panel_cfg.panel_udelay);

	// BURSTSIZE = 8x128bit bursts
	__raw_writel(__raw_readl(&dispc->gfx_attributes) | (1<<7), &dispc->gfx_attributes);
	udelay(dpi_panel_cfg.panel_udelay);

	// ENABLE
	__raw_writel(__raw_readl(&dispc->gfx_attributes) | GFX_ENABLE, &dispc->gfx_attributes);
	udelay(dpi_panel_cfg.panel_udelay);


	// Setup the DSS1 clock divider - disable DSS1 clock, change divider, enable DSS clock

	val = __raw_readl(&omap4_prcm.cm_dss_dss_clkctrl);
	// TURN OFF: 1=DSS_CM_ON,8=DSS_OPTFuncClockEN,10=SYS_OPTFuncClockEN
	val = val & ~((1 << 1) | (1 << 8) | (1 << 10));
	__raw_writel(val, &omap4_prcm.cm_dss_dss_clkctrl);
	udelay(dpi_panel_cfg.panel_udelay);

	val = __raw_readl(&omap4_prcm.cm_div_m5_dpll_per);
	val = val | (8 << 0);
	__raw_writel(val, &omap4_prcm.cm_div_m5_dpll_per);
	udelay(dpi_panel_cfg.panel_udelay);

	// TURN ON: 1=DSS_CM_ON,8=DSS_OPTFuncClockEN,10=SYS_OPTFuncClockEN
	__raw_writel(((1 << 1) | (1 << 8) | (1 << 10)), &omap4_prcm.cm_dss_dss_clkctrl);
	udelay(dpi_panel_cfg.panel_udelay);

	// LCD control xxxx xxxx xxxx 0000 0000 0010 0000 1001
	// OVERLAYOPTIMIZATION
	val = __raw_readl(&dispc->control2);
	val = val | (0 << 12);
	__raw_writel(val, &dispc->control2);

	// Frame data only loaded every frame
	val = __raw_readl(&dispc->config1);
	val = val | (0 << 12);
	__raw_writel(val, &dispc->config1);

	omap4_dss_panel_config(&dpi_panel_cfg);

	udelay(dpi_panel_cfg.panel_udelay);

	omap4_apply_dss_disp_config(&dpi_panel_cfg, PANEL_DELAY_COUNT, PANEL_TIMEOUT);

	omap4_dss_enable(&dpi_panel_cfg);

	// Add delay to prevent blinking screen.
	udelay(dpi_panel_cfg.panel_udelay);

	/* Turn on backlight */
	set_omap_pwm(0x7F);
}

void show_image(u_int16_t *start, u_int16_t *end)
{
	u_int16_t *target_addr = (u_int16_t *)gfx_cfg.frame_buffer;
	//u_int16_t *start = (u_int16_t *)_binary_lowbattery_rle_start;
	//u_int16_t *end = (u_int16_t *)_binary_lowbattery_rle_end;

	/* Convert the RLE data into RGB565 */
	for (; start != end; start += 2) {
		u_int16_t count = start[0];
		while (count--) {
			*target_addr++ = start[1];
		}
	}

	/* CM_DIV_M5_DPLL_PER Set bit8 = 1, force HSDIVIDER_CLKOUT2 clock enabled*/
	__raw_writew(__raw_readw(0x4A00815C) | 0x100, 0x4A00815C);
	/* CM_SSC_DELTAMSTEP_DPLL_PER */
	__raw_writew(0XCC , 0x4A008168);
	/* CM_SSC_MODFREQDIV_DPLL_PER */
	__raw_writew(0X264 , 0x4A00816C);
	/* CM_CLKMODE_DPLL_PER Set bit12 = 1, force DPLL_SSC_EN enabled*/
	__raw_writew(__raw_readw(0x4A008140) | 0x1000 , 0x4A008140);
}

void set_omap_pwm(u8 brightness)
{
	/* Pull up GPIO 119 & 120 */
	__raw_writel(0x001B001B, 0x4a100110);
	//GPIO_OE
	__raw_writel(__raw_readl(0x48059134) & ~(0x11 << 23) , 0x48059134);
	//GPIO_DATAOUT
	__raw_writel(__raw_readl(0x4805913C) | (0x11 << 23), 0x4805913C);

	/* CM_CLKSEL_CORE to select 32KHz (0) or CM_SYS_CLK=26Mhz (1) as clock source */
	*(int*)0x4A009428 &= ~(0x1 << 24);

	/* GPT10 clock enable */
	*(int*)0x4A009428 |= 0x2;

	/* Set autoreload mode */
	*(int*)0x48086024 |= 0x2;

	/* Enable prescaler */
	*(int*)0x48086024 |= (0x1 << 5);

	/* GPT10 PWM configuration */
	*(int*)0x48086040 = 0x00000004;   /* TSICR */
	if (brightness == 0xFF) {
		//Set brightness
		*(int*)0x48086038 = 0xFFFFFF00;   /* TMAR */
		*(int*)0x4808602C = 0xFFFFFF00;   /* TLDR */
		*(int*)0x48086030 = 0x00000001;   /* TTGR */
		*(int*)0x48086028 = 0xFFFFFF00;   /* TCRR */
		*(int*)0x48086024 = 0x000018e3;   /* TCLR */
	}
	else {
		//Set brightness
		*(int*)0x48086038 = 0xFFFFFF00 | (brightness & 0xFF);   /* TMAR */
		*(int*)0x4808602C = 0xFFFFFF00;   /* TLDR */
		*(int*)0x48086030 = 0x00000001;   /* TTGR */
		*(int*)0x48086028 = 0xFFFFFF00;   /* TCRR */
		*(int*)0x48086024 = 0x00001863;   /* TCLR */
	}
}

