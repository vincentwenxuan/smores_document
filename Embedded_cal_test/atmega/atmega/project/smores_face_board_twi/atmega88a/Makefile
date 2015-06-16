# This Makefile simply invokes the generic Makefile outside
# project folder.

# --------------------------------------------------------
# Specify the device you are using and its clock.
# --------------------------------------------------------

DEVICE     	= atmega88a
CLOCK      	= 8000000

# --------------------------------------------------------
# if you are using JTAGICE mkII, let PROGRAMMER = jtag2isp;
# if you are using AVRISP mkII, let PROGRAMMER = avrispmkII.
# --------------------------------------------------------

PROGRAMMER	= avrispmkII

# --------------------------------------------------------
# If you want to specify your customized library, edit the
# USER_LIB directory. If not, comment USER_LIB.
# --------------------------------------------------------

# USER_LIB = ../../../smores_common_library

include ../../../Makefile
