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

#include <common.h>
#include <asm/omap_common.h>
#include <asm/io.h>
#include <asm/arch/dss.h>
#include <video_fb.h>

extern struct prcm_regs const omap4_prcm;

struct dispc_regs *dispc = (struct dispc_regs *) OMAP4_DISPC_BASE;
struct dss_regs *dss = (struct dss_regs *) OMAP4_DSS_BASE;

/* Configure Panel Specific Parameters */
void omap4_dss_panel_config(const struct panel_config *panel_cfg)
{
	if (panel_cfg->channel == OMAP_DSS_CHANNEL_LCD) {
		writel(panel_cfg->pol_freq, &dispc->pol_freq1);
		udelay(panel_cfg->panel_udelay);
		writel(PANEL_DISPC_DIVISOR(panel_cfg->pck_div, panel_cfg->lck_div), &dispc->divisor1);
		writel(PANEL_LCD_SIZE(panel_cfg->x_res, panel_cfg->y_res), &dispc->size_lcd1);
		writel(PANEL_TIMING_H(panel_cfg->hbp, panel_cfg->hfp, panel_cfg->hsw), &dispc->timing_h1);
		writel(PANEL_TIMING_V(panel_cfg->vbp, panel_cfg->vfp, panel_cfg->vsw), &dispc->timing_v1);
		writel(panel_cfg->panel_type << TFTSTN_SHIFT |
			panel_cfg->data_lines << TFTDATALINES_SHIFT, &dispc->control1);
	}
	else {
		writel(panel_cfg->pol_freq, &dispc->pol_freq2);
		udelay(panel_cfg->panel_udelay);
		writel(PANEL_DISPC_DIVISOR(panel_cfg->pck_div, panel_cfg->lck_div), &dispc->divisor2);
		writel(PANEL_LCD_SIZE(panel_cfg->x_res, panel_cfg->y_res), &dispc->size_lcd2);
		writel(PANEL_TIMING_H(panel_cfg->hbp, panel_cfg->hfp, panel_cfg->hsw), &dispc->timing_h2);
		writel(PANEL_TIMING_V(panel_cfg->vbp, panel_cfg->vfp, panel_cfg->vsw), &dispc->timing_v2);
		writel(panel_cfg->panel_type << TFTSTN_SHIFT |
			panel_cfg->data_lines << TFTDATALINES_SHIFT, &dispc->control2);
	}
	writel(panel_cfg->load_mode << LOADMODE_SHIFT, &dispc->config1);
}

void omap4_graphics_config(const struct graphics_config *gfx_cfg, const struct panel_config *panel_cfg)
{
	if (panel_cfg->channel == OMAP_DSS_CHANNEL_LCD) {
		writel(panel_cfg->panel_color, &dispc->default_color0);
		writel(panel_cfg->trans_color, &dispc->trans_color0);
	}
	else {
		writel(panel_cfg->panel_color, &dispc->default_color2);
		writel(panel_cfg->trans_color, &dispc->trans_color2);
	}

	writel((u32) gfx_cfg->frame_buffer, &dispc->gfx_ba0);

	if (!gfx_cfg->frame_buffer)
		return;

	writel(DISPC_GFX_FIFO_THRESHOLD(gfx_cfg->fifo_threshold_low, gfx_cfg->fifo_threshold_high), &dispc->gfx_fifo_threshold);
	writel(gfx_cfg->row_inc, &dispc->gfx_row_inc);
	writel(gfx_cfg->pixel_inc, &dispc->gfx_pixel_inc);
	writel(gfx_cfg->window_skip, &dispc->gfx_window_skip);
	writel(((gfx_cfg->x_pos << 0) | (gfx_cfg->y_pos << 16)), &dispc->gfx_position);
	writel(gfx_cfg->gfx_format << GFX_FORMAT_SHIFT, &dispc->gfx_attributes);
	writel(PANEL_LCD_SIZE(panel_cfg->x_res, panel_cfg->y_res), &dispc->gfx_size);
}

int omap4_apply_dss_disp_config(const struct panel_config *panel_cfg, int delay_count, int timeout)
{
	unsigned int count, ctrl;
	int ret = 0;

	if (panel_cfg->channel == OMAP_DSS_CHANNEL_LCD) {
		ctrl = readl(&dispc->control1);
		ctrl |= GO_LCD;
		writel(ctrl, &dispc->control1);
	}
	else {
		ctrl = readl(&dispc->control2);
		ctrl |= GO_LCD;
		writel(ctrl, &dispc->control2);
	}

	// wait for configuration to take effect
	do {
		for (count = 0; count < delay_count; ++count);
		if (panel_cfg->channel == OMAP_DSS_CHANNEL_LCD)
			ctrl = readl(&dispc->control1);
		else
			ctrl = readl(&dispc->control2);
		timeout--;
	} while((ctrl & GO_LCD) && (timeout > 0));

	if (!timeout)
		ret = -1; // timeout

	return ret;
}

void omap4_reset_display_controller(unsigned int delay_count, unsigned int timeout)
{
	unsigned int reg_val, fclk;
	unsigned int count;

	// Store dss clocks
	fclk = readl(&omap4_prcm.cm_dss_dss_clkctrl);

	// Reset the display controller
	writel(DSS_SOFTRESET, &dispc->sysconfig);

	while (!((reg_val=readl(&dispc->sysstatus)) & DSS_RESETDONE) && (timeout > 0)) { // dispc sys state
		for (count = 0; count < delay_count; ++count);
		timeout--;
	}

	if (!(reg_val & DSS_RESETDONE)) {
		// TIMEOUT.. DO SOMETHING?
	}

	reg_val = readl(&dispc->sysconfig);
	reg_val &= ~DSS_SOFTRESET;
	writel(reg_val, &dispc->sysconfig);

	// Restore old clock settings
	writel(fclk, &omap4_prcm.cm_dss_dss_clkctrl);
}

/* Enable LCD and DIGITAL OUT in DSS */
void omap4_dss_enable(const struct panel_config *panel_cfg)
{
	struct dispc_regs *dispc = (struct dispc_regs *) OMAP4_DISPC_BASE;
	u32 l;

	if (panel_cfg->channel == OMAP_DSS_CHANNEL_LCD) {
		l = readl(&dispc->control1);
		l |= LCD_ENABLE | GO_LCD;
		writel(l, &dispc->control1);
	}
	else {
		l = readl(&dispc->control2);
		l |= LCD_ENABLE | GO_LCD;
		writel(l, &dispc->control2);
	}
}

void omap4_fill_black_data(unsigned int framebuffer, const struct panel_config *panel_cfg)
{
	unsigned int x, y;
	u_int16_t *target_addr = (u_int16_t *)framebuffer;

	for (x = 0; x < panel_cfg->x_res; x++)
		for (y = 0; y < panel_cfg->y_res; y++)
			*target_addr++ = 0x0000;
}

#ifdef CONFIG_CFB_CONSOLE
int __board_video_init(void)
{
	return -1;
}

int board_video_init(void)
			__attribute__((weak, alias("__board_video_init")));

void *video_hw_init(void)
{
	static GraphicDevice dssfb;
	GraphicDevice *pGD = &dssfb;
	struct dispc_regs *dispc = (struct dispc_regs *) OMAP4_DISPC_BASE;

	if (board_video_init() || !readl(&dispc->gfx_ba0))
		return NULL;

	pGD->winSizeX = (readl(&dispc->size_lcd) & 0x7FF) + 1;
	pGD->winSizeY = ((readl(&dispc->size_lcd) >> 16) & 0x7FF) + 1;
	pGD->gdfBytesPP = 4;
	pGD->gdfIndex = GDF_32BIT_X888RGB;
	pGD->frameAdrs = readl(&dispc->gfx_ba0);

	return pGD;
}
#endif
