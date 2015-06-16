# This generic Makefile is intended to be invoked through 
# ’include' by a Makefile in the project directory.
# --------------------------------------------------------
# Custom ATmega Makefile
# edited by: Chao Liu(chao.liu0307@gmail.com)
# updated: Aug 3, 2014
# --------------------------------------------------------

# --------------------------------------------------------
# Support atmega88a and atmega32u4
# --------------------------------------------------------

# --------------------------------------------------------
# you shouldn't change anything below here,
# unless you really know what you're doing
# --------------------------------------------------------

ifeq ($(DEVICE), atmega88a)
	DEVICE_COMMON_LIB = atmega88a_common
	TARGET_DEVICE = m88
else ifeq ($(DEVICE), atmega32u4)
	DEVICE_COMMON_LIB = atmega32u4_common
	TARGET_DEVICE = m32u4
else
$(error DEVICE = $(DEVICE) is unknown.)
endif

COMPILE = avr-gcc -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE)

SRCDIR = src
INCDIR = inc	# directory for header files

DIR_COMMON_LIB = ../../../common_library
DIR_DEVICE_COMMON_LIB = ../../../$(DEVICE_COMMON_LIB)
ifdef USER_LIB
	DIR_USER_LIB = $(USER_LIB)
endif

INCLUDES = -I$(DIR_COMMON_LIB)/inc
INCLUDES += -I$(DIR_DEVICE_COMMON_LIB)/inc
ifdef USER_LIB
	INCLUDES += -I$(DIR_USER_LIB)/inc
endif
INCLUDES += -I$(INCDIR)

_SOURCES = $(wildcard $(DIR_COMMON_LIB)/*/*.c)
_SOURCES += $(wildcard $(DIR_DEVICE_COMMON_LIB)/*/*.c)
ifdef USER_LIB
	_SOURCES += $(wildcard $(DIR_USER_LIB)/*/*.c)
endif
_SOURCES += $(wildcard $(SRCDIR)/*.c)
SOURCES = $(notdir $(_SOURCES))

OBJDIRS = obj
OBJECTS := $(patsubst %.c,%.o, $(SOURCES))
OBJECTS_POS = $(addprefix $(OBJDIRS)/,$(OBJECTS))

vpath %.c $(dir $(_SOURCES))	# directory for source files
vpath %.o $(OBJDIRS)                    # directory for object files
vpath %.elf $(OBJDIRS)
vpath %.hex .

# symbolic targets: 
all:	main.hex
.PHONY : all

.c.o:
	@[ ! -e $@ ] && mkdir -p $(OBJDIRS)
	@$(COMPILE) $(INCLUDES) -c $< -o $(OBJDIRS)/$@
	@echo "[CC]  $^"

.S.o:
	@$(COMPILE) $(INCLUDES) -x assembler-with-cpp -c $< -o $(OBJDIRS)/$@
	@echo "[>-----Generate $@ Successfully-----<]"

.c.s:
	@$(COMPILE) $(INCLUDES) -S $< -o $(OBJDIRS)/$@
	@echo "[>-----Generate $@ Successfully-----<]"

fuse:
ifeq ($(DEVICE), atmega88a)
	@avrdude -p $(TARGET_DEVICE) -c $(PROGRAMMER) -P usb -U lfuse:w:0xe2:m -U hfuse:w:0xdf:m -B10
	@echo "[>-----Program Fuse Done-----<]"
else ifeq ($(DEVICE), atmega32u4)
	@avrdude -p $(TARGET_DEVICE) -c $(PROGRAMMER) -P usb -U lfuse:w:0xe2:m -U hfuse:w:0xdf:m -B10
	@echo "[>-----Program Fuse Done-----<]"
endif

install: flash 

flash: all
ifeq ($(DEVICE), atmega88a)
	@avrdude -p $(TARGET_DEVICE) -c $(PROGRAMMER) -P usb -e -U flash:w:main.hex -B9
	@echo "[>-----ATmega88a Loaded-----<]"
endif
ifeq ($(DEVICE), atmega32u4)
ifneq ($(PROGRAMMER), USB)
	@avrdude -p $(TARGET_DEVICE) -c $(PROGRAMMER) -P usb -e -U flash:w:main.hex -B9
	@echo "[>-----ATmega32U4 Loaded-----<]"
else
	@dfu-programmer $(DEVICE) erase
	@dfu-programmer $(DEVICE) flash main.hex
	@echo "[>-----ATmega32U4 Loaded-----<]"
endif
endif

clean:
	rm -fr main.hex $(OBJDIRS)

# file targets:
main.elf: $(OBJECTS)
	@$(COMPILE) -o $(OBJDIRS)/main.elf $(OBJECTS_POS) -lm
	@echo "[ELF] $(OBJDIRS)/$@"

main.hex: main.elf
	@rm -f main.hex
	@avr-objcopy -j .text -j .data -O ihex $(OBJDIRS)/main.elf main.hex
	@echo "[>-----Generate $@ Successfully-----<]"
	@echo "[>-----Build Successfully-----<]"

# Targets for code debugging and analysis:
disasm:	main.elf
	avr-objdump -d $(OBJDIRS)/main.elf

cpp:
	$(COMPILE) -E main.c
