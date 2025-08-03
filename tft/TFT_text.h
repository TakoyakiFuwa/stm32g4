#ifndef __TFT_TEXT_H__
#define __TFT_TEXT_H__
#include "stdint.h"

/*
配置字体属性，写成结构体方便调用:
显示带进制的数字:
字符串输出:
 */

typedef struct tftt_font{
	const char* font_lib;	//字体库/图片库
	uint16_t 	size_height;		//字体库的高度
	uint16_t 	size_width;			//字体库的宽度
	uint16_t 	ft_color;	//前景色 (front-color字体色)
	uint16_t	bk_color;	//背景色 (back-color背景色)
}tftt_font;

void TFTT_ShowPIC(uint16_t x,uint16_t y,tftt_font pic);
void TFTT_ShowNum(uint16_t x,uint16_t y,uint32_t num,tftt_font font,int8_t digits);
void TFTT_ShowString(uint16_t x,uint16_t y,const char* text,tftt_font font,int8_t NumOfChar);

void TFTT_Test(void);

#endif


