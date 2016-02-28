#include "lib/sparser.h"

extern  uint32_t TICK_Get(void);

void StrParserInit(STR_PARSER_Type *process,uint8_t *numCharParsed,uint8_t strParserCntMax,uint16_t dataLengthMax)
{
	uint8_t i;
	process->state = STR_FINISH;
	process->dataRecvLength = 0;
	process->strParserCntMax = strParserCntMax;
	process->timeout = 0;
	process->timeoutSet = 10000;// 10sec
	process->dataRecvMaxLength = dataLengthMax;
	for(i = 0; i < process->strParserCntMax;i++)
	{
		numCharParsed[i] = 0;
	}
}

void StrParserCtl(uint32_t sysTimeMs,STR_PARSER_Type *process,uint8_t *numCharParsed)
{	
	uint8_t i;
	if(process->state == STR_FINISH) 
		process->timeout = sysTimeMs;
	else
	{
		if(sysTimeMs- process->timeout >= process->timeoutSet)
		{
			process->timeout = sysTimeMs;
			process->state = STR_FINISH;
			for(i = 0; i < process->strParserCntMax;i++)
			{
				numCharParsed[i] = 0;
			}
		}
	}
}

void StrComnandParser(uint32_t sysTimeMs,const STR_INFO_Type *info,STR_PARSER_Type *process,uint8_t *numCharParsed,uint8_t c)
{
	uint32_t i; 
	static uint8_t lastChar = 0;
	StrParserCtl(sysTimeMs,process,numCharParsed);
	switch(process->state)
	{
		case STR_FINISH:
			for(i = 0; i < process->strParserCntMax;i++)
			{
				if(c == info[i].command[numCharParsed[i]] || info[i].command[numCharParsed[i]] == '\t')
				{
						lastChar = c;
						numCharParsed[i]++;
						if(info[i].command[numCharParsed[i]] == '\0')
						{
								process->strInProcess = i;
								process->state = STR_WAIT_FINISH;
								process->dataRecvLength = 0;
						}
				}
				else if(lastChar == info[i].command[0])
				{
						numCharParsed[i] = 1;
				}
				else
					numCharParsed[i] = 0;
			}
		break;
		case STR_WAIT_FINISH:				
			if(info[process->strInProcess].callback(process->dataRecvLength,c) == 0)
			{
				process->state = STR_FINISH;
				//process->timeoutSet = 10000;
				for(i = 0; i < process->strParserCntMax;i++)
				{
					numCharParsed[i] = 0;
				}
			}
			process->dataRecvLength++;
			if(process->dataRecvLength >= process->dataRecvMaxLength)
			{
				process->state = STR_FINISH;
				for(i = 0; i <  process->strParserCntMax;i++)
				{
					numCharParsed[i] = 0;
				}
			}
			break;
		default:
			process->state = STR_FINISH;
			for(i = 0; i <  process->strParserCntMax;i++)
			{
				numCharParsed[i] = 0;
			}
			break;
	}	
}

