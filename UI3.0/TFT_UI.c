#include "TFT_UI.h"
#include "UI_Instance.h"

/*	只是一个显示的框架，主要功能是便于切换页面和切换UI
 *	但具体的UI(包括文本框/按键/输入框)甚至是相关的显示函数(TFT_ShowString/Num)之类
 *	构想上和初衷上原本是需要自己写.....
 *	因为写好的UI没有灵活性，显示函数也是基于发送像素点实现，效率不好
 * 	但目前UI方面正在写UI_Instance.h/c，可以用一个两个函数快速放置预制UI(可以交互的数字/图片/文本)
 *	在屏幕显示函数上，只提供TFT_font.h/c中的数字显示/字符串显示/画矩形/画框
 *	... 只是说...TFT_UI.h/c只是框架，有灵活性高，但具体内容少
 *	其他部分如UI_Instance.h/c把UI功能写死，可以快速放置具体UI，但灵活性差，功能不完善
 *			——2025/8/16-14:55.秦羽
 */

//用于光标闪烁,需要把它放到一个定时器或者循环里自增
uint32_t	CLK_Num = 0;
	//光标闪烁 建议12000,超过100000的数是关闭光标闪烁
#define		kira_time		120000
//页面函数
tft_ui*		(*Page_In[20])(void);
//全局变量，创建新页面的时候用extern
uint16_t	COLOR[16];
tft_font 	FONT[50];	//	8
tft_obj		OBJ[100];	// 	36
tft_ui		UI[30];		//	28
tft_pointer CURSOR;		//	7			//一共 8*50+36*100+28*30+7 = 400+3600+840+7 = 4847


/*	以下是创建页面的例子

//建议每个页面单独创建.h/c文件，例如INS_TEST.h/c

//UI框架库
#include "TFT_UI.h"
//屏幕显示函数库
#include "TFT_font.h"

//搬运全局变量
extern uint16_t		COLOR[];
extern tft_font 	FONT[];
extern tft_obj		OBJ[];	
extern tft_ui		UI[];	
extern tft_pointer 	CURSOR;	

//Obj下标分配，每个页面的下标不互通
#define 	InOBJ_aaa		0
#define 	InOBJ_bbb		1
#define 	InOBJ_ccc		2
#define 	InOBJ_ddd		3
#define 	InOBJ_eee		4
//UI下标分配
#define 	InUI_aaa		0
#define 	InUI_ccc		1
#define 	InUI_ddd		2

void Example_Render_aaa(tft_obj* o)
{//该obj往屏幕上显示的内容
	//"o->fonts[o->status]" 调用当前状态的字体
	TFTF_ShowString(o->x,o->y,o->value_text,o->fonts[o->status],o->colors[o->status],3);
	//像TFTF_DrawRect/Frame这种参数里是"color" 而不是 "color_index" 的函数,不能直接传 o->colors[0]
	//需要传全局变量"COLOR[]"
	uint8_t color_index = o->colors[o->status];
	TFTF_DrawRect(o->x,o->y,width,height,COLOR[color_index])
}
void Example_LEFT_aaa(tft_ui* u)
{//左键使光标指向InUI_ccc
	//"UI_CursorChange"中有OBJ刷新，不需要用UI_Render()来重新刷新
	UI_CursorChange(&UI[InUI_ccc]);
}
void Example_DOWN_aaa(tft_ui* u)
{
	u->obj->value_num--;
	//对该ui的数值进行操作后，需要手动重新刷新一下该UI的显示
	UI_Render(u->obj);
	//也可以在这个UI的操作函数里面刷新其他OBJ的显示，或者对其他OBJ的数值进行操作
	OBJ[InOBJ_bbb].value_num = 10;
	UI_Render(&OBJ[InOBJ_bbb]);
}

tft_ui* Page_Example(void)
{
	//->创建OBJ
	//"InFT_ASC_PIXEL_3216" 绑定字体时直接绑定字体下标，不用绑定全局变量"FONT[]"
	//"InCor_Black<<4|InCor_White" 白底黑字，默认绑定到obj.colors[0]上，同样"InFT_ASC_PIXEL_3216"也绑定到obj.fonts[0]上
	//"Example_Render_aaa" 	该obj的渲染方式，有写好的一些INS_Render_xxx在UI_INS_Func.h/c中
						//	或者可以自己写来绑定 格式是void XXXX_Render_XXXX(tft_obj* obj)
	OBJ[InOBJ_aaa] = UI_CreateOBJ(10,20,InFT_ASC_PIXEL_3216,InCor_Black<<4|InCor_White,Example_Render_aaa);
		//如果该obj用于显示数字，则需要在下面额外添加初始值,不赋值默认=0
	OBJ[InOBJ_aaa].value_num = 1234;
		//显示字符也需要初始值，这里的obj.value_text容量只有10个char
	Other_StringCpy(OBJ[InOBJ_aaa].value_text,"example");
		//如果需要其他的状态，就绑定其他的字体和颜色，不绑定默认都是UI_CreateOBJ的参数(参考UI_CreateOBJ实现)
	OBJ[InOBJ_aaa].colors[1] = InCor_BigRed<<4|InCor_Blue;
	OBJ[InOBJ_aaa].fonts[1] = InFT_ASC_XXX_3216;
	
		//创建其他OBJ
	OBJ[InOBJ_bbb] = UI_CreateOBJ(...);
	OBJ[InOBJ_ccc] = UI_CreateOBJ(...);
	...
	
	//->创建UI
	//有些OBJ可能需要被光标选中，可以通过绑定UI来添加交互的方法函数
	UI[InUI_aaa] = UI_CreateUI(&OBJ[InOBJ_aaa]);
		//操作函数需要额外绑定，不然默认为NULL_UI_Func
		//在UI_INS_Func.h/c中也有已经写好的部分操作函数，不过可能只用于参考，建议还是自己写相关操作
	UI[InUI_aaa].Func_Event_LEFT = Example_LEFT_aaa;
	UI[InUI_aaa].Func_Event_DOWN = Example_DOWN_aaa;
	
		//绑定其他UI
	UI[InUI_ccc] = UI_CreateUI(&OBJ[InOBJ_ccc]);
	...
	
	//->页面整体刷新
	//UI_RenderOBJs会把除了数组内的其他OBJ的is_present置0，然后刷新数组内OBJ显示
	//刷新顺序与OBJ在数组中的位置有关，例如下面的InOBJ_eee是第一个刷新在屏幕上的，建议用作背景
	//考虑只在页面创建函数的最后使用
	uint8_t index_objs[] = {
		InOBJ_eee
		,InOBJ_aaa
		,InOBJ_bbb
		,InOBJ_ccc
		,InOBJ_ddd
	};
	UI_RenderOBJs(index_objs,sizeof(index_objs)/sizeof(uint8_t));
	
	//进入这个页面后，光标指向的第一个UI
	return &UI[InUI_aaa];
	
	//创建完成后，把该页面函数在TFT_UI.c的IF_UI_PageInit中实现绑定，如下
	//static uint8_t IF_UI_PageInit(void)
	//{
	//		...
	//		Page_In[InPG_Example] = Page_Example;
	//		...
	//		return InPG_Test1;
	//}
}

//在UI_Instance.h/c中有快速创建 数字/字符/图片 的函数 INS_Obj_Num/Text/Pic(创建OBJ) INS_UI_Num/Text/Pic(创建UI)
//不过是今天(2025/8/16)刚写好的....等下整理下....

以上是示例*/


/*  接口  */
/* 十六个颜色初始化 */
static void IF_UI_ColorInit(void)
{
	uint32_t colors[16] = {
		0x000000	
		,0x3b2e7e	//藏蓝
		,0x9d2933	//胭脂红
		,0xd9b611	//秋香色
		,0x40de5a	//草绿
		,0x88ada6	//水色
		,0x725e82	//乌色
		,0xf47983	//桃红
		,0xff8936	//橘黄
		,0x0eb83a	//葱青
		,0x70f3ff	//蔚蓝
		,0xff2121	//大红
		,0xeacd76	//金
		,0x75664d	//黎明
		,0xd6ecf0	//月白
	    ,0xFFFFFF	
	};
	for(int i=0;i<16;i++)
	{
		COLOR[i] = Other_RGB888To565(colors[i]);
	}
}
/* 字体绑定 */
#include "qy_ascii_font.h"
#include "qy_pic.h"
extern const char font_ASCII_PIXEL_2412[][36];
extern const char font_ASCII_PIXEL_3216[][64];
extern const char qyf_pic01_setting_3232[];
extern const char qyf_pic01_github_3232[];
extern const char qyf_pic01_cloud_3232[];
static tft_font UI_CreateFont(const char* font_lib,uint8_t height,uint16_t width);
/**@brief  字体绑定，创建字体绑定到全局变量
  *		   创建UI时可以用下标(InFT_ASC/PIC_xxx)代替
  *@param  void
  *@retval void
  */
static void IF_UI_FontInit(void)
{
	FONT[InFT_ASC_PIXEL_2412]	= UI_CreateFont((const char*)font_ASCII_PIXEL_2412,24,12);
	FONT[InFT_ASC_PIXEL_3216] 	= UI_CreateFont((const char*)font_ASCII_PIXEL_3216,32,16);
	FONT[InFT_PIC_Set_3232] 	= UI_CreateFont((const char*)qyf_pic01_setting_3232,32,32);
	FONT[InFT_PIC_Github_3232] 	= UI_CreateFont((const char*)qyf_pic01_github_3232,32,32);
	FONT[InFT_PIC_Cloud_3232] 	= UI_CreateFont((const char*)qyf_pic01_cloud_3232,32,32);
	
}
/**@brief  页面绑定
  *@param  void
  *@retval uint8_t 程序初始化完成后进入的第一个页面
  */
static uint8_t IF_UI_PageInit(void)
{
	Page_In[InPG_Test_Ins] = Page_TEST;
	
	return InPG_Test_Ins;
}


//以上是接口
//下面的具体函数在"出bug"之前不用调整.....



/*  函数实现  */

//渲染队列
tft_obj*	Render_OBJ[100];			//4847+120*4 = 5327
uint8_t		index_render=0;	
tft_obj		null_obj;					//is_present = 127,作为渲染尾的标志
tft_ui		null_ui;					

/**@biref  空函数，默认绑定到OBJ显示上
  */
void NULL_OBJ_FUNC(tft_obj* obj)
{
	
}
/**@brief  空函数，默认绑定到UI操作上
  */
void NULL_UI_FUNC(tft_ui* obj)
{
	
}
/**@brief  空界面
  */
tft_ui* NULL_PAGE_FUNC(void)
{
	return &null_ui;
}
/**@brief  UI框架初始化，初始化完成直接进入第一个页面
  */
void Init_UI(void)
{
	//颜色初始化
	IF_UI_ColorInit();
	//字体初始化
	IF_UI_FontInit();
	//和NULL相关的初始化
	null_obj = UI_CreateOBJ(0,0,0,0,NULL_OBJ_FUNC);
	null_obj.is_present = 127;
	null_ui = UI_CreateUI(&null_obj);
	for(int i=0;i<100;i++)
	{
		OBJ[i] = null_obj;
	}
	for(int i=0;i<30;i++)
	{
		UI[i] = null_ui;
	}
	//光标初始化
	CURSOR.index_color = 0x8D;
	CURSOR.parameter = 0;
	for(int i=0;i<4;i++)
	{
		CURSOR.temp_colors[i] = null_obj.colors[i];
	}
	CURSOR.ui = &null_ui;
	//创建渲染队列
	for(int i=0;i<100;i++)
	{
		Render_OBJ[i] = &null_obj;
	}
	index_render = 0;
	//进入页面
	tft_ui* start_ui = Page_In[IF_UI_PageInit()]();

	UI_CursorChange(start_ui);
}
/**@brief  放到循环或者定时器定期调用里
  */
void Circle_Render(void)
{
	//光标闪烁
	static uint32_t temp_clk_num;
	if(CLK_Num<temp_clk_num)
	{
		temp_clk_num = CLK_Num;
	}
	if(CLK_Num-temp_clk_num>=kira_time)
	{
		temp_clk_num = CLK_Num;
		tft_obj* o = CURSOR.ui->obj;
		if(o->colors[0]==CURSOR.index_color)
		{
			for(int i=0;i<4;i++)
			{
				o->colors[i] = CURSOR.temp_colors[i];
			}
		}
		else
		{
			for(int i=0;i<4;i++)
			{
				o->colors[i] = CURSOR.index_color;
			}	
		}
		UI_Render(o);
	}
	//正常的渲染队列
	for(int i=0;i<100;i++)
	{
		if(Render_OBJ[i]->is_present==127)
		{
			break;
		}
		Render_OBJ[i]->Func_Render_N(Render_OBJ[i]);
		Render_OBJ[i]->is_present &= 0xFD;
		Render_OBJ[i] = &null_obj;
	}
	index_render = 0;
}
/**@brief  把obj添加到渲染队列中
  *@param  obj 要渲染的tft_obj
  */
void UI_Render(tft_obj* obj)
{
	if(obj->is_present==1)
	{
		Render_OBJ[index_render] = obj;
		obj->is_present |= 0x02;
		index_render++;
	}
}
/**@brief  创建字体
  *@param  font_lib		字体库，也可以是图片库，用(const char*)来强转
  *@param  height/width	该字体中单个字符的宽度/高度，例如font_ASC_Pixel_2412就是height=24,width=12
  *@retval void
  */
static tft_font UI_CreateFont(const char* font_lib,uint8_t height,uint16_t width)
{
	tft_font f;
	f.font_lib = font_lib;
	f.height = height;
	f.width = width;
	return f;
}
/**@brief  创建Object，用于页面显示
  *@param  x,y 			在页面中的位置
  *@param  index_font	字体的下标 InFT_xxx_xxx 一般在TFT_UI.h中定义
  *@param  color		uint8_t类型的都是下标，uint16_t是RGB565
  *						可以是InCor_AAAA<<4|InCor_BBBB(AAA是前景色，BBB是背景色)，也可以用0xAB,0x23这种16进制数随机抽一个配色
  *@param  Render		当前Object的显示方式 
  *@retval void
  */
tft_obj UI_CreateOBJ(uint16_t x,uint16_t y,uint8_t index_font,uint8_t color,void (*Render)(struct tft_obj* u))
{
	tft_obj obj;
	//赋值
	obj.x = x;
	obj.y = y;
	for(int i=0;i<4;i++)
	{
		obj.fonts[i] = index_font;
		obj.colors[i] = color;
	}
	obj.Func_Render_N = Render;
	//初始值
	obj.status = 0;
	obj.is_present = 0;
	obj.value_num = 0;
	obj.value_text[0] = '\0';
	obj.index_signal = 0;
	
	return obj;
}
/**@brief  建议用于页面刷新
  *@param  index_obj  	要在屏幕上显示的OBJ下标组，显示顺序和数组内存储顺序有关
  *@param  num_obj		下标数量，一般是sizeof(index_obj)/sizeof(uint8_t)
  *@add	   建议在创建页面函数(Page_In)的最后加上这个，实现页面刷新
  */
void UI_RenderOBJs(uint8_t* index_obj,uint8_t num_obj)
{
	for(int i=0;i<100;i++)
	{
		if(OBJ[i].is_present!=127)
		{
			OBJ[i].is_present = 0;
		}
	}
	uint8_t index=0;
	for(int i=0;i<num_obj;i++)
	{
		index = index_obj[i];
		OBJ[index].is_present = 1;
		UI_Render(&OBJ[index]);
	}
}
/**@brief  将一个OBJ绑定操作函数
  *@param  obj 要绑定的OBJ，应该用全局变量"OBJ[]"来传
  *@retval tft_ui 创建的UI，需要用全局变量"UI[]"来接收
  *@add	   默认操作函数都是空函数，需要自己额外绑定
  */
tft_ui UI_CreateUI(tft_obj* obj)
{
	tft_ui u;
	u.obj = obj;
	u.Func_Event_DOWN = NULL_UI_FUNC;
	u.Func_Event_LEFT = NULL_UI_FUNC;
	u.Func_Event_UP	  = NULL_UI_FUNC;
	u.Func_Event_RIGHT= NULL_UI_FUNC;
	u.Func_Event_F = NULL_UI_FUNC;
	return u;
}
/**@brief  更换光标指向的UI
  *@param  ui  要指向的UI
  */
void UI_CursorChange(tft_ui* ui)
{
	tft_obj* o = CURSOR.ui->obj;
	//归还原本颜色
	if(o->colors[0]==CURSOR.index_color)
	{
		for(int i=0;i<4;i++)
		{
			o->colors[i] = CURSOR.temp_colors[i];
		}
	}
	//记录新UI颜色
	CURSOR.ui = ui;
	for(int i=0;i<4;i++)
	{
		CURSOR.temp_colors[i] = ui->obj->colors[i];
		ui->obj->colors[i] = CURSOR.index_color;
	}
	//重新刷新
	UI_Render(o);
	UI_Render(ui->obj);
}
/**@brief  更换页面
  *@param  page_index 页面的下标，应该在TFT_UI.h中定义
  *@add    注，Page_In函数中应该有调用UI_RenderOBJs();
  */
void UI_ChangePage(uint8_t page_index)
{
	UI_CursorChange(Page_In[page_index]());
}
/**@brief  从RGB888转换成RGB565
  *@param  RGB888 颜色代码
  *@retval RGB565颜色代码
  */
uint16_t Other_RGB888To565(uint32_t RGB888)
{
	uint16_t RGB565 = 0;
	RGB565 = RGB888>>19;
	RGB565 = RGB565<<6;
	RGB565 |= ((RGB888>>10)&0x3F);
	RGB565 = RGB565<<5;
	RGB565 |= ((RGB888>>3)&0x1F);
	return RGB565;
}
/**@brief  字符串赋值
  *@param  target  要赋值的目标
  *@param  _words  字符串
  */
void Other_StringCpy(char* target,const char* _words)
{
	int i=0;
	for(;_words[i]!='\0';i++)
	{
		target[i] = _words[i];
	}
	target[i] = '\0';
}


