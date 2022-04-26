 /*
 * I2C_slave.h
 *
 * Created: 9/29/2021 9:57:04 AM
 *  Author: Tom
 */

#ifndef I2C_SLAVE_H
#define I2C_SLAVE_H

#include <avr/interrupt.h>
#include <stdint.h>

#define DATA_BYTE 4

extern volatile uint8_t data[8];
extern volatile uint8_t RECEIVED;
extern volatile int i;

void I2C_init(uint8_t address);
void I2C_stop(void);



ISR(TWI_vect);

#endif