#include "qy_scanf.h"
#include "qy_printf.h"
#include "usart.h"
#include "TFT_UI.h"
#include "TFT_font.h"
#include "UI_Instance_example.h"
#include "UI_Ins_ColorBox.h"

extern uint8_t buff;
extern tft_ui UI[10];
extern tft_pointer UI_CURSOR;
extern tft_page 	PAGE[20];

uint8_t pointer_ui_index = 0;
void QY_Scanf_Handler(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART1)//检测是否为USART1中断
	{	
		QY_Printf("%c",buff);
		switch(buff)
		{
		case 'w':
			UI_CURSOR.ptr_ui->Func_Event_UP(UI_CURSOR.ptr_ui);
			QY_Printf("\r\n 'w'->上 \r\nuser:");
			break;
		case 's':
			UI_CURSOR.ptr_ui->Func_Event_DOWN(UI_CURSOR.ptr_ui);
			QY_Printf("\r\n 's'->下 \r\nuser:");
			break;
		case 'a':
			UI_CURSOR.ptr_ui->Func_Event_LEFT(UI_CURSOR.ptr_ui);
			QY_Printf("\r\n 'a'->左 \r\nuser:");
			break;
		case 'd':
			UI_CURSOR.ptr_ui->Func_Event_RIGHT(UI_CURSOR.ptr_ui);
			QY_Printf("\r\n 'd'->右 \r\nuser:");		
			break;
		case '\r':
		case '\n':case 'f':
			UI_CURSOR.ptr_ui->Func_Event_F(UI_CURSOR.ptr_ui);
			QY_Printf("\r\n 'enter'/'f'-> \r\nuser:");
			break;
		case 'q':
			QY_Printf("\r\n 'q'->进入颜色配置 \r\nuser:");
			ColorBox_Enter();
			break;
		case 'r':
			HAL_NVIC_SystemReset();
			break;
		default:
			QY_Printf("\r\n 这里没有按键绑定 \r\nuser:");
		}
		
		
		//重新启用接受中断
		
		HAL_UART_Receive_IT(&huart1,&buff,sizeof(buff));
	}
}


