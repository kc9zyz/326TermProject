/*!***********
*	\brief RTC communication module
*	
*	This module handles communication with the RTC module
*	over I2C
*
*	\addtogroup RTC
*	@{
***/

/*!
* Structure to hold time information
*/
typedef struct {
	uint16_t year; //!< Year A.D.
	uint8_t month; //!< Month in number
	uint8_t date; //!< Day of month
	uint8_t hour; //!< Hour in 24 hour format
	uint8_t minute; //!< Minute in decimal format
	uint8_t second; //!< Second in decimal format
} time_t;

/*!
*	\brief Read RTC Clock
*	
*	Reads the time from the RTC
*	\return the current time
*/
time_t getTime(void);

/*!
*	\brief Set RTC Clock
*	
*	Sets the time on the RTC
* \param time The current time to send to the RTC
*	\return the current time
*/
time_t setTime(time_t time);

/*!
*	\brief Encodes to binary coded decimal
*	
*	Converts a decimal number to its BCD form
*	\param decimal The decimal form of a number
*	\return BCD form of decimal number
*/
uint8_t bcdEncode(uint8_t decimal);

/*!
*	\brief Decodes binary coded decimal
*	
*	Converts a BCD number to decimal form
*	\param bcd The BCD representation of a number
*	\return decimal form of BCD number
*/
uint8_t bcdDecode(uint8_t bcd);

//! @}