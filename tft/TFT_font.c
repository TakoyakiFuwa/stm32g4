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
	//字体库/图形库
#include "qy_ascii_font.h"
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

/*  关于形状的部分  */

/**@brief  填充矩形
  *@param  -
  *@param  color 颜色
  *@retval void
  */
void TFTF_DrawRect(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint16_t color)
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
void TFTF_DrawFrame(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint16_t color,int8_t thick)
{
	if(thick<=0)
	{
		return;
	}
	TFTF_DrawRect(x,y,width,thick,color);
	TFTF_DrawRect(x,y+height-thick,width,thick,color);
	TFTF_DrawRect(x,y+thick,thick,height-thick*2,color);
	TFTF_DrawRect(x+width-thick,y+thick,thick,height-thick*2,color);
}
/**@brief  两点之间连线
  *@param  x1,y1 起始点
  *@param  x2,y2 终止点
  *@param  color 颜色
  *@param  thick 厚度
  *@add    注：从前辈项目里面ctrl cv来的（函数不错 我的了）
  */
void TFTF_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color,int8_t thick)
{
	if(thick<=0)
	{
		return;
	}
	uint16_t t;
	int xerr = 0, yerr = 0, delta_x, delta_y, distance;
	int incx, incy, uRow, uCol;
	int d_thick = thick/2;
	
	delta_x = x2 - x1; // 计算坐标增量
	delta_y = y2 - y1;
	uRow = x1;
	uCol = y1;
	if (delta_x > 0)
		incx = 1; // 设置单步方向
	else if (delta_x == 0)
		incx = 0; // 垂直线
	else
	{
		incx = -1;
		delta_x = -delta_x;
	}
	if (delta_y > 0)
		incy = 1;
	else if (delta_y == 0)
		incy = 0; // 水平线
	else
	{
		incy = -1;
		delta_y = -delta_y;
	}
	if (delta_x > delta_y)
		distance = delta_x; // 选取基本增量坐标轴
	else
		distance = delta_y;
	for (t = 0; t <= distance + 1; t++) // 画线输出
	{ // 画点
		TFTF_SetRect(uRow-d_thick,uCol-d_thick,thick,thick);
		for(int i=0;i<thick*thick;i++)
		{
			TFTF_Pixel(color);
		}
		xerr += delta_x;
		yerr += delta_y;
		if (xerr > distance)
		{
			xerr -= distance;
			uRow += incx;
		}
		if (yerr > distance)
		{
			yerr -= distance;
			uCol += incy;
		}
	}
}

/*  关于取模显示的部分  */

/**@brief  创建一个字体
  *@param  - 字体参数
  */
tft_font TFTF_CreateFont(const char* font_lib,uint16_t height,uint16_t width,uint16_t ft_color,uint16_t bk_color)
{
	tft_font f;
	f.font_lib = font_lib;
	f.size_height = height;
	f.size_width = width;
	f.ft_color = ft_color;
	f.bk_color = bk_color;
	return f;
}
/**@brief  显示单张单色图片
  *@param  x,y 坐标(左上角)
  *@param  pic01 图片内容
  */
void TFTF_Single_01Pic(uint16_t x,uint16_t y,tft_font pic01)
{
	TFTF_SetRect(x,y,pic01.size_width,pic01.size_height);
	uint32_t a=pic01.size_width*pic01.size_height/8;
	for(uint32_t i=0;i<a;i++)
	{
		for(int j=0;j<8;j++)
		{
			if( (pic01.font_lib[i]&(0x01<<j)) != 0 )
			{
				TFTF_Pixel(pic01.ft_color);
			}
			else
			{
				TFTF_Pixel(pic01.bk_color);
			}
		}
	}
}
/**@brief  显示单张rgb565图片
  *@param  x,y 坐标(左上角)
  *@param  pic565 图片内容
  */
void TFTF_Single_595Pic(uint16_t x,uint16_t y,tft_font pic565)
{
	TFTF_SetRect(x,y,pic565.size_width,pic565.size_height);
	uint32_t a=pic565.size_width*pic565.size_height*2;
	for(uint32_t i=0;i<a;i+=2)
	{
		//如果颜色不对 用另一行代码
		TFTF_Pixel( ( pic565.font_lib[i]|(pic565.font_lib[i+1]<<8) ) );
		//TFTF_Pixel( ( (pic[i]<<8)|pic[i+1] ) );
	}
}
/**@brief  显示单个字符
  *@param  	x,y 	位置
  *@param	_char 	要显示的字符
  *@param  font		字体
  */
void TFTF_Single_Char(uint16_t x,uint16_t y,char _char,tft_font font)
{
	TFTF_SetRect(x,y,font.size_width,font.size_height);
	uint32_t a=font.size_width*font.size_height/8;
	uint16_t index = (_char-' ')*font.size_height*font.size_width/8;
	for(uint32_t i=0;i<a;i++)
	{
		for(int j=0;j<8;j++)
		{
			if( (font.font_lib[index+i]&(0x01<<j)) != 0 )
			{
				TFTF_Pixel(font.ft_color);
			}
			else
			{
				TFTF_Pixel(font.bk_color);
			}
		}
	}
}

/*  关于文本的部分  */

/*@brief  显示一个数字
 *@param  -
 *@param  digits  显示的位数，超过位数会吞掉低位
 */
void TFTF_ShowNum(uint16_t x,uint16_t y,uint32_t num,tft_font font,int8_t digits)
{
	uint32_t num_length = 1;
	int8_t overflow_digits = digits;
	//测量数字位数
	for(;num_length<=num;num_length*=10)
	{
		digits--;
	}
	for(;digits>0;digits--)
	{
		num_length*=10;
	}
	digits=0;
	//从高位开始显示
	int8_t i=0;
	for(num_length/=10;num_length>=1;num_length/=10)
	{
		if(++digits>overflow_digits)
		{
			TFTF_Single_Char(x+font.size_width*(--i),y,'-',font);
			return;
		}
		TFTF_Single_Char(x+font.size_width*(i++),y,num/num_length+'0',font);
		//减去最高位
		num -= (num - (num%num_length));
	}
}
/*@brief  字符串
 *@param  -
 *@param  NumOfChar  显示的数量
 */
void TFTF_ShowString(uint16_t x,uint16_t y,const char* text,tft_font font,int8_t NumOfChar)
{
	int i=0;
	for(;text[i]!='\0';i++)
	{
		if(--NumOfChar<0)
		{
			TFTF_Single_Char(x+font.size_width*(--i),y,'-',font);
			return;
		}
		TFTF_Single_Char(x+font.size_width*(i),y,text[i],font);
	}
	for(;NumOfChar>0;NumOfChar--)
	{
		TFTF_Single_Char(x+font.size_width*(i++),y,' ',font);
	}
}




/**测试接口
  */
void TFTF_Test(void)
{
	tft_font f;
	f.font_lib = (const char*)font_ASCII_PIXEL_3216;
	f.size_width = 16;
	f.size_height = 32;
	f.bk_color = COLOR_YELLOW;
	f.ft_color = COLOR_RED;
	TFTF_ShowNum(10,100,876,f,4);
	TFTF_ShowString(10,132,"Hello",f,5);
	
	f.font_lib = (const char*)qyf_pic01_github_6464;
	f.size_height = 64;
	f.size_width  = 64;
	TFTF_Single_01Pic(100,100,f);
}






