#ifndef __QY_TFT_H__
#define __QY_TFT_H__
#include "stdint.h"

void Init_TFT(void);
void TFT_SendColor(uint16_t rgb565);
void TFT_SetRect(uint16_t X1, uint16_t Y1, uint16_t width, uint16_t height);
//void TFT_SetRotation(uint8_t rotation); //初始化时设定好即可
uint16_t TFT_RGB888To565(uint32_t RGB888);
void TFT_Test(void);

#endif
