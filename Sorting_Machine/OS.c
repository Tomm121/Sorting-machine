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
char buffer_debug[10];
char buffer_TWI[10];
int CHOIX_RES;
unsigned long res_read;
char FIRST = TRUE;
const unsigned long dureeAntiRebond = 10;
unsigned long timing;
uint8_t try;


void state_machine(void)
{
	
}



///////////////////////////////////////////
///// -----  BOUCLE PRINCIPALE ----- /////
/////////////////////////////////////////
void Loop_OS(void)
{
	i = -2; // Variable externe de la classe I2C servant comme indice de la position des donnees dans le tableau de reception des donnees "data", il demarre à -2 car reception de 2 bytes non data après le SLA+R/W 
	PWM_LEDs(duty_cycle_leds); // Activation du panneau de LEDs avec une certaine luminosite pilote en PWM
	while (1)
	{
		if(FIRST == TRUE)
		{
			val_res_wtd =0; //valeur de résistance souhaitée
			CHOIX_RES = FALSE; // valeur bool si le choix de la valeur est terminé ou non
			x_tab = 0; // indice de la position dans le tableau d'entier qui recoit les chiffres, au fur et à mesure des appuis sur le bouton poussoir de l'encodeur pour les valider 
			chiffre = 0; //valeur du chiffre qui s'incrémente et décrémente quand on tourne l'encodeur rotatif 
			timing = 0; // valeur du timer en ms
			affichage_line1("Choix resistance : ");
			affichage_chiffre_lcd();
			enable_btns(); // Active les interuptions des boutons de la partie interface utilisateur
		}
		timing = 0; 
		SET_BIT(TIMSK0,TOIE0); // activation du timer pour calculer le temps entre deux impulsions de l'encodeur afin d'eviter les rebonds
		while(CHOIX_RES == FALSE) // Attente de l'appui sur le bouton de demarrage du tri pour indiquer que le choix de valeur est terminé
		{
		}
		CLR_BIT(TIMSK0,TOIE0); // Arret du timer
		timing = 0; // Reinitialisation  du timer
		affichage_line1("Table vibrante...");
		SET_BIT(TIMSK0,TOIE0); // Activation du timer pour avoir un timeout si aucun composant ne tombe au bout d'un certain temps
		table_vibrante_ON(); // Activation de la table vibrante
		while(timing != TIMEOUT);
		CLR_BIT(TIMSK0,TOIE0); // Arret du timer
		table_vibrante_OFF(); // Desactivation table vibrante
		timing = 0; // Reinitialisation  du timer
		reset_data();
		affichage_line1("Attente info de");
		affichage_line2("la Raspberry...");
		RECEIVED = FALSE;
		do 
		{
			try++;
			convoyeur(); // Activation du convoyeur
			itoa(try,buffer_debug,10);
			lcd_gotoxy(15,1);
			lcd_puts(buffer_debug);
			_delay_ms(1500);
		}
		while(RECEIVED == FALSE && try != TRYOUT);
		if (try != TRYOUT) // si il y a encore des composants, on ne repasse pas par le choix de la valeur de composant
		{
			try = 0;
			CHOIX_RES = TRUE; // On ne doit plus refaire le choix du composant au debut
			res_read = unmask_data(data); // fonction qui demasque les differents bytes envoyes par la raspberry et les remet dans le bon ordre
			affichage_line1("Resistance lue : ");
			affichage_long(res_read); // Affichage de la valeur de résistance scanée envoyée par la Raspberry, démasquée par la fonction precedente
			_delay_ms(1500); // Temps d'affichage sur le LCD 
			servomoteur(); // Activation de servomoteur en fonction du resultat
			//convoyeur(); // Activation du convoyeur
			timing =  0;
			FIRST = FALSE;
			reset_buf();
		}
		else // Dans le cas où il n'y a plus de composants, on se remet à "l'etat initial" du choix de la valeur.
		{
			try = 0;
			FIRST = TRUE;
			reset_tab();
			reset_buf();
			affichage_line1("Tri termine");
			_delay_ms(1500);
		}
	}
}

////////////////////////////////////////////
///// -----  FONCTIONS DIVERSES ----- /////
//////////////////////////////////////////

void affichage_chiffre_lcd(void)
{
	itoa(chiffre,buffer_debug,10);
	lcd_gotoxy(x_tab,1);
	lcd_puts(buffer_debug);
}

void affichage_line1(char *s)
{
	lcd_clrscr();
	lcd_gotoxy(0,0);
	lcd_puts(s);
}

void affichage_line2(char *s)
{
	//lcd_clrscr();
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
	for (int i = 0;i <10 ;  i++)
	{
		val_res_wtd_tab[i] = 0;
	}
}

void reset_buf(void)
{
	for (int i = 0;i <10 ;  i++)
	{
		buffer_debug[i] = 0;
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
		// Erreur à cause de la fonction "pow" qui n'est pas assez precise
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
// 	lcd_clrscr();
// 	lcd_puts("Convoyeur...");
	for (int i = 0; i < stepsPerRev; i++)
	{
		SET_BIT(PORTL,PL3);
		_delay_us(pulseWidthMicros);
		CLR_BIT(PORTL,PA3);
		_delay_us(microsBtwnSteps);
	}
	//lcd_clrscr();
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
}


ISR(INT4_vect) // Channel A de l'encodeur rotatif en pin 2 
{
	disable_btns();
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
	enable_btns();
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
	
}

ISR(INT2_vect) // bouton poussoir
{
	disable_btns();
	
	val_res_wtd_tab[x_tab] = chiffre;
	compute_value();
	affichage_line1("Valeur souhaite : ");
	affichage_long(val_res_wtd);
	_delay_ms(2000);
	CHOIX_RES = TRUE;
}



