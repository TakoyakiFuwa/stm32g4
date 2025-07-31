#ifndef __TFT_FONT_H__
#define __TFT_FONT_H__
#include "stdint.h"
/*	有关函数命名前缀为TFTF_ (即TFT_Font)
 */
void TFTF_Rect(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint16_t color);
void TFTF_Frame(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint16_t color,int8_t thick);
void TFTF_Pic01(uint16_t x,uint16_t y,const char* pic,uint16_t width,uint16_t height,uint16_t ft_color,uint16_t bk_color);
void TFTF_Pic565(uint16_t x,uint16_t y,const char* pic,uint16_t width,uint16_t height);
void TFTF_ENChar(uint16_t x,uint16_t y,uint16_t width,const char* en_font,uint16_t ft_color,uint16_t bk_color);
void TFTF_CNChar(uint16_t x,uint16_t y,uint16_t width,const char* cn_font,uint16_t ft_color,uint16_t bk_color);

/*  测试接口  */
void TFTF_Test(void);

#endif
