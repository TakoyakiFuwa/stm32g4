#ifndef __UI_INSTANCE_H__
#define __UI_INSTANCE_H__
#include "TFT_UI.h"

//数字
tft_obj 	INS_Obj_Num(uint16_t x,uint16_t y,uint8_t digit,uint8_t color,uint8_t font_index,uint32_t value_num);
tft_ui 		INS_UI_Num_Normal(tft_obj* obj,int8_t front_ui,int8_t next_ui);
tft_ui 		INS_UI_Num_Digit(tft_obj* obj,uint8_t default_digit,int8_t front_ui,int8_t next_ui);
void 		INS_Status_Num_Normal(tft_obj* obj,uint16_t n_1,uint8_t color1,uint16_t n_2,uint8_t color2,uint16_t n_3,uint8_t color3);

//文本
tft_obj 	INS_Obj_Text(uint16_t x,uint16_t y,const char* text,uint8_t digit,uint8_t color,uint8_t font_index);
tft_ui 		INS_UI_Text(tft_obj* obj,int8_t front_ui,int8_t next_ui);
void 		INS_Status_Text(tft_obj* o,const char* words,uint16_t color,uint8_t font);

//图片
tft_obj INS_Obj_Pic(uint16_t x,uint16_t y,uint8_t color,uint8_t pic_index);
tft_ui INS_UI_Pic(tft_obj* o,int8_t front_ui,int8_t next_ui);
void INS_Status_Pic(tft_obj* o,uint8_t color,uint8_t pic_index);

//测试页面
tft_ui* Page_TEST(void);

#endif
