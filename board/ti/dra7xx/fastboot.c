#include <common.h>
#include <asm/io.h>
#include <asm-generic/gpio.h>
#include <config.h>

#define FASTBOOT_BUTTON_GPIO 171

int check_fastboot(void)
{
	/* Check if we are coming from a warm reset */
	if (__raw_readl(DRA7XX_PRM_RSTST) & DRA7XX_PRM_RSTST_RESET_WARM_BIT)
		if (!strncmp((const char *)DRA7XX_PUBLIC_SAR_RAM_1_FREE,
			"bootloader", DRA7XX_REBOOT_REASON_SIZE)) {
			strncpy((char *)DRA7XX_PUBLIC_SAR_RAM_1_FREE, "",
				DRA7XX_REBOOT_REASON_SIZE);
			return 1;
		}
	return 0;
}

void fastboot_reboot_bootloader(void) {
	/* clear all reset events */
	__raw_writel(DRA7XX_PRM_RSTST_CLR, PRM_RSTST);
	strncpy((char *)DRA7XX_PUBLIC_SAR_RAM_1_FREE, "bootloader",
			DRA7XX_REBOOT_REASON_SIZE - 1);
	*(((char*)DRA7XX_PUBLIC_SAR_RAM_1_FREE) +
					DRA7XX_REBOOT_REASON_SIZE - 1) = '\0';
	/* trigger warm reset */
	__raw_writel(DRA7XX_PRM_RSTCTRL_RESET_WARM_BIT, DRA7XX_PRM_RSTCTRL);
}

