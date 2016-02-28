#ifndef __AT_COMMAND_PARSER_H__
#define __AT_COMMAND_PARSER_H__
#include "stdio.h"
#include "string.h"
#include "lib/sparser.h"
#include "at_cmd_common_parser.h"
#include "lib/sparser.h"
#include "at_command_parser.h"
#include "at_cmd_sms_parser.h"
#include "at_cmd_call_parser.h"
#include "u-blox/at_cmd_g100_parser.h"
#include "u-blox/at_u_blox_gprs_task.h"
#include "modem.h"

#define GPRS_DATA_MAX_LENGTH	1024

extern char *commandSeparator;
extern STR_PARSER_Type AT_CmdParser;
extern const uint8_t terminateStr[7];
void AT_CmdProcessInit(void);
void AT_ComnandParser(char c);

void AT_CommandCtl(void);


#endif
