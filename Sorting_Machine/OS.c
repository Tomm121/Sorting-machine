/*
* OS.c
*
* Created: 29-09-21 09:57:52
*  Author: Tom
*/


#include "avr/interrupt.h"
#include "avr/io.h"
#include "TIMERS.h"
#include "hardware.h"
#include "OS.h"
#include "main.h"
#include "lcd.h"
#include "PWM.h"
#include "I2C_slave.h"
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>
#include <util/twi.h>



// VARIABLES GLOBALES
unsigned long val_res_wtd;
int chiffre;
int x_tab;
int val_res_wtd_tab[10];
char buffer_itoa[10];
char buffer_TWI[10];
//int CHOIX_RES;
unsigned long res_read;
char FIRST = TRUE;
char FIRST2 = TRUE;
const unsigned long dureeAntiRebond = 10;
unsigned long timing;
uint16_t timing_conv = 0;;
uint8_t try_TV = 0;


enum states{state1,state2,state3,state4,state5,state6};
enum states state = state1;


///////////////////////////////////////////
///// -----  BOUCLE PRINCIPALE ----- /////
/////////////////////////////////////////

void state_machine(void)
{
	i = -2; // Variable externe de la classe I2C servant comme indice de la position des donnees dans le tableau de reception des donnees "data", il demarre ? -2 car reception de 2 bytes non data apr?s le SLA+R/W
	PWM_LEDs(duty_cycle_leds); // Activation du panneau de LEDs avec une certaine luminosite pilote en PWM
	while(1)
	{
		switch (state)
		{
			case state1:
			
				if (FIRST == TRUE)
				{
					val_res_wtd = 0; //valeur de r?sistance souhait?e
					x_tab = 0; // indice de la position dans le tableau d'entier qui recoit les chiffres, au fur et ? mesure des appuis sur le bouton poussoir de l'encodeur pour les valider
					chiffre = 0; //valeur du chiffre qui s'incr?mente et d?cr?mente quand on tourne l'encodeur rotatif
					timing = 0; // valeur du timer en ms
					affichage_line1("Choix resistance : ");
					affichage_chiffre_lcd();
					enable_btns(); // Active les interuptions des boutons de la partie interface utilisateur
					SET_BIT(TIMSK0,TOIE0); // activation du timer pour calculer le temps entre deux impulsions de l'encodeur afin d'eviter les rebonds
					FIRST = FALSE;
				}
				break;
			
			case state2:
				CLR_BIT(TIMSK0,TOIE0); // Arret du timer
				affichage_line1("Table vibrante...");
				table_vibrante_ON(); // Activation de la table vibrante
				_delay_ms(Time_TB);
				table_vibrante_OFF(); // Desactivation table vibrante
				try_TV++;
				state = state3;
				break;
			
			case state3 :
				reset_data();
				affichage_line1("Attente info de");
				affichage_line2("la Raspberry...");
				RECEIVED = FALSE;
				SET_BIT(TIMSK0,TOIE0); // Arret du timer
				timing_conv = 0;
				activation_EN_A4988();
				while(RECEIVED == FALSE && timing_conv < TIMEOUT_CONV)
				{
					convoyeur(); // un pas du convoyeur
				}
				CLR_BIT(TIMSK0,TOIE0); // Arret du timer
				if (RECEIVED == TRUE)
				{
					try_TV = 0;
					RECEIVED = FALSE;
					state = state4;
				}
				else
				{
					desactivation_EN_A4988();
					if (try_TV == TRYOUT_TV)
					{
					
						try_TV = 0;
						state = state6;
					}
					else
					{
						state = state2;
					}
				}
				break;
			
			case state4:
			if (FIRST2 == TRUE)
			{
				affichage_line1("Composant !");
				affichage_line2("Scan en cours...");
				desactivation_EN_A4988();
				FIRST2 = FALSE;
			}
			if (RECEIVED == TRUE)
			{
				RECEIVED = FALSE;
				FIRST2 = TRUE;
				state = state5;
			}
			break;
			
			case state5 :
			res_read = unmask_data(data); // fonction qui demasque les differents bytes envoyes par la raspberry et les remet dans le bon ordre
			affichage_line1("Resistance lue : ");
			affichage_long(res_read); // Affichage de la valeur de r?sistance scan?e envoy?e par la Raspberry, d?masqu?e par la fonction precedente
			_delay_ms(1000); // Temps d'affichage sur le LCD
			servomoteur(); // Activation de servomoteur en fonction du resultat
			_delay_ms(1000);
			PWM_SERVO(CENTRE);
			reset_buf();
			state = state3;
			break;
			
			case state6 :
			FIRST = TRUE;
			reset_tab();
			reset_buf();
			affichage_line1("Tri termine");
			_delay_ms(1500);
			state = state1;
			break;
		}
	}

}




////////////////////////////////////////////
///// -----  FONCTIONS DIVERSES ----- /////
//////////////////////////////////////////

void affichage_chiffre_lcd(void)
{
	itoa(chiffre,buffer_itoa,10);
	lcd_gotoxy(x_tab,1);
	lcd_puts(buffer_itoa);
}

void affichage_line1(char *s)
{
	lcd_clrscr();
	lcd_gotoxy(0,0);
	lcd_puts(s);
}

void affichage_line2(char *s)
{
	lcd_gotoxy(0,1);
	lcd_puts(s);
}

void affichage_long(long val)
{
	lcd_gotoxy(0,1);
	ultoa(val,buffer_TWI,10);
	lcd_puts(buffer_TWI);
}

void reset_tab(void)
{
	for (int i = 0;i !=10 ;  i++)
	{
		val_res_wtd_tab[i] = 0;
	}
}

void reset_buf(void)
{
	for (int i = 0;i !=10 ;  i++)
	{
		buffer_itoa[i] = 0;
	}
}

void reset_data(void)
{
	for (int i = 0; i != 8 ;  i++)
	{
		data[i] = 0;
	}
}

void enable_btns(void)
{
	SET_BIT(EIMSK,INT2);
	SET_BIT(EIMSK,INT4);
	SET_BIT(EIMSK,INT5);
}

void disable_btns(void)
{
	CLR_BIT(EIMSK,INT2);
	CLR_BIT(EIMSK,INT4);
	CLR_BIT(EIMSK,INT5);
}

unsigned long unmask_data(volatile uint8_t data[])
{
	unsigned long data_unmasked;
	data_unmasked = data[0];
	data_unmasked = data_unmasked + ((unsigned long)data[1]<<8);
	data_unmasked = data_unmasked + ((unsigned long)data[2]<<16);
	data_unmasked = data_unmasked + ((unsigned long)data[3]<<24);
	return data_unmasked;
}

void compute_value(void)
{
	int lengh = x_tab ;
	int exposant = x_tab;
	for (int i = 0; i < lengh + 1; i++)
	{
		// Erreur ? cause de la fonction "pow" qui n'est pas assez precise
		long ex = lround(pow(10,exposant));
		if (ex==9999984)
		{
			ex = 10000000;
		}
		if (ex==99999968)
		{
			ex = 100000000;
		}
		val_res_wtd = val_res_wtd +  (val_res_wtd_tab[i] * ex);
		exposant--;
		if (exposant < 0)
		{
			exposant = 0;
		}
	}
}

void my_delay_us(uint16_t us) // Fonction de delay pour entrer une variable en param?tre
{
	while (0 < us)
	{
		_delay_us(1);
		--us;
	}
}

void desactivation_EN_A4988(void)
{
	_delay_ms(500);
	SET_BIT(PORTB,PB0);
	_delay_ms(500);
}
void activation_EN_A4988(void)
{
	_delay_ms(500);
	CLR_BIT(PORTB,PB0);
	_delay_ms(500);
}


///////////////////////////////////
///// -----  MOTEUR DC ----- /////
/////////////////////////////////

void table_vibrante_ON(void)
{
	SET_BIT(PORTL,PL5);
	PWM_MOTEUR_DC(duty_cycle_mot_dc);
}

void table_vibrante_OFF(void)
{
	CLR_BIT(PORTL,PL5);
	OCR2A = 0;
}

//////////////////////////////////////////
///// -----  MOTEUR PAS A PAS ----- /////
////////////////////////////////////////

void convoyeur(void)
{
		SET_BIT(PORTL,PL3);
		my_delay_us(pulseWidthMicros);
		CLR_BIT(PORTL,PL3);
		my_delay_us(microsBtwnSteps);
}

///////////////////////////////////////
///// -----  SERVOMOTEUR ----- ///////
/////////////////////////////////////

void servomoteur(void)
{
	if(val_res_wtd == res_read)
	{
		PWM_SERVO(DROITE);
	}
	else
	{
		PWM_SERVO(GAUCHE);
	}
}

///////////////////////////////////////
///// -----  INTERRUPTIONS ----- /////
/////////////////////////////////////

ISR(TIMER0_OVF_vect)
{
	TCNT0 = 125;
	timing++;
	timing_conv++;
}


ISR(INT4_vect) // Channel A de l'encodeur rotatif en pin 2
{
	static unsigned long dateDernierChangement = 0;
	unsigned long date = timing;
	if ((date - dateDernierChangement) > dureeAntiRebond) {
		uint8_t channel_B = (PINA && 1); // channel B sur la pin 22
		if (channel_B == HIGH)
		{
			if (chiffre < 9)
			{
				chiffre = chiffre + 1;
			}
		}
		else {
			if (chiffre > 0)
			{
				chiffre = chiffre - 1;
			}
		}
		affichage_chiffre_lcd();
		dateDernierChangement = date;
	}
}

ISR(INT5_vect) // SW : bouton poussoir de l'encodeur
{
	disable_btns();

	if(x_tab <8)
	{
		val_res_wtd_tab[x_tab] = chiffre;
		x_tab++;
		chiffre = 0;
	}
	affichage_chiffre_lcd();
	_delay_ms(400); // Eviter une nouvelle interruption si l'utilisateur laisse trop longtemps appuye le bouton
	
	enable_btns();
	sei();
}

ISR(INT2_vect) // bouton poussoir
{
	disable_btns();
	
	val_res_wtd_tab[x_tab] = chiffre;
	compute_value();
	affichage_line1("Valeur souhaite : ");
	affichage_long(val_res_wtd);
	_delay_ms(1500);
	state = state2;
}



