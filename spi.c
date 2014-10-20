#include <avr/io.h>
#include "spi.h"


// initialize the SPI connection
void SPI_MasterInit(void)
{
	/* Sets PORTB as output */
	DDRB = ALL_OUTPUT;
	/* Enable SPI, Master, set clock rate fck/16 */
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}