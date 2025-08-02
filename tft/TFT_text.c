#include "TFT_text.h"
#include "TFT_font.h"

/*	打算在原有的TFT_font（对单个字符/图片进行取模显示）库上
 *	进行进一步写来显示 字符串/数字/窗口 的库
 *				——2025/8/2
 */

/*  字库  */
#include "qy_ascii_font.h"
/*
 *		结构体定义 测试稳定后应当移动到.h文件中
 */
typedef struct tftt_font{
	const char* font_lib;	//字体库/图片库
	uint16_t 	size_height;		//字体库的高度
	uint16_t 	size_width;			//字体库的宽度
	uint16_t 	ft_color;	//前景色 (front-color字体色)
	uint16_t	bk_color;	//背景色 (back-color背景色)
}tftt_font;
typedef enum tftt_obj_type{
	tftt_obj_type_text,
	tftt_obj_type_num,
	tftt_obj_type_pic
}tftt_obj_type;

//文本框/数字/图片
typedef struct tftt_obj{
	tftt_font font;
	void* value;		//文本/数字
	uint16_t x;			//位置（相对所在UI）
	uint16_t y;
}tftt_obj;
//UI（由obj组成）
typedef struct tftt_UI{
	uint16_t 	id_ui;			//后续可通过ID直接访问改内容
	uint16_t 	x;			//位置 (左上角)
	uint16_t	y;			
	uint16_t 	width;		//尺寸
	uint16_t 	height;
	void (*Func_Render)(struct tftt_UI* self);						//具体的渲染方式
	void (*Func_ChangeValue)(struct tftt_UI* self,void* param2);	//更改某个值时的操作
	void (*Func_Event_UP)(struct tftt_UI self,void* param);
	tftt_obj*	objs;			//绑定一个创建时的数组
}tftt_UI;
//具体页面
typedef struct tftt_page{
	uint16_t id_page;
	tftt_UI*	UIs;			//绑定一个创建时的数组
}tftt_page;






/*  通过结构体显示  */
//罢了...罢了...以后不用数字字体了...
//void TFTT_SingleNum(uint16_t x,uint16_t y,uint8_t num,tftt_font font)
//{
//	//num*font.size_height*font.size_width/8
//	//在数字库里数字就是下标num
//	//font.size_height*font.size_width 一个数字在数组中占的数量
//	//再除去1byte的8位
//	TFTF_Pic01(x,y,(const char*)&font.font_lib[num*font.size_height*font.size_width/8],font.size_height,font.size_width,font.ft_color,font.bk_color);
//}
void TFTT_SingleASCII(uint16_t x,uint16_t y,char ascii,tftt_font font)
{
	//字库和标准ascii相差32 即一个空格' '的偏移量
	TFTF_Pic01(x,y,(const char*)&font.font_lib[(ascii-' ')*font.size_height*font.size_width/8],font.size_height,font.size_height/2,font.ft_color,font.bk_color);
}
void TFTT_SinglePIC(uint16_t x,uint16_t y,tftt_font pic)
{
	TFTF_Pic01(x,y,(const char*)&pic.font_lib[pic.size_height*pic.size_height/8],pic.size_height,pic.size_height/2,pic.ft_color,pic.bk_color);
}
/*@brief  显示一个数字
 *@param  -
 *@param  digits  显示的位数，超过位数会吞掉低位
 */
void TFTT_Num(uint16_t x,uint16_t y,uint32_t num,tftt_font font,int8_t digits)
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
		if(++digits>=overflow_digits)
		{
			TFTT_SingleNum(x+font.size_width*(i++),y,10,font);
			return;
		}
		TFTT_SingleNum(x+font.size_width*(i++),y,num/num_length,font);
		//减去最高位
		num -= (num - (num%num_length));
		
	}
}

//
void TFTT_Test(void)
{
	tftt_font num_font;
	num_font.bk_color = 0x0;
	num_font.ft_color = 0xFFFF;
	num_font.font_lib = (const char*)font_NUM_PIXEL_3216;
	num_font.size_height = 32;
	num_font.size_width = 16;
	TFTT_Num(10,10,123456,num_font,4);
	
}


















