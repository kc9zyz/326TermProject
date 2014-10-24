/* 

2/11/13 Edited by Aaron Weiss @ SparkFun

Library for Sparkfun Si4703 breakout board.
Simon Monk. 2011-09-09

This is a library wrapper and a few extras to the excellent code produced
by Nathan Seidle from Sparkfun (Beerware).

Nathan's comments......

Look for serial output at 57600bps.

The Si4703 ACKs the first byte, and NACKs the 2nd byte of a read.

1/18 - after much hacking, I suggest NEVER write to a register without first reading the contents of a chip.
ie, don't updateRegisters without first readRegisters.

If anyone manages to get this datasheet downloaded
http://wenku.baidu.com/view/d6f0e6ee5ef7ba0d4a733b61.html
Please let us know. It seem to be the latest version of the programming guide. It had a change on page 12 (write 0x8100 to 0x07)
that allowed me to get the chip working..

Also, if you happen to find "AN243: Using RDS/RBDS with the Si4701/03", please share. I love it when companies refer to
documents that don't exist.

1/20 - Picking up FM stations from a plane flying over Portugal! Sweet! 93.9MHz sounds a little soft for my tastes,s but
it's in Porteguese.

ToDo:
Display current status (from 0x0A) - done 1/20/11
Add RDS decoding - works, sort of
Volume Up/Down - done 1/20/11
Mute toggle - done 1/20/11
Tune Up/Down - done 1/20/11
Read current channel (0xB0) - done 1/20/11
Setup for Europe - done 1/20/11
Seek up/down - done 1/25/11

The Si4703 breakout does work with line out into a stereo or other amplifier. Be sure to test with different length 3.5mm
cables. Too short of a cable may degrade reception.

2/11/13 - Fixed for Arduino 1.0. Changed wire.send and wire.receive to wire.write and wire.read. Also added arduino.h and removed wprogram.h
*/

#ifndef Si4703_Breakout_h
#define Si4703_Breakout_h

#include "Arduino.h"

void powerOn();					// call in setup
void setChannel(int channel);  	// 3 digit channel number
int seekUp(); 					// returns the tuned channel or 0
int seekDown(); 				
void setVolume(int volume); 	// 0 to 15
void readRDS(char* message, long timeout);	
							// message should be at least 9 chars
							// result will be null terminated
							// timeout in milliseconds
uint8_t readRSSI();

uint16_t si4703_registers[16]; //There are 16 registers, each 16 bits large
	
#define  FAIL = 0;
#define  SUCCESS = 1;

#define  SI4703 = 0x10; //0b._001.0000 = I2C address of Si4703 - note that the Wire function assumes non-left-shifted I2C address, not 0b.0010.000W
#define  I2C_FAIL_MAX = 10; //This is the number of attempts we will try to contact the device before erroring out
#define  SEEK_DOWN = 0; //Direction used for seeking. Default is down
#define  SEEK_UP = 1;

//Define the register names
#define  DEVICEID = 0x00;
#define  CHIPID = 0x01;
#define  POWERCFG = 0x02;
#define  CHANNEL = 0x03;
#define  SYSCONFIG1 = 0x04;
#define  SYSCONFIG2 = 0x05;
#define  STATUSRSSI = 0x0A;
#define  READCHAN = 0x0B;
#define  RDSA = 0x0C;
#define  RDSB = 0x0D;
#define  RDSC = 0x0E;
#define  RDSD = 0x0F;

//Register 0x02 - POWERCFG
#define  SMUTE = 15;
#define  DMUTE = 14;
#define  SKMODE = 10;
#define  SEEKUP = 9;
#define  SEEK = 8;

//Register 0x03 - CHANNEL
#define  TUNE = 15;

//Register 0x04 - SYSCONFIG1
#define  RDS = 12;
#define  DE = 11;

//Register 0x05 - SYSCONFIG2
#define  SPACE1 = 5;
#define  SPACE0 = 4;

//Register 0x0A - STATUSRSSI
#define  RDSR = 15;
#define  STC = 14;
#define  SFBL = 13;
#define  AFCRL = 12;
#define  RDSS = 11;
#define  STEREO = 8;

#endif