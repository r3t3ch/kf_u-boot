#include <common.h>
#include <twl6030.h>

#include "kc1_twl6030.h"

//#define DEBUG_POWERBUTTON	1

/* don't check more than 10th of a second, risk burning out */
// 1200 timer ticks == 1 sec
#define PWRBTN_DEBOUNCE_TIMER_RATE	(150)
#define PWRBTN_LONG_PRESS_COUNT		(5)

static struct pwrb {
	unsigned int last_poll_ms;	/* Time we should last polled */

	unsigned int sent_event;
	unsigned int pressed;
	unsigned int released;
} config;


int pwrbutton_getc(void)
{
	int ret = 0;
	int button_pressed = 0;
	unsigned int poll = get_timer(config.last_poll_ms);

	// don't poll too often
	if (poll < PWRBTN_DEBOUNCE_TIMER_RATE)
		return 0;

	config.last_poll_ms = get_timer(0);
	button_pressed = twl6030_get_power_button_status();
	if (button_pressed == 0) {
#ifdef DEBUG_POWERBUTTON
		if (config.pressed == 0)
			printf("===> POWER PRESS\n");
#endif
		config.pressed += 1;
	}
	// button not pressed
	else {
		if (config.pressed > 0) {
#ifdef DEBUG_POWERBUTTON
			printf("===> RELEASED\n");
#endif
			config.released = 1;
		}
	}

	if ((config.pressed >= PWRBTN_LONG_PRESS_COUNT) || (config.released > 0)) {
		if ((config.pressed >= PWRBTN_LONG_PRESS_COUNT) && (config.sent_event == 0)) {
			ret = PWRBTN_KEY_LONG_PRESS; // long press
			config.sent_event = 1; // keep track of sent_event
#ifdef DEBUG_POWERBUTTON
			printf("*** %s:: SEND LONG_PRESS\n", __func__);
#endif
		}
		else if (config.released > 0) { // config.released
			if (config.sent_event == 0) {
				ret = PWRBTN_KEY_PRESS; // single press
#ifdef DEBUG_POWERBUTTON
				printf("*** %s:: SEND PRESS\n", __func__);
#endif
			}
			// reset timing/press stats
			config.sent_event = 0;
			config.pressed = 0;
			config.released = 0;
		}
	}

	return ret;
}


/**
 * Setup power button input driver
 *
 * NOTE: this expects TWL6030 driver to already be setup and running
 */
int drv_twl6030_pwrbutton_init(void)
{
	config.last_poll_ms = get_timer(0);
	config.pressed = 0;
	config.released = 0;
	config.sent_event = 0;
	return 0;
}

