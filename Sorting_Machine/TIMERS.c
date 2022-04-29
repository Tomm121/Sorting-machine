 /*
 * TIMERS.c
 *
 * Created: 9/29/2021 9:57:04 AM
 *  Author: Tom
 */
//INCLUDE
#include "TIMERS.h"
#include "Main.h"
#include "avr/io.h"
#include "avr/interrupt.h"
#include <util/delay.h>



void TIMER0_Init_COUNTER_1ms(void)
{
	// On a une fréquence d'horloge de 1MHz
	// Utilisation du TIMER 0, comptage sur 8 bits
	// Si diviseur par 8 --> 1000000/8 = 125 Khz
	// Une periode = un coup de clock tous les 8us
	// 1 ms / 8us = 125 ==> On doit faire 125 coups de clock pour avoir 1ms
	

	CLR_BIT(TCCR0B,CS02);
	SET_BIT(TCCR0B,CS01);
	CLR_BIT(TCCR0B,CS00);
	
	// Valeur initiale du compteur = 256 - 131 = 125
	TCNT0 = 125;
}

void TIMER1_init_FASTPW_ICRn(void)
{
	SET_BIT(TCCR1A, COM1A1);
	SET_BIT(TCCR1A, WGM11);
	SET_BIT(TCCR1B, WGM12);
	SET_BIT(TCCR1B, WGM13);
	SET_BIT(TCCR1B, CS10);
}

void TIMER2_init_FASTPWM(void)
{
	//SET_BIT(TCCR2A, COM2A1);
	SET_BIT(TCCR2A, COM2B1);
	SET_BIT(TCCR2A, WGM20);
	SET_BIT(TCCR2A, WGM21);
	SET_BIT(TCCR2B, CS20);
	
	//OCR2A = 0;
	TCNT2 = 0;
}










