MAKEFLAGS = -j

PROJNAME  = main
BUILDDIR  = obj
INCDIR    = inc
LIBDIR    = ../../../stm32f37x_common
COMDIR    = ../../../c_common
CPPCOMDIR = ../../../cpp_common
SOURCEDIR = src
ARMDIR   := arm
CPALDIR  := cpal
DSPDIR   := dsp
STDIR    := st
USBDIR   := usb
M4DIR    := m4
USERDIR  := user_lib

ARCH = arm-none-eabi
CC = $(ARCH)-gcc
CXX = $(ARCH)-g++
LD = $(ARCH)-ld
AS = $(ARCH)-as
SZ = $(ARCH)-size
OBJCPY = $(ARCH)-objcopy
OBJDMP = $(ARCH)-objdump

LDSCRIPT = $(LIBDIR)/$(STDIR)/STM32_FLASH.ld
STARTUP  = $(LIBDIR)/$(STDIR)/startup_stm32f37x.s

# compiler includes
INCLUDES = -I$(INCDIR) -I$(LIBDIR)/$(ARMDIR) -I$(LIBDIR)/$(CPALDIR)/$(INCDIR)
INCLUDES += -I$(LIBDIR)/$(USERDIR)/$(INCDIR) -I$(LIBDIR)/$(M4DIR)/$(INCDIR)
INCLUDES += -I$(LIBDIR)/$(STDIR)/$(INCDIR) -I$(LIBDIR)/$(USBDIR)/$(INCDIR)
INCLUDES += -I$(COMDIR)/$(USERDIR)/$(INCDIR)
INCLUDES += -I$(CPPCOMDIR)/$(USERDIR)/$(INCDIR)
INCLUDES += -I$(CPPCOMDIR)/$(EIGEN)

# base compiler flags (common for CC and CXX and linking)
BFLAGS = -O2 -Wall -Wdouble-promotion -Wno-unused-local-typedefs
BFLAGS += -fsingle-precision-constant
BFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m4
BFLAGS += -DARM_MATH_CM4 -D'__FPU_PRESENT=1'
BFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=hard
BFLAGS += -DHSE_VALUE=16000000UL # is this needed/used?
BFLAGS += -fno-common -fno-exceptions -ffunction-sections -fdata-sections
BFLAGS += -flto -ffast-math

# CC-specific compiler flags (not linking!)
CFLAGS = -std=c99

# CXX-specific compiler flags (not linking!)
CXXFLAGS =

# linker-specific flags
LDFLAGS = --static -Wl,--gc-sections
LDFLAGS += -Wl,-Map,$(BUILDDIR)/$(PROJNAME).map
LDFLAGS += -T$(LDSCRIPT)
LDFLAGS += -lm -lnosys
LDFLAGS += -specs=nano.specs -u _printf_float # If size if an issue

AFLAGS  = -mlittle-endian -mthumb -mcpu=cortex-m4
ODFLAGS	= -x --syms
CPFLAGS = --output-target=binary

# collect list of C source files
_CSRCS = $(wildcard $(SOURCEDIR)/*.c)
_CSRCS += $(wildcard $(LIBDIR)/$(CPALDIR)/*/*.c)
_CSRCS += $(wildcard $(LIBDIR)/$(DSPDIR)/*/*.c)
_CSRCS += $(wildcard $(LIBDIR)/$(STDIR)/*/*.c)
_CSRCS += $(wildcard $(LIBDIR)/$(USBDIR)/*/*.c)
_CSRCS += $(wildcard $(LIBDIR)/$(M4DIR)/*/*.c)
_CSRCS += $(wildcard $(LIBDIR)/$(USERDIR)/*/*.c)
_CSRCS += $(wildcard $(COMDIR)/$(USERDIR)/*/*.c)
CSRCS = $(notdir $(_CSRCS))
CSRCS += startup_stm32f37x.s

# collect list of C++ source files
_CXXSRCS = $(wildcard $(SOURCEDIR)/*.cpp)
_CXXSRCS += $(wildcard $(CPPCOMDIR)/$(USERDIR)/*/*.cpp)
CXXSRCS = $(notdir $(_CXXSRCS))

# collect list of C objects
_COBJS = $(patsubst %.c,$(BUILDDIR)/%.o,$(CSRCS))
COBJS = $(patsubst %.s,$(BUILDDIR)/%.o,$(_COBJS))

# collect list of C++ objects
_CXXOBJS = $(patsubst %.cpp,$(BUILDDIR)/%.o,$(CXXSRCS))
CXXOBJS = $(patsubst %.s,$(BUILDDIR)/%.o,$(_CXXOBJS))

VPATH := $(sort $(dir $(_CSRCS) $(_CXXSRCS)))


#------------------------------------------------------------------------------
.PHONY: proj disassemble serialflash flash clean cleanlib

all: proj
ifeq ($(OS),Windows_NT)
	@$(SZ) $(BUILDDIR)/$(PROJNAME).elf
else
	@$(SZ) $(BUILDDIR)/$(PROJNAME).elf | tail -n 1 | awk '{print "Flash:\t", $$1+$$2, "\t[",($$1+$$2)*100/(128*1024),"% ]", "\nRAM:\t", $$2+$$3, "\t[",($$2+$$3)*100/(24*1024),"% ]"}'
endif
	@echo "[>--BinaryGenerated--<]"

proj: $(BUILDDIR)/$(PROJNAME).elf

$(BUILDDIR)/$(PROJNAME).elf: $(COBJS) $(CXXOBJS)
	@echo "  [ELF] $@"
	@$(CXX) $(BFLAGS) $(CFLAGS) $(COBJS) $(CXXFLAGS) $(CXXOBJS) $(CXXFLAGS) -o $@ $(LDFLAGS)
	@$(OBJCPY) $(CPFLAGS) $(BUILDDIR)/$(PROJNAME).elf $(BUILDDIR)/$(PROJNAME).bin

disassemble: proj
	@$(OBJDMP) $(ODFLAGS) $(BUILDDIR)/$(PROJNAME).elf > $(BUILDDIR)/$(PROJNAME).dmp
	@$(OBJDMP) -dS $(BUILDDIR)/$(PROJNAME).elf > $(BUILDDIR)/$(PROJNAME).dis

$(COBJS): | $(BUILDDIR)

$(CXXOBJS): | $(BUILDDIR)

$(BUILDDIR):
ifeq ($(OS),Windows_NT)
	@If Not Exist "$(BUILDDIR)" (mkdir "$(BUILDDIR)")
else
	@mkdir -p $(BUILDDIR)
endif

$(BUILDDIR)/%.o: %.c
	@echo "  [CC]  $^"
	@$(CC) $(INCLUDES) $(BFLAGS) $(CFLAGS) -c -o $@ $^

$(BUILDDIR)/%.o: %.cpp
	@echo "  [CXX] $^"
	@$(CXX) $(INCLUDES) $(BFLAGS) $(CXXFLAGS) -c -o $@ $^

$(BUILDDIR)/startup_stm32f37x.o: $(STARTUP)
	@$(AS) $(AFLAGS) $(STARTUP) -o $@

serialflash: all
	@echo "[>-Downloading to M4-<]"
	@./extra/stm32loader.py -p $(PORT) -evw "$(BUILDDIR)/$(PROJNAME).bin"

flash: all
	@echo "[>-Downloading to M4-<]"
ifeq ($(OS),Windows_NT)
	@"../../../tools/dfu-util/dfu-util" -d 0483:df11 -a 0 -s 0x08000000 -D "$(BUILDDIR)/$(PROJNAME).bin"
else
	@ls -l $(BUILDDIR)/$(PROJNAME).bin
	@sudo dfu-util -d 0483:df11 -a 0 -s 0x08000000 -D "$(BUILDDIR)/$(PROJNAME).bin"
endif
	@echo "[>-----M4 Loaded-----<]"

clean:
ifeq ($(OS),Windows_NT)
	@If Exist "$(BUILDDIR)" rd /S /Q $(BUILDDIR)
else
	@rm -fr $(BUILDDIR)
endif

#------------------------------------------------------------------------------
