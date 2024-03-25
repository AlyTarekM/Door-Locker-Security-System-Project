/******************************************************************************
 *
 * Module: MainControl
 *
 * File Name: mainCTRL.c
 *
 * Description: Source file for the Control ECU main
 *
 * Author: Aly Tarek
 *
 *******************************************************************************/
#include<avr/io.h>
#include"timer1.h"
#include"dcMotor.h"
#include"uart.h"
#include"std_types.h"
#include"common_macros.h"
#include"gpio.h"
#include <util/delay.h>
#include"buzzer.h"
#include"twi.h"
#define READY_SEND 0x10
#define READY_REC 0x11
#define MATCH 1
#define NOMATCH 0
uint8 i;
uint8 pass1[5] = { 0 };
uint8 pass2[5] = { 0 };
uint8 DoorPass[5] = { 0 };
uint8 EEPROM_Saved[5] = { 0 };
uint8 ChangePass[5] = { 0 };
uint8 g_state;
uint8 g_WrongCount = 0;
uint8 tick = 0;
uint8 buzzer_ticks = 0;
uint8 PasswordCompare(uint8 *pass3, uint8 *pass4) {
	uint8 compareCount = 0;

	for (i = 0; i < 5; i++) {
		if (pass3[i] == pass4[i]) {
			compareCount++;

		} else {
			compareCount = 0;

			return NOMATCH;
		}
	}
	if (compareCount == 5) {
		return MATCH;
	}
	compareCount = 0;

}
void ReadFromEEPROM(uint8 *pass) {
	i = 0;
	while (i < 5) {
		while (!EEPROM_readByte(0x0311 + i, pass + i))
			;
		i++;
		_delay_ms(10);
	}
}

void RecievePassFrom_HMI(uint8 *pass) {

	while (UART_recieveByte() != READY_SEND) {
	}
	UART_sendByte(READY_REC);
	GPIO_setupPinDirection(PORTA_ID, PIN2_ID, PIN_OUTPUT);
	GPIO_writePin(PORTA_ID, PIN2_ID, LOGIC_HIGH);
	for (i = 0; i < 5; i++) {
		pass[i] = UART_recieveByte();

		//_delay_ms(50);
	}
}
void WritePasswordEEPROM(uint8 *pass) {
	/*i = 0;
	 while (i < 5) {
	 while (!EEPROM_writeByte(0x0311 + i, pass + i)){}
	 i++;
	 EEPROM_Saved[i]=pass[i];
	 _delay_ms(10);
	 }*/
	for (i = 0; i < 5; i++) {
		EEPROM_writeByte(0x0311, (pass + i));
		EEPROM_Saved[i] = pass[i];
		_delay_ms(10);
	}
}

void OpenDoorTime(void) {
	tick++;
	if (tick == 5) {
		DcMotor_Rotate(Stop, 0);

	} else if (tick == 6) {
		DcMotor_Rotate(A_CW, 100);

	} else if (tick == 11) {
		DcMotor_Rotate(Stop, 0);
		tick = 0;
		Timer1_deInit();
	}
}

void Step1Function(void) {
	uint8 state;
//	GPIO_setupPinDirection(PORTA_ID, PIN2_ID, PIN_OUTPUT);
//			GPIO_writePin(PORTA_ID, PIN2_ID, LOGIC_HIGH);
	RecievePassFrom_HMI(pass1);
	GPIO_setupPinDirection(PORTA_ID, PIN2_ID, PIN_OUTPUT);
	GPIO_writePin(PORTA_ID, PIN2_ID, LOGIC_LOW);
	RecievePassFrom_HMI(pass2);

	state = PasswordCompare(pass1, pass2);
	if (state == MATCH) {
		WritePasswordEEPROM(pass1);
	}
	UART_sendByte(READY_SEND);
	while (UART_recieveByte() != READY_REC) {
	}
	UART_sendByte(state);
}
void buzzer_processing(void) {
	buzzer_ticks++;
	if (buzzer_ticks == 20) {
		Buzzer_off();
		g_WrongCount = 0;
		Timer1_deInit();
		buzzer_ticks = 0;
	}
}
int main(void) {
	SREG |= (1 << 7);
	/*
	 * Configuring the Frame of the UART as the same frame of the HMI UART
	 * then sending the structure to the UART init fn
	 */
	UART_ConfigType Config_UART = { EIGHT_BITS, EVEN_PARITY, ONE_BIT, 9600 };
	TWI_ConfigType Config_TWI = { 0X05, 2 };
	UART_init(&Config_UART);
	DcMotor_Init();
	TWI_init(&Config_TWI);
	Buzzer_init();

	uint8 DoorPassState;
	uint8 changepassState;
	while (1) {

		while (UART_recieveByte() != READY_SEND) {
		}
		UART_sendByte(READY_REC);

		g_state = UART_recieveByte();

		switch (g_state) {
		case 'a':

			Step1Function();

			g_state = 0;
			break;
		case 'b':
			RecievePassFrom_HMI(DoorPass);
			//ReadFromEEPROM(EEPROM_Saved);
			DoorPassState = PasswordCompare(EEPROM_Saved, DoorPass);
			UART_sendByte(READY_SEND);
			while (UART_recieveByte() != READY_REC) {
			}
			UART_sendByte(DoorPassState);
			if (DoorPassState == 1) {
				DcMotor_Rotate(CW, 100);
				Timer1_ConfigType Config_Timer;
				Config_Timer.CTC_initialvalue = 23437;
				Config_Timer.mode = CTC;
				Config_Timer.normal_initialvalue = 0;
				Config_Timer.prescaler = FCPU_1024;
				Timer1_init(&Config_Timer);
				Timer1_setCallBack(OpenDoorTime);
				g_WrongCount = 0;
			} else {
				g_WrongCount++;
			}
			if (g_WrongCount == 3) {
				Buzzer_on();
				Timer1_setCallBack(buzzer_processing);
				Timer1_ConfigType Config_Timer;
				Config_Timer.normal_initialvalue = 0;
				Config_Timer.CTC_initialvalue = 23437;
				Config_Timer.prescaler = FCPU_1024;
				Config_Timer.mode = CTC;
				Timer1_init(&Config_Timer);
				g_WrongCount = 0;
			}
				g_state = 0;
				break;
				case 'c':
				RecievePassFrom_HMI(ChangePass);
				//ReadFromEEPROM(EEPROM_Saved);
				changepassState = PasswordCompare(EEPROM_Saved, ChangePass);
				UART_sendByte(READY_SEND);
				while (UART_recieveByte() != READY_REC) {
				}
				UART_sendByte(changepassState);
				if (changepassState == 0) {
					g_WrongCount++;
				} else {
					g_WrongCount = 0;
				}
				if (g_WrongCount == 3) {

					Buzzer_on();
					Timer1_setCallBack(buzzer_processing);
					Timer1_ConfigType Config_Timer;
					Config_Timer.normal_initialvalue = 0;
					Config_Timer.CTC_initialvalue = 23437;
					Config_Timer.prescaler = FCPU_1024;
					Config_Timer.mode = CTC;
					Timer1_init(&Config_Timer);
					g_WrongCount = 0;

				}

				g_state = 0;
				break;
			}
		}

		return 0;
	}

