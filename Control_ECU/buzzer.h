 /******************************************************************************
 *
 * Module: Buzzer
 *
 * File Name: buzzer.h
 *
 * Description: Header file for the Buzzer driver
 *
 * Author: Aly Tarek
 *
 *******************************************************************************/

#ifndef BUZZER_H_
#define BUZZER_H_

#include "gpio.h"
#include "std_types.h"
#include "common_macros.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define BUZZER_PORT_ID PORTD_ID
#define BUZZER_PIN_ID PIN7_ID

/*******************************************************************************
 *                                Prototypes                                 *
 *******************************************************************************/
/*
 * Function to Initialize and Configure buzzer through GPIO
 */
void Buzzer_init();

/*
 * Function to Turn on Buzzer through GPIO
 */
void Buzzer_on();

/*
 * Function to Turn Off Buzzer through GPIO
 */
void Buzzer_off();

#endif /* BUZZER_H_ */
