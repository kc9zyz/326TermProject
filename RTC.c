/*!********************
*	\brief RTC communication module
*	
*	This module handles communication with the RTC module
*	over I2C
*
*	\addtogroup RTC
*	@{
*/

#include <avr/io.h>
#include "i2c.h"
#include "RTC.h"
#include "DS1307.h"

/*!
*	\brief Read RTC Clock
*	
*	Reads the time from from the RTC
*	\return the current time
*/
time_t getTime(void)
{
	time_t time;

	time.hour = bcdDecode(sendI2C(DS1307,HOURS_REGISTER,0,I2C_READ) & 0x3f);

	time.minute = bcdDecode(sendI2C(DS1307,MINUTES_REGISTER,0,I2C_READ));
	
	time.second = bcdDecode(sendI2C(DS1307,SECONDS_REGISTER,0,I2C_READ));

	time.date = bcdDecode(sendI2C(DS1307,DAYS_REGISTER,0,I2C_READ));

	time.month = bcdDecode(sendI2C(DS1307,MONTHS_REGISTER,0,I2C_READ));

	time.year = bcdDecode(sendI2C(DS1307,YEARS_REGISTER,0,I2C_READ));
	return time;
}

/*!
*	\brief Encodes to binary coded decimal
*	
*	Converts a decimal number to its BCD form
*	\param decimal The decimal form of a number
*	\return BCD form of decimal number
*/
int bcdEncode(uint8_t decimal)
{
	return ((decimal / 10)<<4) | (decimal % 10);
	
}

/*!
*	\brief Decodes binary coded decimal
*	
*	Converts a BCD number to decimal form
*	\param bcd The BCD representation of a number
*	\return decimal form of BCD number
*/
uint8_t bcdDecode(uint8_t bcd)
{
	return (bcd & 0x0F) + 10*((bcd & 0x70)>>4);
}

//! @}
