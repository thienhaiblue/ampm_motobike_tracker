

#include "uip_ppp.h"
#include "uip.h"
#include "tick.h"

struct timer uipTimer;

void UIP_SetHostAddr()
{
	uip_ipaddr_t localIp;

	timer_set(&uipTimer, TICK_SECOND / 2);
		
	uip_ipaddr(&localIp, pppIpAddr[0], pppIpAddr[1], pppIpAddr[2], pppIpAddr[3]);
	
	uip_sethostaddr(&localIp);
}
void UIP_Input()
{
	uip_len = pppFrameSize;
	uip_input();
	if(uip_len){
		uip_buf[0] = 0xFF;
		uip_buf[1] = 0x03;
		uip_buf[2] = 0x00;
		uip_buf[3] = 0x21;
		PPP_Send(uip_buf, uip_len+6);
	}
}
void UIP_Periodic()
{
	U8 i;
	if(timer_expired(&uipTimer)){
		timer_restart(&uipTimer);
				
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

