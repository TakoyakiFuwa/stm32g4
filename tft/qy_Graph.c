#include "qy_Graph.h"
#include "TFT_ILI9341.h"
#include "qy_printf.h"
#include "font.h"

/*	注意渲染方向当前仅适配下图填充方向
 * 		  ^ y
 * 		  |
 * 		  | 	--->3
 * 		  | 	--->2
 * 		  | 	--->1
 * 		  | 		   x
 *填充方向:+------------>
 */
/*	关于图案、文字、数字(变量)绘制
 *			2025/7/30-16:56
 */
////关于图像渲染先等一下吧...
//void TFTG_DrawPic(tftg_pic p)

/**@brief  适配字体库里面1608字体
  *@param  p  默认大小8*16
  */
void TFTG_Font1608(tftg_pic p)
{
	TFT_SetRect(p.rect.x,p.rect.y,8,16);
	for(int down_up=1;down_up>=0;down_up--)
	{
		for(int i=7;i>=0;i--)
		{
			for(int j=0;j<8;j++)
			{
				if( (p.pic[down_up*8+i]&(0x01<<j)) !=0)
				{
					TFT_SendColor(p.color_front);
				}
				else
				{
					TFT_SendColor(p.color_back);
				}
			}
		}
	}
}
/**@breif  创建一个矩形
  *@param  -
  *@retval tftg_rect 创建出的矩形
  */
tftg_rect TFTG_CreateRect(uint16_t x,uint16_t y,uint16_t width,uint16_t height)
{
	tftg_rect r;
	r.x=x;
	r.y=y;
	r.width=width;
	r.height=height;
	return r;
}
/**@brief  填充矩形
  *@param  -
  *@param  color 颜色
  *@retval void
  */
void TFTG_Rect(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint16_t color)
{
	TFT_SetRect(x,y,width,height);
	for(int i=0;i<width*height;i++)
	{
		TFT_SendColor(color);
	}
}
/**@brief  绘制矩形框
  *@param  rect  框的矩形
  *@param  color 框颜色
  *@param  thick 框线宽
  *@retval void
  */
void TFTG_Frame(tftg_rect rect,uint16_t color,int8_t thick)
{
	if(thick<=0)
	{
		return;
	}
	uint8_t d_thick = thick-1;
	uint16_t x = rect.x,y=rect.y,width=rect.width,height=rect.height;
	TFTG_Rect(x-d_thick,y-d_thick,width+d_thick*2+1,d_thick*2+1,color);
	TFTG_Rect(x-d_thick,y+height-d_thick,width+d_thick*2+1,d_thick*2+1,color);
	TFTG_Rect(x-d_thick,y+d_thick,d_thick*2+1,height-d_thick*2,color);
	TFTG_Rect(x-d_thick+width,y+d_thick,d_thick*2+1,height-d_thick*2,color);
}
/**@brief  测试接口
  */
void TFTG_Test(void)
{
	tftg_rect rect = TFTG_CreateRect(10,20,30,40);
	TFTG_Frame(rect,0x6879,1);
	tftg_pic pic;
	pic.color_back  = 0xFFFF;
	pic.color_front = 0x0000;
	pic.rect = TFTG_CreateRect(90,90,8,16);
	pic.pic = asc2_1608[18];
	TFTG_Font1608(pic);
	
	QY_Printf("这里是qy_Graph.h的测试 \r\n");
}












