//I2C defines
#define DS1307 0xD0
#define TW_START 0xA4
#define TW_READY (TWCR & 0x80)
#define TW_STATUS (TWSR & 0xF8)
#define TW_SEND 0x84
#define TW_STOP 0x94
#define TW_NACK 0x84
#define TW_READ 0x01
#define I2C_Stop() TWCR = TW_STOP

//Registers
#define SECONDS_REGISTER 0x00
#define MINUTES_REGISTER 0x01
#define HOURS_REGISTER 0x02
#define DAYOFWK_REGISTER 0x03
#define DAYS_REGISTER 0x04
#define MONTHS_REGISTER 0x05
#define YEARS_REGISTER 0x06