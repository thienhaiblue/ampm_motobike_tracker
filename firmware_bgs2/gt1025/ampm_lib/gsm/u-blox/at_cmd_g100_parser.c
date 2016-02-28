#include "modem.h"
#include "lib/sparser.h"
#include "lib/sys_tick.h"
#include "lib/sys_time.h"
#include "lib/data_cmp.h"
#include "lib/list.h"
#include "at_command_parser.h"
#include "at_send_cmd_task.h"
#include "at_gsm_task.h"
#include "gps/gps.h"


CELL_INFO_TYPE cellInfo[CELL_INFO_MAC];

#define INFO(...)	//PrintfDebug(__VA_ARGS__)

uint8_t cellLocationBuf[128];
uint8_t cellLocationFlag = 0;
cellLocateType cellLocate;
uint32_t cellPass = 0;
uint8_t cellGotInfoFLag = 0;
uint8_t ubloxAgpsOfflineTry = 0;

UBLOX_PHASE_TYPE ubloxPhase = UBLOX_IDLE;

Timeout_Type tUbloxTaskPhaseTimeOut;


const AT_CMD_PACKET_TYPE atGpsCmdStart[] = {
	//"AT+UGPIOC=21,3\r",NULL,NULL,"OK","ERROR",0,1000,100,
	"AT+UGRMC=1\r",NULL,NULL,"OK","ERROR",0,1000,100,
	"AT+UGIND=1\r",NULL,NULL,"OK","ERROR",0,1000,100,
	"AT+UGPS=1,4\r",NULL,NULL,"+UUGIND: 4,0","ERROR",0,60000,100,
};
AT_CMD_LIST_TYPE atGpsCmdListStart[sizeof(atGpsCmdStart)/sizeof(AT_CMD_PACKET_TYPE)];

const AT_CMD_PACKET_TYPE atAgpsOfflineCmdStart[] = {
	//"AT+ULSTFILE=0\r",NULL,NULL,"OK","ERROR",0,1000,100,
	//"AT+UGAOF=\"http://alp.u-blox.com/current_2d.alp\",0,1,3\r",NULL,NULL,"OK","ERROR",0,1000,100,
	//"AT+UPSDA=0,3\r",NULL,NULL,"OK","ERROR",0,1000,100,
	"AT+UGRMC=1\r",NULL,NULL,"OK","ERROR",0,1000,100,
	"AT+UGIND=1\r",NULL,NULL,"OK","ERROR",0,1000,100,
	"AT+UGPS=1,2\r",NULL,NULL,"+UUGIND: 2,0","ERROR",0,20000,100,
};
AT_CMD_LIST_TYPE atAgpsOfflineCmdListStart[sizeof(atAgpsOfflineCmdStart)/sizeof(AT_CMD_PACKET_TYPE)];

enum {
	CELL_IDLE,
	CELL_GET_DATA,
	CELL_PARSER_DATA,
	CELL_FINISH
}cellPhase = CELL_IDLE;

/*
MCC:452, MNC:01, LAC:56d6, RAC:48, CI:005b10a, DLF:10562, ULF: 9612, SC:186, RSCP LEV:25, ECN0 LEV:33

MCC:452, MNC:01, LAC:56d6, RAC:48, CI:005b10d, DLF:10587, ULF: 9637, SC:186, RSCP LEV:26, ECN0 LEV:37

MCC:452, MNC:02, LAC:4edf, RAC:db, CI:0133238, DLF:10812, ULF: 9862, SC:153, RSCP LEV:18, ECN0 LEV:33

MCC:452, MNC:02, LAC:4edf, RAC:db, CI:0133237, DLF:10812, ULF: 9862, SC:152, RSCP LEV:16, ECN0 LEV:28

MCC:452, MNC:02, LAC:4edf, RAC:db, CI:0134868, DLF:10812, ULF: 9862, SC:163, RSCP LEV:10, ECN0 LEV:16

*/

uint32_t CellInfoParser(uint16_t cnt,uint8_t c)
{
	static uint8_t dataCnt,offset;
	uint32_t t1,t2,t3,t4,t5,t6,t7,t8,t9,t10;
	if(c == 'O' || c == 'K')
		return 0;
	switch(cnt)
	{
		case 0:
			dataCnt = 0;
			offset = 0;
			cellLocationBuf[0] = 0;
			cellPhase = CELL_GET_DATA;
			memset((uint8_t *)cellInfo,0,sizeof(CELL_INFO_TYPE)*CELL_INFO_MAC);
		default:
			break;
	}
	switch(cellPhase)
	{
		case CELL_IDLE:
			if(c == 'M')
			{
				dataCnt = 1;
				cellLocationBuf[0] = c;
				cellPhase = CELL_GET_DATA;
			}
		break;
		case CELL_GET_DATA:
			if(c == '\r')
				cellPhase = CELL_PARSER_DATA;
			else if(c != ' ')
			{
				cellLocationBuf[dataCnt] = c;
				dataCnt++;
			}
		break;
		case CELL_PARSER_DATA:
			#ifdef LEON_SARA
			sscanf((char *)cellLocationBuf,"MCC:%d,MNC:%d,LAC:%x,CI:%x,BSIC:%x,Arfcn:%d,RxLev:%d",&t1,&t2,&t3,&t4,&t5,&t6,&t7);
			cellInfo[offset].mcc = t1;
			cellInfo[offset].mnc = t2;
			cellInfo[offset].lac = t3;
			cellInfo[offset].cellId = t4;
			cellInfo[offset].bsic = t5;
			cellInfo[offset].rxLev = t7;
			#else
			sscanf((char *)cellLocationBuf,"MCC:%d,MNC:%d,LAC:%x,RAC:%d,CI:%x,DLF:%d,ULF:%d,SC:%d,RSCPLEV::%d,ECN0LEV:%d",&t1,&t2,&t3,&t4,&t5,&t6,&t7,&t8,&t9,&t10);
			cellInfo[offset].mcc = t1;
			cellInfo[offset].mnc = t2;
			cellInfo[offset].lac = t3;
			cellInfo[offset].rac = t4;
			cellInfo[offset].cellId = t5;
			cellInfo[offset].dlf = t6;
			cellInfo[offset].ulf = t7;
			cellInfo[offset].sc = t8;
			cellInfo[offset].rscp_lev = t9;
			cellInfo[offset].enc0_lev = t10;
			#endif
			cellPhase = CELL_IDLE;
			#ifdef LEON_SARA
			if(cellInfo[offset].mcc != NULL 
				&& cellInfo[offset].mcc != 0xffff
				&& cellInfo[offset].mnc != 0xffff
				&& cellInfo[offset].lac != NULL 
				&& cellInfo[offset].lac != 0xffff
				&& cellInfo[offset].cellId != 0xffff
				&& cellInfo[offset].bsic != 0xffff
				&& cellInfo[offset].rxLev != 0xffff
			)
			#else
			if(cellInfo[offset].mcc != NULL 
				&& cellInfo[offset].mcc != 0xffff
				&& cellInfo[offset].mnc != 0xffff
				&& cellInfo[offset].lac != NULL 
				&& cellInfo[offset].lac != 0xffff
				&& cellInfo[offset].cellId != 0xffffffff
				&& cellInfo[offset].rac != 0xffffffff
				&& cellInfo[offset].dlf != 0xffffffff
				&& cellInfo[offset].ulf != 0xffff
				&& cellInfo[offset].rscp_lev != 0xff
				&& cellInfo[offset].enc0_lev != 0xff
			)
			#endif
			{
				offset++;
			}
			if(offset >= CELL_INFO_MAC)
			{
				ModemCmdTask_Reset();
				cellPhase = CELL_FINISH;
			}
		break;
		case CELL_FINISH:
			return 0;
		break;
	}
	return 0xff;
}



uint32_t CellLocateParser(uint16_t cnt,uint8_t c)
{
	cellLocationBuf[cnt] = c;
	if(c == '\r')
	{
		cellLocationBuf[cnt + 1] = 0;
		cellLocationFlag = 1;
		return 0;
	}
	return 0xff;
}


/*
+UULOC: 27/09/2012,18:26:13.363,21.0213870,105.8091050,0,127,0,0 
nmea_scanf((char *)buff,128,"%d/%d/%d,%d:%d:%d.%d,%f,%f,%f,%f,%f,%f",&t1,&t2,&t3,&t4,&t5,&t6,&t7,
						cellLocateType.lat,cellLocateType.lon,cellLocateType.alt,cellLocateType.uncertainty,
						cellLocateType.speed,cellLocateType.dir;
					);
*/
uint8_t ParserCellLocate(void)
{
	cellLocateType cell;
	uint32_t t1,t2,t3,t4,t5,t6,t7;
	if(cellLocationFlag)
	{
		memset(&cell,0,sizeof(cellLocateType));
		sscanf((char *)cellLocationBuf,"%d/%d/%d,%d:%d:%d.%d,%f,%f,%f,%f",&t1,&t2,&t3,&t4,&t5,&t6,&t7,
		&cell.lat,&cell.lon,&cell.alt,&cell.uncertainty);
		cell.time.mday = t1;
		cell.time.month = t2;
		cell.time.year = t3;
		cell.time.hour = t4;
		cell.time.min = t5;
		cell.time.sec = t6;	
		cellLocationFlag = 0;
		cellPass = 1;
		if((cell.time.year >= 2013) 
			&& (cell.lat <= 90 || cell.lat >= -90) 
			&& (cell.lon <= 180  || cell.lon >= -180)
		)
		{
			if(cell.uncertainty <= 5000)
			{
				cellLocate = cell;
				cellLocate.update = 1;
				return 1;
			}
		}
		cellLocate.update = 0xff;
		return 0xff;
	}
	return 0;
}



void UbloxTask_Init(void)
{
	cellGotInfoFLag = 0;
	ubloxAgpsOfflineTry =0;
	InitTimeout(&tUbloxTaskPhaseTimeOut,SYSTICK_TIME_SEC(3));
	ubloxPhase = UBLOX_IDLE;
}

UBLOX_PHASE_TYPE UbloxTask_GetPhase(void)
{
	return ubloxPhase;
}

void UbloxTask_SetPhase(UBLOX_PHASE_TYPE phase)
{
	ubloxPhase = phase;
}


uint8_t Ublox_Task(void)
{
	uint8_t i;
	DATE_TIME tempTime;
	UBLOX_PHASE_TYPE ubloxOldPhase = ubloxPhase;
	
	if(GsmTask_GetPhase() != MODEM_SYS_COVERAGE_OK)
		return 0;
	if(modemIsBusy == 1 && modemIsBusySource != AT_CMD_G100_PARSER)
		return 0;
	switch(ubloxPhase)
	{
		case UBLOX_IDLE:
			
			break;
		case GET_BATTERY_INFO:
			if (ModemCmdTask_IsIdle())
			{
				if(ModemCmdTask_SendCmd(NULL,NULL,100, modemOk, modemError,1000, 0, "AT+CBC\r") == MODEM_RES_OK)
					ubloxPhase = UBLOX_IDLE;
			}
		break;
		
		case UBLOX_ENABLE_GPRS_ONLINE:
			if(gprsEnableFlag == 0)
			{
				gprsEnableCmd = 1;
			}
			else
			{
				ubloxPhase = UBLOX_START_AGPS_ONLINE;
			}
		break;
		case UBLOX_START_AGPS_ONLINE:
			
			if(gprsEnableFlag == 0)
			{
				ubloxPhase = UBLOX_ENABLE_GPRS_ONLINE;
				break;
			}
			if (ModemCmdTask_IsIdle())
			{
				GPS_Ennable();
				for(i = 0;i < sizeof(atGpsCmdStart)/sizeof(AT_CMD_PACKET_TYPE);i++)
				{
					atGpsCmdListStart[i].pt = (AT_CMD_PACKET_TYPE *)&atGpsCmdStart[i];
					ModemCmdAdd(&atGpsCmdListStart[i]);
				}
				ubloxPhase = UBLOX_WAIT_AGPS_ONLINE;
				InitTimeout(&tUbloxTaskPhaseTimeOut, SYSTICK_TIME_SEC(30));
			}
		break;
		case UBLOX_WAIT_AGPS_ONLINE:
			if(ModemCmdCheckList_IsEmpty() && ModemCmdTask_IsIdle())
			{
				if(ModemCmdTask_GetPhase() == MODEM_CMD_OK)
				{
					ModemCmdTask_SendCmd(NULL,NULL,100, modemOk, modemError,3000, 0, "AT+UGPS=0\r");
					ubloxPhase = UBLOX_AGPS_STOP_ONLINE;
				}
				else
				{
					ModemCmdTask_SendCmd(NULL,NULL,100, modemOk, modemError,3000, 0, "AT+UGPS=0\r");
					ubloxPhase = UBLOX_AGPS_STOP_ONLINE;
				}
			}
		break;
		case UBLOX_AGPS_STOP_ONLINE:
			if(ModemCmdTask_IsIdle())
			{
				ubloxPhase = UBLOX_IDLE;
			}
		break;
		case UBLOX_DELETE_FILE_AGPS_OFFLINE:
			if(ModemCmdTask_IsIdle())
			{
				//if(ModemCmdTask_SendCmd(NULL,NULL,100, modemOk, modemError,3000, 0, "AT+UDELFILE=\"currentXX_alp.bin\"\r") == MODEM_RES_OK)
					ubloxPhase = UBLOX_START_DOWNLOAD_FILE_AGPS_OFFLINE;
			}
			break;
		case UBLOX_ENABLE_GPRS_AGPS_OFFLINE:
			if(gprsEnableFlag == 0)
			{
				gprsEnableCmd = 1;
			}
			else
			{
				ubloxPhase = UBLOX_START_DOWNLOAD_FILE_AGPS_OFFLINE;
			}
			if(CheckTimeout(&tUbloxTaskPhaseTimeOut) == SYSTICK_TIMEOUT)
			{
					ModemCmdTask_SendCmd(NULL,NULL,100, modemOk, modemError,3000, 0, "AT+UGPS=0\r");
					ubloxPhase = UBLOX_STOP_AGPS_OFFLINE;
			}
		break;
		case UBLOX_START_DOWNLOAD_FILE_AGPS_OFFLINE:
			if(gprsEnableFlag == 0)
			{
				ubloxPhase = UBLOX_ENABLE_GPRS_AGPS_OFFLINE;
				InitTimeout(&tUbloxTaskPhaseTimeOut, SYSTICK_TIME_SEC(15));
				break;
			}
			if(ModemCmdTask_IsIdle())
			{
				if(ModemCmdTask_SendCmd(NULL,NULL,100, modemOk, modemError,15000, 0,"AT+UGAOF=\"http://alp.u-blox.com/current_2d.alp\",0,1,3\r") == MODEM_RES_OK)
					ubloxPhase = UBLOX_WAIT_DOWNLOAD_FILE_AGPS_OFFLINE;
			}
		break;
		case UBLOX_WAIT_DOWNLOAD_FILE_AGPS_OFFLINE:
			if(ModemCmdTask_GetPhase() == MODEM_CMD_OK)
			{
				agpsTimeSave.value = rtcTimeSec;
				DB_U32Save(&agpsTimeSave,AGPS_TIME_SAVE_ADDR);
				
				timeSave.value = rtcTimeSec;
				DB_U32Save(&timeSave,TIME_SAVE_ADDR);
				
				ubloxPhase = UBLOX_START_AGPS_OFFLINE;
			}
			else if(ModemCmdTask_GetPhase() == MODEM_CMD_ERROR)
				ubloxPhase = UBLOX_IDLE;
		break;
		case UBLOX_START_AGPS_OFFLINE:
			TIME_FromSec(&tempTime,agpsTimeSave.value);
			if(rtcTimeSec - agpsTimeSave.value >= 24*3600 || tempTime.mday != sysTime.mday) //24hour
			{
				ubloxPhase = UBLOX_DELETE_FILE_AGPS_OFFLINE;
				break;
			}
			if (ModemCmdTask_IsIdle())
			{
				GPS_Ennable();
				for(i = 0;i < sizeof(atAgpsOfflineCmdStart)/sizeof(AT_CMD_PACKET_TYPE);i++)
				{
					atAgpsOfflineCmdListStart[i].pt = (AT_CMD_PACKET_TYPE *)&atAgpsOfflineCmdStart[i];
					ModemCmdAdd(&atAgpsOfflineCmdListStart[i]);
				}
				ubloxPhase = UBLOX_WAIT_AGPS_OFFLINE;
				InitTimeout(&tUbloxTaskPhaseTimeOut, SYSTICK_TIME_SEC(30));
			}
			
		break;
		case UBLOX_WAIT_AGPS_OFFLINE:
			if(ModemCmdCheckList_IsEmpty() && ModemCmdTask_IsIdle())
			{
				if(ModemCmdTask_GetPhase() == MODEM_CMD_ERROR)
				{
					ubloxAgpsOfflineTry++;
					if(ubloxAgpsOfflineTry > 1)
					{
						ubloxAgpsOfflineTry = 0;
						ubloxPhase = UBLOX_STOP_AGPS_OFFLINE;
					}
					else
					{
						ubloxPhase = UBLOX_DELETE_FILE_AGPS_OFFLINE;
					}
					ModemCmdTask_SendCmd(NULL,NULL,100, modemOk, modemError,3000, 0, "AT+UGPS=0\r");
					break;
				}
				if(nmeaInfo.sig)
				{
					timeSave.value = rtcTimeSec;
					DB_U32Save(&timeSave,TIME_SAVE_ADDR);
					if(ModemCmdTask_SendCmd(NULL,NULL,100, modemOk, modemError,3000, 0, "AT+UGPS=0\r") == MODEM_RES_OK)
						ubloxPhase = UBLOX_STOP_AGPS_OFFLINE;	
				}
				if(CheckTimeout(&tUbloxTaskPhaseTimeOut) == SYSTICK_TIMEOUT)
				{
					if(ModemCmdTask_SendCmd(NULL,NULL,100, modemOk, modemError,3000, 0, "AT+UGPS=0\r") == MODEM_RES_OK)
						ubloxPhase = UBLOX_STOP_AGPS_OFFLINE;
				}
			}
		break;
		case UBLOX_STOP_AGPS_OFFLINE:
			if(ModemCmdTask_IsIdle())
			{
				ubloxPhase = UBLOX_IDLE;
			}
		break;
		case UBLOX_GET_CELL_INFO:
			if(ModemCmdTask_SendCmd(CellInfoParser,"AT+COPS=5\r\r\n",100, modemOk, modemError, 30000, 0, "AT+COPS=5\r") == MODEM_RES_OK)
			{
				cellGotInfoFLag = 0;
				ubloxPhase = UBLOX_WAIT_GET_CELL_INFO;
			}
		case UBLOX_WAIT_GET_CELL_INFO:
			if(ModemCmdTask_IsIdle())
			{
				COMM_Putc(0x1A);
				COMM_Putc(0x1A);
				if(ModemCmdTask_SendCmd(NULL,NULL,100, modemOk, modemError, 2000, 1, "AT+COPS=0,2\r") == MODEM_RES_OK)
				{
					ubloxPhase = UBLOX_FINISH_GET_CELL_INFO;
				}
			}
		break;
		case UBLOX_FINISH_GET_CELL_INFO:
			if(ModemCmdTask_IsIdle())
			{
					cellGotInfoFLag = 1;
					ubloxPhase = UBLOX_IDLE;
			}
		break;
	}
	if(ubloxPhase != UBLOX_IDLE)
	{
		modemIsBusy = 1;
		modemIsBusySource = AT_CMD_G100_PARSER;
	}
	else if(modemIsBusy && modemIsBusySource == AT_CMD_G100_PARSER)
	{
		modemIsBusy = 0;
	}
	if((ubloxOldPhase != UBLOX_IDLE && ubloxPhase == UBLOX_IDLE))
		modemIsBusy = 0;
	
	if((atCmdCallParser.incomingCall || VoiceCallTask_GetPhase() != CALL_IDLE) && modemIsBusy)
	{
		modemIsBusy = 0;
		ubloxPhase = UBLOX_IDLE;
		UbloxTask_Init();
	}
	
	return 0;
}


