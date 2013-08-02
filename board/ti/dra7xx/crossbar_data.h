/*
 * (C) Copyright 2013
 * Texas Instruments Incorporated
 *
 * Lokesh Vutla <lokeshvutla@ti.com>
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
#ifndef _CROSSBAR_DATA_DRA7XX_H_
#define _CROSSBAR_DATA_DRA7XX_H_

#include <asm/arch/sys_proto.h>

const struct crossbar_entry mpu_irq_map[] = {
	/* MPU_IRQ, CROSSBAR_IRQ */
	{159, 217},	/* RTC_IRQ */
	{155, 150},	/* MCASP3_IRQ_AREVT */
	{154, 151},	/* MCASP3_IRQ_AXEVT */
	{156, 156},	/* MCASP6_IRQ_AREVT */
	{157, 157},	/* MCASP6_IRQ_AXEVT */
	{136, 251},	/* MAILBOX5 */
	{141, 255},	/* MAILBOX6 */
	{142, 396},	/* IPU1 MMU */
	{143, 145},	/* DSP1 MMU1 */
	{144, 146},	/* DSP2 MMU0 */
	{145, 147},	/* DSP2 MMU1 */
};

const struct crossbar_entry sdma_dreq_map[] = {
	/* SDMA_DREQ, CROSSBAR_DMA */
	{79, 132},	/* MCASP3_DREQ_RX */
	{78, 133},	/* MCASP3_DREQ_TX */
	{63, 138},	/* MCASP6_DREQ_RX */
	{62, 139},	/* MCASP6_DREQ_TX */
};

#endif /* _CROSSBAR_DATA_DRA7XX_H_ */
