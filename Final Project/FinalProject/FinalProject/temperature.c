/*	DHT11 temperature sensor control module
*
*/

#define F_CPU 16000000UL
#include<avr/io.h>
#include<util/delay.h>

//Gets the temperature from the sensor
uint8_t getTemp()
{
	//Variables
	uint16_t count = 0;
	uint8_t checksum = 0;
	uint8_t data[6];
	uint8_t lastPinState = 1;
	uint8_t j=0, i;
	data[4] = data[3] = data[2] = data[1] = data[0] = 0;
	
	//Set port to input to pullup-pull line high
	DDRD  &= ~_BV(7);
	PORTB |= _BV(7);
	//Delay for 500 us
	_delay_ms(2);
	
	//Set PD7 to output, low, delay 20m
	DDRD |= _BV(7);
	PORTD &= ~_BV(7);
	_delay_ms(20);
	
	//Pull high for 40 us
	PORTD |= _BV(7);
	_delay_us(40);
	
	//Set PD7 to input
	DDRD &= ~_BV(7);
	// _delay_us(10);

	//Cycle through all possible bits
	for(i=0; i<85; i++)
	{
		count = 0;
		//Wait until line transitions
		while((PIND & _BV(7)) == lastPinState)
		{
			//Incrase tracking counter
			count++;
			//Wait for 1 microsecond
			_delay_us(1);
			
			//Check if line timeout
			if(count ==255)
				//Exit and move on
				break;
		}
		//Remember what the line is set to
		lastPinState = (PIND & _BV(7));
		//Check if there was a line timout
		if(count ==255)
			//Exit the loop
			break;
		//Check if this is a useful bit
		//Defined by: the 4th+ level check,
		//and an even numbered level check
		if((i >=4) && (i%2 == 0))
		{
			//Shift the byte corresponding to the
			//date 
			data[j/8] <<= 1;
			if(count > 30 )
				data[j/8] |=1;
			j++;
		}

	}
	//Convert to Fahrenheit
	data[2]*=1.8;
	data[2]+=32;
	//return current temperature
	return data[2];

}
