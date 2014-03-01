/*
 * (C) Copyright 2009
 * Texas Instruments, <www.ti.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/io.h>

#include <kc1_board.h>

#include "fg_bq27541.h"

static inline int bq27_i2c_read_u16(u8 reg, u16 *val)
{
	u16 data;

	if (get_mbid() >= 4)
		i2c_set_bus_num(0); // 100
	if (i2c_read(BQ27541_I2C_ADDRESS, reg, 1, (u8 *)&data, 2) == 0) {
		*val=data;
		return 0;
	}
	else
		return -1;
}

static inline int bq27_i2c_read_int(u8 reg, short *val)
{
	short data;

	if(get_mbid() >= 4)
		i2c_set_bus_num(0); // 100
	if(i2c_read(BQ27541_I2C_ADDRESS, reg, 1, (u8 *)&data, 2) == 0){
		*val=data;
		return 0;
	}
	else
		return -1;
}

int get_bat_voltage(void)
{
	u16 bat = 0;
	if (bq27_i2c_read_u16(BQ27541_BAT_VOLTAGE, &bat) == 0) {
		debug("*** %s::Battery voltage=%d mV\n", __func__, bat);
		return bat;
	}
	else
		return -1;
}

int get_bat_temperature(void)
{
	u16 temp = 0;
	short celsius = 0;
	if (bq27_i2c_read_u16(BQ27541_BAT_TEMP, &temp) == 0) {
		//Kelvin to Celsius = K -273.15
		celsius = temp - 2731;
		debug("*** %s::Battery temp=%dc\n", __func__, celsius/10);
		return celsius/10;
	}
	else
		return -1;
}

int get_bat_current(void)
{
	short current = 0;
	bq27_i2c_read_int(BQ27541_BAT_CURRENT, &current);
	debug("*** %s::Battery current=%d\n", __func__, current);
	return current;
}

int get_bat_capacity(void)
{
	short capacity = 0;
	bq27_i2c_read_int(BQ27541_BAT_CAPACITY, &capacity);
	debug("*** %s::Battery capacity=%d\n", __func__, capacity);
	return capacity;
}

