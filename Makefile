mcu = atmega16u4
fcpu = 16000000UL

CC = avr-gcc
CFLAGS := \
	-Wall -Werror -std=c99 -Os -g -mmcu=$(mcu) -DF_CPU=$(fcpu) \
	\
	-ffunction-sections \
	-fdata-sections \
	\
	-funsigned-char \
	-funsigned-bitfields \
	-fpack-struct \
	-fshort-enums \
	\
	-DANTUMBRA_COMMIT_ID='"$(shell git rev-parse HEAD | head -c 16)"' \
	-DANTUMBRA_COMMIT_ID_HEX=$(shell git rev-parse HEAD | head -c 16 | sed 's/../0x&,/g')

LD = avr-gcc
LDFLAGS = -mmcu=$(mcu) -Wl,--gc-sections
LDLIBS =

AR = avr-ar

AS = avr-gcc
ASFLAGS = -mmcu=$(mcu)

# Don't pass down parent options to LUFA or make -B will give a spurious error.
MAKEREC = env -u MFLAGS -u MAKEFLAGS -u MAKELEVEL $(MAKE) -s

.PHONY: all
all: main.bin loader.hex default.eep combined.hex combined.eep

%.elf:
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)
	chmod -x $@

%.hex: %.elf
	avr-objcopy -O ihex $< $@

%.bin: %.elf
	avr-objcopy -O binary $< $@
	chmod -x $@

%.eep: %.eep.txt
	cut -f 1 -d '#' $< | xxd -r -p > $@

.PHONY: clean
clean:
	-rm *.o *.a *.elf *.hex *.bin *.eep combined.eep.txt
	$(MAKEREC) -C rawusb clean
	-rm rawusb/librawusb.a

.PHONY: fuse-dragonisp flash-loader-dragonisp flash-combined-dragonisp
fuse-dragonisp:
	avrdude -C +avrdude-m16u4.conf -c dragon_isp -P usb -p atmega16u4 -B 10 \
		-U lfuse:w:0xFF:m -U hfuse:w:0x9E:m -U efuse:w:0xC8:m
flash-loader-dragonisp: loader.hex default.eep
	avrdude -C +avrdude-m16u4.conf -c dragon_isp -P usb -p atmega16u4 -B 0.5 \
		-U flash:w:loader.hex:i -U eeprom:w:default.eep:r
flash-combined-dragonisp: combined.hex combined.eep
	avrdude -C +avrdude-m16u4.conf -c dragon_isp -P usb -p atmega16u4 -B 0.5 \
		-U flash:w:combined.hex:i -U eeprom:w:combined.eep:r

%.a:
	$(AR) rcs $@ $^

.PHONY: .FORCE
rawusb.a: .FORCE
	$(MAKEREC) -C rawusb lib
	cp rawusb/librawusb.a $@

main.elf: LDLIBS = -lm
main.elf: LDFLAGS += -Wl,--relax
main.elf: main.o led.o proto.o rawusb.a option.o \
	api.o api-core.o api-bootcontrol.o api-eeprom.o api-light.o api-temp.o

LOADER_OFFSET = 0x2000

# --relax interferes with jump tables
loader.elf: LDFLAGS += -Wl,--no-relax
loader.elf: LDFLAGS += -Wl,--section-start=.text=$(LOADER_OFFSET)
loader.elf: LDFLAGS += -Wl,--section-start=.fake_vectors=0x3e00,--undefined=_fake_vectors
loader.elf: LDFLAGS += -Wl,--section-start=.flash_parts=0x3f00,--undefined=flash_write
loader.elf: CFLAGS += -DLOADER_OFFSET=$(LOADER_OFFSET)
loader.elf: loader.o fake-vectors.o led.o flash.o proto.o option.o rawusb.a \
	api.o api-core.o api-bootcontrol.o api-eeprom.o api-flash.o

combined.eep.txt: default.eep.txt
	sed 's/^\s*424f4f54\s\s*01\s\s*01\s*$$/424f4f54 01 00/' $^ > $@

combined.hex: main.hex loader.hex
	(grep -hv '^:00000001FF' $^; printf ':00000001FF\r\n') > $@
