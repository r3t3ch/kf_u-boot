// otter panel setup uses omap4_dss driver

#include <config.h>
#include <common.h>
#include <asm/byteorder.h>
#include <asm/io.h>
#include <asm/omap_common.h>
#include <asm/gpio.h>
#include <command.h>
#include <i2c.h>
#include <lcd.h>

#include "kc1_panel.h"
#include "kc1_debug.h"
#include "omap4_spi.h"

#ifdef CONFIG_LCD
extern char const _binary_initlogo_bmp_gz_start[];
extern char const _binary_initlogo_bmp_gz_end[];
extern char const _binary_lowbattery_bmp_gz_start[];
extern char const _binary_lowbattery_bmp_gz_end[];
extern char const _binary_multi_download_bmp_gz_start[];
extern char const _binary_multi_download_bmp_gz_end[];
#else
extern char const _binary_initlogo_rle_start[];
extern char const _binary_initlogo_rle_end[];
extern char const _binary_lowbattery_rle_start[];
extern char const _binary_lowbattery_rle_end[];
extern char const _binary_multi_download_rle_start[];
extern char const _binary_multi_download_rle_end[];
#endif

static u8 logo_index = 0;
extern struct prcm_regs const **prcm;

static const struct panel_config dpi_panel_cfg = {
	.display_type	= OMAP_DISPLAY_TYPE_DPI,
	.channel	= OMAP_DSS_CHANNEL_LCD2,
	.panel_udelay	= 10000,

	.pixel_clock    = 51200, /* in kHz */
	.lck_div        = 1,
	.pck_div        = 4,

	.x_res          = LCD_WIDTH,
	.y_res          = LCD_HEIGHT,
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
	.video_data_format	= GDF_16BIT_565RGB,
};

#ifdef CONFIG_LCD
vidinfo_t panel_info = {
	.vl_col  = LCD_WIDTH,
	.vl_row  = LCD_HEIGHT,
	.vl_bpix = LCD_COLOR16,
	.cmap 	 = NULL,
};
#endif

void set_logoindex(u8 index)
{
	logo_index = index;
}

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

#ifdef CONFIG_LCD
/*
 * 16bpp color definitions
 */
# define CONSOLE_COLOR_BLACK	0x0000
# define CONSOLE_COLOR_RED	0xF800
# define CONSOLE_COLOR_GREEN	0x07E0
# define CONSOLE_COLOR_YELLOW	0xFFE0
# define CONSOLE_COLOR_BLUE	0x001F
# define CONSOLE_COLOR_MAGENTA	0xF81F
# define CONSOLE_COLOR_CYAN	0x07FF
# define CONSOLE_COLOR_GREY	0x7BEF
# define CONSOLE_COLOR_WHITE	0xFFFF	/* Must remain last / highest	*/

/************************************************************************/
/**  Small utility to check that you got the colours right		*/
/************************************************************************/
#define	N_BLK_VERT	2
#define	N_BLK_HOR	3

static int test_colors[N_BLK_HOR * N_BLK_VERT] = {
	CONSOLE_COLOR_RED,	CONSOLE_COLOR_GREEN,	CONSOLE_COLOR_YELLOW,
	CONSOLE_COLOR_BLUE,	CONSOLE_COLOR_MAGENTA,	CONSOLE_COLOR_CYAN,
};

static void kc1_test_pattern(void)
{
	ushort v_max  = panel_info.vl_row;
	ushort h_max  = panel_info.vl_col;
	ushort v_step = (v_max + N_BLK_VERT - 1) / N_BLK_VERT;
	ushort h_step = (h_max + N_BLK_HOR  - 1) / N_BLK_HOR;
	ushort v, h;
	uint16_t *pix = (uint16_t *)omap4_dss_get_frame_buffer();

	printf("[LCD] Test Pattern: %d x %d [%d x %d]\n",
		h_max, v_max, h_step, v_step);

	/* WARNING: Code silently assumes 8bit/pixel */
	for (v = 0; v < v_max; ++v) {
		uchar iy = v / v_step;
		for (h = 0; h < h_max; ++h) {
			uchar ix = N_BLK_HOR * iy + h / h_step;
			*pix++ = test_colors[ix];
		}
	}
}
#endif

/**********************************************************
 * Routine: initialize_lcd
 * Description: Initializes the LCD and displays the
 *              splash logo
 **********************************************************/
void initialize_lcd(void)
{
	struct dispc_regs *dispc = (struct dispc_regs *) OMAP4_DISPC_BASE;
	struct dsi_regs *dsi2 = (struct dsi_regs *) OMAP4_DSI2_BASE;
	unsigned int val = 0;

	debug("*** %s\n", __func__);
	omap4_dss_register_configs(&dpi_panel_cfg, &gfx_cfg);
	__raw_writel(0x001B001B, 0x4a100110); // 119/120 SELECT GPIO_MUX, PULLEN, PULLUP
	__raw_writew(__raw_readw(0x48059134) & 0xFEFF, 0x48059134);
	__raw_writew(__raw_readw(0x4805913C) | 0x0100, 0x4805913C);

	__raw_writew(__raw_readw(0x48059134) & 0xFF7F, 0x48059134);
	__raw_writew(__raw_readw(0x4805913C) | 0x0080, 0x4805913C);

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

	omap4_dss_reset_display_controller(PANEL_DELAY_COUNT, PANEL_TIMEOUT);

	omap4_dss_fill_black_data();

	// render buffer
#ifdef CONFIG_LCD
	if (logo_index == 2)
		show_image((u_int16_t *)_binary_multi_download_bmp_gz_start, 0);
	else if (logo_index == 1)
		show_image((u_int16_t *)_binary_lowbattery_bmp_gz_start, 0);
	else
		show_image((u_int16_t *)_binary_initlogo_bmp_gz_start, 0);
#else
	if (logo_index == 2)
		show_image((u_int16_t *)_binary_multi_download_rle_start, (u_int16_t *)_binary_multi_download_rle_end);
	else if (logo_index == 1)
		show_image((u_int16_t *)_binary_lowbattery_rle_start, (u_int16_t *)_binary_lowbattery_rle_end);
	else
		show_image((u_int16_t *)_binary_initlogo_rle_start, (u_int16_t *)_binary_initlogo_rle_end);
#endif

	// Enable DSS clocks
	__raw_writel(0x00000502, (*prcm)->cm_dss_dss_clkctrl);

	// Configure the clock source
	// VP_CLK_POL, DE ACTIVE LOW, HSYNC ACTIVE LOW, NO LINE BUFFER, NO HSYNC START SHORT PACKET
	__raw_writel((1<<8 | 0<<9 | 0<<10 | 0<<12 | 0<<17), &(dsi2->ctrl));
	udelay(dpi_panel_cfg.panel_udelay);

	// Configure interconnect parameters
	__raw_writel((1<<0 | 1<<2 | 1<<4 | 1<<13), &(dispc->sysconfig));
	udelay(dpi_panel_cfg.panel_udelay);

	// Disable any interrupts
	__raw_writel(0x0, &(dsi2->irqenable));
	udelay(dpi_panel_cfg.panel_udelay);

	omap4_dss_apply_graphics_config();

	// CHANNELOUT2
	__raw_writel(__raw_readl(&(dispc->gfx_attributes)) | (1<<30), &(dispc->gfx_attributes));
	udelay(dpi_panel_cfg.panel_udelay);

	// ZOrder (above all)
	__raw_writel(__raw_readl(&(dispc->gfx_attributes)) | (0x3<<26), &(dispc->gfx_attributes));
	udelay(dpi_panel_cfg.panel_udelay);

	// ZOrder Enable
	__raw_writel(__raw_readl(&(dispc->gfx_attributes)) | (1<<25), &(dispc->gfx_attributes));
	udelay(dpi_panel_cfg.panel_udelay);

	// ARBITRATION = High priority
	__raw_writel(__raw_readl(&(dispc->gfx_attributes)) | (1<<14), &(dispc->gfx_attributes));
	udelay(dpi_panel_cfg.panel_udelay);

	// BURSTSIZE = 8x128bit bursts
	__raw_writel(__raw_readl(&(dispc->gfx_attributes)) | (1<<7), &(dispc->gfx_attributes));
	udelay(dpi_panel_cfg.panel_udelay);

	// ENABLE
	__raw_writel(__raw_readl(&(dispc->gfx_attributes)) | GFX_ENABLE, &(dispc->gfx_attributes));
	udelay(dpi_panel_cfg.panel_udelay);


	// Setup the DSS1 clock divider - disable DSS1 clock, change divider, enable DSS clock
	// Enable LCD clocks

	val = __raw_readl((*prcm)->cm_dss_dss_clkctrl);
	// TURN OFF: 1=DSS_CM_ON,8=DSS_OPTFuncClockEN,10=SYS_OPTFuncClockEN
	val = val & ~(0x00000502);
	__raw_writel(val, (*prcm)->cm_dss_dss_clkctrl);
	udelay(dpi_panel_cfg.panel_udelay);

	val = __raw_readl((*prcm)->cm_div_m5_dpll_per);
	val = val | (8 << 0);
	__raw_writel(val, (*prcm)->cm_div_m5_dpll_per);
	udelay(dpi_panel_cfg.panel_udelay);

	// TURN ON: 1=DSS_CM_ON,8=DSS_OPTFuncClockEN,10=SYS_OPTFuncClockEN
	__raw_writel(0x00000502, (*prcm)->cm_dss_dss_clkctrl);
	udelay(dpi_panel_cfg.panel_udelay);

	// LCD control xxxx xxxx xxxx 0000 0000 0010 0000 1001
	// OVERLAYOPTIMIZATION
	val = __raw_readl(&(dispc->control2));
	val = val | (0 << 12);
	__raw_writel(val, &(dispc->control2));

	// Frame data only loaded every frame
	val = __raw_readl(&(dispc->config1));
	val = val | (0 << 12);
	__raw_writel(val, &(dispc->config1));

	omap4_dss_apply_panel_config();
	udelay(dpi_panel_cfg.panel_udelay);

	omap4_dss_apply_disp_config(PANEL_DELAY_COUNT, PANEL_TIMEOUT);

	omap4_dss_enable();

	// Add delay to prevent blinking screen.
	udelay(dpi_panel_cfg.panel_udelay);

	/* Turn on backlight */
	set_omap_pwm(0x7F);
dump_dss();
	omap4_dss_video_init(1);
}

void show_image(u_int16_t *start, u_int16_t *end)
{
#ifdef CONFIG_LCD
	char buffer_cmd[100];

	sprintf(buffer_cmd, "bmp info 0x%8x", (u_int16_t *)start);
	debug("*** %s::buffer_cmd = '%s'\n", __func__, buffer_cmd);
	run_command(buffer_cmd, 0);

	sprintf(buffer_cmd, "bmp display 0x%8x 0 0", (u_int16_t *)start);
	debug("*** %s::buffer_cmd = '%s'\n", __func__, buffer_cmd);
	run_command(buffer_cmd, 0);
//	kc1_test_pattern();
#else
	u_int16_t *target_addr = (uint16_t *)omap4_dss_get_frame_buffer();
	if (!target_addr) {
		printf("*** %s:: no framebuffer!\n", __func__);
		return;
	}

	/* Convert the RLE data into RGB565 */
	for (; start != end; start += 2) {
		u_int16_t count = start[0];
		while (count--) {
			*target_addr++ = start[1];
		}
	}
#endif

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

int board_video_init(void)
{
	debug("*** %s\n", __func__);
	initialize_lcd();
	return 0;
}

