/*!********************
*	\brief I2C communication module
*	
*	This module handles communication over I2C
*
*	\addtogroup I2C
*	@{
*/

/*!
*	I2C State enumeration
*/
typedef enum {
	I2C_WRITE, //!<	Write state
	I2C_READ, //!<	Read state
}I2C_state_t;

//Function Prototypes

/*!
*	\brief Initializes I2C peripheral
*	
*/
void initI2c(void);

/*!
*	\brief Sends I2C data bi-directionally
*	
*	Initiates I2C communication between master and slave
*	\param reg The register to read from or write to
*	\param tx	The data to send, NULL if reading
*	\param st	The state of the transaction
*	\return Data from bus if reading, 1 if writing
*/
uint8_t sendI2C(uint8_t addr, uint8_t reg, uint8_t tx, I2C_state_t st);

/*!
*	\brief Send I2C Start Bit
*	\return boolean status of transaction
*/
uint8_t I2C_Start();

/*!
*	\brief Send I2C address
*	\param addr I2C Address of slave device
*	\return boolean status of transaction
*/
uint8_t I2C_SendAddr(uint8_t addr);

/*!
*	\brief Reads one byte from the bus
*	\return data from I2C bus
*/
uint8_t I2C_ReadNACK();

/*!
*	\brief Writes one byte from the bus
*	\return boolean status of transaction
*/
uint8_t I2C_Write(uint8_t data);

//! @}