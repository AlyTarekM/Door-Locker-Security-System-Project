 /******************************************************************************
 *
 * Module: PWM
 *
 * File Name: PWM.h
 *
 * Description: Header file for the PWM driver
 *
 * Author: Aly Tarek
 *
 *******************************************************************************/

#ifndef PWM_H_
#define PWM_H_

#include "std_types.h"

/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/


/*
 * Description:
 * Function used to Initialize TIMER0 and Set it's Duty Cycle
 */

void TIMER0_INIT(uint8 set_duty_cycle);

#endif /* PWM_H_ */

