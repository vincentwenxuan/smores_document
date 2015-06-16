
Use AVR-GCC and avrdude on Windows:

1. Install WinAVR on the computer;

2. Copy libusb0.dll from libusb-win32-bin-1.2.6.0/libusb-win32-bin-1.2.6.0/bin to Windows/Syswow64 if the OS is 64 bit or Windows/System32 if the OS is 32 bit;

3. Copy libusb0.sys to  Windows/System32/drivers/libusb0.sys;

4. Connect AVRISP mkII with your compute. When it asks whether you wish to install the drivers for it automatically or manually, choose MANUAL, and select libusb-win32-bin-1.2.6.0 folder or avrispmkii_libusb-win32_1.2.1.0 folder. If it’s detected successfully you should be able to see AVRISP mkII under Atmel USB Devices in Device Manager.

5. If it’s not successful detected when you plug in AVRISP mkII, check if the AVRISP mkII under Jungo Connectivity. Right click this and select “Update Driver Software”, then select “Browse my computer for driver software” and go to avrispmkii_libusb-win32_1.2.1.0 folder to install the driver.

6. Open cmd.exe and go to the project folder which contains the Makefile. 

7. Input “make” to make the .hex file, input “make fuse” to program the fuse and input “make flash“ to flash the code.