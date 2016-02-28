#ifndef _PPP_H_
#define _PPP_H_

	#include <string.h>
	#include <stdio.h>
	#include "typedef.h"
	#include "modem.h"
	#include "fcs.h"
	#include "ringbuf.h"
	#include "uart1.h"

	
	#define PPP_BUFFER_SIZE				88
	// PPP Packet type
	#define LCP_PACKET	0x21C0
	#define PAP_PACKET	0x23C0
	#define CHAP_PACKET	0x23C2
	#define IPCP_PACKET	0x2180
	#define IP_DATAGRAM	0x2100
	
	// LCP Packet type
	#define CONFIG_REQ		0x01
	#define CONFIG_ACK		0x02
	#define CONFIG_NACK		0x03
	#define CONFIG_REJ		0x04
	#define TERM_REQ		0x05
	#define TERM_ACK		0x06
	#define CODE_REJ		0x07
	#define PROTO_REJ		0x08
	#define ECHO_REQ		0x09
	#define ECHO_REPLY		0x0A
	#define DISC_REQ		0x0B
	
	
	#define PPP_AUTH_PAP	0x00
	#define PPP_AUTH_CHAP	0x01
	
	#define PPP_Info(...)	//	PrintfDebug(__VA_ARGS__)
	#define PPP_Debug(...)	//	PrintfDebug(__VA_ARGS__)
	
	#define PPP_MAX_RESTART		5
	
	extern U8 pppIpAddr[4];
	extern U16 pppFrameSize;
	
	void PPP_Init(void);
	void PPP_SetAuthentication(U8 mode, I8* usr, I8* pwd);
	void PPP_ManageLink(void);
	void PPP_ReInit(void);
	I8 PPP_IsLinked(void);
	void PPP_Send(U8* packet, U16 len);
	
#endif
