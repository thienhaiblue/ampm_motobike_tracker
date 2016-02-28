

#ifndef __SMS_SERVER_H__
#define __SMS_SERVER_H__

#define MAX_ID_LEN						8
#define MAX_PASSWD_LEN				8
#define MAX_MSISDN_LEN				12
#define MAX_MESSAGEID_LEN				8
#define MAX_MSG_LEN					159
#define ORDERTIME_LEN			 		12
#define SERV_CHECK						0	// used for sSendMsg.type
#define SERV_EDIT_PASSWD				1	// used for sSendMsg.type
#define SERV_SEND						2	// used for sSendMsg.type
#define SERV_QUERY						3	// used for sSendMsg.type
#define SERV_GET						4	// used for sSendMsg.type
#define SERV_LINK						5	// used for sSendMsg.type
#define SERV_SEND_WITH_UDHI			6	// used for sSendMsg.type
#define SERV_SEND_WITH_SUBNO			10	// Subno send msg to Msisdn
#define SERV_SEND_WITH_RECV_SUBNO		11	// Msisdn send msg to Subno
#define SERV_SEND_WITH_ALL_SUBNO		12	// Subno  send msg to Subno
#define SERV_QUERY_WITH_SUBNO			13
#define SERV_QUERY_WITH_RECV_SUBNO	14
#define SERV_QUERY_WITH_ALL_SUBNO		15
#define SEND_NOW						100	// used for sSendMsg.tran_type
#define SM_UDHI_ENABLE					0x01
#define SM_PCLID_ENABLE				0x04


#endif

