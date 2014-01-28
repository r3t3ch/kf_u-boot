// Seven modifies source codes from WinCE EBoot

#include <common.h>
#include <asm/byteorder.h>
#include <asm/io.h>
#include <asm/omap_common.h>
#include <command.h>
#include <i2c.h>

#include "kc1_panel.h"
#include "omap4_spi.h"

extern struct prcm_regs const omap4_prcm;

#define PANEL_UDELAY    10000

unsigned int g_LogoX, g_LogoY, g_LogoW, g_LogoH;

/*******************************************************
 * Routine: delay
 * Description: spinning delay to use before udelay works
 ******************************************************/
#if 0
static inline void delay(unsigned long loops){
	__asm__ volatile ("1:\n" "subs %0, %1, #1\n"
			  "bne 1b":"=r" (loops):"0"(loops));
}
#endif

void LcdPdd_LCD_Initialize(void)
{
    unsigned int val = 0;

    val = INREG32(omap4_prcm.cm_dss_dss_clkctrl);
    val = val & ~(0x0502);

    // Setup the DSS1 clock divider - disable DSS1 clock, change divider, enable DSS clock
    OUTREG32(omap4_prcm.cm_dss_dss_clkctrl, val);
    udelay(PANEL_UDELAY);

    SETREG32(omap4_prcm.cm_div_m5_dpll_per, 8 << 0);
    udelay(PANEL_UDELAY);

    val = INREG32(omap4_prcm.cm_dss_dss_clkctrl) ;
    val = val | 0x0502;
    OUTREG32(omap4_prcm.cm_dss_dss_clkctrl, 0x00000502);
    udelay(PANEL_UDELAY);

    // LCD control xxxx xxxx xxxx 0000 0000 0010 0000 1001
    OUTREG32(DSS_DISPC_CONTROL2, DISPC_CONTROL_TFTDATALINES_24 | DISPC_CONTROL_STNTFT);
		
    // Default Color
    OUTREG32(DSS_DISPC_DEFAULT_COLOR2, 0x00000000);

    // LCD control xxxx xxxx xxxx 0000 0000 0010 0000 1001
    // OVERLAYOPTIMIZATION
    SETREG32(DSS_DISPC_CONTROL2, 0 << 12);

    // Default Transparency Color
    OUTREG32(DSS_DISPC_TRANS_COLOR2, 0);

    // Frame data only loaded every frame
    SETREG32(DSS_DISPC_CONFIG1, 0x4 << 0);

    // Signal configuration
    //	OUTREG32(DSS_DISPC_POL_FREQ2, DISPC_POL_FREQ_ONOFF);
    OUTREG32(DSS_DISPC_POL_FREQ2, 0x00003000);
    udelay(PANEL_UDELAY);

    // Configure the divisor
    OUTREG32(DSS_DISPC_DIVISOR2, DISPC_DIVISOR_R(LCD_PIXCLKDIV, LCD_LOGCLKDIV)); // (PCD 4,LCD 1)

    // Configure the panel size
    OUTREG32(DSS_DISPC_SIZE_LCD2, DISPC_SIZE_LCD_R(LCD_HEIGHT, LCD_WIDTH)); // (1024,600)

    // Timing logic for HSYNC signal
    OUTREG32(DSS_DISPC_TIMING_H2, DISPC_TIMING((LCD_HSW-1), (LCD_HFP-1), (LCD_HBP-1)));

    // Timing logic for VSYNC signal
    OUTREG32(DSS_DISPC_TIMING_V2, DISPC_TIMING((LCD_VSW-1), LCD_VFP, LCD_VBP));
}


void LcdPdd_SetPowerLevel(void)
{
    // LCD control xxxx xxxx xxxx 0000 0000 0010 0000 1001
    OUTREG32(DSS_DISPC_CONTROL2, DISPC_CONTROL_TFTDATALINES_24 | DISPC_CONTROL_STNTFT | 1 << 0);

    // Apply display configuration
    SETREG32(DSS_DISPC_CONTROL2, DISPC_CONTROL_GOLCD);

    // Start scanning
    SETREG32(DSS_DISPC_CONTROL2, DISPC_CONTROL_LCDENABLE );

    // Add delay to prevent blinking screen.
    udelay(PANEL_UDELAY); 
}


//  Function:  reset_display_controller
//  This function resets the display subsystem
void reset_display_controller(void)
{
    unsigned int reg_val, timeout, fclk;
    unsigned short count;

    // Enable all display clocks
    fclk = INREG32(omap4_prcm.cm_dss_dss_clkctrl);

    // Reset the display controller
    OUTREG32(DSS_DISPC_SYSCONFIG, 1 << 1);

    // Wait until reset completes or timeout occurs
    timeout = 10000;
    while (!((reg_val=INREG32(DSS_DISPC_SYSSTATE)) & 1 << 0) && (timeout > 0)) { // dispc sys state
        for (count = 0; count < DELAY_COUNT; ++count);		
        timeout--;
    }

    if (!(reg_val & 1 << 0)) {
	//puts("reset_display_controller: DSS DISPC reset timeout.\n");
    }

    reg_val=INREG32(DSS_DISPC_SYSCONFIG);
    reg_val &= ~(1 << 1);
    OUTREG32(DSS_DISPC_SYSCONFIG, reg_val);

    // Restore old clock settings
    OUTREG32(omap4_prcm.cm_dss_dss_clkctrl, fclk);
}


//  Function:  enable_lcd_power
//  This function enables the power for the LCD controller
void enable_lcd_power(void)
{
    OUTREG32(omap4_prcm.cm_dss_dss_clkctrl, 0x00000502);
}


//  Function:  configure_dss
//  This function configures the display sub-system
void configure_dss(unsigned int framebuffer)
{
    // Configure the clock source
    OUTREG32(DSS_DSI2_CONTROL, (1 << 8 | 0 << 9 | 0 << 10 | 0 << 12 | 0 << 17));
    udelay(PANEL_UDELAY);

    // Configure interconnect parameters

    //	OUTREG32(DSS_DISPC_SYSCONFIG, 1<<0 | 1<<3 | 1<<12 );
    OUTREG32(DSS_DISPC_SYSCONFIG, 0x00002015);
    udelay(PANEL_UDELAY);

    // Disable any interrupts
    OUTREG32(DSS_DSI2_IRQENABLE, 0);
    udelay(PANEL_UDELAY);

    // Configure graphic window
    OUTREG32(DSS_DISPC_GFX_BA_0, framebuffer);
    // Configure the position of graphic window
    OUTREG32(DSS_DISPC_GFX_POSITION, BSP_GFX_POS(g_LogoX, g_LogoY));
    // Configure the size of graphic window
    OUTREG32(DSS_DISPC_GFX_SIZE, BSP_GFX_SIZE(g_LogoW,g_LogoH));

    // GW Enabled, RGB24 packed, little Endian
    OUTREG32(DSS_DISPC_GFX_ATTRIBUTES, 1 << 30);
    udelay(PANEL_UDELAY); 
    SETREG32(DSS_DISPC_GFX_ATTRIBUTES, 0x3 << 26);
    udelay(PANEL_UDELAY); 
    SETREG32(DSS_DISPC_GFX_ATTRIBUTES, 1 << 25);
    udelay(PANEL_UDELAY); 
    SETREG32(DSS_DISPC_GFX_ATTRIBUTES, 1 << 14);
    udelay(PANEL_UDELAY); 
    SETREG32(DSS_DISPC_GFX_ATTRIBUTES, 1 << 7);
    udelay(PANEL_UDELAY); 
    SETREG32(DSS_DISPC_GFX_ATTRIBUTES, (1 << 0 | 6 << 1));
    udelay(PANEL_UDELAY); 

    OUTREG32(DSS_DISPC_GFX_FIFO_THRESHOLD, DISPC_GFX_FIFO_THRESHOLD_R(192, 252)); // DISPC_GFX_FIFO_THRESHOLD

    //!!! should be modify??
    OUTREG32(DSS_DISPC_GFX_ROW_INC, 1);
    OUTREG32(DSS_DISPC_GFX_PIXEL_INC, 1);
    OUTREG32(DSS_DISPC_GFX_WINDOW_SKIP, 0);

    // Configure the LCD
    LcdPdd_LCD_Initialize();

}


//  Function:  display_lcd_image
//  This function displays the image in the frame buffer on the LCD
void display_lcd_image(void)
{
    unsigned int  count, ctrl, timeout = 1000;

    // Apply display configuration
    SETREG32(DSS_DISPC_CONTROL2, DISPC_CONTROL_GOLCD);

    // wait for configuration to take effect
    do {
        for (count=0; count<DELAY_COUNT; ++count);
        ctrl = INREG32(DSS_DISPC_CONTROL2);
        timeout--;
    } while((ctrl & DISPC_CONTROL_GOLCD) && (timeout > 0));

    // Power up and start scanning
    LcdPdd_SetPowerLevel();	
}


unsigned int framebuffer = (unsigned int *)0x82000000; //Data shown on display.

//  Function:  lcd_config
//  This function configures the LCD
void lcd_config(void)
{
    reset_display_controller();

    // Enable LCD clocks
    enable_lcd_power();

    // Configure the DSS registers
    configure_dss(framebuffer);

    // Display data on LCD
    display_lcd_image();
}

int show_black_data(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    unsigned int x, y;
    u_int16_t *target_addr = (u_int16_t *)framebuffer;

    //  Fill screen with black data as LG pannel spec.
    g_LogoX = 0;
    g_LogoY = 0;
    g_LogoW = LCD_WIDTH;
    g_LogoH = LCD_HEIGHT;
    for (y= 0; y < LCD_HEIGHT; y++){
        for( x = 0; x < LCD_WIDTH; x++ ){
            *target_addr++ = 0x0000;
        }
    }
    return 0;
}


int show_white_data(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    unsigned int x, y;
    u_int16_t *target_addr = (u_int16_t *)framebuffer;

    //  Fill screen with white data as LG pannel spec.
    g_LogoX = 0;
    g_LogoY = 0;
    g_LogoW = LCD_WIDTH;
    g_LogoH = LCD_HEIGHT;
    for (y= 0; y < LCD_HEIGHT; y++){
        for( x = 0; x < LCD_WIDTH; x++ ){
            *target_addr++ = 0xffff;
        }
    }
    return 0;
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
    __raw_writew(__raw_readw(0x48055134) & 0xFFDF, 0x48055134);     // gpio_OE gpio37
    __raw_writew(__raw_readw(0x4805513C) & ~(0x0020), 0x4805513C);  // gpio_dataout gpio37
    udelay(2000);

    /* Disable GPIO_47 (OMAP_3V_ENABLE, 3.3 V rail) */
    __raw_writel(0x001B0007,0x4a10006C);
    __raw_writew(__raw_readw(0x48055134) & 0x7FFF, 0x48055134);      // gpio_OE gpio47
    __raw_writew(__raw_readw(0x4805513C) & ~( 0x8000), 0x4805513C);  // gpio_dataout gpio47
    udelay(PANEL_UDELAY);

    /* Enable GPIO_47 (OMAP_3V_ENABLE, 3.3 V rail) */
    __raw_writel(0x001B0007,0x4a10006C);
    __raw_writew(__raw_readw(0x48055134) & 0x7FFF, 0x48055134);      // gpio_OE gpio47
    __raw_writew(__raw_readw(0x4805513C) & ~( 0x8000), 0x4805513C);  // gpio_dataout gpio47

    /* Enable GPIO_45 (LCD_PWR_ON) */
    __raw_writew(0x1B,0x4a10006A);
    __raw_writew(__raw_readw(0x48055134) & 0xDFFF, 0x48055134);      // gpio_OE gpio45
    __raw_writew(__raw_readw(0x4805513C) & ~( 0x2000), 0x4805513C);  // gpio_dataout gpio45

    //gpio 119 ADO_SPK_ENABLE
    //gpio_119 gpio_120 pull high
    __raw_writew(__raw_readw(0x48059134) & 0xFF7F, 0x48059134);
    __raw_writew(__raw_readw(0x4805913C) & ~( 0x0080), 0x4805913C);
    __raw_writew(__raw_readw(0x48059134) & 0xFEFF, 0x48059134);
    __raw_writew(__raw_readw(0x4805913C) & ~( 0x0100), 0x4805913C);
}


/**********************************************************
 * Routine: initialize_lcd
 * Description: Initializes the LCD and displays the
 *              splash logo
 **********************************************************/
void initialize_lcd(int show_lowbattery_fb)
{
    char *pwm_on[2]  = {"pwm", "7F"};

    //gpio 119 ADO_SPK_ENABLE
    //__raw_writew(0x1B,0x4a100110);

    //gpio_119 gpio_120 pull high
    __raw_writel(0x001B001B, 0x4a100110);
    __raw_writew(__raw_readw(0x48059134) & 0xFEFF, 0x48059134);
    __raw_writew(__raw_readw(0x4805913C) | 0x0100, 0x4805913C);

    __raw_writew(__raw_readw(0x48059134) & 0xFF7F, 0x48059134);
    __raw_writew(__raw_readw(0x4805913C) | 0x0080, 0x4805913C);

    spi_init();

    /* Enable GPIO_45 (LCD_PWR_ON) */
    __raw_writew(0x1B, 0x4a10006A);
    __raw_writew(__raw_readw(0x48055134) & 0xDFFF, 0x48055134);  // gpio_OE gpio45
    __raw_writew(__raw_readw(0x4805513C) | 0x2000, 0x4805513C);  // gpio_dataout gpio45

    /* Enable GPIO_47 (OMAP_3V_ENABLE, 3.3 V rail) */
    __raw_writel(0x001B0007,0x4a10006C);
    __raw_writew(__raw_readw(0x48055134) & 0x7FFF, 0x48055134);  // gpio_OE gpio47
    __raw_writew(__raw_readw(0x4805513C) | 0x8000, 0x4805513C);  // gpio_dataout gpio47

    udelay(PANEL_UDELAY);
    //kc1 lcd initialize
    spi_command();

    udelay(2000);
    /* Enable GPIO_37 (OMAP_RGB_SHTDOWN, switch to LVDS mode) */
    __raw_writew(0x1B, 0x4a10005A);
    __raw_writew(__raw_readw(0x48055134) & 0xFFDF, 0x48055134);  // gpio_OE gpio37
    __raw_writew(__raw_readw(0x4805513C) | 0x0020, 0x4805513C);  // gpio_dataout gpio37

    show_black_data(NULL, 0, 0, NULL);
    if (show_lowbattery_fb == 1)
        show_lowbattery(NULL, 0, 0, NULL);
    else
        show_splash(NULL, 0, 0, NULL);
    lcd_config();

    /* Turn on backlight */
    set_omap_pwm(NULL, 0, 2, pwm_on);
}

extern char const _binary_lowbattery_rle_start[];
extern char const _binary_lowbattery_rle_end[];

int lbt_datainit(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    return 0;
}

int show_lowbattery(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    u_int16_t *target_addr = (u_int16_t *)framebuffer;
    u_int16_t *start = (u_int16_t *)_binary_lowbattery_rle_start;
    u_int16_t *end = (u_int16_t *)_binary_lowbattery_rle_end;

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

    /* CM_DIV_M5_DPLL_PER Set bit8 = 1, force HSDIVIDER_CLKOUT2 clock enabled*/
    __raw_writew(__raw_readw(0x4A00815C) | 0x100, 0x4A00815C);
    /* CM_SSC_DELTAMSTEP_DPLL_PER */
    __raw_writew(0XCC , 0x4A008168);
    /* CM_SSC_MODFREQDIV_DPLL_PER */
    __raw_writew(0X264 , 0x4A00816C);
    /* CM_CLKMODE_DPLL_PER Set bit12 = 1, force DPLL_SSC_EN enabled*/
    __raw_writew(__raw_readw(0x4A008140) | 0x1000 , 0x4A008140);

    return 0;
}

extern char const _binary_initlogo_rle_start[];
extern char const _binary_initlogo_rle_end[];

int show_splash(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    u_int16_t *target_addr = (u_int16_t *)framebuffer;
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

    return 0;
}

int gpio_37_high(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    //kc1 RGB to LVDS
    //gpio_37
    __raw_writew(0x1B,0x4a10005A);
    __raw_writew(__raw_readw(0x48055134) & 0xFFDF, 0x48055134); // gpio_OE gpio37
    __raw_writew(__raw_readw(0x4805513C) | 0x0020, 0x4805513C); // gpio_dataout gpio37
    return 0;
}

int gpio_37_low(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    //kc1 RGB to LVDS
    //gpio_37
    __raw_writew(0x0B,0x4a10005A);
    __raw_writew(__raw_readw(0x48055134) & 0xFFDF, 0x48055134); // gpio_OE gpio37
    __raw_writew(__raw_readw(0x4805513C) & 0xFFDF, 0x4805513C); // gpio_dataout gpio37
    return 0;
}

int gpio_121_high(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    //Turn on kc1 backlight for A1A
    //gpio_121
    __raw_writew(0x1B,0x4a100114);
    __raw_writel(__raw_readl(0x48059134) & 0xFDFFFFFF, 0x48059134); // gpio_OE gpio_121
    __raw_writel(__raw_readl(0x4805913C) | 0x02000000, 0x4805913C); // gpio_dataout gpio_121
    return 0;
}

int gpio_121_low(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    //Turn off kc1 backlight for A1A
    //gpio_121
    __raw_writew(0x0B,0x4a100114);
    __raw_writel(__raw_readl(0x48059134) & 0xFDFFFFFF, 0x48059134); //gpio_OE gpio_121
    __raw_writel(__raw_readl(0x4805913C) & 0xFDFFFFFF, 0x4805913C); //  gpio_dataout gpio_121
    return 0;
}

int gpio_94_high(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    //Turn on kc1 backlight
    //gpio_94
    __raw_writew(0x1B,0x4a1000D6);
    __raw_writel(__raw_readl(0x48057134) & 0xBFFFFFFF, 0x48057134); // gpio_OE gpio_94
    __raw_writel(__raw_readl(0x4805713C) | 0x40000000, 0x4805713C); // gpio_dataout gpio_94

    //gpio_119
    __raw_writel(0x001B001B,0x4a100110);//gpio_119 and gpio_120
    __raw_writel(__raw_readl(0x48059134) & 0xFF7FFFFF, 0x48059134); // gpio_OE gpio_119
    __raw_writel(__raw_readl(0x4805913C) | 0x00800000, 0x4805913C); // gpio_dataout gpio_119

    //gpio_120
    __raw_writel(__raw_readl(0x48059134) & 0xFEFFFFFF, 0x48059134); // gpio_OE gpio_120
    __raw_writel(__raw_readl(0x4805913C) | 0x01000000, 0x4805913C); // gpio_dataout gpio_120

    return 0;
}

int gpio_94_low(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    //Turn off kc1 backlight
    //gpio_94
    __raw_writew(0x0B,0x4a1000D6);
    __raw_writel(__raw_readl(0x48057134) & 0xBFFFFFFF, 0x48057134); // gpio_OE gpio_94
    __raw_writel(__raw_readl(0x4805713C) & 0xBFFFFFFF, 0x4805713C); // gpio_dataout gpio_94

    //gpio_119
    __raw_writel(0x000B000B,0x4a100110);//gpio_119 and gpio_120
    __raw_writel(__raw_readl(0x48059134) & 0xFF7FFFFF, 0x48059134); // gpio_OE gpio_119
    __raw_writel(__raw_readl(0x4805913C) & 0xFF7FFFFF, 0x4805913C); // gpio_dataout gpio_119

    //gpio_120
    __raw_writel(__raw_readl(0x48059134) & 0xFEFFFFFF, 0x48059134); // gpio_OE gpio_120
    __raw_writel(__raw_readl(0x4805913C) & 0xFEFFFFFF, 0x4805913C); // gpio_dataout gpio_120
    return 0;
}

int set_omap_pwm(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    long brightness;

    if (argc < 2) {
        printf("Usage:\n%s\n", cmdtp->usage);
        return 0;
    }

    brightness = simple_strtol(argv[1], NULL, 16);
    if (brightness < 0 || brightness > 255) {
        printf("Usage:\nsetbacklight 0x0 - 0xFF\n");
        return 0;
    }
    //printf("brightness = %d\n", brightness);

    //printf("GPT10 PWM Set \n");

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

    return 0;
}

U_BOOT_CMD( gpio_121_high, 1, 0, gpio_121_high, "gpio_121_high - gpio_121_high.\n", NULL );
U_BOOT_CMD( gpio_121_low, 1, 0, gpio_121_low, "gpio_121_low - gpio_121_low.\n", NULL );
U_BOOT_CMD( gpio_37_high, 1, 0, gpio_37_high, "gpio_37_high - gpio_37_high.\n", NULL );
U_BOOT_CMD( gpio_37_low, 1, 0, gpio_37_low, "gpio_37_low - gpio_37_low.\n", NULL );
U_BOOT_CMD( gpio_94_high, 1, 0, gpio_94_high, "gpio_94_high - gpio_94_high.\n", NULL );
U_BOOT_CMD( gpio_94_low, 1, 0, gpio_94_low, "gpio_94_low - gpio_94_low.\n", NULL );
U_BOOT_CMD( setbacklight, 2, 0, set_omap_pwm, "setbacklight - set omap pwm.\n", "setbacklight 0x0 - 0xFF\n" );
U_BOOT_CMD( show_white_data, 1, 0, show_white_data, "show_white_data - show white data.\n", NULL );
U_BOOT_CMD( show_black_data, 1, 0, show_black_data, "show_black_data - show black data.\n", NULL );
U_BOOT_CMD( showlowbattery, 1, 0, show_lowbattery, "showlowbattery - show lowbattery image.\n", NULL );
U_BOOT_CMD( lbtinit, 1, 0, lbt_datainit, "lbtinit - prepare lowbattery image.\n", NULL );
