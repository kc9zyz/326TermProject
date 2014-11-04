/*
* Term Project
*
* Created: 11/3/2014 
*  Author: Greg Lepley and Billy Neuson
*
* Term project main C file
*/


#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <string.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>

#include "graphics.h"
#include "state.h"
#include "RTC.h"
#include "spi.h"
#include "i2c.h"

#define F_CPU 16000000UL
#define F_SCL 100000UL
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1






void soundAlarm(void);
void setAlarm(void);
void writeTheTime(void);
void writeAlarmToggle(void);
void writeAlarmSet(void);
void timer0Init();
void delayMS(uint16_t msec);
void delay_ms_alarm(uint16_t msec);

void pwm_PB2(unsigned Ton, unsigned period);
void saveAlarm1();
void readAlarm1();
void saveAlarm2();
void readAlarm2();
void setupADC();
uint8_t queryButton();
void displayMenu();
void WDT_Init(void);


//watchdog basics are included, but not implemented




volatile time_t timeGet; //variable to hold current time
volatile time_t timeAlarm1; //variable to hold alarm1 time
volatile time_t timeAlarm2; //variable to hold alarm2 time
volatile int alarm1On; //variable to hold alarm1 status
volatile int alarm2On; //variable to hold alarm2 status
uint8_t EEMEM Alarm1Time[2]; //EEPROM location for Alarm1
uint8_t EEMEM Alarm2Time[2]; //EEPROM location for Alarm2
volatile uint8_t menuCounter;
volatile uint8_t menuPosition; //current postion in the main menu
volatile uint8_t button; //holds value of what button was pressed
volatile uint8_t toggle; //for toggling display flashes
volatile uint8_t hour;
volatile uint8_t min;
volatile int watchDogCount;//maintains how many times watchdog timer has cycled

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
	
	setupADC(); // Initialize the ADC
	
	
	alarm1On = 1; //initally alarm1 is on
	alarm2On = 1; //initially alarm2 is on
	
	DDRD &= ~_BV(2); 	//set pin 2 as input
	DDRD &= ~_BV(3);	//set pin 3 as input
	PORTD |= _BV(2) | _BV(3); 	//activate pullup resistor on pins 2 and 3

	readAlarm1();		//get the alarm from EEPROM and store in local memory
	readAlarm2();		//get the alarm from EEPROM and store in local memory

	
	int state = S0; // set S0 as the initial state
	int i = 0;
	button = 0;
	menuPosition = 1;
	toggle = 1;
	
	while(1)
	{	
		//state machine for operation
		switch (state) 
		{
			//normal clock display mode. shows indicator for if alarm on or off
			case S0:
				
				i++;
				if(i == 10)
				{
					writeTheTime(); //display the current time on display
					
					i = 0;
				}
				
				//Get the alarm time from EEPROM
				readAlarm1();
				readAlarm2()
				
				//if alarm time matches current time, activate alarm
				if((timeGet.hour == timeAlarm1.hour && 
				   timeGet.minute == timeAlarm1.minute && 
				   timeGet.second == timeAlarm1.second && 
				   alarm1On == 1) || 
				   (timeGet.hour == timeAlarm2.hour &&
				   timeGet.minute == timeAlarm2.minute &&
				   timeGet.second == timeAlarm2.second &&
				   alarm2On == 1))
				{
					soundAlarm(); // activate alarm
				}
				button = queryButton();
				
				//if user presses button 1, advance to S1
				delayMS(15);
				if(queryButton() == 1) 
				{
					delayMS(150);
					if(queryButton() == 0){state = S1; menuPosition = 1;}
				}
				
				break;
			//Display the menu	
			case S1:
			
				i++;
				if(i == 10)
				{
					displayMenu();
					delayMS(15);
					if(queryButton() == 3 )
					{
						delayMS(150);
						if(queryButton() == 0)
						{
							if(menuPosition == 1){menuPosition = 1;}
							else{menuPosition --;}
						}
					}
					if(queryButton() == 4)
					{
						delayMS(150);
						if(queryButton() == 0)
						{
							if(menuPosition == 4){menuPosition = 4;}
							else{menuPosition ++;}
						}
					}
					
					i = 0;
				}
				delayMS(15);
				if(queryButton() == 1)
				{
					delayMS(150);
					if(queryButton() == 0){state = S0;}
				}
				if(queryButton() == 2 && menuPosition == 1)
				{
					delayMS(150);
					if(queryButton() == 0){state = S2;}
				}
				if(queryButton() == 2 && menuPosition == 2)
				{
					delayMS(150);
					if(queryButton() == 0){state = S3;}
				}
				if(queryButton() == 2 && menuPosition == 3)
				{
					delayMS(150);
					if(queryButton() == 0){state = S4;}
				}
				if(queryButton() == 2 && menuPosition == 4)
				{
					delayMS(150);
					if(queryButton() == 0){state = S5;}
				}
				break;
				
			//displays radio interface and allows user to manipulate
			//the radio controls	
			case S2:
				//TODO: Radio display
				
				break;
				
			//displays the current alarm1 time and allows the user to set the hour and
			//minute values of the alarm
			case S3:
				
				//display the interface for setting the alarm time
				i++;
				if(i == 10)
				{
					if(toggle == 1){toggle = 0;}
					else{toggle = 1;}
					writeAlarmSet();
					i = 0;
				}
				
				// rolls over minutes if 60 minutes is reached
				if(timeAlarm.minute == 59)
				{
					timeAlarm.minute = 0;
				}
				//increments the seconds by 1
				else
				{
					timeAlarm1.minute++;
				}
				
				//rolls over hours if 24 hours is reached
				if(timeAlarm1.hour == 23)
				{
					timeAlarm1.hour = 0;
				}
				//increments the seconds by 1
				else
				{
					timeAlarm1.hour++;
				}
				
				delayMS(5);
				
				//check 5 times to see if menu button is pressed
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
				
			//displays the current alarm2 time and allows the user to set the hour and
			//minute values of the alarm
			case S4:
			
				//display the interface for setting the alarm time
				i++;
				if(i == 10)
				{
					if(toggle == 1){toggle = 0;}
					else{toggle = 1;}
					writeAlarmSet();
					i = 0;
				}
				
				// rolls over minutes if 60 minutes is reached
				if(timeAlarm2.minute == 59)
				{
					timeAlarm2.minute = 0;
				}
				//increments the seconds by 1
				else
				{
					timeAlarm2.minute++;
				}
				
				//rolls over hours if 24 hours is reached
				if(timeAlarm2.hour == 23)
				{
					timeAlarm2.hour = 0;
				}
				//increments the seconds by 1
				else
				{
					timeAlarm2.hour++;
				}
				
				delayMS(5);
				
				//check 5 times to see if menu button is pressed
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
				
			//Displays the current time and allows the user to set the hour and minute
			//minute values of the current time stored in the RTC
			case S4:
				//display the interface for setting the alarm time
				i++;
				if(i == 10)
				{
					if(toggle == 1){toggle = 0;}
					else{toggle = 1;}
					writeTimeSet();
					i = 0;
				}
				
				// rolls over minutes if 60 minutes is reached
				if(timeGet.minute == 59)
				{
					timeGet.minute = 0;
				}
				//increments the seconds by 1
				else
				{
					timeGet.minute++;
				}
				
				//rolls over hours if 24 hours is reached
				if(timeGet.hour == 23)
				{
					timeGet.hour = 0;
				}
				//increments the seconds by 1
				else
				{
					timeGet.hour++;
				}
				
				delayMS(5);
				
				//check 5 times to see if menu button is pressed
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
				
			//flashes the current time and plays the appropriate alarm 
			case S5:
				//TODO: alarm sound stub
				break;
			//Allows modification of presets
			case S6:
				//TODO: preset code
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
		//press = DebounceSwitch2();
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
		if(queryButton() == 4) i=3000;	
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
	LCD_gotoXY(0,5);
	LCD_writeChar((char)queryButton()+'0');
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
	if(menuPosition == 2)
	{
		sprintf(alarm, "%02d:%02d", timeAlarm1.hour, timeAlarm1.minute);
		LCD_gotoXY(13,0); //position the cursor for set alarm header
		LCD_writeString_F("Set Alarm1");
		LCD_writeString_megaFont(alarm); //print the Hours and Minutes of alarm in large font
	}
	if(menuPosition == 3)
	{
		sprintf(alarm, "%02d:%02d", timeAlarm2.hour, timeAlarm2.minute);
		LCD_gotoXY(13,0); //position the cursor for set alarm header
		LCD_writeString_F("Set Alarm2");
		LCD_writeString_megaFont(alarm); //print the Hours and Minutes of alarm in large font
	}
	
	
}
//Generates the GUI for setting the system time
void writeTimeSet(void)
{
	LCD_clear();
	char time[15];
	sprintf(time, "%02d:%02d", timeGet.hour, timeGet.minute);
	LCD_gotoXY(13,0); //position the cursor for set time header
	LCD_writeString_F("Set Time");
	LCD_writeString_megaFont(time); //print the Hours and Minutes in large font
	
	
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

void saveAlarm1()
{
	uint8_t temp[2];
	temp[0] = timeAlarm1.hour;
	temp[1] = timeAlarm1.minute;
	eeprom_update_block((const void*)temp, (void*)AlarmTime1, 2);
}

void readAlarm1()
{
	uint8_t temp[2];
	eeprom_read_block((void*)temp, (const void*)AlarmTime1, 2);
	timeAlarm1.hour = temp[0];
	timeAlarm1.minute = temp[1];
}

void saveAlarm2()
{
	uint8_t temp[2];
	temp[0] = timeAlarm2.hour;
	temp[1] = timeAlarm2.minute;
	eeprom_update_block((const void*)temp, (void*)AlarmTime2, 2);
}

void readAlarm2()
{
	uint8_t temp[2];
	eeprom_read_block((void*)temp, (const void*)AlarmTime2, 2);
	timeAlarm2.hour = temp[0];
	timeAlarm2.minute = temp[1];
}

//Setup the ADC peripheral
void setupADC()
{
	DDRC &= ~_BV(0); //setup PC0 as input
	ADMUX = _BV(REFS0); //select PC0 as ADC input
	ADMUX |= _BV(ADLAR); //left align bits into ADCH
	ADCSRA |= _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0); //setup 128 division factor
	ADCSRB = 0;
}

//Find which buttons are pressed
uint8_t queryButton()
{
	uint8_t status = 0;
	uint8_t result = 0;
	
	ADCSRA |= _BV(ADEN); //enable the ADC
	ADCSRA |= _BV(ADSC); //start the ADC conversion
	while((ADCSRA & _BV(ADIF)) == 0); //wait for conversion to complete
	uint8_t result1 = ADCH; //store the MS 8 bits in result
	while((ADCSRA & _BV(ADIF)) == 0); //wait for conversion to complete
	uint8_t result2 = ADCH; //store the MS 8 bits in result
	while((ADCSRA & _BV(ADIF)) == 0); //wait for conversion to complete
	uint8_t result3 = ADCH; //store the MS 8 bits in result
	while((ADCSRA & _BV(ADIF)) == 0); //wait for conversion to complete
	uint8_t result4 = ADCH; //store the MS 8 bits in result
	while((ADCSRA & _BV(ADIF)) == 0); //wait for conversion to complete
	uint8_t result5 = ADCH; //store the MS 8 bits in result
	while((ADCSRA & _BV(ADIF)) == 0); //wait for conversion to complete
	uint8_t result6 = ADCH; //store the MS 8 bits in result
	while((ADCSRA & _BV(ADIF)) == 0); //wait for conversion to complete
	uint8_t result7 = ADCH; //store the MS 8 bits in result
	while((ADCSRA & _BV(ADIF)) == 0); //wait for conversion to complete
	uint8_t result8 = ADCH; //store the MS 8 bits in result
	while((ADCSRA & _BV(ADIF)) == 0); //wait for conversion to complete
	uint8_t result9 = ADCH; //store the MS 8 bits in result
	while((ADCSRA & _BV(ADIF)) == 0); //wait for conversion to complete
	uint8_t result10 = ADCH; //store the MS 8 bits in result
	if(result1 == result2 && result2 == result3 && result3 == result4 && result4 == result5 &&
	   result5 == result6 && result6 == result7 && result7 == result8 && result8 == result9 &&
	   result9 == result10)
	{
		result = ADCH; //store the MS 8 bits in result
	}
	ADCSRA ^ _BV(ADEN); //turn off the ADC
	int converted = (5000 / 256) * result; //calculate the voltage from the ADC value

	if(converted > 2250 && converted <2750) {status = 1;}
	if(converted > 2750 && converted <3100) {status = 2;}
	if(converted > 3100 && converted <3300) {status = 3;}
	if(converted > 3300 && converted <3750) {status = 4;}
		
	return status;
}

//Display the menu of options
void displayMenu() 
{
	char One[6]			   =	" Radio";
	char OneSelected[6]	   =	">Radio";
	char Two[12]		   =	" Set Alarm 1";
	char TwoSelected[12]   =	">Set Alarm 1";
	char Three[12]		   =	" Set Alarm 2";
	char ThreeSelected[12] =	">Set Alarm 2";
	char Four[9]           =	" Set Time";
	char FourSelected[9]   =	">Set Time";
	
	LCD_clear();
	if(menuPosition == 1)
	{
		LCD_gotoXY(0,1);
		LCD_writeString_F(OneSelected);
		LCD_gotoXY(0,2);
		LCD_writeString_F(Two);
		LCD_gotoXY(0,3);
		LCD_writeString_F(Three);
		LCD_gotoXY(0,4);
		LCD_writeString_F(Four);
		
	}
	if(menuPosition == 2)
	{
		LCD_gotoXY(0,1);
		LCD_writeString_F(One);
		LCD_gotoXY(0,2);
		LCD_writeString_F(TwoSelected);
		LCD_gotoXY(0,3);
		LCD_writeString_F(Three);
		LCD_gotoXY(0,4);
		LCD_writeString_F(Four);
	}
	if(menuPosition == 3)
	{
		LCD_gotoXY(0,1);
		LCD_writeString_F(One);
		LCD_gotoXY(0,2);
		LCD_writeString_F(Two);
		LCD_gotoXY(0,3);
		LCD_writeString_F(ThreeSelected);
		LCD_gotoXY(0,4);
		LCD_writeString_F(Four);
	}
	if(menuPosition == 4)
	{
		LCD_gotoXY(0,1);
		LCD_writeString_F(One);
		LCD_gotoXY(0,2);
		LCD_writeString_F(Two);
		LCD_gotoXY(0,3);
		LCD_writeString_F(Three);
		LCD_gotoXY(0,4);
		LCD_writeString_F(FourSelected);
	}
	LCD_gotoXY(0,5);
	LCD_writeChar((char)queryButton()+'0');
}

//initialize watchdog
void WDT_Init(void)
{
	MCUSR = 0; //reset the status register of the MCU
	wdt_disable(); //disable the watchdog
	//disable interrupts
	cli();
	//reset watchdog
	wdt_reset();
	//set up WDT interrupt
	WDTCSR = (1<<WDCE) | (1<<WDE);
	//Start watchdog timer with 4s prescaler
	WDTCSR = (1<<WDIE)|(1<<WDP3);
	//Enable global interrupts
	sei();
}
//Watchdog timeout ISR
ISR(WDT_vect)
{
	//Increments WatchDogCount
	watchDogCount++;
}