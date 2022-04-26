/*
 * PWM.h
 *
 * Created: 20-10-21 12:20:53
 *  Author: Tom
 */ 


#ifndef PWM_H_
#define PWM_H_

#define F_CPU 1000000UL

void PWM_SERVO(int match_value);
void PWM_MOTEUR_DC(int duty_cycle);
void PWM_LEDs(int duty_cycle);


#endif /* PWM_H_ */