/*
 * main.h
 *
 * Created: 29-09-21 10:13:33
 *  Author: Tom
 */ 


#ifndef MAIN_H_
#define MAIN_H_


// Gestion registre par bit unique
#define SET_BIT(port,bit)  (port |= (1<<bit))   //set bit in port
#define CLR_BIT(port,bit)  (port &= ~(1<<bit))  //clear bit in port
#define TOGGLE_IO(port,bit) (port ^= (1<<bit))   //toggle


#define TRUE				1
#define FALSE				0


#endif /* MAIN_H_ */
