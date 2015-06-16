/*
Author Chaitanya Bhargava (cbhar@seas.upenn.edu)
Settings for I2C usage on the STM32F303
mGeneral includes all the ST library files and the wait functions
*/
#ifndef I2C_V2_H
#define	I2C_V2_H

#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

void I2C_config(void);
uint8_t I2C_write_1byte(uint8_t SLAVE_ADDR,uint8_t reg,uint8_t data);
uint8_t I2C_write(uint8_t SLAVE_ADDR,uint8_t reg,uint8_t *data, int len);
uint8_t I2C_read_1byte(uint8_t SLAVE_ADDR, uint8_t reg);
uint8_t* I2C_read(uint8_t SLAVE_ADDR, uint8_t reg,uint8_t *rxbuf,int len);


#ifdef	__cplusplus
}
#endif

#endif	/* I2C_V2_H */
