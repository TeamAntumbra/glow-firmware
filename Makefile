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

AR = avr-ar

AS = avr-gcc
ASFLAGS = -mmcu=$(mcu)

# Don't pass down parent options to LUFA or make -B will give a spurious error.
MAKEREC = env -u MFLAGS -u MAKEFLAGS -u MAKELEVEL $(MAKE) -s

.PHONY: all
all: main.hex main.bin

%.elf:
	$(LD) $(LDFLAGS) -o $@ $^ $(libs)

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

main.elf: libs = -lm
main.elf: LDFLAGS += -Wl,--relax
main.elf: led.o main.o rawusb.a
