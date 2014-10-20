/*
* Lab8
*
* Created: 10/8/2014 4:05:59 PM
*  Author: Greg Lepley and Billy Neuson
*
* Program is designed to activate a MOSFET that powers a solenoid
* for a maximum of 15 seconds when the button is pressed. If the button is pressed
* during the period of time that the solenoid is active, the process is ended and
* the solenoid is turned off.
*/


#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <string.h>

#include "graphics.h"
#include "DS1307.h"

#define F_CPU 16000000UL
#define F_SCL 100000UL
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1
#define ALL_OUTPUT	0xFF // 1111 1111

// define the different cases of state machine
#define S0 0
#define S1 1
#define S2 2
#define S3 3
#define S4 4


void soundAlarm(void);
void setAlarm(void);
void writeTheTime(void);
void writeAlarmToggle(void);
void writeAlarmSet(void);
void timer0Init();
void delayMS(uint16_t msec);
void delay_ms_alarm(uint16_t msec);

void pwm_PB2(unsigned Ton, unsigned period);
uint8_t DebounceSwitch1();
uint8_t DebounceSwitch1Short();
uint8_t DebounceSwitch2();
uint8_t DebounceSwitch2Short();

volatile time_t timeGet; //variable to hold current time
volatile uint8_t press; //signal to hold button press status
volatile uint8_t menu; //signal to hold menu button press status
volatile uint8_t presslong; //signal to hold button press status
volatile uint8_t menulong; //signal to hold menu button press status
volatile time_t timeAlarm; //variable to hold alarm time
volatile int alarmOn; //variable to hold alarm status

int main(void)
{
	timer0Init(); //initialize timer0
	
	initI2c(); // initialize I2C connection
	
	// Set up SPI connection
	SPI_MasterInit();
	spi_init();
	
	// Initialize LCD and clear
	LCD_init();
	delay_ms(100);
	LCD_clear();
	
	menu = 0; //clears the menu button press
	press = 0; //clears the select button press variable
	menulong = 0; //clears the menu button press
	presslong = 0; //clears the select button press variable
	
	alarmOn = 0; //initally alarm is off
	
	DDRD &= ~_BV(2); 	//set pin 2 as input
	DDRD &= ~_BV(3);	//set pin 3 as input
	PORTD |= _BV(2) | _BV(3); 	//activate pullup resistor on pins 2 and 3

	setAlarm();		//set the alarm to the current time

	
	int state = S0; // set S0 as the initial state
	int i = 0;
	
	
	while(1)
	{	
		//state machine for operation
		switch (state) 
		{
			//normal clock display mode. shows indicator for if alarm on or off
			case S0:
				
				menu = 0; //clears the menu button press
				press = 0; //clears the select button press variable
				menulong = 0; //clears the menu button press
				presslong = 0; //clears the select button press variable
				
				i++;
				if(i == 10)
				{
					writeTheTime(); //display the current time on display
					i = 0;
				}
				
				//if alarm time matches current time, activate alarm
				if(timeGet.hour == timeAlarm.hour && 
				   timeGet.minute == timeAlarm.minute && 
				   timeGet.second == timeAlarm.second && 
				   alarmOn == 1)
				{
					soundAlarm(); // activate alarm
				}
				delayMS(5);
				menu = DebounceSwitch1Short();
				press = DebounceSwitch2Short();
				
				//if user presses button 1, advance to S1
				if(menu == 1) {state = S1;}
				break;
				
			//displays the alarm time and alarm status. pressing button 2 allows
			//the user to toggle the alarm's status as on or off	
			case S1:
				menu = 0; //clears the menu button press
				press = 0; //clears the select button press variable
				menulong = 0; //clears the menu button press
				presslong = 0; //clears the select button press variable
				
				//display the interface for toggling the alarm
				i++;
				if(i == 10)
				{
					writeAlarmToggle();
					i = 0;
				}
				
				
				delayMS(5);
				
				//check 5 times to see if both buttons are being pressed
				//causes program to return to S0 if true
				menulong = DebounceSwitch1();
				presslong = DebounceSwitch2();
				if(menulong == 1 && presslong == 1) {state = S0;}
				menulong = DebounceSwitch1();
				presslong = DebounceSwitch2();
				if(menulong == 1 && presslong == 1) {state = S0;}
				menulong = DebounceSwitch1();
				presslong = DebounceSwitch2();
				if(menulong == 1 && presslong == 1) {state = S0;}
				menulong = DebounceSwitch1();
				presslong = DebounceSwitch2();
				if(menulong == 1 && presslong == 1) {state = S0;}
				menulong = DebounceSwitch1();
				presslong = DebounceSwitch2();
				if(menulong == 1 && presslong == 1) {state = S0;}
					
				menu = DebounceSwitch1Short();
				press = DebounceSwitch2Short();
				
				//if user presses only button 1, advances to S2
				if(menu	== 1) {state = S2;}
					
				//if user presses only button 2, toggles the status of alarm
				if(press == 1 && menu == 0 && menulong == 0)
				{
					if(alarmOn == 0)
					{
						alarmOn = 1;
					}
					else
					{
						alarmOn = 0;
					}
				}
				break;
				
			//displays the current alarm time and allows the user to set the hour and
			//minute values of the alarm
			case S2:
				menu = 0; //clears the menu button press
				press = 0; //clears the select button press variable
				menulong = 0; //clears the menu button press
				presslong = 0; //clears the select button press variable
				
				//display the interface for setting the alarm time
				i++;
				if(i == 10)
				{
					writeAlarmSet();
					i = 0;
				}
				
				
				delayMS(5);
				
				//check 5 times to see if both buttons are being pressed
				//causes program to return to S0 if true
				menulong = DebounceSwitch1();
				presslong = DebounceSwitch2();
				if(menulong == 1 && presslong == 1) {state = S0;}
				menulong = DebounceSwitch1();
				presslong = DebounceSwitch2();
				if(menulong == 1 && presslong == 1) {state = S0;}
				menulong = DebounceSwitch1();
				presslong = DebounceSwitch2();
				if(menulong == 1 && presslong == 1) {state = S0;}
				menulong = DebounceSwitch1();
				presslong = DebounceSwitch2();
				if(menulong == 1 && presslong == 1) {state = S0;}
				menulong = DebounceSwitch1();
				presslong = DebounceSwitch2();
				if(menulong == 1 && presslong == 1) {state = S0;}
					
				menu = DebounceSwitch1Short();
				press = DebounceSwitch2Short();
				
				//if user presses only button 2, advance to S3 to increment minutes
				if(press == 1 && menu == 0 && menulong == 0) {state = S3;}
					
				//if user presses only button 1, advance to S4 to increment hours
				if(menu == 1 && press == 0 && presslong == 0) {state = S4;}
				break;
				
			//increments the minutes value of the alarm time. rolls over
			//if necessary
			case S3:
				// rolls over minutes if 60 minutes is reached
				if(timeAlarm.minute == 59)
				{
					timeAlarm.minute = 0;
				}
				//increments the seconds by 1
				else
				{
					timeAlarm.minute++;
				}
				//returns to S2
				state = S2;
				break;
				
			//increments the hours value of the alarm time. rolls over
			//if necessary
			case S4:
				//rolls over hours if 24 hours is reached
				if(timeAlarm.hour == 23)
				{
					timeAlarm.hour = 0;
				}
				//increments the seconds by 1
				else
				{
					timeAlarm.hour++;
				}
				//returns to S2
				state = S2;
				break;
		}
		
	}
}



//sounds the alarm duty cycle
void soundAlarm()
{
	int j = 0;
	pwm_PB2(67, 672); //set duty cycle to 10%, 23Hz signal
	for(int i = 0 ; i < 625 ; i++)
	{
		writeTheTime();
		press = DebounceSwitch2();
		delayMS(5);
		j++;
		if(j == 10)
		{
			writeTheTime(); //display the current time on display
			j = 0;
		}
		//calls function that delays 
		//for 15 seconds, but ends delay if
		//pushbutton is pressed again
		//if push button pressed, end for loop
		if(press == 1) i=3000;	
	}
	pwm_PB2(0, 672);//set duty cycle to 0%
}

// sets the alarm time seconds 30sec ahead of current seconds
void setAlarm(void)
{
	//Get time from RTC
	timeGet = getTime();
	timeAlarm = getTime();
	timeAlarm.second = 0;
}

// Obtains the time and date from the RTC, converts the data to strings,
// and displays the strings on the LCD screen
void writeTheTime(void)
{
	//get the current time and date from the RTC
	timeGet = getTime();
	//convert integer values into time string
	char time[15];
	sprintf(time, "%02d:%02d", timeGet.hour, timeGet.minute);
	//convert integer values into date string
	char date[15];
	sprintf(date, "%02d/%02d/%02d", timeGet.month, timeGet.date, timeGet.year);
	//convert integer value into seconds string
	char theSeconds[15];
	sprintf(theSeconds, "(%02d)", timeGet.second);
	//convert integer value into alarm seconds string
	char alarm[15];
	sprintf(alarm, "%02d", timeAlarm.second);
	
	LCD_clear(); //clear the LCD
	LCD_gotoXY(0,0); //position the cursor for alarm seconds entry
	if(alarmOn == 1)
	{
		LCD_writeString_F("Alarm: ON");
	}
	else
	{
		LCD_writeString_F("Alarm: OFF");
	}
	
	LCD_gotoXY(15,1); //position the cursor for the date text
	LCD_writeString_F(date);//print the date on LCD
	LCD_writeString_megaFont(time); //print the Hours and Minutes in large font
	LCD_gotoXY(30,5);//position the cursor for the seconds entry
	LCD_writeString_F(theSeconds); //print the seconds to the screen
}

//Generates the GUI for toggling the alarm on and off
void writeAlarmToggle(void)
{
	LCD_clear();
	char alarm[15];
	sprintf(alarm, "%02d:%02d", timeAlarm.hour, timeAlarm.minute);
	
	LCD_gotoXY(0,0); //position the cursor for alarm status
	//prints the alarm status on the display
	if(alarmOn == 1)
	{
		LCD_writeString_F("Alarm: ON");
	}
	else
	{
		LCD_writeString_F("Alarm: OFF");
	}
	LCD_writeString_megaFont(alarm); //print the Hours and Minutes of alarm in large font
}

//Generates the GUI for setting the alarm time
void writeAlarmSet(void)
{
	LCD_clear();
	char alarm[15];
	sprintf(alarm, "%02d:%02d", timeAlarm.hour, timeAlarm.minute);
	
	LCD_gotoXY(13,0); //position the cursor for set alarm header
	LCD_writeString_F("Set Alarm");
	LCD_writeString_megaFont(alarm); //print the Hours and Minutes of alarm in large font
}

void pwm_PB2(unsigned Ton, unsigned period)

{
	
	DDRB |= _BV(2); //set PB2 as output
	
	OCR1A = period >> 1; //set as half of the period
	
	OCR1B = Ton >> 1; //set as half of the time on
	
	TCCR1A = _BV(COM1B1) | _BV(WGM11) | _BV(WGM10); //allow timer1 to control PB2, mode 11
	
	TCCR1B = _BV(WGM13) | _BV(CS10) | _BV(CS12); //1:1024 pre-scaler, timer mode 11

}

//Initializes the timer
void timer0Init()
{
	//Set timer 0 prescaler to 1024
	TCCR0B = _BV(CS02) | _BV(CS00);
}

//Debounce button 1 for a press and hold
uint8_t DebounceSwitch1()
{
	static uint16_t State = 0; 		// Current debounce status
	// read switch, upper 5 bits of State are don't cares
	State=(State<<1) | (PIND & (1<<PD3))>>3 | 0xfc00;
	if(State==0xfc00)return 1; 		// indicates 0 level is
	// stable for 10 consecutive calls
	return 0;
}

//Debounce button 2 for a press and hold
uint8_t DebounceSwitch2()
{
	static uint16_t State = 0; 		// Current debounce status
	// read switch, upper 5 bits of State are don't cares
	State=(State<<1) | (PIND & (1<<PD2))>>2 | 0xf800;
	if(State==0xf800)return 1; 		// indicates 0 level is
	// stable for 10 consecutive calls
	return 0;
}

//Debounce button 1 for a quick press
uint8_t DebounceSwitch1Short()
{
	static uint16_t State = 0; 		// Current debounce status
	// read switch, upper 5 bits of State are don't cares
	State=(State<<1) | (PIND & (1<<PD3))>>3 | 0xf800;
	if(State==0xfc00)return 1; 		// indicates 0 level is
	// stable for 10 consecutive calls
	return 0;
}

//Debounce button 2 for a quick press
uint8_t DebounceSwitch2Short()
{
	static uint16_t State = 0; 		// Current debounce status
	// read switch, upper 5 bits of State are don't cares
	State=(State<<1) | (PIND & (1<<PD2))>>2 | 0xf000;
	if(State==0xf800)return 1; 		// indicates 0 level is
	// stable for 10 consecutive calls
	return 0;
}

void delayMS(uint16_t msec)
{
	//Convert 5 milliseconds into cycles when prescaler is 1024
	uint16_t delay = 16 * 5;

	msec /= 5; //Scale to number of loops needed
	while(msec)
	{
		TCNT0=0; //Clear timer
		OCR0A=delay; //Set target cycles
		TIFR0 |=_BV(OCF0A); //Clear output compare flag

		while ((TIFR0 & _BV(OCF0A)) ==0);  //wait until flag bit is dropped
		
		msec --;
	}
}

//Delays for the specified number of milliseconds
void delay_ms_alarm(uint16_t msec)
{
	//Convert 5 milliseconds into cycles when prescaler is 1024
	uint16_t delay = 16 * 5;

	msec /= 5; //Scale to number of loops needed
	while(msec)
	{
		TCNT0=0; //Clear timer
		OCR0A=delay; //Set target cycles
		TIFR0 |=_BV(OCF0A); //Clear output compare flag

		while ((TIFR0 & _BV(OCF0A)) ==0);  //wait until flag bit is dropped
		
		msec --;
	}
}
