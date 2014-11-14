/*
 * si4703.c
 * defines from a Nathan Seidle from Sparkfun
 * I owe him a beer due to the its inclusion (Beerware license).
 * https://www.sparkfun.com/products/10663
 */ 

#include <stdio.h>
#include "si4703.h"
#include "i2c.h"
#include "util/delay.h"

//data representation of all 32 bytes 
uint16_t si4703_registers[16];
uint8_t reg_index;
uint16_t ret;

void si4703_setVolume(uint8_t volume) {
	if (volume>0x0F) {
		volume = 0x0F;
	}
	si4703_pull();
	si4703_registers[SYSCONFIG2] = (si4703_registers[SYSCONFIG2] & 0xFFF0) | volume; //set volume
	si4703_Get();
}


void si4703_setChannel(int newChannel) {
	//Freq(MHz) = 0.200(in USA) * Channel + 87.5MHz
	//97.3 = 0.2 * Chan + 87.5
	//9.8 / 0.2 = 49
	newChannel *= 10; //973 * 10 = 9730
	newChannel -= 8750; //9730 - 8750 = 980
	
	newChannel /= 20; //980 / 20 = 49
	
	//These steps come from AN230 page 20 rev 0.5
	si4703_pull();
	si4703_registers[CHANNEL] &= 0xFE00; //Clear out the channel bits
	si4703_registers[CHANNEL] |= newChannel; //Mask in the new channel
	si4703_registers[CHANNEL] |= (1<<TUNE); //Set the TUNE bit to start
	si4703_Get();
	
	while( !(si4703_registers[STATUSRSSI] & _BV(STC)) ) {
		si4703_pull();
	} //setting channel complete!
	
	si4703_pull();
	si4703_registers[CHANNEL] &= ~(1<<TUNE); //Clear the tune bit after a tune has completed
	si4703_Get();
	
	while( (si4703_registers[STATUSRSSI] & _BV(STC)) ) {
		si4703_pull();
	} //wait for the radio to clean up the STC bit
}

uint16_t si4703_getChannel(void) {
	si4703_pull();
	ret = si4703_registers[READCHAN] & 0x03FF; //Mask out everything but the lower 10 bits
	
	//freq(MHz) = 0.200(in USA) * Channel + 87.5MHz
	//x = 0.2 * Chan + 87.5
	ret *= 2; //49 * 2 = 98	 (USA)
	
	ret += 875; //98 + 875 = 973
	return ret;
}

uint8_t si4703_seek(enum DIRECTION dir) {
	si4703_pull();
	si4703_registers[POWERCFG] &= ~(1<<SKMODE); //enable wrapping of frequencies
	if (dir == DOWN) {
		printf("up\n");
		si4703_registers[POWERCFG] &= ~(_BV(SEEKUP));
	} else {
		printf("down\n");
		si4703_registers[POWERCFG] |= _BV(SEEKUP);
	}
	si4703_registers[POWERCFG] |= (_BV(SEEK)); //start seeking
	
	si4703_Get();
	while( !(si4703_registers[STATUSRSSI] & _BV(STC)) ) {
		si4703_pull();
	} //seek complete!
	
	ret = si4703_registers[STATUSRSSI] & (1<<SFBL); //Store the value of SFBL
	DDRB |= _BV(4);//Reset is an output
	DDRC |= _BV(4);//SDIO is an output
	//SET_RST_RADIO; //SETH(FM_DDRRESET, FM_RESET);
	PORTC &= ~_BV(4); //SETH(FM_DDRSDIO, FM_SDIO);
	//PORTC |= _BV(5);
	PORTB &= ~_BV(4); //lower the reset pin
	_delay_ms(1);
	//PORTC &= _BV(4); //SETL(FM_PORTSDIO, FM_SDIO);
	PORTB |= _BV(4);  //SETL(FM_PORTRESET, FM_RESET);
	_delay_ms(10);
	
	PORTC |= (_BV(5) | _BV(4));
	_delay_ms(10);
	si4703_registers[POWERCFG] &= ~(_BV(SEEK)); //stop seeking
	si4703_Get();
	while( (si4703_registers[STATUSRSSI] & _BV(STC)) ) {
		si4703_pull();
	} //wait for the radio to clean up the STC bit
	
	return ret;
}


/* Call init() first */
void si4703_powerOn(void) {
	CLEAR_SI4703_RESET;
	_delay_ms(1);
	SET_SI4703_RESET;
	si4703_pull();
	si4703_registers[OSCCTRL] = 0x8100;
	si4703_Get();
	_delay_ms(500); //Wait for oscillator to settle
	si4703_pull();
	si4703_registers[POWERCFG] = 0x4001; //Enable the IC
	si4703_registers[SYSCONFIG1] |= _BV(RDS); //Enable RDS
	si4703_registers[SYSCONFIG2] &= ~(_BV(SPACE1) | _BV(SPACE0)); //FOrce 200 kHz channel spacing (USA)
	si4703_registers[SYSCONFIG2] &= ~(0x000F); //Turn down for what?!?
	//si4703_registers[SYSCONFIG2] |= 0x0001; //Lowest volume setting
	si4703_Get();
	_delay_ms(110); //Waiting the max powerup time for the radio
}


//The device starts reading at reg_index 0x0A, and has 32 bytes
void si4703_pull(void) {
	I2C_Start();
	I2C_SendAddr(SI4703_ADDR | TW_READ);
	int i;
	//Read registers
	for(i = 0x0A ; ; i++) {
		if(i == 0x10) i = 0; //Loop back to zero
		si4703_registers[i] = I2C_ReadACK() << 8;
		if(i!=0x09)
			si4703_registers[i] |= I2C_ReadACK();
		if(i==0x09)
			si4703_registers[i] |= I2C_ReadNACK();
		
	}
	

}


//The device starts writing at reg_index 4, and there are only 12 bytes that are controlling
void si4703_Get(void) {
	
	I2C_SendAddr(SI4703_ADDR);
	int i;
	for (i=0x00; i<0x06; i++) {
		I2C_Write(si4703_registers[i+2] >> 8);
		I2C_Write(si4703_registers[i+2] & 0x00FF);
	}
	
	I2C_Stop();
}
