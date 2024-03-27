/******************************************************************************
 *
 * Module: MainHMI
 *
 * File Name: mainHMI.c
 *
 * Description: Source file for the HMI main
 *
 * Author: Aly Tarek
 *
 *******************************************************************************/

#include"lcd.h"
#include"gpio.h"
#include <avr/io.h>
#include"timer1.h"
#include"uart.h"
#include"keypad.h"
#include "std_types.h"
#include <util/delay.h>
#define READY_SEND 0x10
#define READY_REC 0x11
uint8 Step1_Flag = 0;
uint8 ErrorCounter = 0;
uint8 pass1[5];
uint8 pass2[5];
uint8 DoorPass[5];
uint8 i;
uint8 timerDoneFlag = 0;
uint8 ErrorDone_Flag = 0;
uint8 g_Pass1_Flag = 0; //Flag to Illustrate that password1 has been entered
uint8 g_state = 0;
uint8 PassMatch = 0;
uint8 tick = 0;
void Set_Password1(void) {
	uint8 key;
	for (i = 0; i <= 5; i++) {
		key = KEYPAD_getPressedKey();
		if (key != 13) {
			pass1[i] = key;
			LCD_displayCharacter('*');
		} else {
			break;
		}
		_delay_ms(500);
	}
}
void Set_DoorPassword(void) {
	uint8 key;
	for (i = 0; i <= 5; i++) {
		key = KEYPAD_getPressedKey();
		if (key != 13) {
			DoorPass[i] = key;
			LCD_displayCharacter('*');
		} else {
			break;
		}
		_delay_ms(500);
	}
}
void Set_Password2(void) {
	uint8 key;
	LCD_clearScreen();
	LCD_displayString("plz re-enter the");
	LCD_moveCursor(1, 0);
	LCD_displayString("same pass:");
	for (i = 0; i <= 5; i++) {
		key = KEYPAD_getPressedKey();
		if (key != 13) {
			pass2[i] = key;
			LCD_displayCharacter('*');
		} else {
			g_Pass1_Flag = 0;
			break;
		}
		_delay_ms(500);
	}
}
void SendPassToCTRL_ECU(uint8 *pass) {
	UART_sendByte(READY_SEND);
	while (UART_recieveByte() != READY_REC) {
	}
	for (i = 0; i < 5; i++) {
		UART_sendByte(pass[i]);
	}
}
/*
 * Function that implements step 1(creating password and sending it to CTRL ECU
 */
uint8 Step1Function(void) {
	Step1_Flag = 1;
	uint8 PasswordState;
	g_state = 'a';
	UART_sendByte(READY_SEND);
	while (UART_recieveByte() != READY_REC) {
	}
	UART_sendByte(g_state);
	LCD_clearScreen();
	LCD_displayString("plz enter pass:");
	LCD_moveCursor(1, 0);
	Set_Password1();
//	UART_sendByte(READY_SEND);
//	while (UART_recieveByte() != READY_REC) {}
	SendPassToCTRL_ECU(pass1);

	Set_Password2();

//	UART_sendByte(READY_SEND);
//
//	while (UART_recieveByte() != READY_REC) {}
//	LCD_clearScreen();
//										_delay_ms(500);
	SendPassToCTRL_ECU(pass2);

	while (UART_recieveByte() != READY_SEND) {
	}
	UART_sendByte(READY_REC);
//LCD_clearScreen();
	PasswordState = UART_recieveByte();
	return PasswordState;

}
void Step2Function_MainOP(void) {
	LCD_clearScreen();
	LCD_displayString("+ : Open Door");
	LCD_moveCursor(1, 0);
	LCD_displayString("- : Change Password");
	uint8 key;
	key = KEYPAD_getPressedKey();
	switch (key) {
	case '+':
		OpenDoorPassword();
		break;
	case '-':
		ChangePassword();
		break;
	}

}
void LCD_Error(void) {
	static uint8 one_min = 0;
	one_min++;
	if (one_min == 20) {
		GPIO_setupPinDirection(PORTD_ID, PIN5_ID, PIN_OUTPUT);
		GPIO_writePin(PORTD_ID, PIN5_ID, LOGIC_HIGH);
		Timer1_deInit();
		one_min = 0;
		ErrorDone_Flag = 0;
	}
}
void ChangePassword(void) {
	uint8 passState;
	uint8 flag = 0;
	g_state = 'c';
	UART_sendByte(READY_SEND);
	while (UART_recieveByte() != READY_REC) {}
	UART_sendByte(g_state);
	LCD_clearScreen();
	LCD_displayString("plz enter pass:");
	LCD_moveCursor(1, 0);
	Set_Password1();
	SendPassToCTRL_ECU(pass1);
	while (UART_recieveByte() != READY_SEND) {}
	UART_sendByte(READY_REC);
	passState = UART_recieveByte();
	while (ErrorCounter < 2) {
		if (passState == 1) {
			LCD_clearScreen();
			LCD_displayString("Password Match!");
			ErrorCounter = 0;
			_delay_ms(200);
			LCD_clearScreen();
			Step1Function();
			break;
		} else {
			LCD_clearScreen();
			LCD_displayString("No Match!");
			_delay_ms(500);
			ErrorCounter++;
			LCD_clearScreen();
			LCD_displayString("pls enter pass:");
			LCD_moveCursor(1, 0);
			g_state = 'c';
			UART_sendByte(READY_SEND);
			while (UART_recieveByte() != READY_REC) {}
			UART_sendByte(g_state);
			Set_Password1();
			SendPassToCTRL_ECU(pass1);
			while (UART_recieveByte() != READY_SEND) {}
			UART_sendByte(READY_REC);
			passState = UART_recieveByte();

		}
	}
	if ((passState == 1) && (ErrorCounter == 2)) {
		LCD_clearScreen();
		LCD_displayString("Password Match!");
		ErrorCounter = 0;
		_delay_ms(200);
		LCD_clearScreen();
		Step1Function();
	}
	if ((ErrorCounter == 2) && (passState == 0)) {
		ErrorDone_Flag = 1;
		LCD_clearScreen();
		LCD_displayString("LockDown!");
		Timer1_ConfigType Config_Timer;
		Config_Timer.normal_initialvalue = 0;
		Config_Timer.CTC_initialvalue = 2930;
		Config_Timer.prescaler = FCPU_1024;
		Config_Timer.mode = CTC;
		Timer1_init(&Config_Timer);
		Timer1_setCallBack(LCD_Error);
		ErrorCounter = 0;
	}

}
void OpenDoorTime(void) {
	tick++;
	if (tick == 5) {
		LCD_clearScreen();
		LCD_displayString("Door Is Unlocked!");

	} else if (tick == 6) {
		LCD_clearScreen();
		LCD_displayString("Door Is Locking!");

	} else if (tick == 11) {
		LCD_clearScreen();
		LCD_displayString("Door Is Locked!");
		tick = 0;
		Timer1_deInit();
		timerDoneFlag = 0;
	}
}
void OpenDoorPassword(void) {
	g_state = 'b';
	UART_sendByte(READY_SEND);
	while (UART_recieveByte() != READY_REC) {
	}
	UART_sendByte(g_state);
	LCD_clearScreen();
	LCD_displayString("plz enter pass:");
	LCD_moveCursor(1, 0);
	Set_DoorPassword();
	SendPassToCTRL_ECU(DoorPass);
	while (UART_recieveByte() != READY_SEND) {
	}
	UART_sendByte(READY_REC);
	PassMatch = UART_recieveByte();
	if (PassMatch == 1) {
		ErrorCounter = 0;
		LCD_clearScreen();
		timerDoneFlag = 1;
		LCD_displayString("Door Is");
		LCD_moveCursor(1, 0);
		LCD_displayString("Unlocking!");
		Timer1_ConfigType Config_Timer;
		Config_Timer.CTC_initialvalue = 2930;
		Config_Timer.mode = CTC;
		Config_Timer.normal_initialvalue = 0;
		Config_Timer.prescaler = FCPU_1024;
		Timer1_init(&Config_Timer);
		Timer1_setCallBack(OpenDoorTime);

	} else {
		while (ErrorCounter < 2) {
			if (PassMatch == 1) {
				ErrorCounter = 0;
				LCD_clearScreen();
				timerDoneFlag = 1;
				LCD_displayString("Door Is");
				LCD_moveCursor(1, 0);
				LCD_displayString("Unlocking!");
				Timer1_ConfigType Config_Timer;
				Config_Timer.CTC_initialvalue = 2930;
				Config_Timer.mode = CTC;
				Config_Timer.normal_initialvalue = 0;
				Config_Timer.prescaler = FCPU_1024;
				Timer1_init(&Config_Timer);
				Timer1_setCallBack(OpenDoorTime);
				break;
			} else {
				LCD_clearScreen();
				LCD_displayString("No Match!");
				_delay_ms(500);
				ErrorCounter++;
				LCD_clearScreen();
				LCD_displayString("pls enter pass:");
				LCD_moveCursor(1, 0);
				g_state = 'b';
				UART_sendByte(READY_SEND);
				while (UART_recieveByte() != READY_REC) {
				}
				UART_sendByte(g_state);
				Set_DoorPassword();
				SendPassToCTRL_ECU(DoorPass);
				while (UART_recieveByte() != READY_SEND) {
				}
				UART_sendByte(READY_REC);
				PassMatch = UART_recieveByte();

			}
		}
	}
	if (ErrorCounter == 2 && PassMatch == 1) {
		ErrorCounter = 0;
		LCD_clearScreen();
		timerDoneFlag = 1;
		LCD_displayString("Door Is");
		LCD_moveCursor(1, 0);
		LCD_displayString("Unlocking!");
		Timer1_ConfigType Config_Timer;
		Config_Timer.CTC_initialvalue = 2930;
		Config_Timer.mode = CTC;
		Config_Timer.normal_initialvalue = 0;
		Config_Timer.prescaler = FCPU_1024;
		Timer1_init(&Config_Timer);
		Timer1_setCallBack(OpenDoorTime);
	}
	if (ErrorCounter == 2 && PassMatch == 0) {
		ErrorDone_Flag = 1;
		LCD_clearScreen();
		LCD_displayString("LockDown!");
		Timer1_ConfigType Config_Timer;
		Config_Timer.normal_initialvalue = 0;
		Config_Timer.CTC_initialvalue = 2930;
		Config_Timer.prescaler = FCPU_1024;
		Config_Timer.mode = CTC;
		Timer1_init(&Config_Timer);
		Timer1_setCallBack(LCD_Error);
		ErrorCounter = 0;
	}

}

int main(void) {
	SREG |= (1 << 7);
	LCD_init();
	uint8 stateResult;
	UART_ConfigType Config_UART = { EIGHT_BITS, EVEN_PARITY, ONE_BIT, 9600 };
	UART_init(&Config_UART);
	while (1) {
		if (Step1_Flag == 0) {
			stateResult = Step1Function();
		}
		if (stateResult == 1) {
			if (timerDoneFlag == 0 && ErrorDone_Flag == 0) {
				Step2Function_MainOP();
			}
		} else {
			stateResult=Step1Function();
		}

	}

	return 0;
}
