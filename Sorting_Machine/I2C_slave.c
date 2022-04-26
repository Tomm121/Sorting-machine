 /*
 * I2C_slave.c
 *
 * Created: 9/29/2021 9:57:04 AM
 *  Author: Tom
 */
#include <util/twi.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "I2C_slave.h"
#include "OS.h"
#include "Main.h"
#include "lcd.h"

volatile uint8_t data[8];
volatile uint8_t RECEIVED = FALSE;
volatile int i = 0;

void I2C_recieved(uint8_t data_recieved)
{
	data[i] = data_recieved;
		i++;
}

void I2C_requested()
{
	TWDR = data[i];
	i++;
}

void I2C_init(uint8_t address)
{
	cli();
	// load address into TWI address register
	TWAR = address << 1;
	// set the TWCR to enable address matching and enable TWI, clear TWINT, enable TWI interrupt
	TWCR = (1<<TWIE) | (1<<TWEA) | (1<<TWINT) | (1<<TWEN);
	sei();
}

void I2C_stop(void)
{
	// clear acknowledge and enable bits
	cli();
	TWCR = 0;
	TWAR = 0;
	sei();
}

ISR(TWI_vect)
{
	switch(TW_STATUS)
	{
		case TW_SR_DATA_ACK:
		// received data from master, call the receive callback
		I2C_recieved(TWDR);
		if (i == DATA_BYTE)
		{
			RECEIVED = TRUE;
			i = -2;
		}
		/*RECEIVED = TRUE;*/
		TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
		break;
		case TW_ST_SLA_ACK:
		// master is requesting data, call the request callback
		I2C_requested();
		TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
		break;
		case TW_ST_DATA_ACK:
		// master is requesting data, call the request callback
		I2C_requested();
		TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
		break;
		case TW_BUS_ERROR:
		// some sort of erroneous state, prepare TWI to be readdressed
		TWCR = 0;
		TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
		break;
		default:
		TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
		break;
	}
}
