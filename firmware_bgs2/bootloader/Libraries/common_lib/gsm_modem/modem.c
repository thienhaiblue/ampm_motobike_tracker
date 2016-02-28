
#include "modem.h"
#include "gps.h"
#include "uart1.h"
#include "at_command_parser.h"

#define GSM_RING_PIN		1<<8
#define GSM_RING_PORT		2


#define MODEM_RI()				

#define MODEM_DCD()					//((GPIO_ReadValue(2) >> 3) & (1))

#define MODEM_Status()		

static void GetIMEI(uint8_t *buf, uint16_t bufSize);

uint8_t modemId[16];
volatile uint8_t modemRinging = 0;
volatile int8_t  modemStarted = 0;
uint8_t csqBuff[16];
float csqValue = 0;

void MODEM_FlushBuffer(void);


uint8_t MODEM_Init(void)
{
	uint8_t numTry,i;
	MODEM_Info("\r\nGSM->TURNING ON...\r\n");
	POWER_PIN_SET_OUTPUT;
	RESET_PIN_SET_OUTPUT;
	RTS_PIN_SET_OUTPUT;
	RTS_PIN_CLR;
	
	RESET_PIN_SET; 
	
	POWER_PIN_SET;    // Turn on GSM
	DelayMs(5);             // Delay 5ms
	POWER_PIN_CLR;      // Turn off GSM
	DelayMs(200);            // Delay 200ms
	POWER_PIN_SET;     // Turn on GSM

	// ---------- Turn on GPRS module ----------
	RESET_PIN_CLR;    // Reset GSM
	DelayMs(200);           // Delay 200ms
	RESET_PIN_SET;    // Start GSM module (Release reset)  
	
	MODEM_FlushBuffer();
	COMM_Puts("AT\r");
	if(MODEM_CheckResponse("OK", 3000))
	{
		MODEM_Info("\r\nGSM->NOT RESPONSE\r\n");
		numTry = 10;
		while(1)
		{
			watchdogFeed[WTD_MAIN_LOOP] = 0;
			MODEM_Info("\r\nGSM->AT\r\n");
			COMM_Puts("AT\r");	
			if(!MODEM_CheckResponse("OK", 3000)) break;
			numTry--;
			if(numTry == 0)
			{
				MODEM_Info("\r\nGSM->RETURN\r\n");
				return 0xff;
			}
		}
	}

	MODEM_Info("GSM->AT:OK\n");
	// clear all configs
	MODEM_Info("\r\nGSM->CONFIGS CLEAR\r\n");
	COMM_Puts("ATZ\r");
	if(MODEM_CheckResponse("OK", 3000))
	{
		MODEM_Info("\r\nGSM->FAILS\r\n");
		MODEM_Info("\r\nGSM->RETURN\r\n");
		return 0xff;
	}
	//LED network status indication enable
	MODEM_Info("\r\nGSM->ENABLE STATUS LED\r\n");
	COMM_Puts("AT+UGPIOC=20,2\r");
	if(MODEM_CheckResponse("OK", 3000))
	{
		MODEM_Info("\r\nGSM->FAILS\r\n");
		MODEM_Info("\r\nGSM->RETURN\r\n");
		return 0xff;
	}
	
	MODEM_Info("\r\nGSM->DTE DISABLE\r\n");
	COMM_Puts("AT&K0\r");
	if(MODEM_CheckResponse("OK", 3000))
	{
		MODEM_Info("\r\nGSM-> DTE DISABLE FAILS\r\n");
		MODEM_Info("\r\nGSM->RETURN\r\n");
		return 0xff;
	}
	MODEM_Info("\r\nGSM->POWEWER SAVE MODE ENABLE:CONTROL BY RTS PIN\r\n");
	COMM_Puts("AT+UPSV=2\r");
	if(MODEM_CheckResponse("OK", 3000))
	{
		MODEM_Info("\r\nGSM-> UPSV FAILS\r\n");
		MODEM_Info("\r\nGSM->RETURN\r\n");
		return 0xff;
	}
	// Set SMS format to text mode
	MODEM_Info("\r\nGSM->SMS SET TEXT MODE\r\n");
	COMM_Puts("AT+CMGF=1\r");
	if(MODEM_CheckResponse("OK", 3000))
	{
		MODEM_Info("\r\nGSM-> AT+CMGF=1 FAILS\r\n");
		MODEM_Info("\r\nGSM->RETURN\r\n");
		return 0xaa; //loi SIM
	}
	// Set SMS DISPLAY  mode
	MODEM_Info("\r\nGSM->SET SMS DISPLAYMODE\r\n");
	COMM_Puts("AT+CNMI=2,1,0,0,0\r");
	if(MODEM_CheckResponse("OK", 3000)){
		MODEM_Info("\r\nGSM-> AT+CNMI=2,1,0,0,0 FAILS\r\n");
		MODEM_Info("\r\nGSM->RETURN\r\n");
		return 0xff;
	}
	// give the modem 20s to ready for dialing
	COMM_Puts("AT+CREG?\r");
	if(MODEM_CheckResponse("+CREG: 0,1", 3000))
	{
		MODEM_Info("\r\nNOT RESPONSE\r\n");
		numTry = 120;
		MODEM_Info("\r\nGSM->CGREG :**");
		while(1)
		{
			watchdogFeed[WTD_MAIN_LOOP] = 0;
			MODEM_Info("**");
			COMM_Puts("AT+CREG?\r");	
			if(!MODEM_CheckResponse("+CREG: 0,1", 3000)) break;
			numTry--;
			if(numTry == 0)	
			{
				MODEM_Info("\r\nGSM->RETURN\r\n");
				return 0xff;
			}
		}
	}
	MODEM_Info("\r\nGSM->CGREG:OK\r\n");
 	// read modem IMEI
	GetIMEI(modemId, sizeof modemId);
	MODEM_Info("GSM-> IMEI:%s\n", modemId);

	if(strlen((char *)modemId) == 0) return 0xff;

	if(strcmp((char *)sysCfg.imei, (char *)modemId))
	{
		strcpy((char *)sysCfg.imei, (char *)modemId);
		strcpy((char *)sysCfg.id, (char *)modemId);
		CFG_Save();
	}
	MODEM_Info("\r\nGSM->AT+CLIP=1->(SHOW COMING PHONE NUMBER)\r\n");
	COMM_Puts("AT+CLIP=1\r");
	if(MODEM_CheckResponse("OK", 3000)){
		MODEM_Info("\r\nGSM-> AT+CLIP=1 FAILS\r\n");
		MODEM_Info("\r\nGSM->RETURN\r\n");
		return 0xff;
	}
	DelayMs(1000);
	MODEM_GetCSQ();
	//SendSMS("0978779222","thienhaiblue");
	InternetSetProfiles((uint8_t *)sysCfg.gprsApn,(uint8_t *)sysCfg.gprsUsr,(uint8_t *)sysCfg.gprsPwd);
	DelayMs(3000);
	i = 5;
	while(i)
	{	
		if(MODEM_EnableGPRS())	
		{
			MODEM_DisableGPRS();
			i--;
		}
		else
		{
			break;
		}
	}
	if(i == 0) return 0xff;
	//SendSMS("0978779222","thienhaiblue");
	return 0;
}

uint8_t MODEM_GprsSendData(uint8_t socket,uint8_t *data,uint32_t length)
{
	COMPARE_TYPE cmp1;
	Timeout_Type tOut;
	uint8_t buf[32],c;
	uint32_t i;
	uint32_t tryNum = 5;
	MODEM_FlushBuffer();
	sprintf((char *)buf, "AT+USOWR=%d,%d\r",socketMask[socket],length);
	
	for(;;)
	{	
		DelayMs(100);
		COMM_Puts(buf);
		InitTimeout(&tOut,TIME_MS(5000));
		InitFindData(&cmp1,"@");
		while(CheckTimeout(&tOut))
		{
			if(RINGBUF_Get(&commBuf,&c) == 0)
			{
				if(FindData(&cmp1,c) == 0)
				{
					break;
				}
			}
		}
		if(CheckTimeout(&tOut) == TIMEOUT)
		{
			tryNum --;
			if(tryNum == 0) 
			{
				tcpSocketStatus[socket] = SOCKET_CLOSE;
				return 0xff;
			}
			continue;
		}
		i = length;
		for(i = 0; i < length ;i++)
		{
			COMM_Putc(data[i]);
		}
		InitTimeout(&tOut,TIME_MS(5000));
		InitFindData(&cmp1,"OK");
		while(CheckTimeout(&tOut))
		{
			if(RINGBUF_Get(&commBuf,&c) == 0)
			{
				if(FindData(&cmp1,c) == 0)
				{
					MODEM_Info("\r\nGSM->%s OK\r\n",buf);
					DelayMs(100);
					return 0;
				}
			}
		}
		MODEM_Info("\r\nGSM->%s FAILS\r\n",buf);
		tryNum --;
		if(tryNum == 0) 
		{
			tcpSocketStatus[socket] = SOCKET_CLOSE;
			return 0xff;
		}
	}
}
/*

Enable GPRS funtion
AT+UPSDA=0,3\r
*/
uint8_t MODEM_EnableGPRS(void)
{
	DelayMs(100);
	MODEM_FlushBuffer();
	MODEM_Info("\r\nGSM->ENABLE GPRS FUNTION\r\n");
	COMM_Puts("AT+UPSDA=0,3\r");
	if(MODEM_CheckResponse("OK", 5000)){
		MODEM_Info("\r\nGSM-> ENABLE GPRS FUNTION FAILS\r\n");
		return 0xff;
	}
	return 0;
}


uint8_t MODEM_DisableGPRS(void)
{
	DelayMs(100);
	MODEM_FlushBuffer();
	MODEM_Info("\r\nGSM->DISABLE GPRS FUNTION\r\n");
	COMM_Puts("AT+UPSDA=0,4\r");
	if(MODEM_CheckResponse("OK", 5000)){
		MODEM_Info("\r\nGSM-> DISABLE GPRS FUNTION\r\n");
		return 0xff;
	}
	return 0;
}

uint8_t MODEM_CreateSocket(uint8_t socket)
{
	COMPARE_TYPE cmp1,cmp2;
	Timeout_Type tOut;
	uint8_t c;
	DelayMs(100);
	MODEM_FlushBuffer();
	socketMask[socket] = 0xff;
	createSocketNow = socket;
	MODEM_Info("\r\nGSM->CREATE SOCKET\r\n");
	COMM_Puts("AT+USOCR=6\r");
	InitFindData(&cmp1,"OK");
	InitFindData(&cmp2,"ERROR");
	InitTimeout(&tOut,TIME_MS(3000));
	while(CheckTimeout(&tOut))
	{
		if(RINGBUF_Get(&commBuf,&c) == 0)
		{
			if(FindData(&cmp1,c) == 0)
			{
				if((socketMask[socket] == 0xff)) 
				{
					break;
				}	
				MODEM_Info("\r\nGSM->CREATE SOCKET OK\r\n");
				return 0;
			}
			if(FindData(&cmp2,c) == 0)
			{
				break;
			}
		}
	}
	MODEM_Info("\r\nGSM->CREATE SOCKET FAILS\r\n");
	return 0xff;
}
uint16_t MODEM_CheckGPRSDataOut(uint8_t socket)
{
	COMPARE_TYPE cmp1,cmp2;
	Timeout_Type tOut;
	uint8_t buf[32],c;
	DelayMs(100);
	MODEM_FlushBuffer();
	MODEM_Info("\r\nGSM->CHECK OUTCOMING UNACK SOCKET:%d\r\n",socket);
	sprintf((char *)buf, "AT+USOCTL=%d,11\r",socketMask[socket]);
	COMM_Puts(buf);
	InitFindData(&cmp1,"OK");
	InitFindData(&cmp2,"ERROR");
	InitTimeout(&tOut,TIME_MS(1000));
	while(CheckTimeout(&tOut))
	{
		if(RINGBUF_Get(&commBuf,&c) == 0)
		{
			if(FindData(&cmp1,c) == 0)
			{
				MODEM_Info("\r\nGPRS->UNACK:%dBytes\r\n",GPRS_dataUnackLength[socket]);
				return GPRS_dataUnackLength[socket];
			}
			if(FindData(&cmp2,c) == 0)
			{
				MODEM_Info("\r\nGPRS->ERROR\r\n");
				//tcpSocketStatus[socket] = SOCKET_CLOSE;
				break;
			}
		}
	}
	//num of data will check in at_command_parser.c 
	// return GPRS_dataUnackLength
	MODEM_Info("\r\nGPRS->UNACK:%dBytes\r\n",GPRS_dataUnackLength[socket]);
	return 0xffff;
}
uint8_t MODEM_CloseSocket(uint8_t socket)
{
	COMPARE_TYPE cmp1,cmp2;
	Timeout_Type tOut;
	uint8_t c;
	uint8_t buf[32];
	DelayMs(100);
	MODEM_FlushBuffer();
	MODEM_Info("\r\nGSM->CLOSE SOCKET:%d\r\n",socket);
	sprintf((char *)buf, "AT+USOCL=%d\r",socket);
	COMM_Puts(buf);
	InitFindData(&cmp1,"OK");
	InitFindData(&cmp2,"ERROR");
	InitTimeout(&tOut,TIME_MS(3000));
	while(CheckTimeout(&tOut))
	{
		if(RINGBUF_Get(&commBuf,&c) == 0)
		{
			if(FindData(&cmp1,c) == 0)
			{
				//MODEM_Info("\r\nGSM->CLOSE SOCKET OK\r\n");
				return 0;
			}
			if(FindData(&cmp2,c) == 0)
			{
				break;
			}
		}
	}
	//MODEM_Info("\r\nGSM->CLOSE SOCKET FAILS\r\n");
	return 0xff;
}
/*
Connect socket
AT+USOCO=0,"220.128.220.133",2020
*/
// uint8_t cellBuf[128];
// uint8_t GetCellLocate(uint8_t *buff)
// {
// 	Timeout_Type tOut;
// 	uint8_t c,i;
// 	uint8_t buf[32];
// 	DelayMs(100);
// 	MODEM_FlushBuffer();
// 	MODEM_Info("\r\nGSM->GET CELL LOCATE\r\n");
// 	sprintf((char *)buff, "AT+ULOC=2,2,1,120,1\r");
// 	COMM_Puts(buff);
// 	InitTimeout(&tOut,TIME_SEC(3));
// 	i = 0;
// 	while(CheckTimeout(&tOut))
// 	{
// 		if(RINGBUF_Get(&commBuf,&c) == 0)
// 		{
// 			buff[i++] = c;		
// 		}
// 	}
// 	MODEM_Info("\r\nGSM->%s\r\n",cellBuf);
// 	return 0xff;
// }

cellLocateType cellLocate;

/*
+UULOC: 27/09/2012,18:26:13.363,21.0213870,105.8091050,0,127,0,0 
nmea_scanf((char *)buff,128,"%d/%d/%d,%d:%d:%d.%d,%f,%f,%f,%f,%f,%f",&t1,&t2,&t3,&t4,&t5,&t6,&t7,
						cellLocateType.lat,cellLocateType.lon,cellLocateType.alt,cellLocateType.uncertainty,
						cellLocateType.speed,cellLocateType.dir;
					);
*/
uint8_t GetCellLocate(uint8_t *buff)
{
	COMPARE_TYPE cmp1;
	Timeout_Type tOut;
	uint8_t c,i;
	uint32_t t1,t2,t3,t4,t5,t6,t7;
	DelayMs(100);
	MODEM_FlushBuffer();
	MODEM_Info("\r\nGSM->GET CELL LOCATE\r\n");
	sprintf((char *)buff, "AT+ULOC=2,2,1,500,1\r");
	COMM_Puts(buff);
	InitFindData(&cmp1,"+UULOC: ");
	InitTimeout(&tOut,TIME_SEC(3));
	i = 0;
	while(CheckTimeout(&tOut))
	{
		if(RINGBUF_Get(&commBuf,&c) == 0)
		{
			if(FindData(&cmp1,c) == 0)
			{
				i = 1;
				break;
			}
		}
	}
	if(i)
	{	
		InitFindData(&cmp1,"\r\n");
		InitTimeout(&tOut,TIME_SEC(3));
		i = 0;
		while(CheckTimeout(&tOut))
		{
			if(RINGBUF_Get(&commBuf,&c) == 0)
			{
				buff[i++] = c;
				if(FindData(&cmp1,c) == 0)
				{
					nmea_scanf((char *)buff,128,"%d/%d/%d,%d:%d:%d.%d,%f,%f,%f,%f,%f,%f",&t1,&t2,&t3,&t4,&t5,&t6,&t7,
						&cellLocate.lat,&cellLocate.lon,&cellLocate.alt,&cellLocate.uncertainty,
						&cellLocate.speed,&cellLocate.dir);
					cellLocate.ew = 'E';
					cellLocate.ns = 'N';
					if(cellLocate.lat < 0) cellLocate.ns = 'S';
					if(cellLocate.lon < 0) cellLocate.ew = 'W';
					cellLocate.lon = neamFormatLatLng(cellLocate.lon);
					cellLocate.lat = neamFormatLatLng(cellLocate.lat);
					cellLocate.time.mday = t1;
					cellLocate.time.month = t2;
					cellLocate.time.year = t3;
					cellLocate.time.hour = t4;
					cellLocate.time.min = t5;
					cellLocate.time.sec = t6;
					return 0;
				}
			}
		}
	}
	return 0xff;
}


uint8_t MODEM_ConnectSocket(uint8_t socket,uint8_t *ip,uint16_t port,uint8_t useIp)
{
	COMPARE_TYPE cmp1,cmp2;
	Timeout_Type tOut;
	uint8_t c,i;
	uint8_t buf[32];
	DelayMs(100);
	MODEM_FlushBuffer();
	socketMask[socket] = 0;
	for(i = 0; i < SOCKET_NUM;)
	{
		if(tcpSocketStatus[i] == SOCKET_OPEN)
		{
			if((socketMask[i] == socketMask[socket]) && (i != socket))
			{
				socketMask[socket]++;
				i = 0;
				continue;
			}
		}
		i++;
	}
	MODEM_CloseSocket(socketMask[socket]);
	if(MODEM_CreateSocket(socket)) return 0xff;
	DelayMs(500);
	if(useIp == 0)
	{
		MODEM_Info("\r\nGSM->GET DOMAIN\r\n");
		sprintf((char *)buf, "AT+UDNSRN=0,\"%s\"\r",ip);
		COMM_Puts(buf);
		if(MODEM_CheckResponse("OK", 5000))
		{
			MODEM_Info("\r\nGSM->GET DOMAIN FAILS\r\n");
			return 0xff;
		}
		ip = DNS_serverIp;
	}
	MODEM_Info("\r\nGSM->IP:%s\r\n",ip);
	MODEM_Info("\r\nGSM->CONNECT SOCKET:%d:%s:%d\r\n",socketMask[socket],ip,port);
	sprintf((char *)buf, "AT+USOCO=%d,\"%s\",%d\r",socketMask[socket],ip,port);
	COMM_Puts(buf);
	InitFindData(&cmp1,"OK");
	InitFindData(&cmp2,"ERROR");
	InitTimeout(&tOut,TIME_SEC(10));
	while(CheckTimeout(&tOut))
	{
		if(RINGBUF_Get(&commBuf,&c) == 0)
		{
			if(FindData(&cmp1,c) == 0)
			{
				MODEM_Info("\r\nGSM->CONNECT SOCKET OK\r\n");
				return 0;
			}
			if(FindData(&cmp2,c) == 0)
			{
				break;
			}
		}
	}
	MODEM_Info("\r\nGSM->CONNECT SOCKET FAILS\r\n");
	return 0xff;
}

uint8_t VoiceSetup(void)
{
	DelayMs(500);
	COMM_Puts("AT+USPM=0,3,0,0\r");
	if(MODEM_CheckResponse("OK",3000))
		return 0xff;
	DelayMs(500);
	COMM_Puts("AT+UMGC=0,12,12000\r");
	if(MODEM_CheckResponse("OK",3000))
		return 0xff;
	DelayMs(500);
	COMM_Puts("AT+CRSL=3\r");
	if(MODEM_CheckResponse("OK",3000))
		return 0xff;
	DelayMs(500);
	COMM_Puts("AT+USGC=3,0,0,16384,24576,16384\r");
	if(MODEM_CheckResponse("OK",3000))
		return 0xff;
	DelayMs(500);
	return 0;
}

uint8_t CallingProcess(uint8_t callAction)
{
	if(callAction)
	{
		if(VoiceSetup())
			return 0xff;
		COMM_Puts("ATA\r");
		if(MODEM_CheckResponse("OK",3000))
			return 0xff;
	}
	else
	{
		COMM_Puts("ATH\r");
		if(MODEM_CheckResponse("OK",3000))
			return 0xff;
	}
	return 0;
}

uint8_t InternetSetProfiles(uint8_t *apn,uint8_t *usr,uint8_t *pwd)
{	
	uint8_t buf[32];
	DelayMs(100);
	MODEM_FlushBuffer();
	MODEM_Info("\r\nGSM->CLEAR APN PROFILE\r\n");
	COMM_Puts("AT+UPSD=0,0\r");
	if(MODEM_CheckResponse("OK", 1000)){
		MODEM_Info("\r\nGSM-> AT+UPSD=0,0 FAILS\r\n");
		return 0xff;
	}
	
	MODEM_Info("\r\nGSM->SET APN PROFILE\r\n");
	sprintf((char *)buf, "AT+UPSD=0,1,\"%s\"\r", apn);
	COMM_Puts(buf);
	if(MODEM_CheckResponse("OK", 1000)){
		MODEM_Info("\r\nGSM-> %s FAILS\r\n",buf);
		return 0xff;
	}
	
	MODEM_Info("\r\nGSM->SET USE NAME\r\n");
	sprintf((char *)buf, "AT+UPSD=0,2,\"%s\"\r", usr);
	COMM_Puts(buf);
	if(MODEM_CheckResponse("OK", 1000)){
		MODEM_Info("\r\nGSM-> %s FAILS\r\n",buf);
		return 0xff;
	}
	
	MODEM_Info("\r\nGSM->SET PASSWORD\r\n");
	sprintf((char *)buf, "AT+UPSD=0,3,\"%s\"\r", pwd);
	COMM_Puts(buf);
	if(MODEM_CheckResponse("OK", 1000)){
		MODEM_Info("\r\nGSM-> %s FAILS\r\n",buf);
		return 0xff;
	}
	return 0;
}



void MODEM_GetIMEI(uint8_t *imei)
{
	strcpy((char *)imei, (char *)modemId);
}

/**
* \brief hangup a call
*/


void MODEM_Hangup(void)
{
	//MODEM_CommandMode();
	DelayMs(100);
	COMM_Puts("ATH\r");
	MODEM_CheckResponse("OK", 2000);
}


void MODEM_FlushBuffer(void)
{
	uint8_t c;
	while(RINGBUF_Get(&commBuf, &c)==0);
}


void MODEM_DataMode(void)
{	
	DelayMs(100);
	COMM_Puts("ATO\r");
	MODEM_CheckResponse("CONNECT", 10000);
	MODEM_FlushBuffer();
}




void MODEM_EndRing(void)
{
	modemRinging = 0;
}


uint8_t MODEM_GetCSQ(void)
{
	uint8_t i,c;
	DelayMs(100);
	MODEM_FlushBuffer();
	COMM_Puts("AT+CSQ\r");				// cancel calling if busy.
	if(!MODEM_CheckResponse("+CSQ:", 2000))
	{
		i = 0;
		while(RINGBUF_Get(&commBuf,&c) == 0)
		{
				if((c == '\r') || (c == '\n'))
					break;
				csqBuff[i] = c;
				i++;
				if(i >= sizeof(csqBuff)) 
				{
					break;		
				}
		}
		csqBuff[i] = '\0';
		sscanf((char *)csqBuff,"%f",&csqValue);
		return 0;
	}
	return 0xff;
}

uint8_t MODEM_Started(void)
{
	return modemStarted;
}

// is Carrier Detected
uint8_t MODEM_CarrierDetected(void)
{
	return 0;
}
uint8_t SendSMS(const uint8_t *recipient, const uint8_t *data)
{
	uint8_t buf[32];
	uint8_t i;
	DelayMs(100);
	MODEM_FlushBuffer();
	MODEM_Info("\r\nGSM->SMS SEND TO %s...\r\n", recipient);
	MODEM_Info("\r\nGSM->DATA:");
	sprintf((char *)buf, "AT+CMGS=\"%s\"\r", recipient);
	COMM_Puts(buf);
	if(MODEM_CheckResponse("> ", 20000))
	{
		MODEM_Info("\r\nGSM->SMS SEND FAILS\r\n");
		return 0xff;
	}
	
	for(i = 0;i < 160;i++)
	{
		if(data[i] == '\0')
			break;
		//DbgCfgPutChar(data[i]);
		COMM_Putc(data[i]);
	}
	COMM_Putc(0x1A);
	
	if(MODEM_CheckResponse("OK", 10000)){
		MODEM_Info("\r\nGSM->SMS SEND FAILS\r\n");
		return 0xff;
	}
	MODEM_Info("\r\n");
	MODEM_Info("\r\nOK\r\n");
	
	return 0;
}
uint8_t MODEM_SendSMS(const uint8_t *recipient, uint8_t *data)
{
// 	uint8_t buf[161];
// 	uint16_t len;
// 	len = strlen((char *)data);
// 	while(len)
// 	{
// 		if(len > 160) len = 160;
// 		strncpy((char *)buf, (char *)data, len);
// 		data += len;
// 		buf[len] = 0;
		data[160] = 0;
		MODEM_Info("\r\nGSM->SMS:%s\r\n",data);
		if(SendSMS(recipient, data)) return 0xff;
//		len = strlen((char *)data);
	return 0;
}

uint8_t MODEM_CheckResponse(uint8_t *str,uint32_t t)
{
	COMPARE_TYPE cmp;
	uint8_t c;
	InitFindData(&cmp,str);
	while(t--)
	{
		DelayMs(1);
		if(RINGBUF_Get(&commBuf,&c) == 0)
		{
			if(FindData(&cmp,c) == 0)
			{
				return 0;
			}
		}
	}
	return 0xff;
}
// wait for reponse from modem within specific time out
// maximum waiting period = (time / 10) seconds
uint8_t MODEM_Wait(const uint8_t *res, uint32_t time)
{
	uint8_t c, i=0;
	time *= 100;
	while(time--){
		DelayMs(1);
		while(RINGBUF_Get(&commBuf, &c)==0){
			if(c==res[i]){
				if(res[++i]==0)return 1;
			}else if(c==res[0]) i = 1;
			else i = 0;
		}
	}
	
	return 0;
}
uint8_t MODEM_Gets(uint8_t *buf, uint16_t bufSize, uint8_t match)
{
	uint8_t c;
	uint16_t i = 0;
	
	if(bufSize <= 1 || buf == NULL) return 0;
	
	while(RINGBUF_Get(&commBuf, &c)==0){
		if(c == match) break;
		if(buf != NULL)
			buf[i++] = c;
		if(i>=bufSize-1) break;
	}
	
	buf[i] = 0;
	
	return i;
}
uint8_t MODEM_GetCellId(uint32_t *lac, uint32_t *ci)
{
	uint8_t tmp[16];
	uint8_t tmp2[16];
	
	MODEM_CheckResponse("OK", 1000);
	MODEM_FlushBuffer();
	
	COMM_Puts("AT+CREG?\r");
	if(MODEM_CheckResponse("+CREG: 2,1,\"", 1000)) return 0xff;
	
	DelayMs(100);
	MODEM_Gets(tmp, sizeof tmp, '\"');
	
	//if(MODEM_CheckResponse(",\"", 1000)) return 0xff;	  //thienhailbue  note:need modifi to LPC17xx
	
	MODEM_Gets(tmp2, sizeof tmp2, '\"');
	
	if(lac != NULL)
		sscanf((char *)tmp, "%X", lac);
		
	if(ci != NULL)
		sscanf((char *)tmp2, "%X", ci);
	
	return 0;
}
uint8_t MODEM_GetOperatorName(uint8_t *buf, uint16_t bufSize)
{
	uint8_t c;
	
	COMM_Puts("AT+COPS?\r");
//	if(MODEM_CheckResponse("+COPS: 0,0,\"", 5000) == 0) return 0xff;		 //thienhailbue  note:need modifi to LPC17xx
	DelayMs(1000);
	
	while(RINGBUF_Get(&commBuf, &c)==0){
		if(c == '\"' || bufSize-- == 1){
			*buf = 0;
			// buffer should be empty
			MODEM_CheckResponse("OK", 1000);
			return 0;
		}
		*buf++ = c;
	}
	
	return 0xff;
}
uint32_t MODEM_CheckAccount(void)
{
	uint8_t c, state=0;
	uint8_t buf[16], *pbuf = buf;
	
	COMM_Puts("ATD*101#\r");
	if(!MODEM_CheckResponse("+CUSD:", 5000) == 0) return 0;
	DelayMs(1000);
	
	while(RINGBUF_Get(&commBuf, &c)==0){
		switch(state){
			case 0:
				if(c == 'a') state ++;
				break;
			case 1:
				if(c == ' ') state ++;
				break;
			case 2:
				if(c == ' '){
					*pbuf = 0;
					// buffer should be empty
					MODEM_CheckResponse("OK", 1000);
					return atol((char *)buf);
				}
				*pbuf++ = c;
				break;
		}
	}
	
	return 0;
}



static void GetIMEI(uint8_t *buf, uint16_t bufSize)
{	
	MODEM_CheckResponse("OK", 1000);
	MODEM_FlushBuffer();
	
	COMM_Puts("AT+CGSN\r");
	MODEM_CheckResponse("\n", 1000);
	DelayMs(100);
	
	MODEM_Gets(buf, bufSize, '\r');
	
	MODEM_CheckResponse("OK", 1000);
}




