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
