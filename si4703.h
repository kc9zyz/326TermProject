/*
 * si4703.h
 * 
 * Author: CptSpaceToaster
 *
 * References: 
 * Programming Guide: http://www.silabs.com/support%20documents/technicaldocs/AN230.pdf
 * Datasheet: https://www.sparkfun.com/datasheets/BreakoutBoards/Si4702-03-C19-1.pdf
 * 
 * Some of the defines came from a Nathan Seidle from Sparkfun
 * I owe him a beer due to the its inclusion (Beerware license).
 * https://www.sparkfun.com/products/10663
 */

#ifndef SI4703_H
#define SI4703_H

//definitions
enum DIRECTION {
	DOWN,
	UP,
};

#define SI4703_RST_PIN               4
#define SET_SI4703_RESET             PORTB |= _BV(SI4703_RST_PIN)
#define CLEAR_SI4703_RESET           PORTB &= ~_BV(SI4703_RST_PIN)

#define SI4703_ADDR (0x10<<1) //device address shifted over one

//Define the register names
#define DEVICEID 0x00
#define CHIPID  0x01
#define POWERCFG  0x02
#define CHANNEL  0x03
#define SYSCONFIG1  0x04
#define SYSCONFIG2  0x05
#define OSCCTRL 0x07
#define STATUSRSSI  0x0A
#define READCHAN  0x0B
#define RDSA  0x0C
#define RDSB  0x0D
#define RDSC  0x0E
#define RDSD  0x0F

//Register 0x02 - POWERCFG
#define SMUTE  15
#define DMUTE  14
#define SKMODE  10
#define SEEKUP  9
#define SEEK  8

//Register 0x03 - CHANNEL
#define TUNE  15

//Register 0x04 - SYSCONFIG1
#define RDS  12
#define DE  11

//Register 0x05 - SYSCONFIG2
#define SPACE1  5
#define SPACE0  4

//Register 0x0A - STATUSRSSI
#define RDSR  15
#define STC  14
#define SFBL  13
#define AFCRL  12
#define RDSS  11
#define STEREO  8

#define SI4703_FM_HIGH               1079
#define SI4703_FM_LOW                881
#define SI4703_VOL_HIGH              15
#define SI4703_VOL_LOW               0

//functions
void si4703_setVolume(uint8_t volume);
void si4703_setChannel(int newChannel);
uint16_t si4703_getChannel(void);
uint8_t si4703_seek(uint8_t direction);

void si4703_init(void);
void si4703_powerOn(void);
void si4703_pull(void);
void si4703_push(void);
//void seek_TWI_devices();

#endif //SI4703_H
