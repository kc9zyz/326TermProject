/*
*	\brief FM raio control module
*
*/
//Stores RDS data
typedef struct{
	uint8_t valid;
	uint16_t block1; 
	uint16_t block2;
	uint16_t block3;
	uint16_t block4;
} RDS_t;

typedef struct{
	//FM STATUS PARAMETERS
}fmReturn_t;

//Seeks up for a radio station
fmReturn_t seekUp();

//Sets the channel to the passed parameter
fmReturn_t setChannel(int channel);

//Sets the volume to the level passed
fmReturn_t volumeSet(int volume);

//Initializes the fm module
fmReturn_t fmInit();

//Mutes the radio
fmReturn_t fmMute();

//Puts the FM radio in shutdown mode
fmReturn_t fmShutdown();
RDS_t getRDS();
