#ifndef __KC1_PANEL_H
#define __KC1_PANEL_H

#include <asm/arch/dss.h>

#define PANEL_DELAY_COUNT	100
#define PANEL_TIMEOUT		1000

static const struct panel_config dpi_panel_cfg = {
	.display_type	= OMAP_DISPLAY_TYPE_DPI,
	.channel	= OMAP_DSS_CHANNEL_LCD2,
	.panel_udelay	= 10000,

	.pixel_clock    = 51200, /* in kHz */
	.lck_div        = 1,
	.pck_div        = 4,

	.x_res          = 1024,
	.y_res          = 600,
	.hfp            = 160,   /* HFP fix 160 */
	.hsw            = 10,    /* HSW = 1~140 */
	.hbp            = 160,   /* HSW + HBP = 160 */
	.vfp            = 10,    /* VFP fix 12 */
	.vsw            = 2,     /* VSW = 1~20 */
	.vbp            = 23,    /* VSW + VBP = 23 */

	.pol_freq	= 0x00003000, /* Pol Freq */

	.panel_type	= ACTIVE_DISPLAY, /* TFT */
	.data_lines	= LCD_INTERFACE_24_BIT, /* 24 Bit RGB */
	.load_mode	= 0x02, /* Frame Mode */
	.panel_color	= 0x00000000,
	.trans_color	= 0x00000000,
};

static const struct graphics_config gfx_cfg = {
	.x_pos			= 0,
	.y_pos			= 0,
	.gfx_format		= OMAP_DISP_GFX_RGB16,
	.fifo_threshold_low	= 192,
	.fifo_threshold_high	= 252,
	.row_inc		= 1,
	.pixel_inc		= 1,
	.window_skip		= 0,
	.frame_buffer		= (unsigned int *)0x82000000,
};

#define DISPC_CONFIG_LOADMODE(mode)             ((mode) << 1)
#define BSP_LCD_CONFIG                          (DISPC_CONFIG_FUNCGATED | DISPC_CONFIG_LOADMODE(2) ) //Color phase rotation

void turn_off_lcd(void);
void initialize_lcd(int show_lowbattery_fb);

void set_omap_pwm(u8 brightness);
void show_image(u_int16_t *start, u_int16_t *end);

#endif /* KC1_PANEL_H */
