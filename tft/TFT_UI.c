#include "TFT_UI.h"
#include "TFT_font.h"		//提供字体结构体，不可删
#include "qy_printf.h"		//提供QY_Printf	用于串口调试
#include "UI_Instance_example.h"	//"Instance"实例化 提供INS_Init_UI()和INS_Init_Page()
									//example表示只是提供参考例程，真正项目应该创建UI_Instance.h/c(UI实例化)文件

//
/*	在TFT_font中的基础上实现了渲染框架的搭建
 *	参考面向对象的方式，创建了 page 和 UI 作为"class"类
 *	通过调用 UI_Create- 来实现实例化	
 *	
 *	与TFT_font的耦合性其实较低，只用到了tft_font
 *	考虑有机会进一步降耦合
 *			——2025/8/6-9:22.秦羽
 */

extern const char font_ASCII_PIXEL_4020[][100];
/*  全局变量  */
//光标，项目中用到的全部字体/UI/页面
tft_pointer UI_CURSOR;				//光标
tft_font 	FONT[300];				//字体
tft_ui 		UI[100];				//UI
tft_page 	PAGE[20];				//页面

/*  渲染队列  */
//关于渲染队列，不用于接口，不需要使用
//UI渲染队列
tft_ui* QUEUE_RENDER_UI[200];	//渲染UI队列 200算是渲染缓冲区大小，
								//更改时应该同时更改UI_Init"创建渲染队列"中for循环的数量
uint16_t queue_render_ui_index=0;		//渲染队列队尾(可添加渲染的位置)
tft_ui	 	null_ui;	//用于表示UI渲染的队列尾，变相实现空指针,is_present=100 表示空
tft_font	null_font;	//表示未绑定font size_width=size_height = 999;
//渲染函数队列，仅用于页面刷新时将页面刷新函数放进渲染队列中
void (*QUEUE_RENDER_FUNC[100])(void);	//渲染函数队列
uint16_t queue_render_func_index=0;

/*  函数  */
/**@brief  用于充当函数指针的空指针
  *@add    避免程序调用的函数是空的导致异常
  *		   NULL
  */
void NULL_UI_Func(struct tft_ui* none)
{
	QY_Printf("\r\n 警告！当前UI对象没有绑定该操作函数 \r\n");
}
void NULL_VOID_Func(void)
{
	QY_Printf("\r\n 警告！调用空函数指针 \r\n");
}
void NULL_ChangeState(struct tft_ui* u,int8_t state)
{
	if(state<0||state>=4)
	{
//		QY_Printf("\r\n 当前UI进入异常状态 \r\n");
		return;
	}
	if(u->font!=UI_CURSOR.cursor_font)
	{
		u->font = u->d_font[state];
	}
}
/**@brief  UI初始化，需要放在循环前(常规初始化位置就可以)
  *@param  void
  *@retval void
  *@add	   包括:UI创建接口/Page创建接口/光标创建/渲染队列创建
  */
void Init_UI(void)
{
	//创建充当"空指针"的内容
	null_ui = UI_CreateUI(0,0,&FONT[0],NULL_UI_Func);
	null_ui.is_present = 100;	//作为渲染队列尾的标志位
	null_font = TFTF_CreateFont((const char*)font_ASCII_PIXEL_4020,999,999,0,0);
	//UI创建接口
	UI_CURSOR.cursor_font = INS_Init_UI();
	//页面创建接口
	UI_CURSOR.ptr_page = INS_Init_Page();
	//创建光标
	UI_CURSOR.ptr_ui = UI_CURSOR.ptr_page->start_ui;
	UI_CURSOR.temp_font = UI_CURSOR.ptr_page->start_ui->font;
	UI_CURSOR.parameter = 0;
	UI_CURSOR.temp_ui = UI_CURSOR.ptr_ui;
	//创建渲染队列
		//函数渲染
	for(int i=0;i<100;i++)
	{
		QUEUE_RENDER_FUNC[i] = NULL_VOID_Func;
	}
		//UI渲染
	for(int i=0;i<200;i++)
	{
		QUEUE_RENDER_UI[i] = &null_ui;
	}
	//第一次渲染
	UI_ChangePage(UI_CURSOR.ptr_page);
}
/**@brief  具体的渲染函数，用于向屏幕输出
  *@param  void
  *@retval void
  *@add    放在循环中或者定时器定时调用，tft_ui.Func_Render_N只在这里直接调用
  *@add	   如果被UI_ChangePage中断打断有可能出现显示bug
  *		   如果真的有用UI_ChangePage中断打断CircleRender_UI的需求，提醒我再改一下
  */
int bug_Bad_solution = 1;
void CircleRender_UI(void)
{
	bug_Bad_solution = 1;
	//遍历函数渲染队列
	for(int i=0;QUEUE_RENDER_FUNC[i]!=NULL_VOID_Func;i++)
	{
		if(i>=100)
		{
			break;
		}
		QUEUE_RENDER_FUNC[i]();
		QUEUE_RENDER_FUNC[i] = NULL_VOID_Func;
	}
	queue_render_func_index = 0;
	//遍历UI渲染队列
	for(int i=0;i<200;i++)
	{
		if(QUEUE_RENDER_UI[i]->is_present==100 || bug_Bad_solution==0)//遍历到队列尾，退出
		{
			break;
		}
		QUEUE_RENDER_UI[i]->Func_Render_N(QUEUE_RENDER_UI[i]);
		QUEUE_RENDER_UI[i]->readyto_present = 0;	//本次渲染结束，下次可继续添加到渲染队列
		
		QUEUE_RENDER_UI[i] = &null_ui;
	}
	queue_render_ui_index = 0;
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
		QUEUE_RENDER_UI[queue_render_ui_index] = u;	//将该UI添加到渲染队列
		u->readyto_present = 1;					//UI的准备渲染标志位换成1
												//再次对该UI调用"添加到队列"函数时无效，可避免重复渲染

		queue_render_ui_index++;						//队尾向后移动
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
	u.font = font;
	u.readyto_present = 0;
	u.is_present = 0;
	u.Func_Render_N = Func_Render;
	//绑定空操作函数
	u.Func_ChangeState 	= NULL_ChangeState;
	u.Func_StateRule	= NULL_UI_Func;
	u.Func_Event_UP 	= NULL_UI_Func;
	u.Func_Event_DOWN 	= NULL_UI_Func;
	u.Func_Event_LEFT 	= NULL_UI_Func;
	u.Func_Event_RIGHT 	= NULL_UI_Func;
	u.Func_Event_F		= NULL_UI_Func;
	//不一定会用上，后续补充内容
	u.parameter = 0;
	for(int i=0;i<=3;i++)
	{
		u.d_font[i] = font;
	}
	
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
		if(QUEUE_RENDER_UI[i]->is_present==100)
		{//遍历到队列尾，退出
			break;
		}
		QUEUE_RENDER_UI[i] =  &null_ui;
		queue_render_ui_index = 0;
	}
	//固定内容渲染（将页面渲染函数添加到渲染队列）
	QUEUE_RENDER_FUNC[queue_render_func_index++] = new_page->Page_Render_N;
	//将本页面ui.ispresent=1,readyto_present=0
	for(int i=0;i<100;i++)
	{
		if(new_page->ui_index[i]==999)
		{
			break;
		}
		UI[new_page->ui_index[i]].is_present = 1;
		UI[new_page->ui_index[i]].readyto_present = 0;
		UI_AddRender(&UI[new_page->ui_index[i]]);
	}
	//渲染本页面的ui
	for(int i=0;i<100;i++)
	{
		if(new_page->ui_index[i]==999)
		{
			break;
		}
	}
	//绑定指针
	UI_Cursor_ChangeUI(new_page->start_ui);
	UI_CURSOR.ptr_page = new_page;
	bug_Bad_solution = 0;
}













