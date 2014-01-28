#
# (C) Copyright 2000, 2001, 2002
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

include $(TOPDIR)/config.mk

LIB	= $(obj)lib$(BOARD).o

COBJS	:= kc1.o
COBJS	+= kc1_board.o

# Drivers
COBJS	+= kc1_leds.o
COBJS	+= kc1_panel.o
COBJS	+= kc1_twl6030.o
COBJS	+= fg_bq27541.o
COBJS	+= pmic_smb347.o
COBJS	+= omap4_spi.o

ifdef CONFIG_CMD_FASTBOOT
COBJS	+= mmc.o
COBJS	+= device_tree.o
COBJS	+= fastboot.o
endif

SRCS	:= $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS)) splash.o lowbattery.o

$(LIB):	$(obj).depend $(OBJS)
	$(call cmd_link_o_target, $(OBJS))

splash.o:
	$(OBJCOPY) -I binary -O elf32-littlearm -B arm initlogo.rle $@

lowbattery.o:
	$(OBJCOPY) -I binary -O elf32-littlearm -B arm lowbattery.rle $@

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
