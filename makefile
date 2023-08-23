
MCU             = atmega328p
# Only used by avrdude
PROGRAMMER      = arduino
DEVICE		= /dev/ttyUSB0

CPUFREQ         = 16000000 #16MHz

# https://www.engbedded.com/fusecalc/
# Int. RC Osc. 8MHz
# No WD
# No BOD
LFUSE           = 0xe0
HFUSE           = 0xd9
EFUSE		= 0xff

CC              = avr-gcc
CFLAGS          = -std=c11 -Wall -mmcu=$(MCU) -DF_CPU=$(CPUFREQ)
LDFLAGS         = -mmcu=$(MCU)
BUILDDIR        = build
SOURCEDIR       = src
OBJECTDIR       = obj

OUTPUT          = analog

SRCS = $(wildcard $(SOURCEDIR)/*.c)
OBJS = $(SRCS:.c=.o)
OBJ  = $(OBJS:$(SOURCEDIR)/%=$(OBJECTDIR)/%)

build: dir $(OBJ)
	@echo LD $(OBJ)
	@$(CC) $(CFLAGS) -o $(BUILDDIR)/$(OUTPUT) $(OBJ) $(LDFLAGS)
	@avr-objcopy -O ihex $(BUILDDIR)/$(OUTPUT) $(BUILDDIR)/$(OUTPUT).hex
	@echo
	@avr-size --mcu=$(MCU) --format=avr $(BUILDDIR)/$(OUTPUT)

debug: -D _DEBUG
debug: build;

dir:
	@mkdir -p $(OBJECTDIR)
	@mkdir -p $(BUILDDIR)

$(OBJECTDIR)/%.o: $(SOURCEDIR)/%.c
	@echo CC $<
	@$(CC) -c $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	@echo RM $(OBJ)
	@echo RM $(BUILDDIR)/$(OUTPUT)
	@rm -df  $(OBJ)
	@rm -Rdf $(BUILDDIR) $(OBJECTDIR)

all: clean build

flash: build
	@sudo avrdude -p $(MCU) -c $(PROGRAMMER) -P $(DEVICE)  -U flash:w:$(BUILDDIR)/$(OUTPUT).hex:i


fuse:
	@sudo avrdude -p $(MCU) -c $(PROGRAMMER) -P $(DEVICE) -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m

burn: fuse build
