#include "TFT_font.h"
#include "qy_printf.h"//用于代码编写时调试输出，可删

/*	打算脱离屏幕驱动和单片机底层 
 *	来实现一个完全在上层的取模实现库
 *	
 *	通过实现以下接口后即可使用
 *	注意... >>>屏幕显示方向<<< 和字符取模方向需要调整为以下方向
 *			^ y
 *			|
 *			|   3---> 
 *			|   2---> 
 *			|   1---> 
 *			|			 x 
 *	填充方向:+------------>   
 *	（即img2lcd中的 扫描模式:水平扫描 自底至顶扫描）
 *	（以及PCtoLCD2002中 设置 阴码 逐行式 逆向(低位在前) 
 *					   文字水平翻转 ）
 *	注：以下提到坐标相关都是指 左下角
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
  *@param  ft_color(FronT) 前景色 即1显示的颜色
  *@param  bk_color(BacKground) 背景色 即0显示的颜色
  */
void TFTF_Pic01(uint16_t x,uint16_t y,const char* pic,uint16_t width,uint16_t height,uint16_t ft_color,uint16_t bk_color)
{
	TFTF_SetRect(x,y,width,height);
	uint32_t a=width*height/8;
	for(uint32_t i=0;i<a;i++)
	{
		for(int j=0;j<8;j++)
		{
			if( (pic[i]&(0x80>>j)) != 0 )
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
/**@brief  显示PCtoLCD2002取模的英文字符
  *@param  xy 		位置
  *@param  width	英文宽度 注:英文字符高度为宽度的2倍
  *@param  en_font	字符模板
  *@param  ft_color(FronT) 前景色 即1显示的颜色
  *@param  bk_color(BacKground) 背景色 即0显示的颜色
  *@retval void
  */
void TFTF_ENChar(uint16_t x,uint16_t y,uint16_t width,const char* en_font,uint16_t ft_color,uint16_t bk_color)
{
	TFTF_SetRect(x,y,width,width*2);
	for(int i=width*width*2/8-1;i>=0;i--)
	{
		for(int j=0;j<8;j++)
		{
			if( (en_font[i]&(0x80>>j)) != 0 )
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
/**@brief  显示PCtoLCD2002取模的汉字，或者图片
  *@param  xy 		位置
  *@param  width	中文宽度 注:中文的宽和高一致
  *@param  cn_font	字符模板
  *@param  ft_color(FronT) 前景色 即1显示的颜色
  *@param  bk_color(BacKground) 背景色 即0显示的颜色
  *@retval void
  */
void TFTF_CNChar(uint16_t x,uint16_t y,uint16_t width,const char* cn_font,uint16_t ft_color,uint16_t bk_color)
{
	TFTF_SetRect(x,y,width,width);
	for(int i=width*width/8-1;i>=0;i--)
	{
		for(int j=0;j<8;j++)
		{
			if( (cn_font[i]&(0x80>>j)) != 0 )
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


void TFTF_Test(void)
{
	TFTF_Frame(10,20,50,64,0x897B,3);
	TFTF_Pic01(65,20,qyf_pic01_github_6464,64,64,QYF_COLOR_RED,QYF_COLOR_YELLOW);
	TFTF_Pic565(130,20,qyf_pic595_bing_6432,64,32);
	TFTF_ENChar(130,50,8,qyf_EN_DJB_8[1+'0'-' '],QYF_COLOR_RED,QYF_COLOR_YELLOW);
	TFTF_ENChar(130+8,50,16,qyf_EN_DJB_16[2+'0'-' '],QYF_COLOR_RED,QYF_COLOR_YELLOW);
	TFTF_ENChar(130+8+16,50,24,qyf_EN_DJB_24[3+'0'-' '],QYF_COLOR_RED,QYF_COLOR_YELLOW);
//	TFTF_ENChar(130+8+16+24,50,32,qyf_EN_DJB_32[4+'0'-' '],QYF_COLOR_RED,QYF_COLOR_YELLOW);
	TFTF_CNChar(130+24*0,100,24,qyf_CN_DJB_24[0],QYF_COLOR_RED,QYF_COLOR_YELLOW);
	TFTF_CNChar(130+24*1,100,24,qyf_CN_DJB_24[1],QYF_COLOR_RED,QYF_COLOR_YELLOW);
	TFTF_CNChar(130+24*2,100,24,qyf_CN_DJB_24[2],QYF_COLOR_RED,QYF_COLOR_YELLOW);
	TFTF_CNChar(130+24*3,100,24,qyf_CN_DJB_24[3],QYF_COLOR_RED,QYF_COLOR_YELLOW);
	TFTF_CNChar(130+24*4,100,24,qyf_CN_DJB_24[4],QYF_COLOR_RED,QYF_COLOR_YELLOW);
	
	char test_string[] = {"String.123!+-{|}"};
	for(int i=0;test_string[i]!='\0';i++)
	{
		TFTF_ENChar(10+i*16,150,16,qyf_EN_DJB_16[test_string[i]-' '],QYF_COLOR_RED,QYF_COLOR_YELLOW);
	}
	
	QY_Printf("这里是TFT_Font测试接口 \r\n");
}














