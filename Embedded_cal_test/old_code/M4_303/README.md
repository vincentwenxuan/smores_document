M4_303
========
Use Utilities/zadig to install the drivers for the F303. For more details see getting started with M4 on the medesign website.

Use the M4_303/M4303_template to get started.
The HSE clock settings have been updated for 16Mhz input clock. Hence the template will no longer work with the discovery board. Either use the discovery template or copy the correct clock settings from Useful_libraries/Disco_clock_config
mGeneral has led settings for the SMORES board. change them according to custom board

For M4_303 library [apis](#)

USB not tested yet.
Memory usage shown needs to be updated for the f303 in the makefile

There are example codes and user libraries in the M4_303/user_libraries_projects
each library has its own platform config. You can combine them into one.


NOTE - it is recommended not to toggle the leds on the smores board faster than 2 MHz
