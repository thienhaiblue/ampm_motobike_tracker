#ifndef _RESOLVER_H_
#define _RESOLVER_H_

#include "uip_ppp.h"
#include "resolv.h"
#include "typedef.h"

void RESOLVER_Reset(void);
void RESOLVER_Manage(void);
uip_ipaddr_t *RESOLVER_Lookup(const I8 *name);
void RESOLVER_Query(const I8 *name);

#endif
