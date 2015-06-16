# --------------------------------------------------------
# Custom M2 Makefile
# written by: Jonathan Bohren & Jonathan Fiene
# updated: Feb 2, 2013
#
# Custom ATmega88 Makefile
# edited by: Chao Liu(chao.liu0307@gmail.com)
# updated: Aug 3, 2014
# --------------------------------------------------------

# --------------------------------------------------------
# if you are using JTAGICE mkII, let PROGRAMMER = jtag2isp;
# if you are using AVRISP mkII, let PROGRAMMER = avrispmkII.
# --------------------------------------------------------

PROGRAMMER	= avrispmkII

# --------------------------------------------------------
# if you want to use one of our pre-compiled libraries,
# add it to the line below (e.g. "LIBRARIES = libsaast.a")
# --------------------------------------------------------

# --------------------------------------------------------
# you shouldn't change anything below here,
# unless you really know what you're doing
# --------------------------------------------------------
DEVICE     	= atmega88a
CLOCK      	= 8000000

AVRDUDE_TARGET	= m88

COMPILE = avr-gcc -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE)

SRCDIRS = src
INCDIRS = -Iinc	# directory for header files

OBJDIRS = obj
OBJECTS := $(patsubst %.c,%.o,$(notdir $(wildcard $(SRCDIRS)/*.c)))
OBJECTS_POS = $(addprefix $(OBJDIRS)/,$(OBJECTS))

vpath %.h $(INCDIRS)	# directory for header files
vpath %.c $(SRCDIRS)	# directory for source files
vpath %.o $(OBJDIRS)	# directory for object files
vpath %.elf $(OBJDIRS)

# symbolic targets: 
all:	main.hex
.PHONY : all

.c.o:
	@[ ! -e $@ ] && mkdir -p $(OBJDIRS)
	@$(COMPILE) $(INCDIRS) -c $< -o $(OBJDIRS)/$@
	@echo "[CC]  $^"

.S.o:
	@$(COMPILE) $(INCDIRS) -x assembler-with-cpp -c $< -o $(OBJDIRS)/$@
	@echo "[>-----Generate $@ Successfully-----<]"

.c.s:
	@$(COMPILE) $(INCDIRS) -S $< -o $(OBJDIRS)/$@
	@echo "[>-----Generate $@ Successfully-----<]"


fuse: 
	@avrdude -p $(AVRDUDE_TARGET) -c $(PROGRAMMER) -P usb -U lfuse:w:0xe2:m -U hfuse:w:0xdf:m -B10
	@echo "[>-----Program Fuse Done-----<]"

install: flash 

flash: all
	@avrdude -p $(AVRDUDE_TARGET) -c $(PROGRAMMER) -P usb -e -U flash:w:main.hex -B9
	@echo "[>-----ATmega88a Loaded-----<]"

clean:
	rm -fr main.hex $(OBJDIRS)

# file targets:
main.elf: $(OBJECTS)
	@$(COMPILE) -o $(OBJDIRS)/main.elf $(OBJECTS_POS) $(LIBRARIES) -lm
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
