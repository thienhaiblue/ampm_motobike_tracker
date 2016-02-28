#include "resolver.h"
#include "tick.h"
#include "config_pool.h"

#define RESOLVER_INTERVAL		(TICK_SECOND * 60 * 60 * 48)

U64 resolverTick;
enum{
	INITIAL,
	QUERY,
	STANDBY,
}resolverState;
	
void RESOLVER_Reset()
{
	resolverState = INITIAL;
	resolverTick = (U64)TICK_Get();
}

void RESOLVER_Manage()
{
	uip_ipaddr_t dnsServerIp;
	switch(resolverState){
		case INITIAL:
			resolv_init();			
			uip_ipaddr(&dnsServerIp, 208, 67, 222, 222);
			resolv_conf(&dnsServerIp);
			
		case QUERY:
			resolv_query((const char *)sysCfg.fserverName);
			resolv_query((const char *)sysCfg.dserverName);
			resolverTick = TICK_Get64();
			resolverState = STANDBY;
			
		case STANDBY:
			if(TICK_Get64() - resolverTick > RESOLVER_INTERVAL)
				resolverState = QUERY;
			break;
			
		default:
			break;
	}
}
uip_ipaddr_t *RESOLVER_Lookup(const I8 *name)
{
	return (uip_ipaddr_t*)resolv_lookup(name);
}
void RESOLVER_Query(const I8 *name)
{
	resolv_query(name);
}
void resolv_found(I8 *name, uip_ipaddr_t *ipaddr)
{
	if(ipaddr != NULL){
		MODEM_Info("RESOLV: name: %s, address: %d.%d.%d.%d\n", name, ipaddr->u8[0], ipaddr->u8[1], ipaddr->u8[2], ipaddr->u8[3]);
	}
}

