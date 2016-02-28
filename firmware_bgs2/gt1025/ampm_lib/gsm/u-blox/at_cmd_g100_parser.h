#ifndef __AT_CMD_G100_PARSER_H__
#define __AT_CMD_G100_PARSER_H__

#include <stdint.h>
#include "stdio.h"
#include "string.h"
#include "at_cmd_common_parser.h"


#define CELL_INFO_MAC	5


/**
 *CELL LOCATE
 */
typedef struct _cellLocate
{
    GSM_DATE_TIME time;       /**< UTC of position */
		float  lat;        /**< Latitude in NDEG - [degree][min].[sec/60] */
		float  lon;        /**< Longitude in NDEG - [degree][min].[sec/60] */
    float  speed;      /**< Speed over the ground in knots */
    float  dir;  /**< Track angle in degrees True */
    float  uncertainty; 
		float alt;
		uint8_t update;
} cellLocateType;

typedef enum{
	UBLOX_IDLE = 0,
	GET_BATTERY_INFO,
	UBLOX_ENABLE_GPRS_ONLINE,
	UBLOX_START_AGPS_ONLINE,
	UBLOX_WAIT_AGPS_ONLINE,
	UBLOX_AGPS_STOP_ONLINE,
	UBLOX_DELETE_FILE_AGPS_OFFLINE,
	UBLOX_ENABLE_GPRS_AGPS_OFFLINE,
	UBLOX_START_DOWNLOAD_FILE_AGPS_OFFLINE,
	UBLOX_WAIT_DOWNLOAD_FILE_AGPS_OFFLINE,
	UBLOX_START_AGPS_OFFLINE,
	UBLOX_WAIT_AGPS_OFFLINE,
	UBLOX_STOP_AGPS_OFFLINE,
	UBLOX_GET_CELL_INFO,
	UBLOX_WAIT_GET_CELL_INFO,
	UBLOX_FINISH_GET_CELL_INFO
}UBLOX_PHASE_TYPE;

typedef struct{
	uint16_t mcc;
	uint16_t mnc;
	uint16_t lac;
	uint32_t cellId;
	#ifdef LEON_SARA
	uint16_t bsic;
	uint16_t rxLev;
	#else
	uint32_t rac;
	uint32_t dlf;
	uint16_t ulf;
	uint16_t sc;
	uint8_t rscp_lev;
	uint8_t enc0_lev;
	#endif
	
}CELL_INFO_TYPE;


extern void GPS_Ennable(void);

extern uint8_t cellLocationBuf[128];
extern uint8_t cellLocationFlag;
extern CELL_INFO_TYPE cellInfo[CELL_INFO_MAC];
extern uint8_t cellGotInfoFLag;
extern cellLocateType cellLocate;

uint32_t CellLocateParser(uint16_t cnt,uint8_t c);
uint8_t ParserCellLocate(void);
void UbloxTask_Init(void);
UBLOX_PHASE_TYPE UbloxTask_GetPhase(void);
void UbloxTask_SetPhase(UBLOX_PHASE_TYPE phase);
uint8_t Ublox_Task(void);

#endif
