 /*
 * TIMERS.h
 *
 * Created: 9/29/2021 9:57:04 AM
 *  Author: Tom
 */
#ifndef _TIMERS_H_
#define _TIMERS_H_
#define F_CPU 1000000UL
#define DROITE 450
#define CENTRE 1400
#define GAUCHE 2400



//DEFINE


//PROTOTYPE FONCTIONS EXTERNES
void TIMER0_Init_COUNTER_1ms(void);
void TIMER1_init_FASTPW_ICRn(void);
void TIMER2_init_FASTPWM(void);





#endif /* _TIMERS_H */
