#ifndef _RS485_CAMERA_H_
#define _RS485_CAMERA_H_
#include <stdint.h>

#define CAMERA_DATA_SIZE	512
#define CONFIG_USE_GPS_IN_CAMERA	1
#define CONFIG_USE_ID_IN_PACKETS	1
#define PACKET_TYPE_WRITE_FILE			0xF2
#define PACKET_TYPE_IMAGE_FILE			0xF3

typedef struct __attribute__((packed)){
	uint16_t size;				// = sizeof (PACKET_WRITE_FILE)
	uint8_t type;				// = 0xF2
	
	uint8_t hwVersion[8];
	uint8_t fwVersion[8];
	uint16_t status;
	uint8_t gpsSig;
	float gpsLat;				// (-180 - +180)
	float gpsLon;			// (-90 - +90)
	float gpsAlt;
	float gpsSpeed;				// km/h
	float gpsHeading;				// 0 - 360
	float gpsHDOP;
	uint8_t gpsSatInUse;
	
	float cellLat;				// (-180 - +180)
	float cellLon;			// (-90 - +90)
	float cellHDOP;
	uint32_t cellMCC;
	uint32_t cellMNC;
	uint32_t cellLAC;
	uint32_t cellCID;
	
	float mileage;
	float pulseSpeed;
	float frequency;	//Tan so cam bien nhien lieu
	
	uint8_t gsmSignal;
	
	uint16_t adc1Value;				// A1: 0 - 4095
	uint16_t adc2Value;				// A2: 0 - 4095
	uint32_t pulseCnt;				// Xung cam bien nhien lieu
	uint32_t doorOpenCount;
	uint32_t drivingTime;
	uint32_t totalDrivingTime;
	
	uint8_t batteryPercent;
	uint8_t drvIndex;
	
	uint16_t iSize;				// = CAMERA_DATA_SIZE + 40 
	uint8_t iType;				// = 0xF3
	uint8_t id[17];
	// time stamp
	uint8_t year;				// year since 2000
	uint8_t month;				// 1 - 12
	uint8_t mday;				// 1 - 31
	uint8_t hour;				// 0 - 23
	uint8_t min;				// 0 - 59
	uint8_t sec;					// 0 - 59
	
	uint8_t cameraNumber;
	
	uint32_t fileSize;			// file size
	uint32_t offset;				// offset of the chunk
	uint32_t len;				// size of the chunk
	uint8_t data[CAMERA_DATA_SIZE];	// data chunk
	uint32_t crc;
}CAMERA_PAKET_TYPE;

/*
typedef struct __attribute__((packed))
{
	int8_t id[18];
	uint32_t fileSize;			// file size
	uint32_t offset;				// offset of the chunk
	uint32_t len;				// size of the chunk
	uint8_t data[CAMERA_DATA_SIZE];	// data chunk
}CAMERA_PAKET_TYPE;
*/

extern uint8_t cameraShotFlag;
extern CAMERA_PAKET_TYPE packetWriteJpegFile;
extern CAMERA_PAKET_TYPE packetReadJpegFile;
uint32_t CAM_crc(uint8_t *buff, uint32_t length);
void CAM_Init(void);
void CAM_Input(uint8_t c);
void CAM_Manage(void);
uint8_t CAM_CheckDataIsReady(void);
uint8_t CAM_SetPacketOffset(uint8_t packet);
uint8_t CAM_CheckEndPacket(void);
uint8_t CAM_GetPacketCnt(void);
uint8_t CAM_NextPacket(void);
uint8_t CAM_SetNextCamId(void);
uint8_t CAM_BackPacket(void);
uint8_t* CAM_GetPacket(void);
void CAM_ChangeId(uint8_t id, uint8_t newId);
uint8_t CAM_TakeSnapshot(uint16_t cameras);
void CAM_Stop(void);
void CAM_Start(void);

#endif
