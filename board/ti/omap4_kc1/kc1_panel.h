#ifndef __KC1_PANEL_H
#define __KC1_PANEL_H

#include <video_fb.h>
#include <asm/arch/dss.h>
#ifdef CONFIG_LCD
#include <lcd.h>
#endif

#define LCD_WIDTH		1024
#define LCD_HEIGHT		600
#define PANEL_DELAY_COUNT	100
#define PANEL_TIMEOUT		1000

void set_logoindex(u8 index);
void turn_off_lcd(void);
void initialize_lcd(void);
void show_image(u_int16_t *start, u_int16_t *end);
void set_omap_pwm(u8 brightness);
int  board_video_init(void);

#endif /* KC1_PANEL_H */
