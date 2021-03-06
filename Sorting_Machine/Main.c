 /*
* main.c
*
* Created: 9/29/2021 9:57:04 AM
*  Author: Tom
*/

#include "main.h"
#include "avr/io.h"
#include "TIMERS.h"
#include "hardware.h"
#include "OS.h"
#include "avr/interrupt.h"
#include <stdlib.h>
#include "lcd.h"
#include "I2C_slave.h"
#include "PWM.h"
#include <util/delay.h>
#include "I2C_slave.h"


// FONCTION PRINCIPALE

int main(void)
{
	// CONFIGURATION DE LA CLOCK A 1MS
	cli();
	CLKPR = 0b10000000;
	CLKPR = 0b00000100;
	sei();
	
	// INITIATION DES CLASSES
	HARDWARE_init();
	TIMER0_Init_COUNTER_1ms();
	TIMER1_init_FASTPW_ICRn();
	TIMER2_init_FASTPWM();
	I2C_init(0x20);
	lcd_init(LCD_DISP_ON);
	
	//Avant demarrage 
	i = -2;
	PWM_SERVO(DROITE);
	_delay_ms(1000);
	PWM_SERVO(CENTRE);
	
	//START BOUCLE INFINIE
	affichage_line1("Attente de la");
	affichage_line2("raspberry ...");
	while(RECEIVED == FALSE);
	state_machine();
	
	return 0;
}







