/**********************************************************************
Name: Hai Nguyen Van
Cellphone: (84) 97-8779-222
Mail:thienhaiblue@ampm.com.vn 
----------------------------------
AMPM ELECTRONICS EQUIPMENT TRADING COMPANY LIMITED.,
Add: 634/6 Phan Van Suu street , Ward 13, Tan Binh District, HCM City, VN

*********************************************************************/

#include "app_config_task.h"
#include "uart_config_task.h"
#include "sst25.h"
#include "lib/sys_tick.h"
#include "uart3.h"

#define FLASH_PACKET_SIZE 32

uint32_t firmwareFileCrc;
uint32_t firmwareFileSize;
uint32_t firmwareFileOffSet;
uint32_t firmwareFileOffSetOld;
uint32_t firmwareSendFileOffSetFlag = 0;
uint32_t firmwareSaveFileOffsetCnt = 0;
uint32_t firmwareStatus = 0xFFFFFFFF;


Timeout_Type tFirmwareTryToUpdate;

uint8_t CfgCalcCheckSum(uint8_t *buff, uint32_t length);

uint8_t configTaskBuf[1024];
RINGBUF configTaskRingBuf;

LOG_TYPE logSource = NO_LOG;


void LoadFirmwareFile(void)
{
	uint8_t buff[8];
	int32_t i;
	uint32_t *u32Pt = (uint32_t *)buff;
	SST25_Read(FIRMWARE_INFO_ADDR + FIRMWARE_STATUS_OFFSET*4,buff,4);
	firmwareStatus = *u32Pt;
	if(firmwareStatus == 0xA5A5A5A5)
	{
		SST25_Read(FIRMWARE_INFO_ADDR + FIRMWARE_FILE_SIZE_OFFSET*4,buff, 4);
		firmwareFileSize = *u32Pt;
		if(firmwareFileSize >= 128*1024*1024)
		{
			firmwareFileOffSet = 0;
			firmwareFileSize = 0;
			firmwareSaveFileOffsetCnt =0;
		}
		else
		{
			SST25_Read(FIRMWARE_INFO_ADDR+FIRMWARE_CRC_OFFSET*4,buff,4);
			firmwareFileCrc = *u32Pt;
			if(firmwareFileCrc == 0xffffffff)
			{
				firmwareStatus   = 0;
				firmwareFileOffSet = 0;
				firmwareFileOffSetOld = 0;
				firmwareFileSize = 0;
				firmwareSaveFileOffsetCnt =0;
				return;
			}
			firmwareFileOffSetOld = 0;
			firmwareFileOffSet = 0;
		}
	}
	else
	{
		firmwareFileOffSet = 0;
		firmwareFileOffSetOld = 0;
		firmwareFileSize = 0;
		firmwareSaveFileOffsetCnt =0;
	}
}

void AppConfigTaskInit(void)
{
	RINGBUF_Init(&configTaskRingBuf,configTaskBuf,sizeof(configTaskBuf)); 
	InitTimeout(&tFirmwareTryToUpdate,SYSTICK_TIME_SEC(1));
}

uint32_t  DbgCfgPrintf(const uint8_t *format, ...)
{
	static  uint8_t  buffer[256];
	uint32_t len,i;
	va_list     vArgs;		    
	va_start(vArgs, format);
	len = vsprintf((char *)&buffer[0], (char const *)format, vArgs);
	va_end(vArgs);
	if(len >= 255) len = 255;
	for(i = 0;i < len; i++)
	{
		if(logSource != NO_LOG)
		{
			RINGBUF_Put(&configTaskRingBuf, buffer[i]);
			RINGBUF_Get(&configTaskRingBuf, &buffer[i]);
			//USART3_PutChar(buffer[i]);
		}
	}
	return 0;
}



uint8_t CfgProcessData(CFG_PROTOCOL_TYPE *cfgProtoRecv,CFG_PROTOCOL_TYPE *cfgProtoSend,uint8_t *cfgSendDataBuff,uint32_t maxPacketSize,uint8_t logSendEnable)
{
	static uint8_t flagCfgProcessInUse = 0;
	uint8_t *u8pt,buf[18];
	uint32_t i,j,u32Temp;
	CFG_DATA_TYPE	pt;
	if(flagCfgProcessInUse) 
	{
		cfgProtoRecv->opcode = 0;
		return 0;
	}
	flagCfgProcessInUse = 1;
	
	
	switch(cfgProtoRecv->opcode)
	{
		case 0x00:
						if(logSource != NO_LOG && logSendEnable)
						{
							i = 0;
							while(RINGBUF_Get(&configTaskRingBuf, &cfgSendDataBuff[i]) == 0)
							{
								i++;
								if(i >= maxPacketSize) break;
							}
							if(i)
							{
								cfgProtoSend->opcode = 0x03;
								cfgProtoSend->start = 0xCA;
								cfgProtoSend->length = i;
								cfgProtoSend->dataPt = cfgSendDataBuff;
								cfgProtoSend->crc = CfgCalcCheckSum(cfgProtoSend->dataPt,cfgProtoSend->length);
							}
						}		
			break;
		case 0x01:
						logSource = NO_LOG;
			break;
		case 0x02:
						logSource  = TCP_IP_LOG;
			break;
		case 0x04:
						logSource  = GSM_AT_CMD_LOG;
			break;
		case 0x05:
						logSource  = GPS_LOG;
			break;
		case 0x06:
						logSource  = ALL_LOG;
			break;
		case 0x0C: //read config
			pt.packetNo = cfgProtoRecv->dataPt[0];
			pt.packetNo |= (uint32_t)cfgProtoRecv->dataPt[1]<<8;
			pt.packetNo |= (uint32_t)cfgProtoRecv->dataPt[2]<<16;
			pt.packetNo |= (uint32_t)cfgProtoRecv->dataPt[3]<<24;
			pt.packetLengthToSend = cfgProtoRecv->dataPt[4];
			pt.packetLengthToSend |= (uint32_t)cfgProtoRecv->dataPt[5]<<8;
			pt.dataPt = (uint8_t *)cfgProtoRecv->dataPt + 4;
			if(pt.packetNo == 0xA5A5A5A5)
			{
				i = sizeof(CONFIG_POOL);
				cfgProtoSend->start = 0xCA;
				cfgProtoSend->opcode = 0x0C;
				cfgProtoSend->length = 12;
				u8pt = (uint8_t *)&pt.packetNo;
				cfgSendDataBuff[0] = u8pt[0];
				cfgSendDataBuff[1] = u8pt[1];
				cfgSendDataBuff[2] = u8pt[2];
				cfgSendDataBuff[3] = u8pt[3];
				//Max file size
				i = sizeof(CONFIG_POOL);
				u8pt = (uint8_t *)&i;
				cfgSendDataBuff[4] = u8pt[0];
				cfgSendDataBuff[5] = u8pt[1];
				cfgSendDataBuff[6] = u8pt[2];
				cfgSendDataBuff[7] = u8pt[3];
				//Max packet size
				i = maxPacketSize;
				u8pt = (uint8_t *)&i;
				cfgSendDataBuff[8] = u8pt[0];
				cfgSendDataBuff[9] = u8pt[1];
				cfgSendDataBuff[10] = u8pt[2];
				cfgSendDataBuff[11] = u8pt[3];
				cfgProtoSend->dataPt = cfgSendDataBuff;
				cfgProtoSend->crc = CfgCalcCheckSum(cfgProtoSend->dataPt,cfgProtoSend->length);
			}
			else if(pt.packetNo == 0x5A5A5A5A)
			{
				cfgProtoSend->start = 0xCA;
				cfgProtoSend->opcode = 0x0C;
				cfgProtoSend->length = 4;
				u8pt = (uint8_t *)&pt.packetNo;
				cfgSendDataBuff[0] = u8pt[0];
				cfgSendDataBuff[1] = u8pt[1];
				cfgSendDataBuff[2] = u8pt[2];
				cfgSendDataBuff[3] = u8pt[3];
				cfgProtoSend->dataPt = cfgSendDataBuff;
				cfgProtoSend->crc = CfgCalcCheckSum(cfgProtoSend->dataPt,cfgProtoSend->length);
			}
			else if(((pt.packetNo + pt.packetLengthToSend) <= sizeof(CONFIG_POOL)) && (pt.packetLengthToSend <= maxPacketSize))
			{
				u8pt = (uint8_t *)&sysCfg + pt.packetNo;
				for(i = 0;i < pt.packetLengthToSend;i++)
				{
					cfgSendDataBuff[i+4] = u8pt[i];
				}
				cfgProtoSend->length = i + 4;
				cfgProtoSend->start = 0xCA;
				cfgProtoSend->opcode = 0x0C;
				u8pt = (uint8_t *)&pt.packetNo;
				cfgSendDataBuff[0] = u8pt[0];
				cfgSendDataBuff[1] = u8pt[1];
				cfgSendDataBuff[2] = u8pt[2];
				cfgSendDataBuff[3] = u8pt[3];
				cfgProtoSend->dataPt = cfgSendDataBuff;
				cfgProtoSend->crc = CfgCalcCheckSum(cfgProtoSend->dataPt,cfgProtoSend->length);
			}
			else
			{
				pt.packetNo = 0xFFFFFFFF;
				cfgProtoSend->start = 0xCA;
				cfgProtoSend->opcode = 0x0C;
				cfgProtoSend->length = 4;
				u8pt = (uint8_t *)&pt.packetNo;
				cfgSendDataBuff[0] = u8pt[0];
				cfgSendDataBuff[1] = u8pt[1];
				cfgSendDataBuff[2] = u8pt[2];
				cfgSendDataBuff[3] = u8pt[3];
				cfgProtoSend->dataPt = cfgSendDataBuff;
				cfgProtoSend->crc = CfgCalcCheckSum(cfgProtoSend->dataPt,cfgProtoSend->length);
			}
		break;
		case 0x0D:
			pt.packetNo = cfgProtoRecv->dataPt[0];
			pt.packetNo |= (uint32_t)cfgProtoRecv->dataPt[1]<<8;
			pt.packetNo |= (uint32_t)cfgProtoRecv->dataPt[2]<<16;
			pt.packetNo |= (uint32_t)cfgProtoRecv->dataPt[3]<<24;
			pt.dataPt = (uint8_t *)cfgProtoRecv->dataPt + 4;
			if(pt.packetNo == 0xA5A5A5A5)
			{		
				cfgProtoSend->start = 0xCA;
				cfgProtoSend->opcode = 0x0D;
				cfgProtoSend->length = 12;
				u8pt = (uint8_t *)&pt.packetNo;
				cfgSendDataBuff[0] = u8pt[0];
				cfgSendDataBuff[1] = u8pt[1];
				cfgSendDataBuff[2] = u8pt[2];
				cfgSendDataBuff[3] = u8pt[3];
				//Max file size
				i = sizeof(CONFIG_POOL);
				u8pt = (uint8_t *)&i;
				cfgSendDataBuff[4] = u8pt[0];
				cfgSendDataBuff[5] = u8pt[1];
				cfgSendDataBuff[6] = u8pt[2];
				cfgSendDataBuff[7] = u8pt[3];
				//Max packet size
				i = maxPacketSize;
				u8pt = (uint8_t *)&i;
				cfgSendDataBuff[8] = u8pt[0];
				cfgSendDataBuff[9] = u8pt[1];
				cfgSendDataBuff[10] = u8pt[2];
				cfgSendDataBuff[11] = u8pt[3];
				cfgProtoSend->dataPt = cfgSendDataBuff;
				cfgProtoSend->crc = CfgCalcCheckSum(cfgProtoSend->dataPt,cfgProtoSend->length);
			}
			else if(((pt.packetNo + cfgProtoRecv->length - 4) <= sizeof(CONFIG_POOL)))
			{
				u8pt = (uint8_t *)&sysCfg + pt.packetNo;
				for(i = 0;i < cfgProtoRecv->length - 4;i++)
				{
					u8pt[i] = pt.dataPt[i];
				}
				
				 pt.packetNo += i;
				if(pt.packetNo >= sizeof(CONFIG_POOL))
				{
					CFG_Save();
					cfgProtoSend->start = 0xCA;
					cfgProtoSend->opcode = 0x0D;
					cfgProtoSend->length = 4;
					u8pt = (uint8_t *)&pt.packetNo;
					cfgSendDataBuff[0] = u8pt[0];
					cfgSendDataBuff[1] = u8pt[1];
					cfgSendDataBuff[2] = u8pt[2];
					cfgSendDataBuff[3] = u8pt[3];
					cfgProtoSend->dataPt = cfgSendDataBuff;
					cfgProtoSend->crc = CfgCalcCheckSum(cfgProtoSend->dataPt,cfgProtoSend->length);
					break;
				}
				if(pt.packetNo > (sizeof(CONFIG_POOL) - maxPacketSize))
				{
					i = (sizeof(CONFIG_POOL) -  pt.packetNo);
				}
				else
				{
					i = maxPacketSize;
				}
				cfgProtoSend->length = 8;
				cfgProtoSend->start = 0xCA;
				cfgProtoSend->opcode = 0x0D;
				u8pt = (uint8_t *)&pt.packetNo;
				cfgSendDataBuff[0] = u8pt[0];
				cfgSendDataBuff[1] = u8pt[1];
				cfgSendDataBuff[2] = u8pt[2];
				cfgSendDataBuff[3] = u8pt[3];
				//Max packet size
				u8pt = (uint8_t *)&i;
				cfgSendDataBuff[4] = u8pt[0];
				cfgSendDataBuff[5] = u8pt[1];
				cfgSendDataBuff[6] = u8pt[2];
				cfgSendDataBuff[7] = u8pt[3];
				cfgProtoSend->dataPt = cfgSendDataBuff;
				cfgProtoSend->crc = CfgCalcCheckSum(cfgProtoSend->dataPt,cfgProtoSend->length);
			}
			else
			{
				pt.packetNo = 0xFFFFFFFF;
				cfgProtoSend->start = 0xCA;
				cfgProtoSend->opcode = 0x0D;
				cfgProtoSend->length = 4;
				u8pt = (uint8_t *)&pt.packetNo;
				cfgSendDataBuff[0] = u8pt[0];
				cfgSendDataBuff[1] = u8pt[1];
				cfgSendDataBuff[2] = u8pt[2];
				cfgSendDataBuff[3] = u8pt[3];
				cfgProtoSend->dataPt = cfgSendDataBuff;
				cfgProtoSend->crc = CfgCalcCheckSum(cfgProtoSend->dataPt,cfgProtoSend->length);
			}
		break;
		case 0x11:
			cfgProtoSend->start = 0xCA;
			cfgProtoSend->opcode = 0x11;
			cfgProtoSend->length = strlen(FIRMWARE_VERSION);
			cfgProtoSend->dataPt = FIRMWARE_VERSION;
			cfgProtoSend->crc = CfgCalcCheckSum(cfgProtoSend->dataPt,cfgProtoSend->length);	
		break;
		case 0x12:
			pt.packetNo = cfgProtoRecv->dataPt[0];
			pt.packetNo |= (uint32_t)cfgProtoRecv->dataPt[1]<<8;
			pt.packetNo |= (uint32_t)cfgProtoRecv->dataPt[2]<<16;
			pt.packetNo |= (uint32_t)cfgProtoRecv->dataPt[3]<<24;
			pt.dataPt = (uint8_t *)cfgProtoRecv->dataPt + 4;
			if(pt.packetNo == 0xA5A5A5A5)
			{
				InitTimeout(&tFirmwareTryToUpdate,SYSTICK_TIME_SEC(600));
				firmwareStatus = 0xA5A5A5A5;
				firmwareFileOffSet = 0;
				firmwareFileSize = pt.dataPt[0];
				firmwareFileSize |= pt.dataPt[1]<<8;
				firmwareFileSize |= pt.dataPt[2]<<16;
				firmwareFileSize |= pt.dataPt[3]<<24;
				 
				firmwareFileCrc = pt.dataPt[4];
				firmwareFileCrc |= pt.dataPt[5]<<8;
				firmwareFileCrc |= pt.dataPt[6]<<16;
				firmwareFileCrc |= pt.dataPt[7]<<24;
				
				SST25_Erase(FIRMWARE_INFO_ADDR,block4k);
				u8pt = (uint8_t *)&firmwareFileSize;
				SST25_Write(FIRMWARE_INFO_ADDR+FIRMWARE_FILE_SIZE_OFFSET*4,u8pt,4);
				i = 0xA5A5A5A5;
				u8pt = (uint8_t *)&i;
				SST25_Write(FIRMWARE_INFO_ADDR+FIRMWARE_STATUS_OFFSET*4,u8pt,4);
				//Write CRC
				u8pt = (uint8_t *)&firmwareFileCrc;
				SST25_Write(FIRMWARE_INFO_ADDR+FIRMWARE_CRC_OFFSET*4,u8pt,4);
				
				cfgProtoSend->start = 0xCA;
				cfgProtoSend->opcode = 0x12;
				cfgProtoSend->length = 12;
				u8pt = (uint8_t *)&pt.packetNo;
				cfgSendDataBuff[0] = u8pt[0];
				cfgSendDataBuff[1] = u8pt[1];
				cfgSendDataBuff[2] = u8pt[2];
				cfgSendDataBuff[3] = u8pt[3];
				//Max file size
				i = firmwareFileSize;
				u8pt = (uint8_t *)&i;
				cfgSendDataBuff[4] = u8pt[0];
				cfgSendDataBuff[5] = u8pt[1];
				cfgSendDataBuff[6] = u8pt[2];
				cfgSendDataBuff[7] = u8pt[3];
				//Max packet size
				i = maxPacketSize;
				u8pt = (uint8_t *)&i;
				cfgSendDataBuff[8] = u8pt[0];
				cfgSendDataBuff[9] = u8pt[1];
				cfgSendDataBuff[10] = u8pt[2];
				cfgSendDataBuff[11] = u8pt[3];
				cfgProtoSend->dataPt = cfgSendDataBuff;
				cfgProtoSend->crc = CfgCalcCheckSum(cfgProtoSend->dataPt,cfgProtoSend->length);
			}
			else
			{
				if(pt.packetNo == firmwareFileOffSet)
				{
					i = pt.packetNo + (uint32_t)cfgProtoRecv->length  - 4;
					firmwareFileOffSet = i;
					u32Temp = cfgProtoRecv->length - 4;
					
// 					u8pt = 0;
// 					for(i = 0;i < u32Temp;i++)
// 					{
// 						if(u8pt[pt.packetNo + i] != pt.dataPt[i])
// 							break;
// 					}
					if((maxPacketSize >= FLASH_PACKET_SIZE) && (u32Temp >= FLASH_PACKET_SIZE))
					{		
							for( i = 0;i < u32Temp; i += FLASH_PACKET_SIZE)
							{
								SST25_Write(pt.packetNo + FIRMWARE_BASE_ADDR + i,pt.dataPt + i,FLASH_PACKET_SIZE);
								SST25_Read(pt.packetNo + FIRMWARE_BASE_ADDR + i,buf, FLASH_PACKET_SIZE);
								for(j = 0;j < FLASH_PACKET_SIZE;j++)
								{
									if(pt.dataPt[i + j] != buf[j]) //back old packet  when data loss
									{
										firmwareFileOffSet = firmwareFileOffSetOld;
										i = u32Temp;
									}
								}
							}
					}
					if((firmwareFileOffSet - firmwareFileOffSetOld) >= 4096)
					{		
						firmwareFileOffSetOld = (firmwareFileOffSet / 4096) * 4096;
					}
				}
				pt.packetNo = firmwareFileOffSet;
				
				if(firmwareFileOffSet >= firmwareFileSize)
				{
					u32Temp = 0;
					for(i = 0; i < FLASH_PACKET_SIZE;i++)
					{
						cfgSendDataBuff[i] = 0xff;
					}
					for(i = 0; i < 1024;i += FLASH_PACKET_SIZE)
					{
						SST25_Write(firmwareFileSize + FIRMWARE_BASE_ADDR + i,cfgSendDataBuff,FLASH_PACKET_SIZE);
					}
					for(i = 0; i < firmwareFileSize;i += FLASH_PACKET_SIZE)
					{
						SST25_Read(i + FIRMWARE_BASE_ADDR,cfgSendDataBuff, FLASH_PACKET_SIZE);
						for(j = 0;j < FLASH_PACKET_SIZE;j++)
						{
// 							u8pt = 0;
// 							if(u8pt[j + i] != cfgSendDataBuff[j])
// 								DelayMs(1);
							if((i + j) < firmwareFileSize)
								u32Temp += cfgSendDataBuff[j];
							else
								break;
						}
						if((i + j) >= firmwareFileSize)
							break;
					}
					if(u32Temp == firmwareFileCrc)
					{
						sysResetMcuFlag = 2;
						firmwareStatus = 0x5A5A5A5A;
						pt.packetNo = 0x5A5A5A5A;
						firmwareFileOffSet = 0;
						SST25_Erase(FIRMWARE_INFO_ADDR,block4k);
						
						//Write file size
						u8pt = (uint8_t *)&firmwareFileSize;
						SST25_Write(FIRMWARE_INFO_ADDR+FIRMWARE_FILE_SIZE_OFFSET*4,u8pt,4);
						//Write CRC
						u8pt = (uint8_t *)&firmwareFileCrc;
						SST25_Write(FIRMWARE_INFO_ADDR+FIRMWARE_CRC_OFFSET*4,u8pt,4);
						//Write Start addr
						i = FIRMWARE_BASE_ADDR;
						u8pt = (uint8_t *)&i;
						SST25_Write(FIRMWARE_INFO_ADDR+FIRMWARE_START_ADDR_OFFSET*4,u8pt,4);
						//Write status
						i = 0x5A5A5A5A;
						u8pt = (uint8_t *)&i;
						SST25_Write(FIRMWARE_INFO_ADDR+FIRMWARE_STATUS_OFFSET*4,u8pt,4);

						SST25_Erase(RESET_CNT_ADDR,block4k);
					}
					else
					{
						firmwareStatus = 0xA5A5A5A5;
						pt.packetNo = 0;
						firmwareFileOffSet = 0;
						SST25_Erase(FIRMWARE_INFO_ADDR,block4k);
						u8pt = (uint8_t *)&firmwareFileSize;
						SST25_Write(FIRMWARE_INFO_ADDR+FIRMWARE_FILE_SIZE_OFFSET*4,u8pt,4);
						i = 0xA5A5A5A5;
						u8pt = (uint8_t *)&i;
						SST25_Write(FIRMWARE_INFO_ADDR+FIRMWARE_STATUS_OFFSET*4,u8pt,4);
					}
				}				
				cfgProtoSend->start = 0xCA;
				cfgProtoSend->opcode = 0x12;
				cfgProtoSend->length = 4;
				u8pt = (uint8_t *)&pt.packetNo;
				cfgSendDataBuff[0] = u8pt[0];
				cfgSendDataBuff[1] = u8pt[1];
				cfgSendDataBuff[2] = u8pt[2];
				cfgSendDataBuff[3] = u8pt[3];
				cfgProtoSend->dataPt = cfgSendDataBuff;
				cfgProtoSend->crc = CfgCalcCheckSum(cfgProtoSend->dataPt,cfgProtoSend->length);
			}
		break;
		case 0xFD:					// factory reset
			pt.packetNo = cfgProtoRecv->dataPt[0];
			pt.packetNo |= (uint32_t)cfgProtoRecv->dataPt[1]<<8;
			pt.packetNo |= (uint32_t)cfgProtoRecv->dataPt[2]<<16;
			pt.packetNo |= (uint32_t)cfgProtoRecv->dataPt[3]<<24;
			pt.dataPt = (uint8_t *)cfgProtoRecv->dataPt + 4;
			if(pt.packetNo == 0xA5A5A5A5)
			{
				strcpy((char *)buf, (char *)sysCfg.id);
				memset((void*)&sysCfg, 0xFF, sizeof(CONFIG_POOL));
				strcpy((char *)sysCfg.id ,(char *)buf);
				CFG_Save();
				NVIC_SystemReset();
			}
		break;

		case 0xFE:					// Reset device
			pt.packetNo = cfgProtoRecv->dataPt[0];
			pt.packetNo |= (uint32_t)cfgProtoRecv->dataPt[1]<<8;
			pt.packetNo |= (uint32_t)cfgProtoRecv->dataPt[2]<<16;
			pt.packetNo |= (uint32_t)cfgProtoRecv->dataPt[3]<<24;
			pt.dataPt = (uint8_t *)cfgProtoRecv->dataPt + 4;
			if(pt.packetNo == 0xA5A5A5A5)
				NVIC_SystemReset();
			break;
		default:
			break;
	}
	cfgProtoRecv->opcode = 0;
	flagCfgProcessInUse = 0;
	return 0;
}


uint8_t CfgParserPacket(PARSER_PACKET_TYPE *parserPacket,CFG_PROTOCOL_TYPE *cfgProtoRecv,uint8_t c)
{
	switch(parserPacket->state)
	{
		case CFG_CMD_WAITING_SATRT_CODE:
			if(c == 0xCA)
			{
				parserPacket->state = CFG_CMD_GET_LENGTH;
				parserPacket->len = 0;
				parserPacket->crc = 0;
				parserPacket->cnt = 0;
			}
			break;
		case CFG_CMD_GET_LENGTH:
				parserPacket->len |= (uint16_t)c<<parserPacket->cnt;
				parserPacket->cnt += 8;
				if(parserPacket->cnt >= 16)
				{
					parserPacket->state = CFG_CMD_GET_OPCODE;
					parserPacket->cnt = 0;
				}
			break;
		case CFG_CMD_GET_OPCODE:		
					parserPacket->opcode = c;
					parserPacket->state = CFG_CMD_GET_DATA;
			break;
		case CFG_CMD_GET_DATA:
				if((parserPacket->cnt >= parserPacket->len) || (parserPacket->len > parserPacket->lenMax))
				{
					parserPacket->state = CFG_CMD_WAITING_SATRT_CODE;
				}
				else
				{
					parserPacket->crc += c;
					cfgProtoRecv->dataPt[parserPacket->cnt]= c;
					parserPacket->cnt++;
					if(parserPacket->cnt == parserPacket->len)
					{
						parserPacket->state = CFG_CMD_CRC_CHECK;
					}
				}
			break;
		case CFG_CMD_CRC_CHECK:
				parserPacket->state= CFG_CMD_WAITING_SATRT_CODE;
				if(parserPacket->crc  == c)
				{	
					cfgProtoRecv->length = parserPacket->len;
					cfgProtoRecv->opcode = parserPacket->opcode;
					return 0;
				}
			break;
		default:
			parserPacket->state = CFG_CMD_WAITING_SATRT_CODE;
			break;				
	}
	return 0xff;
}


uint8_t CfgCalcCheckSum(uint8_t *buff, uint32_t length)
{
	uint32_t i;
	uint8_t crc = 0;
	for(i = 0;i < length; i++)
	{
		crc += buff[i];
	}
	return crc;
}



