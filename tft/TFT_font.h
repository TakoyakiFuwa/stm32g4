#ifndef __TFT_FONT_H__
#define __TFT_FONT_H__
#include "stdint.h"
/*	有关函数命名前缀为TFTF_ (即TFT_Font)
 */

/*  关于形状的部分  */
void TFTF_DrawRect(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint16_t color);
void TFTF_DrawFrame(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint16_t color,int8_t thick);
void TFTF_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color,int8_t thick);

//字体配置的结构体
typedef struct tftf_font{
	const char* font_lib;	//字体库/图片库
	uint16_t 	size_height;		//字体库的高度
	uint16_t 	size_width;			//字体库的宽度
	uint16_t 	ft_color;	//前景色 (front-color字体色)
	uint16_t	bk_color;	//背景色 (back-color背景色)
}tftf_font;
tftf_font TFTF_CreateFont(const char* font_lib,uint16_t height,uint16_t width,uint16_t ft_color,uint16_t bk_color);

/*  关于取模显示的部分  */
void TFTF_Single_01Pic(uint16_t x,uint16_t y,tftf_font pic01);
void TFTF_Single_595Pic(uint16_t x,uint16_t y,tftf_font pic565);
void TFTF_Single_Char(uint16_t x,uint16_t y,char _char,tftf_font font);

/*  关于文本/数字的部分  */
void TFTF_ShowNum(uint16_t x,uint16_t y,uint32_t num,tftf_font font,int8_t digits);
void TFTF_ShowString(uint16_t x,uint16_t y,const char* text,tftf_font font,int8_t NumOfChar);

/*  测试接口  */
void TFTF_Test(void);

#endif
