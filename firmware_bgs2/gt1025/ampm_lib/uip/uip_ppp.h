#ifndef _UIP_PPP_H_
#define _UIP_PPP_H_

#include "ppp.h"
#include "timer.h"
#include "uip.h"

#define IS_IP 0
#define IS_DOMAIN 1

uint8_t Domain_IpCheck(uint8_t *domain_ip);

void UIP_SetHostAddr(void);
void UIP_Periodic(void);
uint32_t UIP_Input(void);

#endif
