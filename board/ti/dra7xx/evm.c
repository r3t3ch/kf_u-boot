/*
 * (C) Copyright 2013
 * Texas Instruments Incorporated, <www.ti.com>
 *
 * Lokesh Vutla <lokeshvutla@ti.com>
 *
 * Based on previous work by:
 * Aneesh V       <aneesh@ti.com>
 * Steve Sakoman  <steve@sakoman.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <common.h>
#include <palmas.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/mmc_host_def.h>

#include "mux_data.h"
#include "crossbar_data.h"

#ifdef CONFIG_USB_EHCI
#include <usb.h>
#include <asm/arch/ehci.h>
#include <asm/ehci-omap.h>
#endif

DECLARE_GLOBAL_DATA_PTR;

const struct omap_sysinfo sysinfo = {
	"Board: DRA7xx\n"
};

/**
 * @brief board_init
 *
 * @return 0
 */
int board_init(void)
{
	gpmc_init();
	gd->bd->bi_boot_params = (0x80000000 + 0x100); /* boot param addr */

	return 0;
}

int board_eth_init(bd_t *bis)
{
	return 0;
}

/**
 * @brief misc_init_r - Configure EVM board specific configurations
 * such as power configurations, ethernet initialization as phase2 of
 * boot sequence
 *
 * @return 0
 */
int misc_init_r(void)
{
	return 0;
}

static void do_set_mux32(u32 base,
			 struct pad_conf_entry const *array, int size)
{
	int i;
	struct pad_conf_entry *pad = (struct pad_conf_entry *)array;

	for (i = 0; i < size; i++, pad++)
		writel(pad->val, base + pad->offset);
}

void set_muxconf_regs_essential(void)
{
	do_set_mux32((*ctrl)->control_padconf_core_base,
		     core_padconf_array_essential,
		     sizeof(core_padconf_array_essential) /
		     sizeof(struct pad_conf_entry));
}

#if !defined(CONFIG_SPL_BUILD) && defined(CONFIG_GENERIC_MMC)
int board_mmc_init(bd_t *bis)
{
	omap_mmc_init(0, 0, 0, -1, -1);
	omap_mmc_init(1, 0, 0, -1, -1);
	return 0;
}
#endif

/*
 * MPU IRQs that are mapped to CROSSBAR are:
 * MPU_IRQ_4,
 * MPU_IRQ_7 to MPU_IRQ_130,
 * MPU_IRQ_133 to MPU_IRQ_159
 * Acccordingly the offset is calculated.
 */
static u16 get_mpu_offset(u16 irq_no)
{
	u16 offset;
	if (irq_no == 4)
		offset = 0x00;
	else if (irq_no == 7)
		offset = 0x02;
	else if (irq_no <= 130)
		offset = (irq_no - 6) * 2;
	else if ((irq_no <= 159) && (irq_no >= 133))
		offset = (irq_no - 8) * 2;
	else
		offset = -1;

	return offset;
}

/*
 * DREQ lines associated with DMA_CROSSBAR are:
 * DMA_SYSTEM	: 0-126
 */
static inline u16 get_dma_offset(u16 irq_no)
{
	return irq_no * 2;
}

static void do_set_crossbar(u32 base, struct crossbar_entry const *array,
			  int size, int module)
{
	u16 i, offset;
	struct crossbar_entry *irq = (struct crossbar_entry *)array;

	for (i = 0; i < size; i++, irq++) {
		switch (module) {
		case CROSSBAR_MPU_IRQ:
			offset = get_mpu_offset(irq->module_irq);
			break;
		case CROSSBAR_SDMA:
			offset = get_dma_offset(irq->module_irq);
			break;
		default:
			offset = -1;
		}

		if (offset == -1)
			printf("Mapping of %d line cannot be done\n",
			       irq->module_irq);
		else
			writew(irq->crossbar_irq, base + offset);
	}
}

void set_crossbar_regs(void)
{
	do_set_crossbar((*ctrl)->control_core_mpu_irq_base, mpu_irq_map,
			sizeof(mpu_irq_map)/sizeof(struct crossbar_entry),
			CROSSBAR_MPU_IRQ);
	do_set_crossbar((*ctrl)->control_core_sdma_dreq_base,
			sdma_dreq_map,
			sizeof(sdma_dreq_map)/sizeof(struct crossbar_entry),
			CROSSBAR_SDMA);
}
