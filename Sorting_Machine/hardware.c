/*
 * hardware.c
 *
 * Created: 29-09-21 10:18:32
 *  Author: Tom
 */ 
#include "avr/io.h"
#include "avr/interrupt.h"
#include "TIMERS.h"
#include "main.h"
	

void HARDWARE_init(void) 
{	
	//SET_DIRECTION (0 = input / 1 = output)
	CLR_BIT(DDRA,DDA0); // channel B
	CLR_BIT(DDRE,DDE4); // channel A
	CLR_BIT(DDRE,DDE5); // SW
	CLR_BIT(DDRD,DDD2); // ENTER
	
	SET_BIT(DDRD,DDD1); // SCL
	SET_BIT(DDRD,DDD0); // SDA
	SET_BIT(DDRL,DDL5); // Moteur DC
 	SET_BIT(DDRL,DDL3); // Moteur pas à pas : STEP
 	SET_BIT(DDRL,DDL1); // Moteur pas à pas : DIR
	SET_BIT(DDRB,DDB5); // PWM Servo
	SET_BIT(DDRB,DDB4); // PWM Moteur DC
	SET_BIT(DDRH,DDH6); // PWM LEDs
	 
	
	// SET PULLUP (0 = disabled / 1 = enabled)
	SET_BIT(PORTA,PA0); 
	SET_BIT(PORTE,PE4); 
	SET_BIT(PORTE,PE5);
	SET_BIT(PORTD,PD2);
	
	
	/// SET INTERRUPTS
	SET_BIT(EICRB,ISC21); // Bouton poussoir
	CLR_BIT(EICRB,ISC20);
	CLR_BIT(EICRB,ISC41); // Channel A de l'encodeur rotatif en pin 2 
	CLR_BIT(EICRB,ISC40);
	CLR_BIT(EICRB,ISC51); // Bouton poussoir de l'encodeur
	CLR_BIT(EICRB,ISC50);
	
	sei(); // Autorise les interruptions globales
}


