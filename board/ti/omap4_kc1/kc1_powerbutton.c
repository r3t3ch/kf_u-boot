#include <common.h>
#include <twl6030.h>

#include "kc1_twl6030.h"

/* don't check more than 10th of a second, risk burning out */
// 1200 timer ticks == 1 sec
#define PWRBTN_DEBOUNCE_TIMER_RATE	(150)
#define PWRBTN_LONG_PRESS_COUNT		(5)
#define PWRBTN_LONG_PRESS_DEBOUNCE	(1000)

static struct pwrb {
//	struct input_config input;	/* The input layer */
	unsigned int last_poll_ms;	/* Time we should last polled */

	unsigned int debounce_ms;
	unsigned int pressed;
	unsigned int released;
} config;


int pwrbutton_getc(void)
{
	int ret = 0;
	int button_pressed = 0;
	unsigned int poll = get_timer(config.last_poll_ms);

	// don't poll too often
	if ((poll < PWRBTN_DEBOUNCE_TIMER_RATE) || (poll < config.debounce_ms))
		return 0;

	config.debounce_ms = 0;
	config.last_poll_ms = get_timer(0);
	button_pressed = twl6030_get_power_button_status();
	if (button_pressed == 0) {
		if (config.pressed == 0)
			debug("===> POWER PRESS\n");
		config.pressed += 1;
	}
	// button not pressed
	else {
		if (config.pressed > 0) {
			debug("===> RELEASED\n");
			config.pressed = 0;
			config.released = 1;
		}
	}

	if ((config.pressed >= PWRBTN_LONG_PRESS_COUNT) || (config.released > 0)) {
		if (config.pressed >= PWRBTN_LONG_PRESS_COUNT) {
			ret = PWRBTN_KEY_ENTER; // long press
			config.debounce_ms = PWRBTN_LONG_PRESS_DEBOUNCE;
			debug("*** %s:: SEND PWRBTN_KEY_ENTER\n", __func__);
		}
		else {
			ret = PWRBTN_KEY_DOWN; // single press
			debug("*** %s:: SEND PWRBTN_KEY_DOWN\n", __func__);
		}

		// reset timing/press stats
		config.pressed = 0;
		config.released = 0;
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
	debug("*** %s::driver register\n", __func__);
	config.last_poll_ms = get_timer(0);
	config.pressed = 0;
	config.released = 0;
	config.debounce_ms = 0;
	return 0;
}

