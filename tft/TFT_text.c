#include "TFT_text.h"
#include "TFT_font.h"

/*	打算在原有的TFT_font（对单个字符/图片进行取模显示）库上
 *	进行进一步写来显示 字符串/数字/窗口 的库
 *				——2025/8/2
 */

/*  字库  */
#include "qy_ascii_font.h"
#include "qy_pic.h"
/*
 *		结构体定义 测试稳定后应当移动到.h文件中
 */

//文本框/数字/图片
typedef struct tftt_obj{
	uint16_t 	id_obj;
	tftt_font 	font;
	char text[20];		//文本/数字
	uint32_t	num;
	uint16_t x_rlt;		//相对位置（relative 相对所在UI）
	uint16_t y_rlt;
}tftt_obj;
//UI（由obj组成）
typedef struct tftt_UI{
	uint16_t 	x;			//位置 (左上角)
	uint16_t	y;			
	uint16_t 	width;		//尺寸
	uint16_t 	height;
	void (*Func_Render)(struct tftt_UI* self);						//具体的渲染方式
	void (*Func_Event_UP)(struct tftt_UI* self,void* param,void* d_param);	//上移
	void (*Func_Event_DOWN)(struct tftt_UI* self,void* param,void* d_param);//下移
	void (*Func_Event_F)(struct tftt_UI* self,void* param,void* d_param);	//确认	(更换颜色)
	void (*Func_Event_Q)(struct tftt_UI* self,void* param,void* d_param);	//退出
	tftt_obj	objs[10];			//绑定一个创建时的数组
	char notes[50];
}tftt_UI;
//具体页面
typedef struct tftt_page{
	uint16_t 	id_page;
	tftt_UI		UIs[20];			//绑定一个创建时的数组
}tftt_page;

void StringCopy(char* result,const char* text)
{
	for(int i=0;text[i]!='\0';i++)
	{
		result[i] = text[i];
	}
}

//创建一个字体
tftt_font TFTT_CreateFont(uint16_t ft_color,uint16_t bk_color,const char* font,uint16_t height,uint16_t width)
{
	tftt_font f;
	f.bk_color = bk_color;
	f.ft_color = ft_color;
	f.font_lib = (const char*)font;
	f.size_height = height;
	f.size_width = width;
	return f;
}

//预制Obj 这个世界充满了预制菜(x)
tftt_obj TFTT_CreateObj(uint16_t x_rlt,uint16_t y_rlt,tftt_font font,uint16_t id,uint32_t num,const char* text)
{
	tftt_obj o;
	o.font = font;
	o.id_obj = id;
	o.num = num;
	StringCopy(o.text,text);
	o.x_rlt = x_rlt;
	o.y_rlt = y_rlt;
	
	return o;
}

void UI_ShowPIC(tftt_UI* ui,tftt_obj obj)
{
	TFTT_ShowPIC(ui->x+obj.x_rlt,ui->y+obj.y_rlt,obj.font);
}
void UI_ShowNum(tftt_UI* ui,tftt_obj obj)
{
	TFTT_ShowNum(ui->x+obj.x_rlt,ui->y+obj.y_rlt,obj.num,obj.font,4);
}
void UI_ShowString(tftt_UI* ui,tftt_obj* obj)
{
	TFTT_ShowString(ui->x+obj->x_rlt,ui->y+obj->y_rlt,obj->text,obj->font,4);
}

//举例->	建一个左侧图标 右侧数字的UI 图标和数字的属性都可变
//		UI的大小是64*128 图标64*64 数字3216的四位数字
void RENDER_GITHUB_ICON(struct tftt_UI* ui)
{
	UI_ShowPIC(ui,ui->objs[0]);
	UI_ShowNum(ui,ui->objs[1]);
	UI_ShowString(ui,&(ui->objs[2]));
//	TFTF_Frame(ui->x,ui->y,ui->width,ui->height,0,3);
}
tftt_UI TFTT_CreateUI_Template(uint16_t x,uint16_t y,uint16_t icon_id,uint16_t num_id)
{
	tftt_UI ui;
	tftt_font font;
	//创建github图标
	font.bk_color = 0x0;
	font.ft_color = 0xFFF;
	font.font_lib = (const char*)qyf_pic01_github_6464;
	font.size_height = 64;
	font.size_width = 64;
	ui.objs[0].font = font;
	ui.objs[0].id_obj = icon_id;
	ui.objs[0].x_rlt = 0;
	ui.objs[0].y_rlt = 0;
	//创建数字
	font.font_lib = (const char*)font_ASCII_PIXEL_3216;
	font.size_height = 32;
	font.size_width = 16;
	ui.objs[1].font = font;
	ui.objs[1].id_obj = num_id;	//考虑把创建做成一个函数
	ui.objs[1].num = 124;
	ui.objs[1].x_rlt = 64;
	ui.objs[1].y_rlt = 0;
	//创建文本
	ui.objs[2] = TFTT_CreateObj(64,32,font,12,0,"GIT_");
	//创建具体UI
	ui.x = x;
	ui.y = y;
	ui.width = 128;
	ui.height = 64;
	
	StringCopy(ui.notes,"用github图标进行测试");
	ui.Func_Render = RENDER_GITHUB_ICON;
	
	return ui;
}




//
void TFTT_Test(void)
{
	
	tftt_UI ui1 = TFTT_CreateUI_Template(10,20,0x64,0x69);
	ui1.Func_Render(&ui1);
	
	tftt_UI ui3 = TFTT_CreateUI_Template(200,120,0x64,0x69);
	ui3.Func_Render(&ui3);
	
}







/*  通过结构体显示  */
void TFTT_SingleChar(uint16_t x,uint16_t y,char _char,tftt_font font)
{
	//字库和标准ascii相差32 即一个空格' '的偏移量
	TFTF_Pic01(x,y,(const char*)&font.font_lib[(_char-' ')*font.size_height*font.size_width/8],font.size_height,font.size_height/2,font.ft_color,font.bk_color);
}
void TFTT_ShowPIC(uint16_t x,uint16_t y,tftt_font pic)
{
	TFTF_Pic01(x,y,(const char*)&pic.font_lib[0],pic.size_height,pic.size_width,pic.ft_color,pic.bk_color);
}
/*@brief  显示一个数字
 *@param  -
 *@param  digits  显示的位数，超过位数会吞掉低位
 */
void TFTT_ShowNum(uint16_t x,uint16_t y,uint32_t num,tftt_font font,int8_t digits)
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
			TFTT_SingleChar(x+font.size_width*(--i),y,'-',font);
			return;
		}
		TFTT_SingleChar(x+font.size_width*(i++),y,num/num_length+'0',font);
		//减去最高位
		num -= (num - (num%num_length));
		
	}
}
/*@brief  字符串
 *@param  -
 *@param  NumOfChar  显示的数量
 */
void TFTT_ShowString(uint16_t x,uint16_t y,const char* text,tftt_font font,int8_t NumOfChar)
{
	int i=0;
	for(;text[i]!='\0';i++)
	{
		if(--NumOfChar<0)
		{
			TFTT_SingleChar(x+font.size_width*(--i),y,'-',font);
			return;
		}
		TFTT_SingleChar(x+font.size_width*(i),y,text[i],font);
	}
	for(;NumOfChar>0;NumOfChar--)
	{
		TFTT_SingleChar(x+font.size_width*(i++),y,' ',font);
	}
}
















