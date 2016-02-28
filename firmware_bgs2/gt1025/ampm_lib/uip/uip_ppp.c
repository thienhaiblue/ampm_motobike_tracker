

#include "uip_ppp.h"
#include "uip.h"
#include "lib/sys_tick.h"

Timeout_Type periodicTime1;
struct timer uipTimer;


uint8_t Domain_IpCheck(uint8_t *domain_ip)
{
	uint8_t *pt = domain_ip;
	while(*pt != NULL)
	{
		if(!((*pt >= '0' && *pt <= '9') || *pt == '.'))
		{
			return IS_DOMAIN;
		}
		pt++;
	}
	return IS_IP;
}


void UIP_SetHostAddr()
{
	uip_ipaddr_t localIp;

	timer_set(&uipTimer, SYSTICK_SECOND/2);
	
	uip_ipaddr(&localIp, pppIpAddr[0], pppIpAddr[1], pppIpAddr[2], pppIpAddr[3]);
	
	uip_sethostaddr(&localIp);
}
uint32_t UIP_Input(void)
{
	uip_len = pppFrameSize;
	return uip_len;
}
void UIP_Periodic(void)
{
	U8 i;
	if(uip_len > 0)
	{
			//UIP_PPP_Info("\r\nUIP_PPP: UIP_DATA\r\n");
			uip_input();
			if(uip_len > 0){
				uip_buf[0] = 0xFF;
				uip_buf[1] = 0x03;
				uip_buf[2] = 0x00;
				uip_buf[3] = 0x21;
				PPP_Send(uip_buf, uip_len+6);
			}
	}
	else if(timer_expired(&uipTimer))
	{
		timer_restart(&uipTimer);
		//UIP_PPP_Info("\r\nUIP_PPP: UIP_Periodic();\r\n");
		for(i=0; i<UIP_CONNS; i++){
			uip_periodic(i);
	 		if(uip_len){
				uip_buf[0] = 0xFF;
				uip_buf[1] = 0x03;
				uip_buf[2] = 0x00;
				uip_buf[3] = 0x21;
				PPP_Send(uip_buf, uip_len+6);
			}
		}
		
		#if UIP_UDP
		for(i=0; i<UIP_UDP_CONNS; i++){
			uip_udp_periodic(i);
	 		if(uip_len){
				uip_buf[0] = 0xFF;
				uip_buf[1] = 0x03;
				uip_buf[2] = 0x00;
				uip_buf[3] = 0x21;
				PPP_Send(uip_buf, uip_len+6);
			}
		}
		#endif
	}
}

