//SPI driver file
#include <avr/io.h>
#include "spi.h"

#define ALL_OUTPUT	0xFF // 1111 1111

// initialize the SPI connection
void SPI_MasterInit(void)
{
	/* Sets PORTB as output */
	DDRB = ALL_OUTPUT;
	/* Enable SPI, Master, set clock rate fck/16 */
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}