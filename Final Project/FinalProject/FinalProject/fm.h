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

fmReturn_t seekUp();
fmReturn_t setChannel(int channel);
fmReturn_t volumeSet(int volume);
fmReturn_t fmInit();
fmReturn_t fmMute();

RDS_t getRDS();
