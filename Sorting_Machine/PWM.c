/*
* PWM.c
*
* Created: 20-10-21 12:20:39
*  Author: Tom
*/

#include "avr/io.h"
#include "Main.h"
#include "PWM.h"
#include "lcd.h"
#include "OS.h"
#include <util/delay.h>
#include <stdlib.h>


void PWM_SERVO(int match_value)
{
	// 50Hz du servo en question
	ICR1 = 19999;

	
	OCR1A = match_value;
}

void PWM_MOTEUR_DC(int duty_cycle)
{
	double full = (duty_cycle/(double) 100)* 255;
	uint8_t entier = (uint8_t) full;
	OCR2A = entier;
}
void PWM_LEDs(int duty_cycle)
{
	TCNT2 = 0;
	double full = (duty_cycle/(double) 100)* 255;
	uint8_t entier = (uint8_t) full;
	OCR2B = entier;
}





