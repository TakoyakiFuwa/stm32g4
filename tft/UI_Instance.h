#ifndef __UI_INSTANCE_H__
#define __UI_INSTANCE_H__
#include "TFT_font.h"
#include "TFT_UI.h"

tft_font* INS_Init_UI(void);
tft_page* INS_Init_Page(void);

//测试接口
void INS_Test(void);

#endif
