#include "TFT_UI.h"
#include "TFT_font.h"		//提供字体结构体，不可删
#include "UI_Instance_example.h"	//实例化接口，不可删
#include "TFT_ILI9341.h"	//提供COLOR		可删
#include "qy_printf.h"		//提供QY_Printf	可删

/*  用到的字库  */
extern const char font_ASCII_PIXEL_3216[][64];

/*  全局变量  */
//关于渲染队列，不用于接口，不需要使用
tft_ui* UI_RENDER_QUEUE[200];	//UI渲染队列 200算是渲染缓冲区大小，
								//更改时应该同时更改UI_Init"创建渲染队列"中for循环的数量
uint16_t ui_render_index;		//渲染队列队尾(可添加渲染的位置)
tft_ui	 null_ui_render_queue;	//用于表示UI渲染的队列尾，变相实现空指针

//光标，项目中用到的全部字体/UI/页面
tft_pointer UI_CURSOR;				//光标
tft_font 	FONT[100];				//字体
tft_ui 		UI[100];				//UI
tft_page 	PAGE[20];				//页面

/**@brief  用于充当函数指针的空指针
  *@add    避免程序调用的函数是空的导致异常
  *		   NULL
  *		   如果ui对象的交互接口不绑定特定的函数
  *		   会异常卡死(程序跑飞)
  *		   所以在创建函数中除了一定绑定的渲染函数
  *		   会把这个函数塞到UI对象的接口上
  */
void NULL_UI_Func(struct tft_ui* none)
{
	QY_Printf("\r\n 警告！当前UI对象没有绑定该操作函数 \r\n");
}
/**@brief  UI初始化，需要放在循环前(常规初始化位置就可以)
  *@param  void
  *@retval void
  *@add	   包括:UI创建接口/Page创建接口/光标创建/渲染队列创建
  */
void Init_UI(void)
{
	//UI创建接口
	UI_CURSOR.cursor_font = INS_Init_UI();
	//页面创建接口
	UI_CURSOR.ptr_page = INS_Init_Page();
	//创建光标
	UI_CURSOR.ptr_ui = UI_CURSOR.ptr_page->start_ui;
	UI_CURSOR.index_ofUI = 0;	//感觉不太对劲，这个在测试完后记得删
	UI_CURSOR.temp_font = UI_CURSOR.ptr_page->start_ui->font;
	//创建渲染队列
	null_ui_render_queue = UI_CreateUI(0,0,&FONT[0],NULL_UI_Func);
	null_ui_render_queue.is_present = 100;	//作为渲染队列尾的标志位
	for(int i=0;i<200;i++)
	{
		UI_RENDER_QUEUE[i] = &null_ui_render_queue;
	}
	//第一次渲染
	UI_ChangePage(UI_CURSOR.ptr_page);
}
/**@brief  具体的渲染函数，用于向屏幕输出
  *@param  void
  *@retval void
  *@add    放在循环中或者定时器定时调用，tft_ui.Func_Render_N只在这里和Page_Change中出现
  */
void CircleRender_UI(void)
{
	for(int i=0;i<100;i++)
	{
		if(UI_RENDER_QUEUE[i]->is_present==100)//遍历到队列尾，退出
		{
			break;
		}
		UI_RENDER_QUEUE[i]->Func_Render_N(UI_RENDER_QUEUE[i]);
		UI_RENDER_QUEUE[i]->readyto_present = 0;	//本次渲染结束，下次可继续添加到渲染队列
		
		UI_RENDER_QUEUE[i] = &null_ui_render_queue;
	}
	ui_render_index = 0;
}
/**@brief  将一个UI添加到渲染队列中
  *@param  u	要渲染的UI
  *@retval void
  *@add	   这个架构里面不直接使用UI的Render方法，而是用这个配合队列渲染
  */
void UI_AddRender(tft_ui* u)
{
	if(u->is_present==1&&u->readyto_present!=1)
	{
		UI_RENDER_QUEUE[ui_render_index] = u;	//将该UI添加到渲染队列
		u->readyto_present = 1;					//UI的准备渲染标志位换成1
												//再次对该UI调用"添加到队列"函数时无效，可避免重复渲染

		ui_render_index++;						//队尾向后移动
	}
}
/**@brief  创建一个UI
  *@param  x,y 	该UI的坐标
  *@param  font 该UI的字体，应绑定全局变量
  *@param  Func_Render	渲染函数
  *@retval tft_ui	应该用全局变量来接收
  */
tft_ui UI_CreateUI(uint16_t x,uint16_t y,tft_font* font,void (*Func_Render)(struct tft_ui* self))
{
	tft_ui u;
	u.x = x;
	u.y = y;
	u.parameter = 0;
	u.font = font;
	u.readyto_present = 0;
	u.is_present = 0;
	u.Func_Render_N = Func_Render;
	//绑定空操作函数
	u.Func_Event_UP 	= NULL_UI_Func;
	u.Func_Event_DOWN 	= NULL_UI_Func;
	u.Func_Event_LEFT 	= NULL_UI_Func;
	u.Func_Event_RIGHT 	= NULL_UI_Func;
	u.Func_Event_F		= NULL_UI_Func;
	u.Func_Event_Q		= NULL_UI_Func;
	
	return u;
}
/**@brief  改变光标Cursor绑定的UI
  *@param  new_ui 要绑定的新UI
  *@retval void
  *@add	   这里的光标Cursor提示方式是用背景色来作为提示
*		   也可以通过修改其他属性来实现光标提示的功能
  */
void UI_Cursor_ChangeUI(tft_ui* new_ui)
{
	//把当前UI颜色改回来
	UI_CURSOR.ptr_ui->font = UI_CURSOR.temp_font;
	UI_Cursor_Refresh();
	//记录新指向的UI
	UI_CURSOR.temp_font = new_ui->font;
	//更改新指向的UI颜色
	UI_CURSOR.cursor_font->font_lib = new_ui->font->font_lib;
	UI_CURSOR.cursor_font->size_height = new_ui->font->size_height;
	UI_CURSOR.cursor_font->size_width = new_ui->font->size_width;
	new_ui->font = UI_CURSOR.cursor_font;
	//光标指向新的UI
	UI_CURSOR.ptr_ui = new_ui;
	UI_Cursor_Refresh();
}
/**@brief  重新渲染一下UI指向的内容，应该在每个UI绑定的按键交互中用
  *@param  void
  *@param  void
  *@add	   按键交互后，可能会改变UI自身的属性，需要重新渲染刷新，
  *		   可以在按键交互函数末尾加一行这个
  */
void UI_Cursor_Refresh(void)
{
	UI_AddRender(UI_CURSOR.ptr_ui);
}
/**@brief  创捷页面
  *@add	   关于页面创建和UI_CraeteUI函数格式不同，
  *		   tft_page中含有数组ui_index[100]，不可以用 数组=数组
  *		   而UI_CreateUI函数中并没有对tft_ui的数组(notes[20]/value_text[20]进行操作)
  *		   所以UI_CreateUI可以用返回值的写法，而UI_CreatePage不可以
  *@add	   总之出问题首先考虑结构体中的数组问题
  */
void UI_CreatePage(tft_page* page,uint16_t* ui_index,uint16_t number_ofUI,tft_ui* start_ui,void (*Page_Render_N)(void))
{
	uint16_t i=0;
	for(;i<number_ofUI;i++)
	{
		page->ui_index[i] = ui_index[i];
	}
	for(;i<100;i++)
	{
		page->ui_index[i] = 999;
	}
	page->start_ui = start_ui;
	page->Page_Render_N = Page_Render_N;
}
/**@brief  更改当前显示的页面
  *@param  new_page 要更改绑定的位置
  *@retval void
  */
void UI_ChangePage(tft_page* new_page)
{
	//上一个页面的ui.ispresent=0,readyto_present=0
	tft_page* p = UI_CURSOR.ptr_page;
	for(int i=0;p->ui_index[i]!=999;i++)
	{
		if(i>=100)
		{//异常
			QY_Printf("页面加载异常! \r\n");
			break;
		}
		UI[p->ui_index[i]].is_present = 0;
		UI[p->ui_index[i]].readyto_present = 0;
	}
	//清理循环渲染队列
	for(int i=0;i<200;i++)
	{
		if(UI_RENDER_QUEUE[i]->is_present==100)
		{//遍历到队列尾，退出
			break;
		}
		UI_RENDER_QUEUE[i] =  &null_ui_render_queue;
		ui_render_index = 0;
	}
	//固定内容渲染
	new_page->Page_Render_N();
	//渲染本页面的ui
	for(int i=0;i<100;i++)
	{
		if(new_page->ui_index[i]==999)
		{
			break;
		}
		UI[new_page->ui_index[i]].Func_Render_N(&UI[new_page->ui_index[i]]);
	}
	//将本页面ui.ispresent=1,readyto_present=0
	for(int i=0;i<100;i++)
	{
		if(new_page->ui_index[i]==999)
		{
			break;
		}
		UI[new_page->ui_index[i]].is_present = 1;
		UI[new_page->ui_index[i]].readyto_present = 0;
	}
	//绑定指针
	UI_Cursor_ChangeUI(new_page->start_ui);
	UI_CURSOR.ptr_page = new_page;
}










//下面进行测试
void Clock_Num_Render(tft_ui* u)
{
	TFTF_ShowNum(u->x,u->y,u->value_num,*u->font,2);
}
void Clock_Num_Add1(tft_ui* u)
{
	u->value_num += 1;
	UI_Cursor_Refresh();
}
void Clock_Num_Mul1(tft_ui* u)
{
	u->value_num -= 1;
	UI_Cursor_Refresh();
}
void Clock_Left(tft_ui* u)
{
	if(UI_CURSOR.index_ofUI==0)
	{
		UI_CURSOR.index_ofUI = 3;
	}
	UI_CURSOR.index_ofUI--;
	UI_Cursor_ChangeUI(&UI[UI_CURSOR.index_ofUI]);
}
void Clock_Right(tft_ui* u)
{
	if(++UI_CURSOR.index_ofUI>=3)
	{
		UI_CURSOR.index_ofUI = 0;
	}
	UI_Cursor_ChangeUI(&UI[UI_CURSOR.index_ofUI]);
}
uint8_t timer;
void Clock_Test(void)
{
	if(++UI[2].value_num>=100)
	{
		UI[2].value_num = 0;
		if(++UI[1].value_num>=60)
		{
			UI[1].value_num = 0;
			++UI[0].value_num;
			UI_AddRender(&UI[0]);
		}
		UI_AddRender(&UI[1]);
	}
	UI_AddRender(&UI[2]);
}

void Init_Interface(void)
{
	null_ui_render_queue = UI_CreateUI(0,0,&FONT[0],NULL_UI_Func);
	null_ui_render_queue.is_present = 100;	//作为渲染队列尾的标志位
	for(int i=0;i<200;i++)
	{
		UI_RENDER_QUEUE[i] = &null_ui_render_queue;
	}
	//创建秒表
	FONT[0] = TFTF_CreateFont((const char*)font_ASCII_PIXEL_3216 ,32,16,0,COLOR_GREEN);
	FONT[1] = TFTF_CreateFont((const char*)font_ASCII_PIXEL_3216 ,32,16,0,COLOR_GREEN);
	FONT[2] = TFTF_CreateFont((const char*)font_ASCII_PIXEL_3216 ,32,16,0,COLOR_GREEN);
	FONT[3] = TFTF_CreateFont((const char*)font_ASCII_PIXEL_3216 ,32,16,0,COLOR_GREEN);
	
	UI[0] = UI_CreateUI(10,45,&FONT[0],Clock_Num_Render);
	UI[0].value_num = 0;
	UI[1] = UI_CreateUI(10+48,45,&FONT[1],Clock_Num_Render);
	UI[1].value_num = 0;
	UI[2] = UI_CreateUI(10+96,45,&FONT[2],Clock_Num_Render);
	UI[2].value_num = 0;
	for(int i=0;i<3;i++)
	{
		UI[i].Func_Event_UP = Clock_Num_Add1;
		UI[i].Func_Event_DOWN = Clock_Num_Mul1;
		UI[i].Func_Event_LEFT = Clock_Left;
		UI[i].Func_Event_RIGHT = Clock_Right;
	}
	//创建光标
	FONT[4] = TFTF_CreateFont((const char*)font_ASCII_PIXEL_3216,32,16,COLOR_RED,COLOR_YELLOW);
	UI_CURSOR.cursor_font = &FONT[4];
	UI_CURSOR.ptr_ui = &UI[0];
	UI_CURSOR.temp_font = UI[0].font;
	UI_CURSOR.index_ofUI = 0;
}
//测试时把这个当作main函数
void TFTU_Test(void)
{
	//UI初始化
//	Init_UI();
	//创建接口
	Init_Interface();
	//添加到显示（之后做成页面）
		//页面初始化/切换时，先清理循环渲染队列
		//然后在初始化/切换函数中直接渲染到屏幕
		//即，将上一个页面的ui.ispresent=0,readyto_present=0 -> 清理循环渲染队列 -> 渲染本页面的ui ->将本页面ui.ispresent = 1,readyto_present=0
	TFTF_Single_Char(10+32,45,':',FONT[3]);
	TFTF_Single_Char(10+80,45,':',FONT[3]);
	for(int i=0;i<3;i++)
	{
		UI[i].Func_Render_N(&UI[i]);
		UI[i].is_present = 1;
	}
	UI_Cursor_ChangeUI(&UI[0]);
	UI_Cursor_Refresh();
	while(1)
	{
		Clock_Test();
		HAL_Delay(10);
		CircleRender_UI();
	}	
}





