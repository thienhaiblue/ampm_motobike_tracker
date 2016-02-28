

#ifndef __SPARSER_H__
#define __SPARSER_H__
#include <stdint.h>

/*String Info  definitions structure. */
typedef struct  {
	uint8_t *command;
	uint32_t (*callback)(uint16_t cnt,uint8_t c);
} STR_INFO_Type;

#define STR_PARSER_COUNT(x)   (sizeof (x) / sizeof (STR_INFO_Type))

typedef enum{
	STR_NEW_STATE,
	STR_WAIT_FINISH,
	STR_FINISH
}STR_STATE_Type;


typedef struct  {
   STR_STATE_Type state;
	 uint16_t dataRecvLength;
	 uint16_t dataRecvMaxLength;
	 uint8_t strParserCntMax;
	 uint8_t strInProcess;
	 uint32_t timeout;
	 uint32_t timeoutSet;
} STR_PARSER_Type;

void StrParserInit(STR_PARSER_Type *process,uint8_t *numCharParsed,uint8_t strParserCntMax,uint16_t dataLengthMax);
void StrComnandParser(uint32_t sysTimeMs,const STR_INFO_Type *info,STR_PARSER_Type *process,uint8_t *numCharParsed,uint8_t c);

#endif
