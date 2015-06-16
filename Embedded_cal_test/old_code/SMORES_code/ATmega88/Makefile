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
# if you write separate C files to include in main,
# add their .o targets to the CHILDREN line below
# (e.g. "CHILDREN = myfile.o")
#
# to include code supplied by maevarm, add a .o target
# tag to the parents line (e.g. "PARENTS = "m_bus.o")
# --------------------------------------------------------
MAIN       = main.o 
CHILDREN   = drive_coil.o
PARENTS    = PWM.o TWI.o USART.o

# --------------------------------------------------------
# if you are using JTAGICE mkII, let PROGRAMMER = jtag2isp;
# if you are using AVRISP mkII, let PROGRAMMER = avrispmkII.
# --------------------------------------------------------

PROGRAMMER	= jtag2isp

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

# symbolic targets: 
all:	main.hex

.c.o:
	$(COMPILE) -c $< -o $@

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@

.c.s:
	$(COMPILE) -S $< -o $@

fuse: 
	avrdude -p $(AVRDUDE_TARGET) -c $(PROGRAMMER) -P usb -U lfuse:w:0xe2:m -U hfuse:w:0xdf:m -B10

install: flash 

flash: all
	avrdude -p $(AVRDUDE_TARGET) -c $(PROGRAMMER) -P usb -e -U flash:w:main.hex -B9

clean:
	rm -f main.hex main.elf $(MAIN) $(CHILDREN)

# file targets:
main.elf: $(MAIN) $(CHILDREN) $(PARENTS)
	$(COMPILE) -o main.elf $(MAIN) $(CHILDREN) $(PARENTS) $(LIBRARIES) -lm

main.hex: main.elf
	rm -f main.hex
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex

# Targets for code debugging and analysis:
disasm:	main.elf
	avr-objdump -d main.elf

cpp:
	$(COMPILE) -E main.c
