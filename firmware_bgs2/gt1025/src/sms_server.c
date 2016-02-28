

#include "sms_server.h"



struct sSendMsg 
{
	unsigned char type;					 		
	unsigned char coding;				 		
	unsigned char length;					 		
	unsigned char tran_type;                 		
	char pchID[MAX_ID_LEN+1];			 		
	char pchPasswd[MAX_PASSWD_LEN+1]; 	 		
	char pchMsisdn[MAX_MSISDN_LEN+1];	 		
	char pchMessageID[MAX_MESSAGEID_LEN+1];  	
	char pchMessage[MAX_MSG_LEN+1];          	
	char pchSendTime[ORDERTIME_LEN+1];       	
};

struct sRecvMsg 
{
	unsigned char code;
	unsigned char coding;
	unsigned char length;
	char send_msisdn[MAX_MSISDN_LEN+1];
	char recv_msisdn[MAX_MSISDN_LEN+1];
	char buffer[MAX_MSG_LEN+1];
};











