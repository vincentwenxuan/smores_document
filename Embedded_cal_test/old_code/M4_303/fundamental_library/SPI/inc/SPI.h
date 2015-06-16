/*
Author Chaitanya Bhargava (cbhar@seas.upenn.edu)
Settings for SPI usage on the STM32F303
mGeneral includes all the ST library files and the wait functions
If using some wireless llike the Zigbee or the WiFi then this 
library is not required unless modified as such
*/

#include "SPIplatform_config.h"

#ifndef __SPI_H
#define __SPI_H

#ifdef __cplusplus
 extern "C" {
#endif

void SPI_config(void);

#ifdef __cplusplus
}
#endif

#endif	/* SPI_H */

