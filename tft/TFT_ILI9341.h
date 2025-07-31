#ifndef __TFT_ILI9341_H__
#define __TFT_ILI9341_H__
#include "stdint.h"

void Init_TFT(void);
void TFT_SendColor(uint16_t rgb565);
void TFT_SetRect(uint16_t X1, uint16_t Y1, uint16_t width, uint16_t height);
uint16_t TFT_RGB888To565(uint32_t RGB888);
void TFT_Test(void);

#endif
