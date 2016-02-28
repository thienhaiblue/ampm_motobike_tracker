#ifndef __GSM_HWR_H__
#define __GSM_HWR_H__

#include <stdint.h>
#include "lib/ringbuf.h"
#include "comm.h"

uint8_t MODEM_PowerOn(void);
void MODEM_UartInit(uint32_t baudrate);


#endif



