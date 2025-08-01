#ifndef __TFT_FONT_H__
#define __TFT_FONT_H__
#include "stdint.h"
/*	有关函数命名前缀为TFTF_ (即TFT_Font)
 */
//void TFTF_Line(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color,int8_t thick);
void TFTF_Rect(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint16_t color);
void TFTF_Frame(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint16_t color,int8_t thick);
void TFTF_Pic01(uint16_t x,uint16_t y,const char* pic,uint16_t height,uint16_t width,uint16_t ft_color,uint16_t bk_color);
void TFTF_Pic565(uint16_t x,uint16_t y,const char* pic,uint16_t width,uint16_t height);
  
/*  测试接口  */
void TFTF_Test(void);

#endif
