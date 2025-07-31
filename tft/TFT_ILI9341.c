#include "TFT_ILI9341.h"
#include "gpio.h"
//utf-8

/*	TFT的底层驱动
 *	ILI9341
 *		2025/7/30-11:06
 */

/*	以下是关于引脚接线内容
 *	可根据单片机类型和接口并口/SPI(软件/硬件)进行调整和适配
 *	ILI9341
 *  当前配置为:CS低电平选中，RST低电平复位，D/C高电平数据/低电平指令
 *            MOSI传数据，SCK下降沿时屏幕采集信号
 */

/*	PA0  ->CS
 *	PA1	 ->RST
 *	PA2	 ->D/C
 *	PA3	 ->MOSI
 *	PA4	 ->SCK
 */
/* 				^ y						^ y
 * 				|						|
 * 				|	红  黄				|	3--->
 * 				|						|	2--->
 * 				|	蓝  绿				|	1--->
 * 				|			x			|			 x
 * 	当前显示方向:+------------>	填充方向:+------------>
 */
//PA0  ->CS
#define PIN_CSH		GPIOA->BSRR = (uint32_t)GPIO_PIN_0
#define PIN_CSL		GPIOA->BRR  = (uint32_t)GPIO_PIN_0
//PA1  ->RST
#define PIN_RSTH	GPIOA->BSRR = (uint32_t)GPIO_PIN_1
#define PIN_RSTL	GPIOA->BRR  = (uint32_t)GPIO_PIN_1
//PA2  ->DC
#define PIN_DC_DATA		GPIOA->BSRR = (uint32_t)GPIO_PIN_2
#define PIN_DC_CMD		GPIOA->BRR  = (uint32_t)GPIO_PIN_2
//PA3  ->MOSI
#define PIN_DATAH	GPIOA->BSRR = (uint32_t)GPIO_PIN_3
#define PIN_DATAL	GPIOA->BRR  = (uint32_t)GPIO_PIN_3
//PA4  ->SCK
#define PIN_SCKH	GPIOA->BSRR = (uint32_t)GPIO_PIN_4
#define PIN_SCKL	GPIOA->BRR  = (uint32_t)GPIO_PIN_4

/**@brief  TFT引脚初始化
  *@param  void
  *@retval void
  *@add    移植时根据接线调整
  */
static void Init_TFT_PIN(void)
{
	//引脚初始化
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_MEDIUM;
	HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);   
}
/**@brief  与TFT交换数据
  *@param  byte 交换的数据
  *@retval void
  *@add    交换1byte数据，可根据接口调整成并口
  */
void TFT_Swap(uint8_t byte)
{
	PIN_CSL;//对hal库不太熟悉 有时间回来改成硬件SPI
	for(int i=0;i<8;i++)
	{
		if( (byte&(0x80>>i)) != 0 )
		{
			PIN_DATAH;
		}
		else 
		{
			PIN_DATAL;
		}
		PIN_SCKL;
		PIN_SCKH;
	}
	PIN_CSH;
}




/*
 *	以下是ILI9341的软件驱动
 *	与接口无关 无需更改 
 */




//屏幕长和宽
uint16_t D_TFT_WIDTH = 240;
uint16_t D_TFT_HEIGHT = 320;		
//两个初始化函数
static void TFT_Reset(void);
static void TFT_SoftwareInit(void);
/**@brief  TFT(ILI9341)初始化函数
  *@param  void
  *@retval void
  */
void Init_TFT(void)
{
	Init_TFT_PIN();
	//引脚初始电位
	PIN_CSH;
	PIN_SCKH;
	PIN_RSTH;
	
	//屏幕复位
	TFT_Reset();
	//软件初始化
	TFT_SoftwareInit();
	//简单绘制背景
	TFT_Test();
}
/**@brief  硬件复位
  */
static void TFT_Reset(void)
{
	PIN_RSTL;
	HAL_Delay(200);
	PIN_RSTH;
}
/**@brief  发送指令
  */
static void TFT_SendCmd(uint8_t cmd)
{
	PIN_DC_CMD;
	TFT_Swap(cmd);
}
/**@brief  发送数据
  */
static void TFT_SendData(uint8_t data)
{
	PIN_DC_DATA;
	TFT_Swap(data);
}
/**@brief  在使用TFT_SetRect后用于填充颜色和图案
  */
void TFT_SendColor(uint16_t rgb565)
{
	PIN_DC_DATA;
	TFT_Swap((rgb565>>8));
	TFT_Swap(rgb565);
}
/**@brief  设置显示区
  *@param  X Y 显示起点
  *@param  width height 宽度/高度
  *@retval void
  */
void TFT_SetRect(uint16_t X1, uint16_t Y1, uint16_t width, uint16_t height)
{
	width+=X1-1;
	height+=Y1-1;
	
	TFT_SendCmd(0x2A);
	TFT_SendData(X1>>8);
	TFT_SendData(X1);
	TFT_SendData(width>>8);
	TFT_SendData(width);

	TFT_SendCmd(0x2B);
	TFT_SendData(Y1>>8);
	TFT_SendData(Y1);
	TFT_SendData(height>>8);
	TFT_SendData(height);

	TFT_SendCmd(0x2C);
}
/**@brief  设置屏幕显示方向
  *@param  rotation
  *@add    D7~D2:MY MX MV ML BGR MH (建议0x68 0b0110 1000->X反转横向显示)
  *		         MX/MY->XY翻转
  *				 MV->0竖向/1横向显示
  *				 ML/MH->刷新(填充)方向
  *				 BGR->1
  */
static void TFT_SetRotation(uint8_t rotation)
{
	TFT_SendCmd(0x36);
	if( (rotation&0x20) != 0)
	{
		D_TFT_WIDTH  = 320;
		D_TFT_HEIGHT = 240;
	}
	else 
	{
		D_TFT_WIDTH  = 240;
		D_TFT_HEIGHT = 320;
	}
	TFT_SendData(rotation);
}
/**@brief  从RGB888转换成RGB565
  *@param  RGB888 颜色代码
  *@retval RGB565颜色代码
  */
uint16_t TFT_RGB888To565(uint32_t RGB888)
{
	uint16_t RGB565 = 0;
	RGB565 = RGB888>>19;
	RGB565 = RGB565<<6;
	RGB565 |= ((RGB888>>10)&0x3F);
	RGB565 = RGB565<<5;
	RGB565 |= ((RGB888>>3)&0x1F);
	return RGB565;
}
/**@brief  在main.c中的测试接口
  *@param  void
  *@retval void
  */
void TFT_Test(void)
{
	uint16_t COLOR_RED 		= TFT_RGB888To565(0xFFB6C1);
	uint16_t COLOR_YELLOW 	= TFT_RGB888To565(0xFAD5A5);
	uint16_t COLOR_BLUE		= TFT_RGB888To565(0x5FCDE4);
	uint16_t COLOR_GREEN	= TFT_RGB888To565(0x7cFC00);
	//		红 黄
	//绘制  	蓝 绿
	uint16_t size = (D_TFT_HEIGHT/2)*(D_TFT_WIDTH/2)+20;
	TFT_SetRect(D_TFT_WIDTH/2,0,D_TFT_WIDTH/2,D_TFT_HEIGHT/2);
	for(int i=0;i<size;i++)
	{
		TFT_SendColor(COLOR_GREEN);
	}
	TFT_SetRect(0,D_TFT_HEIGHT/2,D_TFT_WIDTH/2,D_TFT_HEIGHT/2);
	for(int i=0;i<size;i++)
	{
		TFT_SendColor(COLOR_RED);
	}
	TFT_SetRect(D_TFT_WIDTH/2,D_TFT_HEIGHT/2,D_TFT_WIDTH/2,D_TFT_HEIGHT/2);
	for(int i=0;i<size;i++)
	{
		TFT_SendColor(COLOR_YELLOW);
	}
	TFT_SetRect(0,0,D_TFT_WIDTH/2,D_TFT_HEIGHT/2);
	for(int i=0;i<size;i++)
	{
		//想要切实观察刷新方向时可以添加这个Delay
//		HAL_Delay(3);
		TFT_SendColor(COLOR_BLUE);
	}	
}
/**@brief  TFT软件初始化
  */
static void TFT_SoftwareInit(void)
{	
	TFT_SendCmd(0x01);
	HAL_Delay(1000);
		
	//POWER CONTROL A
	TFT_SendCmd(0xCB);
	TFT_SendData(0x39);
	TFT_SendData(0x2C);
	TFT_SendData(0x00);
	TFT_SendData(0x34);
	TFT_SendData(0x02);
	
	//POWER CONTROL B
	TFT_SendCmd(0xCF);
	TFT_SendData(0x00);
	TFT_SendData(0xC1);
	TFT_SendData(0x30);
	
	//DRIVER TIMING CONTROL A
	TFT_SendCmd(0xE8);
	TFT_SendData(0x85);
	TFT_SendData(0x00);
	TFT_SendData(0x78);
	
	//DRIVER TIMING CONTROL B
	TFT_SendCmd(0xEA);
	TFT_SendData(0x00);
	TFT_SendData(0x00);
	
	//POWER ON SEQUENCE CONTROL
	TFT_SendCmd(0xED);
	TFT_SendData(0x64);
	TFT_SendData(0x03);
	TFT_SendData(0x12);
	TFT_SendData(0x81);
	
	//PUMP RATIO CONTROL
	TFT_SendCmd(0xF7);
	TFT_SendData(0x20);
	
	//POWER CONTROL,VRH[5:0]
	TFT_SendCmd(0xC0);
	TFT_SendData(0x23);
	
	//POWER CONTROL,SAP[2:0];BT[3:0]
	TFT_SendCmd(0xC1);
	TFT_SendData(0x10);
	
	//VCM CONTROL
	TFT_SendCmd(0xC5);
	TFT_SendData(0x3E);
	TFT_SendData(0x28);
	
	//VCM CONTROL 2
	TFT_SendCmd(0xC7);
	TFT_SendData(0x86);
	
	//MEMORY ACCESS CONTROL
	TFT_SendCmd(0x36);
	TFT_SendData(0x48);
	
	//PIXEL FORMAT
	TFT_SendCmd(0x3A);
	TFT_SendData(0x55);
	
	//FRAME RATIO CONTROL, STANDARD RGB COLOR
	TFT_SendCmd(0xB1);
	TFT_SendData(0x00);
	TFT_SendData(0x18);
	
	//DISPLAY FUNCTION CONTROL
	TFT_SendCmd(0xB6);
	TFT_SendData(0x08);
	TFT_SendData(0x82);
	TFT_SendData(0x27);
	
	//3GAMMA FUNCTION DISABLE
	TFT_SendCmd(0xF2);
	TFT_SendData(0x00);
	
	//GAMMA CURVE SELECTED
	TFT_SendCmd(0x26);
	TFT_SendData(0x01);
	
	//POSITIVE GAMMA CORRECTION
	TFT_SendCmd(0xE0);
	TFT_SendData(0x0F);
	TFT_SendData(0x31);
	TFT_SendData(0x2B);
	TFT_SendData(0x0C);
	TFT_SendData(0x0E);
	TFT_SendData(0x08);
	TFT_SendData(0x4E);
	TFT_SendData(0xF1);
	TFT_SendData(0x37);
	TFT_SendData(0x07);
	TFT_SendData(0x10);
	TFT_SendData(0x03);
	TFT_SendData(0x0E);
	TFT_SendData(0x09);
	TFT_SendData(0x00);
	
	//NEGATIVE GAMMA CORRECTION
	TFT_SendCmd(0xE1);
	TFT_SendData(0x00);
	TFT_SendData(0x0E);
	TFT_SendData(0x14);
	TFT_SendData(0x03);
	TFT_SendData(0x11);
	TFT_SendData(0x07);
	TFT_SendData(0x31);
	TFT_SendData(0xC1);
	TFT_SendData(0x48);
	TFT_SendData(0x08);
	TFT_SendData(0x0F);
	TFT_SendData(0x0C);
	TFT_SendData(0x31);
	TFT_SendData(0x36);
	TFT_SendData(0x0F);
	
	//EXIT SLEEP
	TFT_SendCmd(0x11);
	HAL_Delay(120);
	
	//TURN ON DISPLAY
	TFT_SendCmd(0x29);
	
	//设置旋转方向
	TFT_SetRotation(0x68);

}














