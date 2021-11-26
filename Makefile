CFLAGS ?= -Os -DF_APU=16000000UL -mmcu=atmega2560
LDFLAGS ?= -mmcu=atmega2560 -Wl,-section-start=.mymem=0x4000

ARDUINO_USB ?= /dev/ttyUSB0

all: hello_world.hex

hello_world.hex: hello_world
	avr-objcopy -O ihex -R .eeprom hello_world hello_world.hex

hello_world: hello_world.o
	avr-gcc $(LDFLAGS) -o $@ $^

hello_world.o: hello_world.c
	avr-gcc $(CFLAGS) -c -o $@ $<

deploy: hello_world.hex
	avrdude -v -patmega2560 -cwiring -P ${ARDUINO_USB} -b115200 -D -U flash:w:hello_world.hex:i

clean: FRC
	rm -f hello_world.elf hello_world.hex hello_world.o hello_world

FRC: