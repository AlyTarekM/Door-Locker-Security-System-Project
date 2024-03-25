/******************************************************************************
 *
 * Module: Timer1
 *
 * File Name: timer1.h
 *
 * Description: Header file for Timer1 driver
 *
 * Author: Aly Tarek
 *
 *******************************************************************************/

#ifndef TIMER1_H_
#define TIMER1_H_

#include "std_types.h"
#include "common_macros.h"

/*******************************************************************************
 *                                 Types                                   *
 *******************************************************************************/
typedef enum{
	NORMAL_MODE,CTC=4
}Timer1_Mode;

typedef enum{
	NO_CLOCK,FCPU,FCPU_8,FCPU_64,FCPU_256,FCPU_1024,EXTERNAL_FALLING,EXTERNAL_RISING
}Timer1_Prescaler;

typedef struct{
	Timer1_Mode mode;
	Timer1_Prescaler prescaler;
	uint16 normal_initialvalue;
	uint16 CTC_initialvalue;
}Timer1_ConfigType;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*
 * Description:
 * Function to initialize timer1 Driver
 */
void Timer1_init(const Timer1_ConfigType * Config_Ptr);

/*
 * Description:
 * Function to Disable Timer1
 */
void Timer1_deInit(void);

/*
 * Description:
 * Function to set the Call Back function address.
 */
void Timer1_setCallBack(void(*a_ptr)(void));


#endif /* TIMER1_H_ */
