/******************************************************************************
 *
 * Module: Timer1
 *
 * File Name: timer1.c
 *
 * Description: Source file for Timer1 driver
 *
 * Author: Aly Tarek
 *
 *******************************************************************************/

#include "timer1.h"
#include "avr/io.h"
#include <avr/interrupt.h>


/*******************************************************************************
 *                 				Global Variables                               *
 *******************************************************************************/

static volatile void (*g_callBackPtr)(void) = NULL_PTR;


/*******************************************************************************
 *                    				  ISR                                 *
 *******************************************************************************/
ISR (TIMER1_COMPA_vect){
	if(g_callBackPtr!= NULL_PTR){
			(*g_callBackPtr)();
		}
}


ISR (TIMER1_OVF_vect){
	if(g_callBackPtr!= NULL_PTR){
		(*g_callBackPtr)();
	}
}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

void Timer1_init(const Timer1_ConfigType * Config_Ptr){

	TCCR1A = (1<<FOC1A);
	TCNT1= Config_Ptr->normal_initialvalue;
	OCR1A= Config_Ptr->CTC_initialvalue;
	/*
	 * Clearing Both WGM01,WGM11 as they will remain 0 in both CTC and Normal modes
	 */
	CLEAR_BIT(TCCR1A,WGM01);
	CLEAR_BIT(TCCR1A,WGM11);
	/*
	 * Inserting Both MODE and PRESCALER in TCCR1B
	 */

	TCCR1B = (TCCR1B & 0xE7) | (((Config_Ptr->mode)<<1) & 0x18);
	TCCR1B = (TCCR1B & 0xF8) | ((Config_Ptr->prescaler) & 0x07);

	TIMSK = (1<<OCIE1A);

}

void Timer1_deInit(void){
	/*
	 * To DeInitialize the timer CLEAR TCCR1B Register(Stop the timer)
	 */
	TCCR1B=0;
}


void Timer1_setCallBack(void(*a_ptr)(void)){
	g_callBackPtr= a_ptr;
}
