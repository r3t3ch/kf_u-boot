/*
 * (C) Copyright 2010
 * Texas Instruments, <www.ti.com>
 * Syed Mohammed Khasim <khasim@ti.com>
 *
 * Referred to Linux Kernel DSS driver files for OMAP3 by
 * Tomi Valkeinen from drivers/video/omap2/dss/
 * editted for omap4 by Michael Scott
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation's version 2 and any
 * later version the License.
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
#include <common.h>
#include <environment.h>
#include <asm/omap_common.h>
#include <asm/io.h>
#include <asm/arch/dss.h>
#include <video_fb.h>
#include <lcd.h>

DECLARE_GLOBAL_DATA_PTR;

extern struct prcm_regs const **prcm;
static int video_init_flag = 0;

struct dispc_regs *dispc = (struct dispc_regs *) OMAP4_DISPC_BASE;
//struct dss_regs *dss = (struct dss_regs *) OMAP4_DSS_BASE;

struct panel_config *panel_cfg;
struct graphics_config *gfx_cfg;

static void *frame_buffer;

/* Configure Panel Specific Parameters */
void omap4_dss_register_configs(const struct panel_config *new_panel_cfg, const struct graphics_config *new_gfx_cfg)
{
	panel_cfg = new_panel_cfg;
	gfx_cfg = new_gfx_cfg;
#ifndef CONFIG_LCD
	frame_buffer = CONFIG_FB_ADDR;
#endif
}

void omap4_dss_apply_panel_config(void)
{
	if (panel_cfg->channel == OMAP_DSS_CHANNEL_LCD) {
		__raw_writel(panel_cfg->pol_freq, &dispc->pol_freq1);
		udelay(panel_cfg->panel_udelay);
		__raw_writel(PANEL_DISPC_DIVISOR(panel_cfg->pck_div, panel_cfg->lck_div), &dispc->divisor1);
		__raw_writel(PANEL_LCD_SIZE(panel_cfg->x_res, panel_cfg->y_res), &dispc->size_lcd1);
		__raw_writel(PANEL_TIMING_H(panel_cfg->hbp, panel_cfg->hfp, panel_cfg->hsw), &dispc->timing_h1);
		__raw_writel(PANEL_TIMING_V(panel_cfg->vbp, panel_cfg->vfp, panel_cfg->vsw), &dispc->timing_v1);
		__raw_writel(panel_cfg->panel_type << TFTSTN_SHIFT |
			panel_cfg->data_lines << TFTDATALINES_SHIFT, &dispc->control1);
	}
	else {
		__raw_writel(panel_cfg->pol_freq, &dispc->pol_freq2);
		udelay(panel_cfg->panel_udelay);
		__raw_writel(PANEL_DISPC_DIVISOR(panel_cfg->pck_div, panel_cfg->lck_div), &dispc->divisor2);
		__raw_writel(PANEL_LCD_SIZE(panel_cfg->x_res, panel_cfg->y_res), &dispc->size_lcd2);
		__raw_writel(PANEL_TIMING_H(panel_cfg->hbp, panel_cfg->hfp, panel_cfg->hsw), &dispc->timing_h2);
		__raw_writel(PANEL_TIMING_V(panel_cfg->vbp, panel_cfg->vfp, panel_cfg->vsw), &dispc->timing_v2);
		__raw_writel(panel_cfg->panel_type << TFTSTN_SHIFT |
			panel_cfg->data_lines << TFTDATALINES_SHIFT, &dispc->control2);
	}
	__raw_writel(panel_cfg->load_mode << LOADMODE_SHIFT, &dispc->config1);
}

void omap4_dss_apply_graphics_config(void)
{
	if (panel_cfg->channel == OMAP_DSS_CHANNEL_LCD) {
		__raw_writel(panel_cfg->panel_color, &dispc->default_color0);
		__raw_writel(panel_cfg->trans_color, &dispc->trans_color0);
	}
	else {
		__raw_writel(panel_cfg->panel_color, &dispc->default_color2);
		__raw_writel(panel_cfg->trans_color, &dispc->trans_color2);
	}

	if (!frame_buffer) {
		printf("*** %s::no framebuffer set!\n", __func__);
		return;
	}

	__raw_writel(DISPC_GFX_FIFO_THRESHOLD(gfx_cfg->fifo_threshold_low, gfx_cfg->fifo_threshold_high),
		&dispc->gfx_fifo_threshold);
	__raw_writel(gfx_cfg->row_inc, &dispc->gfx_row_inc);
	__raw_writel(gfx_cfg->pixel_inc, &dispc->gfx_pixel_inc);
	__raw_writel(gfx_cfg->window_skip, &dispc->gfx_window_skip);
	__raw_writel(((gfx_cfg->x_pos << 0) | (gfx_cfg->y_pos << 16)), &dispc->gfx_position);
	__raw_writel(gfx_cfg->gfx_format << GFX_FORMAT_SHIFT, &dispc->gfx_attributes);
	__raw_writel(PANEL_LCD_SIZE(panel_cfg->x_res, panel_cfg->y_res), &dispc->gfx_size);

	// Set framebuffer
	__raw_writel(frame_buffer, &dispc->gfx_ba0);
}

int omap4_dss_apply_disp_config(int delay_count, int timeout)
{
	unsigned int count, ctrl;
	int ret = 0;

	if (panel_cfg->channel == OMAP_DSS_CHANNEL_LCD) {
		ctrl = __raw_readl(&dispc->control1);
		ctrl |= GO_LCD;
		__raw_writel(ctrl, &dispc->control1);
	}
	else {
		ctrl = __raw_readl(&dispc->control2);
		ctrl |= GO_LCD;
		__raw_writel(ctrl, &dispc->control2);
	}

	// wait for configuration to take effect
	do {
		for (count = 0; count < delay_count; ++count);
		if (panel_cfg->channel == OMAP_DSS_CHANNEL_LCD)
			ctrl = __raw_readl(&dispc->control1);
		else
			ctrl = __raw_readl(&dispc->control2);
		timeout--;
	} while((ctrl & GO_LCD) && (timeout > 0));

	if (!timeout)
		ret = -1; // timeout

	return ret;
}

void omap4_dss_reset_display_controller(unsigned int delay_count, unsigned int timeout)
{
	unsigned int reg_val, fclk;
	unsigned int count;

	// Store dss clocks
	fclk = __raw_readl((*prcm)->cm_dss_dss_clkctrl);

	// Reset the display controller
	__raw_writel(DSS_SOFTRESET, &dispc->sysconfig);

	while (!((reg_val=__raw_readl(&dispc->sysstatus)) & DSS_RESETDONE) && (timeout > 0)) { // dispc sys state
		for (count = 0; count < delay_count; ++count);
		timeout--;
	}

	if (!(reg_val & DSS_RESETDONE)) {
		printf("*** %s::DSS TIMED OUT on DSS_RESETDONE!\n", __func__);
	}

	reg_val = __raw_readl(&dispc->sysconfig);
	reg_val &= ~DSS_SOFTRESET;
	__raw_writel(reg_val, &dispc->sysconfig);

	// Restore old clock settings
	__raw_writel(fclk, (*prcm)->cm_dss_dss_clkctrl);
}

/* Enable LCD and DIGITAL OUT in DSS */
void omap4_dss_enable(void)
{
	u32 l;

	if (panel_cfg->channel == OMAP_DSS_CHANNEL_LCD) {
		l = __raw_readl(&dispc->control1);
		l |= LCD_ENABLE | GO_LCD;
		__raw_writel(l, &dispc->control1);
	}
	else {
		l = __raw_readl(&dispc->control2);
		l |= LCD_ENABLE | GO_LCD;
		__raw_writel(l, &dispc->control2);
	}
}

void omap4_dss_fill_black_data(void)
{
	unsigned int x, y;
	u_int16_t *target_addr = (u_int16_t *)frame_buffer;
	if (!target_addr) {
		printf("*** %s:: no framebuffer!\n", __func__);
		return;
	}

	for (x = 0; x < panel_cfg->x_res; x++)
		for (y = 0; y < panel_cfg->y_res; y++)
			*target_addr++ = 0x0000;
}

void omap4_dss_video_init(int flag)
{
	video_init_flag = flag;
}

void *omap4_dss_get_frame_buffer(void)
{
	return frame_buffer;
}

#if defined(CONFIG_CFB_CONSOLE) || defined(CONFIG_LCD)
int __board_video_init(void)
{
	return -1;
}

int board_video_init(void)
			__attribute__((weak, alias("__board_video_init")));

#ifdef CONFIG_CFB_CONSOLE
void *video_hw_init(void)
{
	static GraphicDevice dssfb;
	GraphicDevice *pGD = &dssfb;
	struct dispc_regs *dispc = (struct dispc_regs *) OMAP4_DISPC_BASE;

	if (!video_init_flag)
		if (board_video_init() || !frame_buffer) {
			debug("*** %s::no video init or framebuffer!\n", __func__);
			return NULL;
		}

	pGD->winSizeX = panel_cfg->x_res;
	pGD->winSizeY = panel_cfg->y_res;
	pGD->gdfBytesPP = 2;
	pGD->gdfIndex = gfx_cfg->video_data_format;
	pGD->frameAdrs = frame_buffer;

	return pGD;
}
#endif

#ifdef CONFIG_LCD
void lcd_ctrl_init(void *lcdbase)
{
	frame_buffer = lcdbase;
	return;
}

void lcd_enable(void)
{
	debug("*** %s\n", __func__);
	if (!video_init_flag)
		if (board_video_init())
			return;
}
#endif

#endif /* defined(CONFIG_CFB_CONSOLE) || defined(CONFIG_LCD) */
