#ifndef __KC1_PANEL_H
#define __KC1_PANEL_H

/* LCD DIM / TIMINGS */
#define LCD_WIDTH           1024
#define LCD_HEIGHT          600
#define LCD_HSW             10
#define LCD_HFP             160 
#define LCD_HBP             160
#define LCD_VSW             2
#define LCD_VFP             10
#define LCD_VBP             23

#define INREG32(x)          readl((unsigned int *)(x))
#define OUTREG32(x, y)      __raw_writel((unsigned int )(y),(unsigned int *)(x))
#define SETREG32(x, y)      OUTREG32(x, INREG32(x)|(y))
#define CLRREG32(x, y)      OUTREG32(x, INREG32(x)&~(y))

/* DSS REGISTERS */
#if 0
#define DSS_SYSCONFIG                           0x48050010
#define DSS_SYSSTATUS                           0x48050014
#define DSS_CONTROL                             0x48050040
#endif

/* DISP REGISERS */
#if 0
#define DISPC_SYSCONFIG                         0x48050410
#define DISPC_SYSSTATUS                         0x48050414
#define DISPC_IRQENABLE                         0x4805041C
#define DISPC_CONTROL                           0x48050440
#define DISPC_CONFIG                            0x48050444
#define DISPC_DEFAULT_COLOR0                    0x4805044C
#define DISPC_TRANS_COLOR0                      0x48050454
#define DISPC_TIMING_H                          0x48050464
#define DISPC_TIMING_V                          0x48050468
#define DISPC_POL_FREQ                          0x4805046C
#define DISPC_DIVISOR                           0x48050470
#define DISPC_SIZE_LCD                          0x4805047C
#define DISPC_GFX_BA0                           0x48050480
#define DISPC_GFX_POSITION                      0x48050488
#define DISPC_GFX_SIZE                          0x4805048C
#define DISPC_GFX_ATTRIBUTES                    0x480504A0
#define DISPC_GFX_FIFO_THRESHOLD                0x480504A4
#define DISPC_GFX_ROW_INC                       0x480504AC
#define DISPC_GFX_PIXEL_INC                     0x480504B0
#define DISPC_GFX_WINDOW_SKIP                   0x480504B4
#endif

/* DSS DISP REGISERS */
#define DSS_DISPC_BASE                          0x48041000
#define DSS_DSI1_BASE                           0x48044000
#define DSS_DSI2_BASE                           0x48045000

#define DSS_DISPC_SYSCONFIG                     (DSS_DISPC_BASE + 0x010)
#define DSS_DISPC_SYSSTATE                      (DSS_DISPC_BASE + 0x014)
#define DSS_DISPC_CONFIG1                       (DSS_DISPC_BASE + 0x044)
#define DSS_DISPC_GFX_BA_0                      (DSS_DISPC_BASE + 0x080)
#define DSS_DISPC_GFX_POSITION                  (DSS_DISPC_BASE + 0x088)
#define DSS_DISPC_GFX_SIZE                      (DSS_DISPC_BASE + 0x08C)
#define DSS_DISPC_GFX_ATTRIBUTES                (DSS_DISPC_BASE + 0x0A0)
#define DSS_DISPC_GFX_FIFO_THRESHOLD            (DSS_DISPC_BASE + 0x0A4)
#define DSS_DISPC_GFX_ROW_INC                   (DSS_DISPC_BASE + 0x0AC)
#define DSS_DISPC_GFX_PIXEL_INC                 (DSS_DISPC_BASE + 0x0B0)
#define DSS_DISPC_GFX_WINDOW_SKIP               (DSS_DISPC_BASE + 0x0B4)
#define DSS_DISPC_CONTROL2                      (DSS_DISPC_BASE + 0x238)
#define DSS_DISPC_DEFAULT_COLOR2                (DSS_DISPC_BASE + 0x3AC)
#define DSS_DISPC_TRANS_COLOR2                  (DSS_DISPC_BASE + 0x3B0)
#define DSS_DISPC_SIZE_LCD2                     (DSS_DISPC_BASE + 0x3CC)
#define DSS_DISPC_TIMING_H2                     (DSS_DISPC_BASE + 0x400)
#define DSS_DISPC_TIMING_V2                     (DSS_DISPC_BASE + 0x404)
#define DSS_DISPC_POL_FREQ2                     (DSS_DISPC_BASE + 0x408)
#define DSS_DISPC_DIVISOR2                      (DSS_DISPC_BASE + 0x40C)

#define DSS_DSI2_IRQENABLE                      (DSS_DSI2_BASE + 0x01C)
#define DSS_DSI2_CONTROL                        (DSS_DSI2_BASE + 0x040)


#define DISPC_GFX_FIFO_THRESHOLD_R(low,high)    ((low << 0) | (high << 16))
#define DISPC_CONTROL_GOLCD                     (1 << 5)
#define DISPC_CONTROL_LCDENABLE                 (1 << 0)
#if 0
#define DISPC_SYSCONFIG_AUTOIDLE                (1 << 0)
#define DISPC_SYSCONFIG_SOFTRESET               (1 << 1)
#define DISPC_SYSSTATUS_RESETDONE               (1 << 0)

#define DSS_CONTROL_DISPC_CLK_SWITCH_DSS1_ALWON (0 << 0)
#define DSS_CONTROL_DSI_CLK_SWITCH_DSS1_ALWON   (0 << 1)

#define SYSCONFIG_NOIDLE                        (1 << 3)
#define SYSCONFIG_NOSTANDBY                     (1 << 12)

#define DISPC_GFX_ATTR_GFXENABLE                (1 << 0)
#define DISPC_GFX_ATTR_GFXFORMAT(fmt)           ((fmt) << 1)
#define DISPC_PIXELFORMAT_RGB24                 (0x9)
#define LOGO_GFX_ATTRIBUTES                     (DISPC_GFX_ATTR_GFXENABLE | DISPC_GFX_ATTR_GFXFORMAT(DISPC_PIXELFORMAT_RGB24) )// RGB24 packed, enabled

#define CM_CLKEN_DSS_MASK                   	(0x3)
#define CM_CLKEN_DSS1                       	(1 << 0)
#define CM_CLKEN_DSS2                       	(1 << 1)
#define CM_CLKEN_TV                         	(1 << 2)
#define CM_CLKEN_DSS                        	(1 << 0)
#endif

#define DISPC_CONTROL_GPOUT0                    (1 << 15)
#define DISPC_CONTROL_GPOUT1                    (1 << 16)
#define DISPC_CONTROL_TFTDATALINES_18           (2 << 8)
#define DISPC_CONTROL_TFTDATALINES_24           (3 << 8)
#define DISPC_CONTROL_STNTFT                    (1 << 3)
#define DISPC_CONFIG_FUNCGATED                  (1 << 9)
#define DISPC_CONFIG_PALETTEGAMMATABLE          (1 << 3)
#define DISPC_POL_FREQ_ONOFF                    (1 << 17)

#define DISPC_TIMING(sw,fp,bp)                  ((sw << 0) | (fp << 8) | (bp << 20))
#define DISPC_DIVISOR_R(pcd,lcd)                ((pcd << 0) | (lcd << 16))
#define DISPC_SIZE_LCD_R(lpp,ppl)               (((ppl - 1) << 0) | ((lpp - 1) << 16))

#define DISPC_CONFIG_LOADMODE(mode)             ((mode) << 1)
#define BSP_LCD_CONFIG                          (DISPC_CONFIG_FUNCGATED | DISPC_CONFIG_LOADMODE(2) ) //Color phase rotation
#define BSP_GFX_POS(x,y)                        (((x) << 0) | ((y) << 16) )
#define BSP_GFX_SIZE(x,y)                       (((x - 1) << 0) | ((y - 1) << 16) )

// Select DSS1 clock divider based on display mode
#define BSP_DSS_CLKSEL_DSS1                     ((1 << 12) | (2 << 0))	// DSS1 = DPLL4/A/2 = 43200Mhz
#define BSP_LCD_PIXCLKDIV                       0x4		// PCD pixel clock = 50.8Mhz

#define BSP_CM_CLKSEL_DSS                       (BSP_DSS_CLKSEL_DSS1)
// DSS1 = DPLL4/9 = 96MHz, divide by 4 = 24MHz pixel clock
#define LCD_LOGCLKDIV                           1
// Minimum value for LCD_PIXCLKDIV is 2
#define LCD_PIXCLKDIV                           BSP_LCD_PIXCLKDIV

#define BYTES_PER_PIXEL                         3
#define DELAY_COUNT                             100


void LcdPdd_LCD_Initialize(void);
void LcdPdd_SetPowerLevel(void);
void reset_display_controller(void);
void enable_lcd_power(void);
void configure_dss(unsigned int framebuffer);
void display_lcd_image(void);
void lcd_config(void);
void turn_off_lcd(void);
void initialize_lcd(int show_lowbattery_fb);

#if 0
void show_black_data(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
void show_white_data(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
void lbt_datainit(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
void gpio_37_high(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
void gpio_37_low(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
void gpio_121_high(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
void gpio_121_low(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
void gpio_94_high(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
void gpio_94_low(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
#endif

int show_lowbattery(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
int show_splash(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
int set_omap_pwm(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);

#endif /* KC1_PANEL_H */
