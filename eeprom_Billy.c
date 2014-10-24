/*
*	Uses functions from the RTC library to encode and decode 
*/
#include <avr/eeprom.h>
#include "RTC.h"

//Define locations of eeprom stored bytes
#define EEPROM_HOURS 0x10
#define EEPROM_MINUTES 0x11

//Set alarm time from time_t struct to eeprom
void setStoredAlarmTime(time_t time)
{
	//update eeprom from time_t
	eeprom_update_byte(EEPROM_HOURS, bcdEncode(time.hour));
	eeprom_update_byte(EEPROM_MINUTES, bcdEncode(time.minute));
	
}

//Get alarm time stored in eeprom
time_t getStoredAlarmTime(void)
{
	//Declare time_t return variable
	time_t time;
		
	//Store hours and minutes in time_t struct
	time.hour = bcdDecode(eeprom_read_byte(EEPROM_HOURS));
	time.minute = bcdDecode(eeprom_read_byte(EEPROM_MINUTES));
		
	return time;
}