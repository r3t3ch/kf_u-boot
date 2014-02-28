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
#include <i2c.h>

/* I2C chip addresses */
#define BQ27541_I2C_ADDRESS	0x55

/* Battery REGISTERS */
#define BQ27541_BAT_TEMP	0x06
#define BQ27541_BAT_VOLTAGE	0x08
#define BQ27541_BAT_CURRENT	0x14
#define BQ27541_BAT_CAPACITY	0x2c

/* Function*/
int get_bat_voltage(void);
int get_bat_temperature(void);
int get_bat_current(void);
int get_bat_capacity(void);
