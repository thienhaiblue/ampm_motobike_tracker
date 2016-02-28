#ifndef __UIP_CONF_H__
#define __UIP_CONF_H__

#include <stdint.h>

typedef uint8_t u8_t;
typedef uint16_t u16_t;
typedef uint32_t u32_t;
typedef uint32_t uip_stats_t;

extern void UIP_TCPCallback(void);
extern void UIP_UDPCallback(void);

#define CCIF

#define UIP_CONF_MAX_CONNECTIONS	5

#define UIP_CONF_UDP_CONNS			5

#define UIP_CONF_MAX_LISTENPORTS	1

#define UIP_CONF_BUFFER_SIZE		1500

#define UIP_CONF_BYTE_ORDER			LITTLE_ENDIAN

#define UIP_CONF_LOGGING			1

#define UIP_CONF_UDP				1

#define UIP_CONF_UDP_CHECKSUMS		1

#define UIP_CONF_STATISTICS			0

#define UIP_CONF_LLH_LEN			4

#define UIP_APPCALL					UIP_TCPCallback

typedef void* uip_tcp_appstate_t;

#define UIP_UDP_APPCALL				UIP_UDPCallback


typedef int uip_udp_appstate_t;


// NTP client
#define NTP_TZ   +7

#define NTP_REQ_CYCLE 600

#define NTP_REPEAT  4


#endif /* __UIP_CONF_H__ */

/** @} */
/** @} */
