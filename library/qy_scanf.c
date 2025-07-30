#include "qy_scanf.h"
#include "qy_printf.h"
#include "usart.h"

extern uint8_t buff;

void QY_Scanf_Handler(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART1)//检测是否为USART1中断
	{	
		QY_Printf("%c",buff);
		
		//重新启用接受中断
		
		HAL_UART_Receive_IT(&huart1,&buff,sizeof(buff));
	}
}


