/*
 * OS.h
 *
 * Created: 29-09-21 09:58:32
 *  Author: Tom
 */ 


#ifndef OS_H_
#define OS_H_

// DEFINE
#define HIGH	1 
#define LOW		0
#define Time_TB	1300
#define TIMEOUT_CONV 8000
#define TRYOUT_TV 2
#define F_CPU 1000000UL
#define steps 36
#define pulseWidthMicros 5000  
#define microsBtwnSteps 5000
#define duty_cycle_mot_dc 100 
#define duty_cycle_leds 70 

extern int CHOIX_RES;

// PROTOTYPES
void Loop_OS(void);
void state_machine(void);
void recovery(void);
void affichage_line1(char *s);
void affichage_line2(char *s);
void table_vibrante_ON(void);
void table_vibrante_OFF(void);
void desactiver_table_vibrante(void);
void affichage_chiffre_lcd(void);
void affichage_long(long val);
void convoyeur(void);
void servomoteur(void);
void disable_btns(void);
void enable_btns(void);
void compute_value(void);
void reset_tab(void);
void reset_buf(void);
void reset_data(void);
unsigned long unmask_data(volatile uint8_t data[]);
#endif /* OS_H_ */