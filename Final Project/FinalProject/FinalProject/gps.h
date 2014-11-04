/*
*	\brief Controls the GPS module
*/
#include <RTC.h>

//Get the GPS time
time_t getGPSTime();

//Get the GPS status to determine validity of time
uint8_t getGPSStatus();