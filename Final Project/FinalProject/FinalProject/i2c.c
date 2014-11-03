/*!********************
*	\brief RTC communication module
*	
*	This module handles communication with the RTC module
*	over I2C
*
*	\addtogroup I2C
*	@{
*/

#include <avr/io.h>
#include "i2c.h"

#define F_CPU 16000000UL
#define F_SCL 100000UL


/*!
*	\brief Initializes I2C peripheral
*	
*/
void initI2c(void)
{

	TWSR = 0; //Zero prescaler
	TWBR = ((F_CPU/F_SCL) - 16/2); //Set SCL frequency
}

/*!
*	\brief Sends I2C data bi-directionally
*	
*	Initiates I2C communication between master and slave
*	\param reg The register to read from or write to
*	\param tx	The data to send, NULL if reading
*	\param st	The state of the transaction
*	\return Data from bus if reading, 1 if writing
*/
uint8_t sendI2C(uint8_t addr, uint8_t reg, uint8_t tx, I2C_state_t st)
{
	
	if( st == I2C_WRITE) //Write Data
	{
		I2C_Start();
		I2C_SendAddr(addr);
		I2C_Write(reg);
		I2C_Write(tx);
		TWCR = TW_STOP;
		//I2C_Stop();
		return 1;
	}
	//Read Data
	else if(st == I2C_READ)
	{
		uint8_t data = 0;
		I2C_Start();
		I2C_SendAddr(addr);
		I2C_Write(reg);
		I2C_Start();
		I2C_SendAddr(addr|TW_READ);
		data = I2C_ReadNACK();
		TWCR = TW_STOP;
		//I2C_Stop();
		return data;
	}
	
	return 0;
}

/*!
*	\brief Send I2C Start Bit
*	\return boolean status of transaction
*/
uint8_t I2C_Start()
{
	//Send Start
	TWCR = TW_START;
	while(!TW_READY); //wait
	return(TW_STATUS==0x18); //Check if OK
}

/*!
*	\brief Send I2C address
*	\param addr I2C Address of slave device
*	\return boolean status of transaction
*/
uint8_t I2C_SendAddr(uint8_t addr)
{
	//Send Slave address
	TWDR = addr; //load address into TW register
	TWCR = TW_SEND; //Start Sending
	while(!TW_READY); //wait
	return(TW_STATUS==0x18); //Check if OK
}

/*!
*	\brief Writes one byte from the bus
*	\return boolean status of transaction
*/
uint8_t I2C_Write(uint8_t data)
{
	TWDR = data;
	TWCR = TW_SEND;
	while(!TW_READY);
	return(TW_STATUS!=0x28); //Check if OK
}

/*!
*	\brief Reads one byte from the bus
*	\return data from I2C bus
*/
uint8_t I2C_ReadNACK (void)
{
	TWCR = TW_NACK;
	while(!TW_READY);
	return TWDR;
}

//! @}
