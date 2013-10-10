/*
 * Board specific setup info
 *
 * (C) Copyright 2010
 * Texas Instruments, <www.ti.com>
 *
 * Author :
 *	Aneesh V	<aneesh@ti.com>
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

#include <config.h>
#include <asm/arch/omap.h>
#include <asm/omap_common.h>
#include <asm/arch/spl.h>
#include <linux/linkage.h>

ENTRY(save_boot_params)
	ldr	r1, =OMAP_SRAM_SCRATCH_BOOT_PARAMS
	str	r0, [r1]
	bx	lr
ENDPROC(save_boot_params)

ENTRY(set_pl310_ctrl_reg)
	PUSH	{r4-r11, lr}	@ save registers - ROM code may pollute
				@ our registers
	LDR	r12, =0x102	@ Set PL310 control register - value in R0
	.word	0xe1600070	@ SMC #0 - hand assembled because -march=armv5
				@ call ROM Code API to set control register
	POP	{r4-r11, pc}
ENDPROC(set_pl310_ctrl_reg)

ENTRY(get_l2_aux_ctrl_reg)
	MRC     p15, 1, r0, c15, c0, 0
ENDPROC(get_l2_aux_ctrl_reg)

ENTRY(set_l2_aux_ctrl_reg)
	PUSH	{r4-r11, lr}	@ save registers - ROM code may pollute
				@ our registers
	LDR	r12, =0x104	@ Set L2 Cache Auxiliary control register - value in R0
	.word	0xe1600070	@ SMC #0 - hand assembled because -march=armv5
				@ call ROM Code API to set control register
	POP	{r4-r11, pc}
ENDPROC(set_l2_aux_ctrl_reg)

/**
 * u32 hal_pub_to_sec_dispatcher(u32 id, u32 falg, u32 params)
 * OMAP low level routine to call a secure service/HAL
 * @id: Application ID of HAL API
 * @flag: Flag to indicate the criticality of operation
 * @params: Physical address of parameter list starting
 *          with number of parametrs
 */

ENTRY(hal_pub_to_sec_dispatcher)
	PUSH	{r4-r11, lr}	@ save registers - ROM code may pollute
	mov r6, #0xff		@ set new task
	mov r12, #0		@ Secure Service ID
	dsb
	isb
	dmb
	.word   0xe1600070	@ SMC #0 - hand assembled
	POP     {r4-r11, pc}
ENDPROC(hal_pub_to_sec_dispatcher)
