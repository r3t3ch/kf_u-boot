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

obj-y	:= kc1.o
obj-y	+= kc1_board.o

# Drivers
obj-y	+= kc1_leds.o
obj-y	+= kc1_backlight.o
obj-y	+= kc1_panel.o
obj-y	+= kc1_twl6030.o
obj-y	+= kc1_powerbutton.o
obj-y	+= kc1_debug.o
obj-y	+= fg_bq27541.o
obj-y	+= pmic_smb347.o
obj-y	+= omap4_spi.o
obj-y	+= mmc.o

obj-$(CONFIG_CMD_FASTBOOT) += omap_fastboot.o
obj-$(CONFIG_CMD_FASTBOOT) += cmd_fastboot.o
obj-$(CONFIG_CMD_FASTBOOT) += kc1_fastboot.o
obj-$(CONFIG_CMD_FASTBOOT) += sparse.o

obj-$(CONFIG_CMD_FASTBOOT) += cmd_lcdmenu.o

ifdef CONFIG_LCD
splash.o:
	$(OBJCOPY) -I binary -O elf32-littlearm -B arm initlogo.bmp.gz $@

lowbattery.o:
	$(OBJCOPY) -I binary -O elf32-littlearm -B arm lowbattery.bmp.gz $@

multi_download.o:
	$(OBJCOPY) -I binary -O elf32-littlearm -B arm multi_download.bmp.gz $@

else
splash.o:
	$(OBJCOPY) -I binary -O elf32-littlearm -B arm initlogo.rle $@

lowbattery.o:
	$(OBJCOPY) -I binary -O elf32-littlearm -B arm lowbattery.rle $@

multi_download.o:
	$(OBJCOPY) -I binary -O elf32-littlearm -B arm multi_download.rle $@

endif

obj-y	+= splash.o lowbattery.o multi_download.o

