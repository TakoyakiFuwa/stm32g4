#include "UI_Instance_example.h"
#include "TFT_font.h"		//提供tft_font结构体和(数字/字符/图片)显示函数
#include "TFT_UI.h"			//提供UI显示的具体架构
#include "TFT_ILI9341.h"
#include "qy_printf.h"
#include "UI_Ins_ColorBox.h"

/*	这个文件的很多内容相当于"搭积木"
 *	代码或者函数大多和屏幕上某个UI绑定，
 *	不需要后续使用
 *	所以就不写注释了
 *			——2025/8/6-17:50.秦羽
 */

//来自TFT_UI.h
extern tft_pointer 	UI_CURSOR;				//光标
extern tft_font 	FONT[300];				//字体
extern tft_ui 		UI[100];				//UI
extern tft_page 	PAGE[20];				//页面
//字体
extern const char font_ASCII_PIXEL_2412[][36];
extern const char font_ASCII_PIXEL_3216[][64];
extern const char font_ASCII_PIXEL_4020[][100];
//演示图片
extern const char qyf_pic01_cloud_3232[];
extern const char qyf_pic01_setting_3232[];
extern const char qyf_pic01_hot_3232[] ;
extern const char qyf_pic01_github_3232[];
extern const char qyf_pic01_github_6464[];					//github图标		黑白	64*64
extern const char qyf_pic595_bing_6432[4096];				//微软图标		彩色	64*32

//当前接口为按F跳出颜色选择
//	进入颜色选择后，所有内容停止刷新
//	


/*  全局变量  */
	//颜色修改面板的page缓冲区
extern tft_page* color_changed_page 	;
	//当前修改的颜色处于哪种状态
extern int8_t 	  color_changed_state 	;

/*关于font
 *	0	光标
 *	1	颜色选择框的文本和数字
 *	2	颜色选择框-选中配色
 *  3	颜色选择框-示例图片
 *	4	颜色选择框-颜色外圈的框
 *	...
 *
 *			//打算用一个时钟做页面测试
 *	80	时
 *	81	分
 *	82	秒
 *	83	毫秒
 *	84	时/分：
 *	85	分/秒：
 *	86	秒/毫秒
 */
/*关于页面 
 *	0 颜色选择页
 *
 *
 *
 */



/*  复用性较高的函数，更改和使用时应该谨慎  */
void Render_Num(tft_ui* u)
{
	TFTF_ShowNum(u->x,u->y,u->value_num,*u->font,u->parameter);
}


/* 初始化相关 */
	/*页面渲染*/
void Render_Page1(void)
{
	TFTF_DrawRect(UI[InUI_ColorBox].x,UI[InUI_ColorBox].y,320,51,0xad55);
}


/* 时钟页面测试 */
void Render_Clock(tft_ui* u)
{
	TFTF_ShowNum(u->x,u->y,u->value_num,*u->font,u->parameter);
}
void Render_Clock_Sign(tft_ui* u)
{
	TFTF_Single_Char(u->x,u->y,':',*u->font);
}
void RIGHT_ClockNum80(tft_ui* u)
{//80~83
	UI_Cursor_ChangeUI(&UI[81]);
}
void RIGHT_ClockNum81(tft_ui* u)
{//80~83
	UI_Cursor_ChangeUI(&UI[82]);
}
void RIGHT_ClockNum82(tft_ui* u)
{//80~83
	UI_Cursor_ChangeUI(&UI[83]);
}
void RIGHT_ClockNum83(tft_ui* u)
{//80~83
	UI_Cursor_ChangeUI(&UI[80]);
}
void LEFT_ClockNum80(tft_ui* u)
{//80~83
	UI_Cursor_ChangeUI(&UI[83]);
}
void LEFT_ClockNum81(tft_ui* u)
{//80~83
	UI_Cursor_ChangeUI(&UI[80]);
}
void LEFT_ClockNum82(tft_ui* u)
{//80~83
	UI_Cursor_ChangeUI(&UI[81]);
}
void LEFT_ClockNum83(tft_ui* u)
{//80~83
	UI_Cursor_ChangeUI(&UI[82]);
}

/**	能起出"aaa"这样的函数名，
  *	是真的失去了所有的力气和手段了
  */
void aaa(tft_ui* u)
{
	switch(u->value_num/15)
	{
	case 1:u->Func_ChangeState(u,u->value_num/15);break;
	case 2:u->Func_ChangeState(u,u->value_num/15);break;
	case 3:u->Func_ChangeState(u,u->value_num/15);break;
	default:u->Func_ChangeState(u,0);break;
	}
}
void UP_ClockNum(tft_ui* u)
{
	u->value_num++;
	aaa(u);
	UI_AddRender(u);
}
void DOWN_ClockNum(tft_ui* u)
{
	u->value_num--;
	aaa(u);
	UI_AddRender(u);
}
void ChangeState_ClockNum(struct tft_ui* u,int8_t state)
{
	if(state<0||state>=4)
	{
		return;
	}
	if(u->font!=UI_CURSOR.cursor_font)
	{
		u->font = u->d_font[state];
	}
	else
	{
		UI_CURSOR.temp_font = u->d_font[state];
	}
}




/*  对外接口  */
//固定初始化
tft_font* INS_Init_UI(void)
{
	//字体配置
	//	...
	//打算用一个时钟做页面测试
	//	80	时
	//	81	分
	//	82	秒
	//	83	毫秒
	//	84	时/分：
	//	85	分/秒：
	//	86	秒/毫秒
	for(int i=80;i<=86;i++)
	{
		FONT[i] = TFTF_CreateFont((const char*)font_ASCII_PIXEL_3216,32,16,0x0,0xFFFF);
	}
	//...
	//打算用一个时钟做页面测试
	//	80	时
	//	81	分
	//	82	秒
	//	83	毫秒*10
	for(int i=0;i<=3;i++)
	{
		UI[i+80] = UI_CreateUI(10+i*16*3,20,&FONT[i+80],Render_Clock);
		UI[i+80].parameter = 2;
		UI[i+80].value_num = 0;
		UI[i+80].Func_Event_UP = UP_ClockNum;
		UI[i+80].Func_Event_DOWN = DOWN_ClockNum;
		UI[i+80].Func_ChangeState = ChangeState_ClockNum;
	}
	//FONT[90+]
	for(int i=0;i<4;i++)
	{
		FONT[90+i*3] = TFTF_CreateFont((const char*)font_ASCII_PIXEL_3216,32,16,TFT_RGB888To565(0xc91f37),TFT_RGB888To565(0xa98175));
		UI[80+i].d_font[1] = &FONT[90+i*3];
		FONT[91+i*3] = TFTF_CreateFont((const char*)font_ASCII_PIXEL_3216,32,16,TFT_RGB888To565(0xa6e3e9),TFT_RGB888To565(0x252a34));
		UI[80+i].d_font[2] = &FONT[91+i*3];
		FONT[92+i*3] = TFTF_CreateFont((const char*)font_ASCII_PIXEL_3216,32,16,TFT_RGB888To565(0xeaeaea),TFT_RGB888To565(0xff2e63));
		UI[80+i].d_font[3] = &FONT[92+i*3];
	}
	
	UI[80].Func_Event_LEFT = LEFT_ClockNum80;
	UI[80].Func_Event_RIGHT=RIGHT_ClockNum80;
	UI[81].Func_Event_LEFT = LEFT_ClockNum81;
	UI[81].Func_Event_RIGHT=RIGHT_ClockNum81;
	UI[82].Func_Event_LEFT = LEFT_ClockNum82;
	UI[82].Func_Event_RIGHT=RIGHT_ClockNum82;
	UI[83].Func_Event_LEFT = LEFT_ClockNum83;
	UI[83].Func_Event_RIGHT=RIGHT_ClockNum83;
	//	84	时/分：
	//	85	分/秒：
	//	86	秒/毫秒
	for(int i=0;i<3;i++)
	{
		UI[i+84] = UI_CreateUI(42+i*16*3,20,&FONT[i+84],Render_Clock_Sign);
		UI[i+84].parameter = 1;
	}
	for(int i=80;i<=86;i++)
	{
		UI[i].index_start = 80;
		UI[i].index_end	=86;
	}
	
	//颜色选择框UI的初始化
	Init_ColorBox_UI();
	
	return &FONT[0];
}
tft_page* INS_Init_Page(void)
{
	//页面配置
	//	0 颜色选择页
	Init_ColorBox_Page();
	//	1 时钟页
	uint16_t indexs_ofPage1[] = {80,81,82,83,84,85,86};
	UI_CreatePage(&PAGE[1],indexs_ofPage1,sizeof(indexs_ofPage1)/sizeof(uint16_t),&UI[80],Render_Page1);
	
	//PAGE[0]颜色选择页
//	return &PAGE[0];
	//PAGE[1]时钟页
	return &PAGE[1];
}


//以上是对外接口


void Clock_NumAdd(void)
{
	if(++UI[83].value_num>=100)
	{
		UI[83].value_num = 0;
		if(++UI[82].value_num>=60)
		{
			UI[82].value_num = 0;
			if(++UI[81].value_num>=60)
			{
				UI[81].value_num = 0;
				UI[80].value_num++;
				aaa(&UI[80]);
				UI_AddRender(&UI[80]);
			}
			aaa(&UI[81]);
			UI_AddRender(&UI[81]);
		}
		aaa(&UI[82]);
		UI_AddRender(&UI[82]);
	}
	aaa(&UI[83]);
	if(UI[83].value_num%6==0)
	{
		UI_AddRender(&UI[83]);
	}
}
//测试接口
void INS_Test(void)
{
	Init_UI();
	while(1)
	{
		HAL_Delay(10);
		CircleRender_UI();
	
		Clock_NumAdd();
	}
}





