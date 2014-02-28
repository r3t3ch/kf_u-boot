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

//#define OMAP4DSS_DEBUG		1

#ifdef OMAP4DSS_DEBUG
#define omap4dss_writel(v,a)	debug("[WRITEL] addr=0x%08x, value=0x%08x\n", a, v);__raw_writel((unsigned int )(v),(unsigned int *)(a))
#define omap4dss_writew(v,a)	debug("[WRITEW] addr=0x%08x, value=0x%04x\n", a, v);__raw_writew((unsigned int )(v),(unsigned int *)(a))
#else
#define omap4dss_writel(v,a)	__raw_writel((unsigned int )(v),(unsigned int *)(a))
#define omap4dss_writew(v,a)	__raw_writew((unsigned int )(v),(unsigned int *)(a))
#endif
#define omap4dss_readl(a)	__raw_readl(a)
#define omap4dss_readw(a)	__raw_readw(a)

extern struct prcm_regs const **prcm;
static int video_init_flag = 0;

struct dispc_regs *dispc = (struct dispc_regs *) OMAP4_DISPC_BASE;
//struct dss_regs *dss = (struct dss_regs *) OMAP4_DSS_BASE;

struct panel_config *panel_cfg;
struct graphics_config *gfx_cfg;

static void *frame_buffer;

/* Configure Panel Specific Parameters */
void omap4_dss_register_configs(struct panel_config *new_panel_cfg, struct graphics_config *new_gfx_cfg)
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
		omap4dss_writel((panel_cfg->panel_type & ACTIVE_DISPLAY) << TFTSTN_SHIFT |
			panel_cfg->data_lines << TFTDATALINES_SHIFT, &dispc->control1);

		omap4dss_writel(panel_cfg->panel_color, &dispc->default_color0);
		omap4dss_writel(omap4dss_readl(&dispc->control1), &dispc->control1);
		omap4dss_writel(panel_cfg->trans_color, &dispc->trans_color0);

		// Frame data only loaded every frame
		omap4dss_writel(omap4dss_readl(&(dispc->config1)) | panel_cfg->load_mode << LOADMODE_SHIFT,
			&(dispc->config1));

		omap4dss_writel(panel_cfg->pol_freq, &dispc->pol_freq1);
		udelay(panel_cfg->panel_udelay);
		omap4dss_writel(PANEL_DISPC_DIVISOR(panel_cfg->pck_div, panel_cfg->lck_div), &dispc->divisor1);
		omap4dss_writel(PANEL_LCD_SIZE(panel_cfg->x_res, panel_cfg->y_res), &dispc->size_lcd1);
		omap4dss_writel(PANEL_TIMING_H(panel_cfg->hbp, panel_cfg->hfp, panel_cfg->hsw), &dispc->timing_h1);
		omap4dss_writel(PANEL_TIMING_V(panel_cfg->vbp, panel_cfg->vfp, panel_cfg->vsw), &dispc->timing_v1);
	}
	else {
		omap4dss_writel((panel_cfg->panel_type & ACTIVE_DISPLAY) << TFTSTN_SHIFT |
			panel_cfg->data_lines << TFTDATALINES_SHIFT, &dispc->control2);

		omap4dss_writel(panel_cfg->panel_color, &dispc->default_color2);
		omap4dss_writel(omap4dss_readl(&dispc->control2), &dispc->control2);
		omap4dss_writel(panel_cfg->trans_color, &dispc->trans_color2);

		// Frame data only loaded every frame
		omap4dss_writel(omap4dss_readl(&(dispc->config1)) | panel_cfg->load_mode << LOADMODE_SHIFT,
			&(dispc->config1));

		omap4dss_writel(panel_cfg->pol_freq, &dispc->pol_freq2);
		udelay(panel_cfg->panel_udelay);
		omap4dss_writel(PANEL_DISPC_DIVISOR(panel_cfg->pck_div, panel_cfg->lck_div), &dispc->divisor2);
		omap4dss_writel(PANEL_LCD_SIZE(panel_cfg->x_res, panel_cfg->y_res), &dispc->size_lcd2);
		omap4dss_writel(PANEL_TIMING_H(panel_cfg->hbp, panel_cfg->hfp, panel_cfg->hsw), &dispc->timing_h2);
		omap4dss_writel(PANEL_TIMING_V(panel_cfg->vbp, panel_cfg->vfp, panel_cfg->vsw), &dispc->timing_v2);
	}
}

void omap4_dss_apply_graphics_config(void)
{
	if (!frame_buffer) {
		printf("*** %s::no framebuffer set!\n", __func__);
		return;
	}

	// Set framebuffer
	omap4dss_writel(frame_buffer, &dispc->gfx_ba0);

	omap4dss_writel(((gfx_cfg->x_pos << 0) | (gfx_cfg->y_pos << 16)), &dispc->gfx_position);
	omap4dss_writel(PANEL_LCD_SIZE(panel_cfg->x_res, panel_cfg->y_res), &dispc->gfx_size);

	if (panel_cfg->channel == OMAP_DSS_CHANNEL_LCD2) {
		// CHANNELOUT2
		omap4dss_writel((1<<30), &(dispc->gfx_attributes));
		udelay(panel_cfg->panel_udelay);
	}

	// ZOrder (above all)
	omap4dss_writel(omap4dss_readl(&(dispc->gfx_attributes)) | (0x3<<26), &(dispc->gfx_attributes));
	udelay(panel_cfg->panel_udelay);

	// ZOrder Enable
	omap4dss_writel(omap4dss_readl(&(dispc->gfx_attributes)) | (1<<25), &(dispc->gfx_attributes));
	udelay(panel_cfg->panel_udelay);

	// ARBITRATION = High priority
	omap4dss_writel(omap4dss_readl(&(dispc->gfx_attributes)) | (1<<14), &(dispc->gfx_attributes));
	udelay(panel_cfg->panel_udelay);

	// BURSTSIZE = 8x128bit bursts
	omap4dss_writel(omap4dss_readl(&(dispc->gfx_attributes)) | (1<<7), &(dispc->gfx_attributes));
	udelay(panel_cfg->panel_udelay);

	// ENABLE
	omap4dss_writel(omap4dss_readl(&(dispc->gfx_attributes))
		 | GFX_ENABLE | (gfx_cfg->gfx_format << GFX_FORMAT_SHIFT), &(dispc->gfx_attributes));
	udelay(panel_cfg->panel_udelay);

	// BUFFERFILLING
	omap4dss_writel(omap4dss_readl(&(dispc->config1)) | (1<<17), &(dispc->config1));

	omap4dss_writel(DISPC_GFX_FIFO_THRESHOLD(gfx_cfg->fifo_threshold_low, gfx_cfg->fifo_threshold_high),
		&dispc->gfx_fifo_threshold);
	omap4dss_writel(gfx_cfg->fifo_preload, &dispc->gfx_preload);
	omap4dss_writel(gfx_cfg->row_inc, &dispc->gfx_row_inc);
	omap4dss_writel(gfx_cfg->pixel_inc, &dispc->gfx_pixel_inc);
	omap4dss_writel(gfx_cfg->window_skip, &dispc->gfx_window_skip);
}

int omap4_dss_apply_disp_config(int delay_count, int timeout)
{
	unsigned int count, ctrl;
	int ret = 0;
	unsigned int reg = 0;

	if (panel_cfg->channel == OMAP_DSS_CHANNEL_LCD)
		reg = (unsigned int)&dispc->control1;
	else
		reg = (unsigned int)&dispc->control2;

	omap4dss_writel(omap4dss_readl(reg) | GO_LCD, reg);

	// wait for configuration to take effect
	do {
		for (count = 0; count < delay_count; ++count);
			ctrl = omap4dss_readl(reg);
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
	fclk = omap4dss_readl((*prcm)->cm_dss_dss_clkctrl);

	// Reset the display controller
	omap4dss_writel(DSS_SOFTRESET, &dispc->sysconfig);

	while (!((reg_val=omap4dss_readl(&dispc->sysstatus)) & DSS_RESETDONE) && (timeout > 0)) { // dispc sys state
		for (count = 0; count < delay_count; ++count);
		timeout--;
	}

	if (!(reg_val & DSS_RESETDONE)) {
		printf("*** %s::DSS TIMED OUT on DSS_RESETDONE!\n", __func__);
	}

	omap4dss_writel(omap4dss_readl(&dispc->sysconfig) & ~DSS_SOFTRESET, &dispc->sysconfig);

	// Restore old clock settings
	omap4dss_writel(fclk, (*prcm)->cm_dss_dss_clkctrl);
}

/* Enable LCD and DIGITAL OUT in DSS */
void omap4_dss_enable(void)
{
	u32 l;
	unsigned int reg = 0;

	if (panel_cfg->channel == OMAP_DSS_CHANNEL_LCD)
		reg = &dispc->control1;
	else
		reg = &dispc->control2;

	omap4dss_writel(panel_cfg->panel_type << TFTSTN_SHIFT |
		panel_cfg->data_lines << TFTDATALINES_SHIFT |
		LCD_ENABLE, reg);
	omap4dss_writel(omap4dss_readl(reg) | GO_LCD, reg);
	omap4dss_writel(omap4dss_readl(reg) | LCD_ENABLE, reg);

	// Add delay to prevent blinking screen.
	udelay(panel_cfg->panel_udelay);
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
