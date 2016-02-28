#include "resolver.h"
#include "lib/sys_tick.h"
#include "system_config.h"
#define RESOLVER_INTERVAL		(SYSTICK_SECOND * 60 * 60 * 48)

U64 resolverTick;

enum{
	RSLV_INITIAL,
	RSLV_QUERY,
	RSLV_STANDBY,
}resolverState;
	
void RESOLVER_Reset()
{
	resolverState = RSLV_INITIAL;
	resolverTick = (U64)SysTick_Get();
}

void RESOLVER_Manage(void)
{
	uip_ipaddr_t dnsServerIp;
	switch(resolverState){
		case RSLV_INITIAL:
			resolv_init();			
			uip_ipaddr(&dnsServerIp, 208, 67, 222, 222);
			resolv_conf(&dnsServerIp);
			
		case RSLV_QUERY:
			resolv_query((const char *)sysCfg.priFserverName);
			resolv_query((const char *)sysCfg.priDserverName);
			resolverTick = SysTick_Get64();
			resolverState = RSLV_STANDBY;
			
		case RSLV_STANDBY:
			if(SysTick_Get64() - resolverTick > RESOLVER_INTERVAL)
				resolverState = RSLV_QUERY;
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
		RESOLVER_Info("RESOLV: name: %s, address: %d.%d.%d.%d\n", name, ipaddr->u8[0], ipaddr->u8[1], ipaddr->u8[2], ipaddr->u8[3]);
	}
}

