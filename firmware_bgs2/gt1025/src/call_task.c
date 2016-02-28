/**********************************************************************
Name: Hai Nguyen Van
Cellphone: (84) 97-8779-222
Mail:thienhaiblue@ampm.com.vn 
----------------------------------
AMPM ELECTRONICS EQUIPMENT TRADING COMPANY LIMITED.,
Add: 634/6 Phan Van Suu street , Ward 13, Tan Binh District, HCM City, VN

*********************************************************************/
#include "modem.h"
#include "system_config.h"
#include "lib/sparser.h"
#include "lib/sys_tick.h"
#include "at_command_parser.h"
#include "at_gsm_task.h"
#include "at_send_cmd_task.h"
#include "at_cmd_call_parser.h"
#include "lib/list.h"
#include "sst25.h"
#include "gps/gps.h"
#include "adc_task.h"
#include "sms_task.h"
#include "lib/encoding.h"
#include "alarm_task.h"

SMS_LIST_TYPE locationSms;

const uint8_t smsCallReportLocation[] = "Vị Trí Hiện Tại";

uint8_t locationAskSmsBuf[160];

void CallTask(uint32_t rtc)
{
		uint8_t pdu2uniBuf[256];
		uint16_t smsLen = 0;
		if(VoiceCallTask_GetPhase() == INCOMING_CALL)
		{
			if(atCmdCallParser.getIncomingNumber
				 && locationSms.flag != SMS_NEW_MSG
			)
			{
				addrSendToUserFlag = 1;
				atCmdCallParser.autoAnswer = 1;
				getAddrFlag = 1;
				askAddrCnt = 0;
				smsLen =sprintf((char *)pdu2uniBuf,"%s!\nhttp://maps.google.com/maps?q=%.6f,%.6f",smsCallReportLocation,lastGpsInfo.lat,lastGpsInfo.lon);
				smsLen = utf8s_to_ucs2s((int16_t *)locationAskSmsBuf,pdu2uniBuf);
				big2litel_endian((uint16_t *)locationAskSmsBuf,unilen((uint16_t *)locationAskSmsBuf));
				smsLen *= 2;

				if(ComparePhoneNumber((char *)atCmdCallParser.incomingCallPhoneNo,(char *)sysCfg.userPhone) != NULL)
				{
					Sms_SendMsg(&locationSms,sysCfg.userPhone,(uint8_t *)locationAskSmsBuf,smsLen,SMS_PDU16_MODE,30000,3);
				}
				else
					return;
			}
		}
}

void CALL_TaskInit(void)
{
	
}
