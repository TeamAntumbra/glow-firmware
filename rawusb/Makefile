# Copyright (c) 2015 Antumbra
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

MCU = atmega16u4
ARCH = AVR8
BOARD = USER
F_CPU = 16000000
F_USB = $(F_CPU)
OPTIMIZATION = s
TARGET = rawusb
SRC = rawusb.c $(LUFA_SRC_USB)
LUFA_PATH = ../lufa/LUFA
CC_FLAGS = -DUSE_LUFA_CONFIG_HEADER -Icfg -Wall
LD_FLAGS =

all:

include $(LUFA_PATH)/Build/lufa_core.mk
include $(LUFA_PATH)/Build/lufa_sources.mk
include $(LUFA_PATH)/Build/lufa_build.mk
