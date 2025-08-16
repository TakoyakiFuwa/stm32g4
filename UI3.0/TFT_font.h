#ifndef __TFT_FONT_H__
#define __TFT_FONT_H__
#include "stdint.h"
#include "TFT_UI.h"
/*	有关函数命名前缀为TFTF_ (即TFT_Font)
 */

/*  关于形状的部分  */
void TFTF_DrawRect(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint16_t color);
void TFTF_DrawFrame(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint16_t color,int8_t thick);
void TFTF_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color,int8_t thick);

/*  关于取模显示的部分  */
void TFTF_Single_01Pic(uint16_t x,uint16_t y,uint8_t pic01,uint8_t color_index);
void TFTF_Single_Char(uint16_t x,uint16_t y,char _char,uint8_t font,uint8_t color_index);

/*  关于文本/数字的部分  */
void TFTF_ShowNum(uint16_t x,uint16_t y,uint32_t num,uint8_t font,uint8_t color_index,int8_t digits);
void TFTF_ShowString(uint16_t x,uint16_t y,const char* text,uint8_t font,uint8_t color_index,int8_t NumOfChar);

/*  测试接口  */
//void TFTF_Test(void);

#endif
