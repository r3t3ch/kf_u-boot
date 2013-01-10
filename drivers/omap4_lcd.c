/***********************************************************************
 *
 * Copyright (C) 2012 Quanta, Inc.
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 ***********************************************************************/
#include <common.h>
#include <asm/byteorder.h>
#include <asm/io.h>
#include <command.h>
#include <i2c.h>
#include <otter.h>
#include <omap4_lcd.h>

#define INREG32(x)          readl((unsigned int *)(x))
#define OUTREG32(x, y)      __raw_writel((unsigned int )(y),(unsigned int *)(x))
#define SETREG32(x, y)      OUTREG32(x, INREG32(x)|(y))
#define CLRREG32(x, y)      OUTREG32(x, INREG32(x)&~(y))

#define DSS_SYSCONFIG			0x48050010
#define DSS_SYSSTATUS			0x48050014
#define DSS_CONTROL			0x48050040

#define DISPC_SYSCONFIG			0x48050410
#define DISPC_SYSSTATUS			0x48050414
#define DISPC_IRQENABLE			0x4805041C
#define DISPC_CONTROL			0x48050440
#define DISPC_CONFIG			0x48050444
#define DISPC_DEFAULT_COLOR0		0x4805044C
#define DISPC_TRANS_COLOR0		0x48050454
#define DISPC_TIMING_H			0x48050464
#define DISPC_TIMING_V			0x48050468
#define DISPC_POL_FREQ			0x4805046C
#define DISPC_DIVISOR			0x48050470
#define DISPC_SIZE_LCD			0x4805047C
#define DISPC_GFX_BA0			0x48050480
#define DISPC_GFX_POSITION		0x48050488
#define DISPC_GFX_SIZE			0x4805048C
#define DISPC_GFX_ATTRIBUTES		0x480504A0
#define DISPC_GFX_FIFO_THRESHOLD	0x480504A4
#define DISPC_GFX_ROW_INC		0x480504AC
#define DISPC_GFX_PIXEL_INC		0x480504B0
#define DISPC_GFX_WINDOW_SKIP		0x480504B4

#define CONTROL_DSS_DPLL_SPREADING		0x48002450
#define CONTROL_CORE_DPLL_SPREADING		0x48002454
#define CONTROL_PER_DPLL_SPREADING		0x48002458
#define CONTROL_USBHOST_DPLL_SPREADING		0x4800245C

#define CONTROL_DSS_DPLL_SPREADING_FREQ		0x48002544
#define CONTROL_CORE_DPLL_SPREADING_FREQ	0x48002548
#define CONTROL_PER_DPLL_SPREADING_FREQ		0x4800254C
#define CONTROL_USBHOST_DPLL_SPREADING_FREQ	0x48002550

#define DISPC_GFX_FIFO_THRESHOLD_R(low,high)	((low << 0) | (high << 16))
#define DISPC_CONTROL_GOLCD                     (1 << 5)
#define DISPC_CONTROL_LCDENABLE                 (1 << 0)
#define DISPC_SYSCONFIG_AUTOIDLE                (1 << 0)
#define DISPC_SYSCONFIG_SOFTRESET               (1 << 1)
#define DISPC_SYSSTATUS_RESETDONE               (1 << 0)

#define DSS_CONTROL_DISPC_CLK_SWITCH_DSS1_ALWON (0 << 0)
#define DSS_CONTROL_DSI_CLK_SWITCH_DSS1_ALWON   (0 << 1)

#define SYSCONFIG_NOIDLE                    	(1 << 3)
#define SYSCONFIG_NOSTANDBY                 	(1 << 12)

#define DISPC_GFX_ATTR_GFXENABLE                (1 << 0)
#define DISPC_GFX_ATTR_GFXFORMAT(fmt)           ((fmt) << 1)
#define DISPC_PIXELFORMAT_RGB24                 (0x9)

/* RGB24 packed, enabled */
#define LOGO_GFX_ATTRIBUTES         		(DISPC_GFX_ATTR_GFXENABLE | DISPC_GFX_ATTR_GFXFORMAT(DISPC_PIXELFORMAT_RGB24) )

#define CM_CLKEN_DSS_MASK                   	(0x3)
#define CM_CLKEN_DSS1                       	(1 << 0)
#define CM_CLKEN_DSS2                       	(1 << 1)
#define CM_CLKEN_TV                         	(1 << 2)
#define CM_CLKEN_DSS                        	(1 << 0)

#define DISPC_CONTROL_GPOUT0                    (1 << 15)
#define DISPC_CONTROL_GPOUT1                    (1 << 16)
#define DISPC_CONTROL_TFTDATALINES_18           (2 << 8)
#define DISPC_CONTROL_TFTDATALINES_24           (3 << 8)
#define DISPC_CONTROL_STNTFT                    (1 << 3)
#define DISPC_CONFIG_FUNCGATED                  (1 << 9)
#define DISPC_CONFIG_PALETTEGAMMATABLE          (1 << 3)
#define DISPC_POL_FREQ_ONOFF                    (1 << 17)

#define DISPC_TIMING(sw,fp,bp)			(((sw) << 0) | ((fp) << 8) | ((bp) << 20))
#define DISPC_DIVISOR_R(pcd,lcd)		(((pcd) << 0) | ((lcd) << 16))
#define DISPC_SIZE_LCD_R(lpp,ppl)		((((ppl) - 1) << 0) | (((lpp) - 1) << 16))

#define DISPC_CONFIG_LOADMODE(mode)             ((mode) << 1)

/* Color phase rotation */
#define BSP_LCD_CONFIG				(DISPC_CONFIG_FUNCGATED | DISPC_CONFIG_LOADMODE(2) )
#define BSP_GFX_POS(x,y)			(((x) << 0) | ((y) << 16) )
#define BSP_GFX_SIZE(x,y)			(((x - 1) << 0) | ((y - 1) << 16) )

#define LCD_WIDTH			1024
#define LCD_HEIGHT			600
#define LCD_HSW          		10
#define LCD_HFP          		160 
#define LCD_HBP          		150
#define LCD_VSW          		3
#define LCD_VFP          		12
#define LCD_VBP          		20

/* PCD pixel clock = 50.8Mhz */
#define BSP_LCD_PIXCLKDIV		0x4

/* DSS1 = DPLL4/9 = 96MHz, divide by 4 = 24MHz pixel clock */
#define LCD_LOGCLKDIV			1

/* Minimum value for LCD_PIXCLKDIV is 2 */
#define LCD_PIXCLKDIV			BSP_LCD_PIXCLKDIV

#define DELAY_COUNT			100

unsigned int framebuffer1=CFG_FRAMBUFFER1;
unsigned int framebuffer2=CFG_FRAMBUFFER2;
unsigned int framebuffer3=CFG_FRAMBUFFER3;
unsigned int isImageinit=0;
unsigned int g_LogoX,g_LogoY,g_LogoW,g_LogoH;

int gunzip(void *, int, unsigned char *, unsigned long *);
void spi_init(void);
void spi_command(void);

void set_omap_pwm(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);

enum {
    NONE,
    FASTBOOT_IMG,
#if 0
    LOWBATTERY_IMG,
    LOWBATTERY_CHARGING_IMG,
    AUTHFAILED_IMG,
#endif
};

typedef struct img_t {
	const char *name;
	int type;
	unsigned int x;
	unsigned int y;
	unsigned int target_x;
	unsigned int target_y;
	unsigned int width;
	unsigned int height;
} img_t;

static const struct img_t imgs[] = {
	{
		.name = "FASTBOOT_RED",
		.type = FASTBOOT_RED,
		.x = 0,
		.y = 0,
		.target_x = 100,
		.target_y = 249,
		.width  = 50,
		.height = 50,
	},
	{
		.name = "FASTBOOT_RED_OFF",
		.type = FASTBOOT_RED_OFF,
		.x = 50,
		.y = 0,
		.target_x = 100,
		.target_y = 249,
		.width  = 50,
		.height = 50,
	},
	{
		.name = "FASTBOOT_GREEN",
		.type = FASTBOOT_GREEN,
		.x = 100,
		.y = 0,
		.target_x = 100,
		.target_y = 301,
		.width  = 50,
		.height = 50,
	},
	{
		.name = "FASTBOOT_GREEN_OFF",
		.type = FASTBOOT_GREEN_OFF,
		.x = 150,
		.y = 0,
		.target_x = 100,
		.target_y = 301,
		.width  = 50,
		.height = 50,
	},
	{
		.name = "FASTBOOT",
		.type = FASTBOOT,
		.x = 0,
		.y = 51,
		.target_x = 10,
		.target_y = 175,
		.width  = 83,
		.height = 275,
	},
};

void LcdPdd_LCD_Initialize(void){
	unsigned int val = 0;

	/* CM_DSS_DSS_CLKCTRL */
	val = INREG32(0x4a009120);
	val = val & ~(0x0502);
	/* Setup the DSS1 clock divider - disable DSS1 clock, change divider, enable DSS clock */
	/* CM_DSS_DSS_CLKCTRL */
	OUTREG32(0x4a009120, val);
	udelay(10000);

	/* CM_DIV_M5_DPLL_PER */
	SETREG32(0x4a00815c, 8<<0);
	udelay(10000);
	/* CM_DSS_DSS_CLKCTRL */
	val = INREG32(0x4a009120) ;
	val = val | 0x0502;
	OUTREG32(0x4a009120, 0x00000502);
	udelay(10000);

	/* LCD control xxxx xxxx xxxx 0000 0000 0010 0000 1001 */
	/* DISPC_CONTROL2 */
	OUTREG32(0x48041238, DISPC_CONTROL_TFTDATALINES_24 | DISPC_CONTROL_STNTFT);

	/* Default Color */
	/* DISPC_DEFAULT_COLOR2 */
	OUTREG32(0x480413ac, 0x00000000);

	/* LCD control xxxx xxxx xxxx 0000 0000 0010 0000 1001 */
	/* DISPC_CONTROL2 OVERLAYOPTIMIZATION */
	SETREG32(0x48041238, 0<<12);

	/* Default Transparency Color */
	/* DISPC_TRANS_COLOR2 */
	OUTREG32(0x480413b0, 0);

	/* DISPC_CONFIG1 LOAD_MODE: Frame data only loaded every frame */
	SETREG32(0x48041044, 0x4<<0);

	/* Signal configuration */
	OUTREG32(0x48041408,0x00003000);//DISPC_POL_FREQ2
	udelay(10000);

	/* Configure the divisor */
	/* DISPC_DIVISOR2 (PCD 4,LCD 1) */
	OUTREG32(0x4804140c,DISPC_DIVISOR_R(LCD_PIXCLKDIV,LCD_LOGCLKDIV));
	/* Configure the panel size */
	/* DISPC_SIZE_LCD2 (1024,600) */
	OUTREG32(0x480413cc,DISPC_SIZE_LCD_R(LCD_HEIGHT,LCD_WIDTH));
	/* Timing logic for HSYNC signal */
	/* DISPC_TIMING_H2 */
	OUTREG32(0x48041400,DISPC_TIMING(LCD_HSW-1,LCD_HFP-1,LCD_HBP-1));
	/* Timing logic for VSYNC signal */
	/* DISPC_TIMING_V2 */
	OUTREG32(0x48041404,DISPC_TIMING(LCD_VSW-1,LCD_VFP,LCD_VBP));
}

void LcdPdd_SetPowerLevel(void){
	/* LCD control xxxx xxxx xxxx 0000 0000 0010 0000 1001 */
	/* DISPC_CONTROL2 */
	OUTREG32(0x48041238, DISPC_CONTROL_TFTDATALINES_24 | DISPC_CONTROL_STNTFT | 1<<0);

	/* Apply display configuration */
	/* DISPC_CONTROL2 */
	SETREG32(0x48041238, DISPC_CONTROL_GOLCD);
	/* Start scanning */
	/* DISPC_CONTROL2 */
	SETREG32(0x48041238, DISPC_CONTROL_LCDENABLE );
	/* Add delay to prevent blinking screen. */
	udelay(10000); 
}

/*
 *  Function:  reset_display_controller
 *  This function resets the display subsystem
 */
void reset_display_controller(void){
	unsigned int reg_val,timeout,fclk;
	unsigned short count;

	/* Enable all display clocks */
	/* CM_DSS_DSS_CLKCTRL */
	fclk = INREG32(0x4a009120);

	/* Reset the display controller */
	/* DSI_SYSCONFIG */
	OUTREG32(0x48041010, 1<<1);

	/* Wait until reset completes or timeout occurs */
	timeout=10000;
	
	while(!((reg_val=INREG32(0x48041014)) & 1<<0) && (timeout > 0)){
		for(count=0; count<DELAY_COUNT; ++count);		
			timeout--;
	}

	/* DSI_SYSCONFIG */
	reg_val=INREG32(0x48041010);
	reg_val &=~(1<<1);
	OUTREG32(0x48041010,reg_val);

	/* Restore old clock settings */
	/* CM_DSS_DSS_CLKCTRL */
	OUTREG32(0x4a009120, fclk);
}

/*
 *  Function:  enable_lcd_power
 *  This function enables the power for the LCD controller
 */
void enable_lcd_power( void ){
	/* CM_DSS_DSS_CLKCTRL */
	OUTREG32(0x4a009120, (0x00000502));
}

/*
 *  Function:  configure_dss
 *  This function configures the display sub-system
 */
void configure_dss( unsigned int framebuffer ){
	/* Configure the clock source */
	/* DSI_CTRL */
	OUTREG32(0x48045040, 1<<8 | 0<<9 | 0<<10 | 0<<12 | 0<<17);
	udelay(10000);

	// Configure interconnect parameters
	OUTREG32(0x48041010, 0x00002015);
	udelay(10000);

	/* Disable any interrupts */
	/* DSI_IRQENABLE */
	OUTREG32(0x4804501c,0);
	udelay(10000);

	/* Configure graphic window */
	/* DISPC_GFX_BA_0 */
	OUTREG32(0x48041080,framebuffer);
	/* Configure the position of graphic window */
	/* DISPC_GFX_POSITION */
	OUTREG32(0x48041088,BSP_GFX_POS(g_LogoX,g_LogoY));
	/* Configure the size of graphic window */
	/* DISPC_GFX_SIZE */
	OUTREG32(0x4804108c,BSP_GFX_SIZE(g_LogoW,g_LogoH));

	// GW Enabled, RGB24 packed, little Endian
	OUTREG32(0x480410a0,1<<30);
	udelay(10000); 
	SETREG32(0x480410a0,0x3<<26);
	udelay(10000); 
	SETREG32(0x480410a0,1<<25);
	udelay(10000); 
	SETREG32(0x480410a0,1<<14);
	udelay(10000); 
	SETREG32(0x480410a0,1<<7);
	udelay(10000);
	/* DISPC_GFX_ATTRIBUTES */
	SETREG32(0x480410a0,1<<0 | 6<<1);
	udelay(10000);

	/* DISPC_GFX_FIFO_THRESHOLD */
	OUTREG32(0x480410a4,DISPC_GFX_FIFO_THRESHOLD_R(192,252));

	/* FIXME:should be modify */
	/* DISPC_GFX_ROW_INC */
	OUTREG32(0x480410ac,1);
	/* DISPC_GFX_PIXEL_INC */
	OUTREG32(0x480410b0,1);
	/* DISPC_GFX_WINDOW_SKIP */
	OUTREG32(0x480410b4,0);

	/* Configure the LCD */
	LcdPdd_LCD_Initialize();
}

/*
 *  Function:  display_lcd_image
 *  This function displays the image in the frame buffer on the LCD
 */
void display_lcd_image(void){
	unsigned int  count, ctrl, timeout = 1000;

	/* Apply display configuration */
	/* DISPC_CONTROL2 */
	SETREG32(0x48041238,DISPC_CONTROL_GOLCD);

	/* wait for configuration to take effect */
	do{
		for(count=0;count<DELAY_COUNT;++count);
		/* DISPC_CONTROL2 */
		ctrl=INREG32(0x48041238);
		timeout-- ;
	}while((ctrl & DISPC_CONTROL_GOLCD) && (timeout > 0));

	/* Power up and start scanning */
	LcdPdd_SetPowerLevel();	
}

/*
 *  Function:  lcd_config
 *  This function configures the LCD
 */
void lcd_config(void){
	// reset_display_controller();

	/* Enable LCD clocks */
	enable_lcd_power();

	/* Configure the DSS registers */
	configure_dss(framebuffer1);

	/* Display data on LCD */
	display_lcd_image();
}

void show_black_data(void){
	u_int16_t *target_addr = (u_int16_t *)framebuffer1;
	unsigned long len = LCD_WIDTH*LCD_HEIGHT*2;
	memset(target_addr  , 0x0, len);
}

void show_white_data(void){ //kc1 black data
	u_int16_t *target_addr = (u_int16_t *)framebuffer1;
	unsigned long len = LCD_WIDTH*LCD_HEIGHT*2;
	memset(target_addr  , 0xff, len);
}

/**********************************************************
 * Routine: turn_off_lcd
 * Description: Turns off the LCD and backlight
 *
 **********************************************************/
void turn_off_lcd(void)
{
	char *pwm_on[2]  = {"pwm", "00"};
	/* Turn on backlight */
	set_omap_pwm(NULL, 0, 2, pwm_on);
	/* Disable GPIO_37 (OMAP_RGB_SHTDOWN, switch to LVDS mode) */
	__raw_writew(0x1B,0x4a10005A);
	/* gpio_OE gpio37 */
	__raw_writew(__raw_readw(0x48055134) & 0xFFDF, 0x48055134);
	/* gpio_dataout gpio37 */
	__raw_writew(__raw_readw(0x4805513C) & ~(0x0020), 0x4805513C);
	udelay(2000);
	/* Disable GPIO_47 (OMAP_3V_ENABLE, 3.3 V rail) */
	__raw_writel(0x001B0007,0x4a10006C);
	/* gpio_OE gpio47 */
	__raw_writew(__raw_readw(0x48055134) & 0x7FFF, 0x48055134);
	/* gpio_dataout gpio47 */
	__raw_writew(__raw_readw(0x4805513C) & ~( 0x8000), 0x4805513C);
	udelay(10000);
	/* Enable GPIO_47 (OMAP_3V_ENABLE, 3.3 V rail) */
	__raw_writel(0x001B0007,0x4a10006C);
	/* gpio_OE gpio47 */
	__raw_writew(__raw_readw(0x48055134) & 0x7FFF, 0x48055134);
	/* gpio_dataout gpio47 */
	__raw_writew(__raw_readw(0x4805513C) & ~( 0x8000), 0x4805513C);

	/* Enable GPIO_45 (LCD_PWR_ON) */
	__raw_writew(0x1B,0x4a10006A);
	/*gpio_OE gpio45 */
	__raw_writew(__raw_readw(0x48055134) & 0xDFFF, 0x48055134);
	/* gpio_dataout gpio45 */
	__raw_writew(__raw_readw(0x4805513C) & ~( 0x2000), 0x4805513C);
	/* gpio 119 ADO_SPK_ENABLE */
	/* gpio_119 gpio_120 pull high */
	__raw_writew(__raw_readw(0x48059134) & 0xFF7F, 0x48059134);
	__raw_writew(__raw_readw(0x4805913C) & ~( 0x0080), 0x4805913C);
	__raw_writew(__raw_readw(0x48059134) & 0xFEFF, 0x48059134);
	__raw_writew(__raw_readw(0x4805913C) & ~( 0x0100), 0x4805913C);
}

extern char const _binary_fastboot_logo_gz_start[];
extern char const _binary_fastboot_logo_gz_end[];


int load_image(int index, void *src_addr)
{
    static int loaded = NONE;

    int ret = 0;
    void *data = NULL;
	unsigned long src_len = ~0UL, dst_len = LCD_WIDTH*LCD_HEIGHT*2;

    if (index != loaded) {
        switch (index) {
        case NONE:
            loaded = index;
            break;
        case FASTBOOT_IMG:
            data = (void *) _binary_fastboot_logo_gz_start;
            memset((void *) framebuffer3, 0, dst_len);
            loaded = index;
            break;
#if 0
        case LOWBATTERY_IMG:
            data = (void *) _binary_lowbattery_gz_start;
            loaded = index;
            break;
        case LOWBATTERY_CHARGING_IMG:
            data = (void *) _binary_lowbattery_charging_gz_start;
            loaded = index;
            break;
        case AUTHFAILED_IMG:
            data = (void *) _binary_authfailed_gz_start;
            loaded = index;
            break;
#endif
        default:
            return -1;
        }
    }
    
    if (data)
        ret = gunzip(src_addr, dst_len, data, &src_len);
    return ret;
}

void showimage(int type){
	struct img_t img;
	u_int16_t *target_addr = (u_int16_t *)framebuffer1;
	u_int16_t *src_addr = (u_int16_t *)framebuffer2;
	u_int16_t *tmp_addr = (u_int16_t *)framebuffer3;
	unsigned long dst_len = LCD_WIDTH*LCD_HEIGHT*2;
	int i;

	load_image(FASTBOOT_IMG, src_addr);

	for(i =0 ; i < FASTBOOT_IMG_NUM; i++){
		if (type == imgs[i].type){
			img=imgs[i];
			break;
		}
	}

	if (i == FASTBOOT_IMG_NUM)
		return;

	src_addr += (img.y * LCD_WIDTH);
	tmp_addr += (img.target_y * LCD_WIDTH);	
	for(i =0 ; i < img.height ; i++){
		tmp_addr += img.target_x;
		src_addr += img.x;
		memcpy( tmp_addr, src_addr , img.width *2);
		tmp_addr += (LCD_WIDTH - img.target_x);
		src_addr += (LCD_WIDTH - img.x);
	}

	memcpy(target_addr , (void *) framebuffer3 , dst_len);
}

extern char const _binary_initlogo_rle_start[];
extern char const _binary_initlogo_rle_end[];
extern char const _binary_normalboot_rle_start[];
extern char const _binary_normalboot_rle_end[];
extern char const _binary_recoveryboot_rle_start[];
extern char const _binary_recoveryboot_rle_end[];
extern char const _binary_boot2_rle_start[];
extern char const _binary_boot2_rle_end[];
//extern char const _binary_booting_rle_start[];
//extern char const _binary_booting_rle_end[];

void show_splash(void)
{
	u_int16_t *target_addr = (u_int16_t *)framebuffer1;
	u_int16_t *start = (u_int16_t *)_binary_initlogo_rle_start;
	u_int16_t *end = (u_int16_t *)_binary_initlogo_rle_end;

	/* Convert the RLE data into RGB565 */
	for (; start != end; start += 2) {
		u_int16_t count = start[0];

		while (count--) {
			*target_addr++ = start[1];
		}
	}

	/* Compute position and size of logo */
	g_LogoX = 0;
	g_LogoY = 0;
	g_LogoW = LCD_WIDTH;
	g_LogoH = LCD_HEIGHT;

	return;
}

void show_normalboot_splash(void)
{
	u_int16_t *target_addr = (u_int16_t *)framebuffer1;
	u_int16_t *start = (u_int16_t *)_binary_normalboot_rle_start;
	u_int16_t *end = (u_int16_t *)_binary_normalboot_rle_end;

	/* Convert the RLE data into RGB565 */
	for (; start != end; start += 2) {
		u_int16_t count = start[0];

		while (count--) {
			*target_addr++ = start[1];
		}
	}

	/* Compute position and size of logo */
	g_LogoX = 0;
	g_LogoY = 0;
	g_LogoW = LCD_WIDTH;
	g_LogoH = LCD_HEIGHT;
	return;
}

void show_recovery_splash(void)
{
	u_int16_t *target_addr = (u_int16_t *)framebuffer1;
	u_int16_t *start = (u_int16_t *)_binary_recoveryboot_rle_start;
	u_int16_t *end = (u_int16_t *)_binary_recoveryboot_rle_end;

	/* Convert the RLE data into RGB565 */
	for (; start != end; start += 2) {
		u_int16_t count = start[0];

		while (count--) {
			*target_addr++ = start[1];
		}
	}

	/* Compute position and size of logo */
	g_LogoX = 0;
	g_LogoY = 0;
	g_LogoW = LCD_WIDTH;
	g_LogoH = LCD_HEIGHT;
	return;
}

void show_boot2_splash(void)
{
	u_int16_t *target_addr = (u_int16_t *)framebuffer1;
	u_int16_t *start = (u_int16_t *)_binary_boot2_rle_start;
	u_int16_t *end = (u_int16_t *)_binary_boot2_rle_end;

	/* Convert the RLE data into RGB565 */
	for (; start != end; start += 2) {
		u_int16_t count = start[0];

		while (count--) {
			*target_addr++ = start[1];
		}
	}

	/* Compute position and size of logo */
	g_LogoX = 0;
	g_LogoY = 0;
	g_LogoW = LCD_WIDTH;
	g_LogoH = LCD_HEIGHT;
	return;
}

#if 0
void show_booting_splash(void)
{
	u_int16_t *target_addr = (u_int16_t *)framebuffer1;
	u_int16_t *start = (u_int16_t *)_binary_booting_rle_start;
	u_int16_t *end = (u_int16_t *)_binary_booting_rle_end;

	/* Convert the RLE data into RGB565 */
	for (; start != end; start += 2) {
		u_int16_t count = start[0];

		while (count--) {
	        *target_addr++ = start[1];
		}
	}

	/* Compute position and size of logo */
	g_LogoX = 0;
	g_LogoY = 0;
	g_LogoW = LCD_WIDTH;
	g_LogoH = LCD_HEIGHT;
	return;
}
#endif

#if 0
void show_authfailed(void)
{
	show_black_data();

	load_image(AUTHFAILED_IMG, (void *) framebuffer2);

	/* Now copy the gunzipped data into the frame buffer */
	memcpy((void *)framebuffer1, (void *)framebuffer2, LCD_WIDTH * LCD_HEIGHT * 2);

	return;
}

void show_lowbattery(void)
{
	show_black_data();

	load_image(LOWBATTERY_IMG, (void *) framebuffer2);

	/* Now copy the gunzipped data into the frame buffer */
	memcpy((void *)framebuffer1, (void *)framebuffer2, LCD_WIDTH * LCD_HEIGHT * 2);

	return;
}

void show_lowbattery_charging(void)
{
	show_black_data();

	load_image(LOWBATTERY_CHARGING_IMG, (void *) framebuffer2);

	/* Now copy the gunzipped data into the frame buffer */
	memcpy((void *)framebuffer1, (void *)framebuffer2, LCD_WIDTH * LCD_HEIGHT * 2);

	return;
}
#endif

void set_omap_pwm(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int counterR;
	long brightness;

	if (argc < 2) {
		printf("Usage:\n%s\n", cmdtp->usage);
		return;
	}

	brightness = simple_strtol(argv[1], NULL, 16);
	if (brightness < 0 || brightness > 255) {
		printf("Usage:\nsetbacklight 0x0 - 0xFF\n");
		return;
	}

	/* Pull up GPIO 119 & 120 */
	__raw_writel(0x001B001B, 0x4a100110);
	/* GPIO_OE */
	__raw_writel(__raw_readl(0x48059134) & ~(0x11 << 23) , 0x48059134);
	/* GPIO_DATAOUT */
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
	//*(int*)0x48086038 = 0xFFFF7FFF;   /* TMAR */
	if (brightness == 0xFF) {
		//Set brightness
		*(int*)0x48086038 = 0xFFFFFF00;   /* TMAR */
		//*(int*)0x4808602C = 0xFFFF3FFF;   /* TLDR */
		*(int*)0x4808602C = 0xFFFFFF00;   /* TLDR */
		*(int*)0x48086030 = 0x00000001;    /* TTGR */

		*(int*)0x48086028 = 0xFFFFFF00;    /* TCRR */

		//*(int*)0x48086024 = 0x000018E3;   /* TCLR */
		*(int*)0x48086024 = 0x000018e3;   /* TCLR */
	}
	else {
		//Set brightness
		*(int*)0x48086038 = 0xFFFFFF00 | (brightness & 0xFF);   /* TMAR */
		//*(int*)0x4808602C = 0xFFFF3FFF;   /* TLDR */
		*(int*)0x4808602C = 0xFFFFFF00;   /* TLDR */
		*(int*)0x48086030 = 0x00000001;    /* TTGR */

		*(int*)0x48086028 = 0xFFFFFF00;    /* TCRR */

		//*(int*)0x48086024 = 0x000018E3;   /* TCLR */
		*(int*)0x48086024 = 0x00001863;   /* TCLR */
	}

	counterR = (*(int*)0x48086028);
}

void enable_qvx_spreading(void)
{
	/*
        CM_CLKSEL2_PLL (0x48004D44)
        (b19~b8)PERIPH_DPLL_MULT = 432 (M)
        (b6~b0)PERIPH_DPLL_DIV = 12 (N)

        CM_CLKSEL_DSS (0x48004E40)
        (b5~b0)CLKSEL_DSS1 = 5 (M4)

	Fin = 26MHz
        Fout = fc = 26(MHz)*M/(N+1)/M4 = 172.8MHz (PER_PLL)
	Fm (max) < Fref / 70 = 28.57KHz
	Set Fm = 28KHz
	Deviation = (fm/fc) x 10^(PPR/10) = (28k/172.8M) x 10^1 = 0.16%
	Δf = 0.28MHz
	ΔM = M (Δf/fc) = 432 x (1.6/1000) = 0.7
	0.7 x 2^18 = 183500.8 ~ 183501
	Fref/(4 x fm) = 2M/(4x0.028) = 17.857 = 18 x 2^0

	R_PER_DELTA_M_INT = 0
	R_PER_DELTA_M_FRACT = 0x2CCCD
	R_PER_MOD_FREQ_EXP = 0x12
	R_PER_MOD_FREQ_MANT = 0
	*/
        //printf("Last 3 \n");
	//SETREG32(0x4a008168, 0x00000000); //CM_SSC_DELTAMSTEP_DPLL_PER
        //printf("Last 2 \n");
	//SETREG32(0x4a00816c, 0x00000000 ); //CM_SSC_MODFREQDIV_DPLL_PER
        //printf("Last 1 \n");
	//SETREG32(0x4a008140, 1<<12);//CM_CLKMODE_DPLL_PER
        //printf("Last 0 \n");
	/* 	f_inp=38.4MHz,M=20,N=0,M5=10,LCD=1,PCD=3
		DISPC2_PCLK=38.4*20/(0+1)*2/10/1/3=51.2MHz
		f_c=f_inp*M*2/((N+1)*M5)=38.4M*20*2/((0+1)*10)
	*/
	/* CM_DIV_M5_DPLL_PER Set bit8 = 1, force HSDIVIDER_CLKOUT2 clock enabled*/
	__raw_writew(__raw_readw(0x4A00815C) | 0x100, 0x4A00815C);
	/* CM_SSC_DELTAMSTEP_DPLL_PER */
	__raw_writew(0XCC , 0x4A008168);
	/* CM_SSC_MODFREQDIV_DPLL_PER */
	__raw_writew(0X264 , 0x4A00816C);
	/* CM_CLKMODE_DPLL_PER Set bit12 = 1, force DPLL_SSC_EN enabled*/
	__raw_writew(__raw_readw(0x4A008140) | 0x1000 , 0x4A008140);
}

/**********************************************************
 * Routine: initialize_lcd
 * Description: Initializes the LCD and displays the
 *              splash logo
 **********************************************************/
void initialize_lcd(int show_what)
{
	char *pwm_on[2]  = {"pwm", "7F"};

	/* gpio_119 gpio_120 pull high */
	__raw_writel(0x001B001B,0x4a100110);
	__raw_writew(__raw_readw(0x48059134) & 0xFEFF, 0x48059134);
	__raw_writew(__raw_readw(0x4805913C) | 0x0100, 0x4805913C);

	__raw_writew(__raw_readw(0x48059134) & 0xFF7F, 0x48059134);
	__raw_writew(__raw_readw(0x4805913C) | 0x0080, 0x4805913C);

	spi_init();

	/* Enable GPIO_45 (LCD_PWR_ON) */
	__raw_writew(0x1B,0x4a10006A);
	/*gpio_OE gpio45 */
	__raw_writew(__raw_readw(0x48055134) & 0xDFFF, 0x48055134);
	/* gpio_dataout gpio45 */
	__raw_writew(__raw_readw(0x4805513C) | 0x2000, 0x4805513C);

	/* Enable GPIO_47 (OMAP_3V_ENABLE, 3.3 V rail) */
	__raw_writel(0x001B0007,0x4a10006C);
	/* gpio_OE gpio47 */
	__raw_writew(__raw_readw(0x48055134) & 0x7FFF, 0x48055134);
	/* gpio_dataout gpio47 */
	__raw_writew(__raw_readw(0x4805513C) | 0x8000, 0x4805513C);

	udelay(10000);
	/* kc1 lcd initialize */
	spi_command();

	udelay(2000);
	/* Enable GPIO_37 (OMAP_RGB_SHTDOWN, switch to LVDS mode) */
	__raw_writew(0x1B,0x4a10005A);
	/* gpio_OE gpio37 */
	__raw_writew(__raw_readw(0x48055134) & 0xFFDF, 0x48055134);
	/* gpio_dataout gpio37 */
	__raw_writew(__raw_readw(0x4805513C) | 0x0020, 0x4805513C);

	show_black_data();
#if 0
    if(show_what==OTTER_LCD_DISPLAY_LOW_BATT_SCREEN) {
		show_lowbattery();
	} else if (show_what==OTTER_LCD_DISPLAY_LOW_BATT_CHARGING_SCREEN) {
		show_lowbattery_charging();
        } else {
	    show_splash();
	}
#endif
    show_splash();
	lcd_config();
	enable_qvx_spreading();
	/* Turn on backlight */
	set_omap_pwm(NULL, 0, 2, pwm_on);
}

int do_lcd (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	char *pwm_on[2]  = {"pwm", "7F"};
	if (argc == 3){
		if((strncmp(argv[1], "setbrightness", strlen("setbrightness")) == 0)) {
			pwm_on[1]  = argv[2];
			set_omap_pwm(NULL, 0, 2, pwm_on);
			printf("setting brightness to '%s'\n", argv[2]);
		}else if ((strncmp(argv[1], "showimage", strlen("showimage")) == 0)) {
			showimage(simple_strtoul(argv[2], NULL, 16) - 1);
		}else {
			printf ("Usage:\n%s", cmdtp->usage);
		}
	}else if (argc == 2){
		if (strncmp(argv[1], "white", strlen("white")) == 0){
			printf("show white data\n");
			show_white_data();
		}else if (strncmp(argv[1], "black", strlen("black")) == 0){
			printf("show black data\n");
			show_black_data();
		}else if (strncmp(argv[1], "off", strlen("off")) == 0){
			turn_off_lcd();
		}else if (strncmp(argv[1], "on", strlen("on")) == 0){
			initialize_lcd(OTTER_LCD_DISPLAY_SPLASH);
		}else if (strncmp(argv[1], "showlogo", strlen("showlogo")) == 0){
			show_splash();
		} else if (strncmp(argv[1], "shownormalboot", strlen("shownormalboot")) == 0){
			show_normalboot_splash();
		} else if (strncmp(argv[1], "showrecovery", strlen("showrecovery")) == 0) {
			show_recovery_splash();
		} else if (strncmp(argv[1], "showboot2", strlen("showboot2")) == 0){
			show_boot2_splash();
#if 0
		} else if (strncmp(argv[1], "showbooting", strlen("showbooting")) == 0){
			show_booting_splash();
#endif
#if 0
		} else if (strncmp(argv[1], "showauthfailed", strlen("showauthfailed")) == 0){
			show_authfailed();
		} else if (strncmp(argv[1], "showlowbatterycharging", strlen("showlowbatterycharging")) == 0) {
			show_lowbattery_charging();
		} else if (strncmp(argv[1], "showlowbattery", strlen("showlowbattery")) == 0){
			show_lowbattery();
#endif
		}else{
			printf ("Usage:\n%s", cmdtp->usage);
		}
	}else {
		printf ("Usage:\n%s", cmdtp->usage);
	}


	return 0;
}

U_BOOT_CMD(
	qlcd,3,1,do_lcd,
	"qlcd    - lcd functions\n",
	"qlcd white - show white data.\n"
	"qlcd black - show black data.\n"
	"qlcd showlogo - show logo.\n"
	"qlcd showauthfailed - show authentication failed screen.\n"
	"qlcd off - turn off lcd.\n"
	"qlcd on  - turn on lcd.\n"
	"qlcd showimage <1 ~ 5>.\n"
	"     1 - FASTBOOT RED\n"
	"     2 - FASTBOOT RED_OFF \n"
	"     3 - FASTBOOT GREEN \n"
	"     4 - FASTBOOT GREEN_OFF \n"
	"     5 - FASTBOOT \n"
	"qlcd setbrightness <value>\n"
	"     the value will be 0 ~ 0xff\n"
	);

