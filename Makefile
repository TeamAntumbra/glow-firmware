mcu = atmega16u4
fcpu = 16000000UL

CC = avr-gcc
CFLAGS = \
	-Wall -std=c99 -Os -g -mmcu=$(mcu) -DF_CPU=$(fcpu) \
	\
	-ffunction-sections \
	-fdata-sections \
	\
	-funsigned-char \
	-funsigned-bitfields \
	-fpack-struct \
	-fshort-enums

LD = avr-gcc
LDFLAGS = -mmcu=$(mcu) -Wl,--gc-sections
LDLIBS =

AR = avr-ar

AS = avr-gcc
ASFLAGS = -mmcu=$(mcu)

# Don't pass down parent options to LUFA or make -B will give a spurious error.
MAKEREC = env -u MFLAGS -u MAKEFLAGS -u MAKELEVEL $(MAKE) -s

.PHONY: all
all: main.bin loader.hex

%.elf:
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%.hex: %.elf
	avr-objcopy -O ihex $< $@

%.bin: %.elf
	avr-objcopy -O binary $< $@

.PHONY: clean
clean:
	-rm *.o *.a *.elf *.hex *.bin
	$(MAKEREC) -C rawusb clean
	-rm rawusb/librawusb.a

%.a:
	$(AR) rcs $@ $^

.PHONY: .FORCE
rawusb.a: .FORCE
	$(MAKEREC) -C rawusb lib
	cp rawusb/librawusb.a $@

main.elf: LDLIBS = -lm
main.elf: LDFLAGS += -Wl,--relax
main.elf: main.o led.o proto.o rawusb.a

# --relax interferes with jump tables
loader.elf: LDFLAGS += -Wl,--no-relax
loader.elf: LDFLAGS += -Wl,--section-start=.text=0x2d00
loader.elf: LDFLAGS += -Wl,--section-start=.fake_vectors=0x3e00,--undefined=_fake_vectors
loader.elf: LDFLAGS += -Wl,--section-start=.flash_parts=0x3f00,--undefined=flash_write
loader.elf: loader.o fake-vectors.o led.o flash.o proto.o rawusb.a
