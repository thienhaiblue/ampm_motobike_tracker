
#ifndef __UART_CONFIG_TASK_H__
#define __UART_CONFIG_TASK_H__

#include "app_config_task.h"

#define  UART_PACKET_SIZE		36

void UartConfigTaskInit(void);
void UartConfigTask(void);


#endif

