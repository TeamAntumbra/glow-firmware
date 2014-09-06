mcu = atmega16u4
fcpu = 16000000UL

CC = avr-gcc
CFLAGS := \
	-Wall -std=c99 -Os -g -mmcu=$(mcu) -DF_CPU=$(fcpu) \
	\
	-ffunction-sections \
	-fdata-sections \
	\
	-funsigned-char \
	-funsigned-bitfields \
	-fpack-struct \
	-fshort-enums \
	\
	-DANTUMBRA_COMMIT_ID='"$(shell git rev-parse HEAD)"' \
	-DANTUMBRA_COMMIT_ID_HEX=$(shell git rev-parse HEAD | sed 's/../0x&,/g')

LD = avr-gcc
LDFLAGS = -mmcu=$(mcu) -Wl,--gc-sections
LDLIBS =

AR = avr-ar

AS = avr-gcc
ASFLAGS = -mmcu=$(mcu)

# Don't pass down parent options to LUFA or make -B will give a spurious error.
MAKEREC = env -u MFLAGS -u MAKEFLAGS -u MAKELEVEL $(MAKE) -s

.PHONY: all
all: main.bin loader.hex default.eep

%.elf:
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%.hex: %.elf
	avr-objcopy -O ihex $< $@

%.bin: %.elf
	avr-objcopy -O binary $< $@

%.eep: %.eep.txt
	cut -f 1 -d '#' $< | xxd -r -p > $@

.PHONY: clean
clean:
	-rm *.o *.a *.elf *.hex *.bin *.eep
	$(MAKEREC) -C rawusb clean
	-rm rawusb/librawusb.a

.PHONY: fuse-dragonisp flash-dragonisp
fuse-dragonisp:
	avrdude -C +avrdude-m16u4.conf -c dragon_isp -P usb -p atmega16u4 -B 10 \
		-U lfuse:w:0xFF:m -U hfuse:w:0x9E:m -U efuse:w:0xC8:m
flash-dragonisp: loader.hex default.eep
	avrdude -C +avrdude-m16u4.conf -c dragon_isp -P usb -p atmega16u4 -B 0.5 \
		-U flash:w:loader.hex:i -U eeprom:w:default.eep:r

%.a:
	$(AR) rcs $@ $^

.PHONY: .FORCE
rawusb.a: .FORCE
	$(MAKEREC) -C rawusb lib
	cp rawusb/librawusb.a $@

main.elf: LDLIBS = -lm
main.elf: LDFLAGS += -Wl,--relax
main.elf: main.o led.o proto.o rawusb.a

LOADER_OFFSET = 0x2500

# --relax interferes with jump tables
loader.elf: LDFLAGS += -Wl,--no-relax
loader.elf: LDFLAGS += -Wl,--section-start=.text=$(LOADER_OFFSET)
loader.elf: LDFLAGS += -Wl,--section-start=.fake_vectors=0x3e00,--undefined=_fake_vectors
loader.elf: LDFLAGS += -Wl,--section-start=.flash_parts=0x3f00,--undefined=flash_write
loader.elf: CFLAGS += -DLOADER_OFFSET=$(LOADER_OFFSET)
loader.elf: loader.o fake-vectors.o led.o flash.o proto.o option.o rawusb.a \
	api.o api-core.o api-bootcontrol.o
