/**********************************************************************
Name: Hai Nguyen Van
Cellphone: (84) 97-8779-222
Mail:thienhaiblue@ampm.com.vn 
----------------------------------
AMPM ELECTRONICS EQUIPMENT TRADING COMPANY LIMITED.,
Add: 634/6 Phan Van Suu street , Ward 13, Tan Binh District, HCM City, VN

*********************************************************************/
#include "at_command_parser.h"

#include "lib/sys_tick.h"

#define INFO(...)	//DbgCfgPrintf(__VA_ARGS__)


static const STR_INFO_Type AT_ProcessCmd[] = {
	"+CLIP: \"", GetCallerNumber,
	"RING" , RingCallProcess,
  "+CMGL: ", SMS_Process,
	"+CMGR: ",SMS_ReadMsg,
	"BUSY",CallBusy,
	"NO ANSWER",CallNoAnswer,
	"NO CARRIER",CallCancel,
	"+CMTI: \"SM\",",SMS_Process,
	"+CMTI: \"ME\",",SMS_Process,
	"+CMTI: \"MT\",",SMS_Process,
	"+CBC: ",GetBatteryStatus,
	"+CEER: ",CallErrorReport,
	"ATD",CallOutProcess,
	"+CCLK: \"",GsmGetTime,
};


char *commandSeparator = " ,:\r\n";
const uint8_t terminateStr[7] = "\r\nOK\r\n";
uint8_t cmdCnt[STR_PARSER_COUNT(AT_ProcessCmd)];
STR_PARSER_Type AT_CmdParser;

void AT_CmdProcessInit(void)
{
	AT_CmdCallParserInit();
	AT_CmdSmsParserInit();
	StrParserInit(&AT_CmdParser,cmdCnt,STR_PARSER_COUNT(AT_ProcessCmd),1024);
}

void AT_ComnandParser(char c)
{
	static uint32_t RingBuffInit = 0;
	if(RingBuffInit == 0)// int ring buff
	{
		RingBuffInit = 1;
		AT_CmdProcessInit();
	}
	StrComnandParser(SysTick_Get(),AT_ProcessCmd,&AT_CmdParser,cmdCnt,c);
}




