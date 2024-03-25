 /******************************************************************************
 *
 * Module: DC_Motor
 *
 * File Name: dcMotor.c
 *
 * Description: Source file for the DC Motor driver
 *
 * Author: Aly Tarek
 *
 *******************************************************************************/


#include "dcMotor.h"
#include"PWM.h"

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

void DcMotor_Init(void){
	/*
	 * setup the direction for the two motor pins through the GPIO driver
	 */
	GPIO_setupPinDirection(DC_MOTOR_PORT_ID,DC_MOTOR_PIN1_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(DC_MOTOR_PORT_ID,DC_MOTOR_PIN2_ID,PIN_OUTPUT);

	/*
	 * Stop at the DC-Motor at the beginning through the GPIO driver
	 */
	GPIO_writePin(DC_MOTOR_PORT_ID,DC_MOTOR_PIN1_ID,LOGIC_LOW);
	GPIO_writePin(DC_MOTOR_PORT_ID,DC_MOTOR_PIN2_ID,LOGIC_LOW);
}

void DcMotor_Rotate(DcMotor_State state,uint8 speed){
	if(state==Stop){
		GPIO_writePin(DC_MOTOR_PORT_ID,DC_MOTOR_PIN1_ID,LOGIC_LOW);
		GPIO_writePin(DC_MOTOR_PORT_ID,DC_MOTOR_PIN2_ID,LOGIC_LOW);
	}
	else if(state== CW){
		GPIO_writePin(DC_MOTOR_PORT_ID,DC_MOTOR_PIN1_ID,LOGIC_LOW);
		GPIO_writePin(DC_MOTOR_PORT_ID,DC_MOTOR_PIN2_ID,LOGIC_HIGH);
	}
	else if(state== A_CW){
		GPIO_writePin(DC_MOTOR_PORT_ID,DC_MOTOR_PIN1_ID,LOGIC_HIGH);
		GPIO_writePin(DC_MOTOR_PORT_ID,DC_MOTOR_PIN2_ID,LOGIC_LOW);
	}

	TIMER0_INIT(speed);
}
