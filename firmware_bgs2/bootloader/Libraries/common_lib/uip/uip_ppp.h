#ifndef _UIP_PPP_H_
#define _UIP_PPP_H_

#include "ppp.h"
#include "timer.h"
#include "uip.h"

void UIP_SetHostAddr(void);
void UIP_Periodic(void);
void UIP_Input(void);

#endif
