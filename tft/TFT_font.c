#include "TFT_font.h"

/*	打算脱离屏幕驱动和单片机底层 
 *	来实现一个完全在上层的取模实现库
 *	
 *	通过实现以下接口后即可使用
 *	注意... >>>屏幕显示方向<<< 和字符取模方向需要调整为以下方向
 *	填充方向:+------------>
 *			|			 x
 *			|	1  2  3
 *			|	|  |  |
 *			|	|  |  |
 *			|	V  V  V
 *			v y
 *	（即img2lcd中的 扫描模式:	垂直扫描 字节内像素数据反序）
 *	（以及PCtoLCD2002中 设置  阴码 逐列式 逆向(低位在前) ）
 *	注：以下提到坐标相关都是指 左上角
 *				2025/7/31-9:39
 */

/*  接口部分  */
	//屏幕驱动库
#include "TFT_ILI9341.h"
	//字体库
#include "qy_font.h"
#include "qy_pic.h"
	//主要实现的两个内部用函数
static void TFTF_SetRect(uint16_t x,uint16_t y,uint16_t width,uint16_t height)
{
	TFT_SetRect(x,y,width,height);
}
static void TFTF_Pixel(uint16_t rgb565)
{
	TFT_SendColor(rgb565);
}



/*  以下是软件实现  */
/*  移植时不用处理 
 *  如果想要优化效率
 *	可以进行修改调试
 */
/**@brief  填充矩形
  *@param  -
  *@param  color 颜色
  *@retval void
  */
void TFTF_Rect(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint16_t color)
{
	TFTF_SetRect(x,y,width,height);
	for(int i=0;i<width*height;i++)
	{
		TFTF_Pixel(color);
	}
}
/**@brief  绘制矩形框
  *@param  x y width height
  *@param  color 框颜色
  *@param  thick 框线宽
  *@retval void
  *@add    注：框架线是向内收缩的 x y width height是最大外边框
  */
void TFTF_Frame(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint16_t color,int8_t thick)
{
	if(thick<=0)
	{
		return;
	}
	TFTF_Rect(x,y,width,thick,color);
	TFTF_Rect(x,y+height-thick,width,thick,color);
	TFTF_Rect(x,y+thick,thick,height-thick*2,color);
	TFTF_Rect(x+width-thick,y+thick,thick,height-thick*2,color);
}
/**@brief  用于显示单色的图片		注:01 指的是只有0和1单色
  *@param  -位置和大小 			注:width建议8的整数倍
  *@param  pic					字模/图片
  *@param  ft_color(FronT) 		前景色 即1显示的颜色
  *@param  bk_color(BacKground) 背景色 即0显示的颜色
  */
void TFTF_Pic01(uint16_t x,uint16_t y,const char* pic,uint16_t height,uint16_t width,uint16_t ft_color,uint16_t bk_color)
{
	TFTF_SetRect(x,y,width,height);
	uint32_t a=width*height/8;
	for(uint32_t i=0;i<a;i++)
	{
		for(int j=0;j<8;j++)
		{
			if( (pic[i]&(0x01<<j)) != 0 )
			{
				TFTF_Pixel(ft_color);
			}
			else
			{
				TFTF_Pixel(bk_color);
			}
		}
	}
}
/**@brief  显示来自Image2Lcd的16位真彩色图片
  *@param  -位置和大小	无限制
  *@param  pic  两个相邻的char组成一个rgb565
  *@retval void
  */
void TFTF_Pic565(uint16_t x,uint16_t y,const char* pic,uint16_t width,uint16_t height)
{
	TFTF_SetRect(x,y,width,height);
	uint32_t a=width*height*2;
	for(uint32_t i=0;i<a;i+=2)
	{
		//如果颜色不对 用另一行代码
		TFTF_Pixel( ( pic[i]|(pic[i+1]<<8) ) );
		//TFTF_Pixel( ( (pic[i]<<8)|pic[i+1] ) );
	}
}





/*
 *	以下全是关于数码管数字字体测试的内容
 */
void TFTF_ENChar(uint16_t x,uint16_t y,uint16_t height,const char* en_font,uint16_t ft_color,uint16_t bk_color)
{
	TFTF_SetRect(x,y,height/2,height);
	uint32_t a=height*height/2/8;
	for(int i=0;i<a;i++)
	{
		for(int j=0;j<8;j++)
		{
			if( (en_font[i]&(0x01<<j)) != 0 )
			{
				TFTF_Pixel(ft_color);
			}
			else
			{
				TFTF_Pixel(bk_color);
			}
		}
	}
}
#define COLOR_BLUE    0x07FF
#define COLOR_RED     0xEC10
#define COLOR_YELLOW  0xEF31
#define COLOR_GREEN   0x7FC0
void FONT3216_NAME(const char* font,uint16_t y)
{
	TFTF_ENChar(18*0,y-32,32,(const char*)&font[0*64],QYF_COLOR_RED,QYF_COLOR_YELLOW);//3216
	TFTF_ENChar(18*1,y-32,32,(const char*)&font[1*64],QYF_COLOR_RED,QYF_COLOR_YELLOW);
	TFTF_ENChar(18*2,y-32,32,(const char*)&font[2*64],QYF_COLOR_RED,QYF_COLOR_YELLOW);
	TFTF_ENChar(18*3,y-32,32,(const char*)&font[3*64],QYF_COLOR_RED,QYF_COLOR_YELLOW);
}
void FONT1608_TEST(const char* font,uint16_t y)
{
	TFTF_ENChar( 68+10*1,y-16,16,(const char*)&font[1*16],QYF_COLOR_RED,QYF_COLOR_YELLOW);//1608
	TFTF_ENChar( 68+10*2,y-16,16,(const char*)&font[6*16],QYF_COLOR_RED,QYF_COLOR_YELLOW);
	TFTF_ENChar( 68+10*3,y-16,16,(const char*)&font[0*16],QYF_COLOR_RED,QYF_COLOR_YELLOW);
	TFTF_ENChar( 68+10*4,y-16,16,(const char*)&font[8*16],QYF_COLOR_RED,QYF_COLOR_YELLOW);
}
void FONT2412_TEST(const char* font,uint16_t y)
{
	TFTF_ENChar(108+14*1,y-24,24,(const char*)&font[2*36],QYF_COLOR_RED,QYF_COLOR_YELLOW);//2412
	TFTF_ENChar(108+14*2,y-24,24,(const char*)&font[4*36],QYF_COLOR_RED,QYF_COLOR_YELLOW);
	TFTF_ENChar(108+14*3,y-24,24,(const char*)&font[1*36],QYF_COLOR_RED,QYF_COLOR_YELLOW);
	TFTF_ENChar(108+14*4,y-24,24,(const char*)&font[2*36],QYF_COLOR_RED,QYF_COLOR_YELLOW);
}
void FONT3216_TEST(const char* font,uint16_t y)
{
	TFTF_ENChar(164+16*1,y-32,32,(const char*)&font[3*64],QYF_COLOR_RED,QYF_COLOR_YELLOW);//3216
	TFTF_ENChar(164+16*2,y-32,32,(const char*)&font[2*64],QYF_COLOR_RED,QYF_COLOR_YELLOW);
	TFTF_ENChar(164+16*3,y-32,32,(const char*)&font[1*64],QYF_COLOR_RED,QYF_COLOR_YELLOW);
	TFTF_ENChar(164+16*4,y-32,32,(const char*)&font[6*64],QYF_COLOR_RED,QYF_COLOR_YELLOW);
	TFTF_ENChar(164+18*5,y-32,32,(const char*)&font[8*64],QYF_COLOR_RED,QYF_COLOR_YELLOW);
}


//40-80-120-160-200-240
//PIXEL_LCD
void FONT_PIXEL(uint16_t y)
{
	FONT3216_NAME((const char*) qyf__EN_PIXEL_3216,y);
	FONT1608_TEST((const char*)qyf_NUM_PIXEL_1608,y);
	FONT2412_TEST((const char*)qyf_NUM_PIXEL_2412,y);
	FONT3216_TEST((const char*)qyf_NUM_PIXEL_3216,y);
	FONT3216_NAME((const char*) qyf__EN_PIXEL_3216,y);
	FONT1608_TEST((const char*)qyf_NUM_PIXEL_1608,y);
	FONT2412_TEST((const char*)qyf_NUM_PIXEL_2412,y);
	FONT3216_TEST((const char*)qyf_NUM_PIXEL_3216,y);
}
//NI7SEG
void FONT_NI7SEG(uint16_t y)
{
	FONT3216_NAME((const char*) qyf__EN_NI7SEG_3216,y);
	FONT1608_TEST((const char*)qyf_NUM_NI7SEG_1608,y);
	FONT2412_TEST((const char*)qyf_NUM_NI7SEG_2412,y);
	FONT3216_TEST((const char*)qyf_NUM_NI7SEG_3216,y);
}
//MingLIU
void FONT_MINGLIU(uint16_t y)
{
	FONT3216_NAME((const char*) qyf__EN_MINGLIU_3216,y);
	FONT1608_TEST((const char*)qyf_NUM_MINGLIU_1608,y);
	FONT2412_TEST((const char*)qyf_NUM_MINGLIU_2412,y);
	FONT3216_TEST((const char*)qyf_NUM_MINGLIU_3216,y);
}
//YFF(电子数字.TTF)
void FONT_YFF(uint16_t y)
{
	
	FONT3216_NAME((const char*) qyf__EN_YFF_3216,y);
	FONT1608_TEST((const char*)qyf_NUM_YFF_1608,y);
	FONT2412_TEST((const char*)qyf_NUM_YFF_2412,y);
	FONT3216_TEST((const char*)qyf_NUM_YFF_3216,y);

}
//PIXymbolsDigitClocksW90
void FONT_PIXY(uint16_t y)
{
	FONT3216_NAME((const char*) qyf__EN_PIXY_3216,y);
	FONT1608_TEST((const char*)qyf_NUM_PIXY_1608,y);
	FONT2412_TEST((const char*)qyf_NUM_PIXY_2412,y);
	FONT3216_TEST((const char*)qyf_NUM_PIXY_3216,y);
}
//advanced-pixel-lcd-7-1
void FONT_ADVANCE(uint16_t y)
{
	FONT3216_NAME((const char*) qyf__EN_ADVANCE_3216,y);
	FONT1608_TEST((const char*)qyf_NUM_ADVANCE_1608,y);
	FONT2412_TEST((const char*)qyf_NUM_ADVANCE_2412,y);
	FONT3216_TEST((const char*)qyf_NUM_ADVANCE_3216,y);
}


//六个数码管字体测试
void FontDig_Test(void)
{
	FONT_PIXEL(40);
	FONT_NI7SEG(80);
	FONT_MINGLIU(120);
	FONT_YFF(160);
	FONT_PIXY(200);
	FONT_ADVANCE(240);	
//	TFTF_Pic01(10,10,qyf_pic01_github_6464,64,64,COLOR_YELLOW,COLOR_BLUE);
//	TFTF_Pic01(80,10,qyf_NUM_PIXEL_3216[6],32,16,COLOR_RED,COLOR_YELLOW);
}
/**测试接口
  */
void TFTF_Test(void)
{
	FontDig_Test();
}






