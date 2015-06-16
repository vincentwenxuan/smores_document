ATmega88
========
The folder contains the ATmega88 template, atmega programming environment, examples showing how to use the libraries and Utility.
 - ATmega88Template: The template contains all necessary files for user to work with ATmega88;
 - atmega: 
    * common_library: contain commonly used libraries for users, including common_peripherals, ADC, PWM, TWI_slave and USART;
    * project: all projects' folder are here and put all source files including main.c in src folder and put all header files in inc folder;
    * Makefile: common Makefile;
    * smores_common_library: contain commonly used libraries for smores project, including drive_coils and wiper_encoder.
 - Utility: all necessary files and README for programming atmega in Windows.

In order to use the programming environment, install gcc-avr, avr-libc, avrdude and libusb first.  

How to use:

 - Make your own project_peripherals_customized-name.h and modify project_peripherals.h accordingly;
 - Modify the Makefile in project to specify the user's onw directories for header files and source files.
 - Figure out what programmer you are using and change the PROGRAMMER in project Makefile accordingly;
 - All the codes are made to work with 8MHz internal RC oscillator. With a new ATmega88 chip, first program the fuse using “make fuse”;
 - Use command “make” to generate the object files and hex file and all object files are in obj folder;
 - Use command “make flash” to flash the code to the chip;
 
