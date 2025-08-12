#include "UI_Ins_TestPage.h"
//#include "UI_Index.h"
#include "TFT_UI.h"
#include "qy_printf.h"

/*
 *
 *
 */

//页面组成
extern tft_pointer 	UI_CURSOR;				//光标
extern tft_font 	FONT[300];				//字体
extern tft_ui 		UI[200];				//UI
extern tft_page 	PAGE[20];				//页面

//字库
extern const char font_ASCII_PIXEL_2412[][36];
extern const char font_ASCII_PIXEL_3216[][64];
//图片库
extern const char qyf_pic01_setting_2424[];
extern const char qyf_pic01_start_2424[];
extern const char qyf_pic01_stop_2424[];
extern const char qyf_pic01_github_6464[];					//github图标		黑白	64*64

//渲染
void Render_TP_Background(tft_ui* u)
{
	UI_CURSOR.parameter = InUI_TP_TR_Start;
	TFTF_DrawRect(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,0);
}
void Render_Clock_Num(tft_ui* u)
{
	//渲染前进行状态检查
	u->Func_StateRule(u);
	//渲染
	TFTF_ShowNum(u->x,u->y,u->value_num,*u->font,u->parameter);
}
void Render_Text(tft_ui* u)
{
	TFTF_ShowString(u->x,u->y,u->value_text,*u->font,u->parameter);
}
void Render_Pic(tft_ui* u)
{
	TFTF_Single_01Pic(u->x,u->y,*u->font);
}
void Render_TextWithLine(tft_ui* u)
{
	TFTF_ShowString(u->x,u->y,u->value_text,*u->font,u->parameter);
	TFTF_DrawRect(u->x,u->y+u->font->size_height+1,u->parameter*u->font->size_width,1,u->font->bk_color);
}
void Render_TP_Timer_F(tft_ui* u)//秒表外边框
{
	TFTF_DrawFrame(u->x,u->y,11*16+20,32+24+3+20,u->font->bk_color,5);
}
void Render_TP_TR_TextWithPic(tft_ui* u)
{
	Render_TextWithLine(u);
	u->d_font[2]->bk_color = u->font->bk_color;
	u->d_font[2]->ft_color = u->font->ft_color;
	TFTF_Single_01Pic(u->x+u->parameter*u->font->size_width,u->y,*u->d_font[2]);
	TFTF_DrawRect(u->x+u->parameter*u->font->size_width,u->y+25,24,1,u->d_font[2]->bk_color);
}
void Render_TP_ClockSign(tft_ui* u)
{
	TFTF_Single_Char(u->x,u->y,':',*u->font);
	TFTF_Single_Char(u->x+48,u->y,':',*u->font);
	TFTF_Single_Char(u->x+96,u->y,'-',*u->font);
}
//状态更改函数
void StateRule_Clock(tft_ui* u)
{
	if(u->font==UI_CURSOR.cursor_font)
	{
		return;
	}
	switch(u->value_num/15)
	{
	case 0:u->font = u->d_font[0];break;
	case 1:u->font = u->d_font[1];break;
	case 2:u->font = u->d_font[2];break;
	case 3:u->font = u->d_font[3];break;
	default:u->font = u->d_font[0];break;
	}
}
void StateRule_Clock_MS(tft_ui* u)
{
	if(u->font==UI_CURSOR.cursor_font)
	{
		return;
	}
	switch(u->value_num/25)
	{
	case 0:u->font = u->d_font[0];break;
	case 1:u->font = u->d_font[1];break;
	case 2:u->font = u->d_font[2];break;
	case 3:u->font = u->d_font[3];break;
	default:u->font = u->d_font[0];break;
	}
}

//交互函数
void UP_TP_ClockNum(tft_ui* u)
{
	u->value_num++;
	UI_AddRender(u);
}
void DOWN_TP_ClockNum(tft_ui* u)
{
	if(u->value_num==0)
	{
		return;
	}
	u->value_num--;
	UI_AddRender(u);
}
void LR_TP_toStart(tft_ui* u)
{
	UI_Cursor_ChangeUI(&UI[InUI_TP_TR_Start]);
	UI_CURSOR.parameter = InUI_TP_TR_Start;
}
void StateRule_TP_TR_Start(tft_ui* u)
{
	if(u->font!=UI_CURSOR.cursor_font)
	{
		if(u->value_num==0)
		{
			u->font = u->d_font[0];
		}
		else
		{
			u->font = u->d_font[1];
		}
	}
	u->d_font[2]->bk_color = u->font->bk_color;
	u->d_font[2]->ft_color = u->font->ft_color;
	if(u->value_num==0)
	{
		Other_StringCpy(u->value_text,"Start");
		u->d_font[2]->font_lib = (const char*)qyf_pic01_start_2424;
	}
	else
	{
		Other_StringCpy(u->value_text,"Stop_");
		u->d_font[2]->font_lib = (const char*)qyf_pic01_stop_2424;
	}
}
void DOWN_TP_TR_Start(tft_ui* u)
{
	if(u->value_num==0)
	{
		u->value_num = 1;
	}
	else
	{
		u->value_num = 0;
	}
	u->Func_StateRule(u);
	UI_AddRender(u);
}
void DOWN_TP_TR_Reset(tft_ui* u)
{
	for(int i=InUI_TP_Hour;i<=InUI_TP_MS;i++)
	{
		UI[i].value_num = 0 ;
		UI[i].Func_StateRule(&UI[i]);
		UI_AddRender(&UI[i]);
	}
}
void DOWN_TP_TR_Set(tft_ui* u)
{
	UI_Cursor_ChangeUI(&UI[InUI_TP_Hour]);
	UI_CURSOR.parameter = InUI_TP_Hour;
}
void RIGHT_TP_TR_Set(tft_ui* u)
{
	UI_Cursor_ChangeUI(&UI[InUI_TP_ICON]);
}	
void LEFT_TP_ICON(tft_ui* u)
{
	UI_Cursor_ChangeUI(&UI[InUI_TP_TR_Set]);
	UI_CURSOR.parameter = InUI_TP_TR_Set;
}
void RIGHT_TP_ICON(tft_ui* u)
{
	UI_Cursor_ChangeUI(&UI[InUI_TP_SetClock]);
}
void LEFT_TP_SetClock(tft_ui* u)
{
	UI_Cursor_ChangeUI(&UI[InUI_TP_ICON]);
}
void RIGHT_TP_SetClock(tft_ui* u)
{
	UI_Cursor_ChangeUI(&UI[InUI_TP_C_Botton]);
}
void DOWN_TP_SetClock(tft_ui* u)
{
	UI_Cursor_ChangeUI(&UI[InUI_TP_ClockH]);
	UI_CURSOR.parameter = InUI_TP_ClockH;
}
void LR_TP_toSetClock(tft_ui* u)
{
	UI_Cursor_ChangeUI(&UI[InUI_TP_SetClock]);
}
void LEFT_TP_C_Botton(tft_ui* u)
{
	UI_Cursor_ChangeUI(&UI[InUI_TP_SetClock]);
}
void RIGHT_TP_C_Botton(tft_ui* u)
{
	UI_Cursor_ChangeUI(&UI[InUI_TP_Num]);
}
void StateRule_TP_C_Botton(tft_ui* u)
{
	if(u->value_num==0)
	{
		Other_StringCpy(u->value_text,"START");
		UI[InUI_TP_C_Icon].font->font_lib = (const char*)qyf_pic01_start_2424;
	}
	else
	{
		Other_StringCpy(u->value_text,"STOP_");
		UI[InUI_TP_C_Icon].font->font_lib = (const char*)qyf_pic01_stop_2424;
	}
	UI_AddRender(&UI[InUI_TP_C_Icon]);
}
void LEFT_TP_Num(tft_ui* u)
{
	UI_Cursor_ChangeUI(&UI[InUI_TP_C_Botton]);
}


//独立初始化
/* 秒表 */
void Init_TP_Timer(void)
{
	uint16_t timer_x = 0,timer_y = 0;
	uint16_t index_font = InFT_Timer_Start-1;
	//时分秒
	for(int i=0;i<4;i++)
	{
		FONT[++index_font] = TFTF_CreateFont((const char*)font_ASCII_PIXEL_3216,32,16,TFTF_RGB888To565(0),TFTF_RGB888To565(0xFFFFFF));
		UI[InUI_TP_Hour+i] = UI_CreateUI(timer_x+10+i*48,timer_y+10,&FONT[index_font],Render_Clock_Num);
			//属性初始化
		UI[InUI_TP_Hour+i].value_num = 0;
		UI[InUI_TP_Hour+i].parameter = 2;
		UI[InUI_TP_Hour+i].index_start = InUI_TP_Hour;
		UI[InUI_TP_Hour+i].index_end = InUI_TP_t_StMS;
			//交互绑定
		UI[InUI_TP_Hour+i].Func_Event_DOWN = DOWN_TP_ClockNum;
		UI[InUI_TP_Hour+i].Func_Event_UP = UP_TP_ClockNum;
		UI[InUI_TP_Hour+i].Func_Event_LEFT = LEFT_CursorMove;
		UI[InUI_TP_Hour+i].Func_Event_RIGHT = RIGHT_CursorMove;
		UI[InUI_TP_Hour+i].Func_StateRule = StateRule_Clock;
			//字体绑定
		UI[InUI_TP_Hour+i].d_font[0] = &FONT[index_font];
		FONT[++index_font] = TFTF_CreateFont((const char*)font_ASCII_PIXEL_3216,32,16,TFTF_RGB888To565(0x6a2c70),TFTF_RGB888To565(0xfcbad3));
		UI[InUI_TP_Hour+i].d_font[1] = &FONT[index_font];
		FONT[++index_font] = TFTF_CreateFont((const char*)font_ASCII_PIXEL_3216,32,16,TFTF_RGB888To565(0x3fc1c9),TFTF_RGB888To565(0xf08a5d));
		UI[InUI_TP_Hour+i].d_font[2] = &FONT[index_font];
		FONT[++index_font] = TFTF_CreateFont((const char*)font_ASCII_PIXEL_3216,32,16,TFTF_RGB888To565(0x30e3ca),TFTF_RGB888To565(0xb83b5e));
		UI[InUI_TP_Hour+i].d_font[3] = &FONT[index_font];
	}
		//特殊处理
	UI[InUI_TP_Hour].Func_Event_LEFT = LR_TP_toStart;
	UI[InUI_TP_MS].Func_Event_RIGHT =  LR_TP_toStart;	
	UI[InUI_TP_MS].Func_StateRule = StateRule_Clock_MS; //ms每隔25更换字体(状态)
	//时分秒中间的标点
	for(int i=0;i<3;i++)
	{
		FONT[++index_font] = TFTF_CreateFont((const char*)font_ASCII_PIXEL_3216,32,16,TFTF_RGB888To565(0),TFTF_RGB888To565(0xFFFFFF));
		UI[InUI_TP_t_HtM+i] = UI_CreateUI(timer_x+10+32+48*i,timer_y+10,&FONT[index_font],Render_Text);
			//属性初始化
		UI[InUI_TP_t_HtM+i].value_text[0] = ':';
		UI[InUI_TP_t_HtM+i].value_text[1] = '\0';
		UI[InUI_TP_t_HtM+i].parameter = 1;
		UI[InUI_TP_t_HtM+i].index_start = InUI_TP_Hour;
		UI[InUI_TP_t_HtM+i].index_end = InUI_TP_t_StMS;
	}
		//特殊处理
	UI[InUI_TP_t_StMS].value_text[0] = '-';	//秒和毫秒之间用-
	//外边框
	FONT[++index_font] = TFTF_CreateFont((const char*)font_ASCII_PIXEL_2412,0,0,TFTF_RGB888To565(0xfc5c9c),TFTF_RGB888To565(0x95e1d3));
	UI[InUI_TP_Timer_F] = UI_CreateUI(timer_x,timer_y,&FONT[index_font],Render_TP_Timer_F);
	UI[InUI_TP_Timer_F].index_start = InUI_TP_Timer_F;
	UI[InUI_TP_Timer_F].index_end = InUI_TP_TR_Reset;
	//start
	FONT[++index_font] = TFTF_CreateFont((const char*)font_ASCII_PIXEL_2412,24,12,TFTF_RGB888To565(0xFFFFFF),TFTF_RGB888To565(0x2eFF72));
	UI[InUI_TP_TR_Start] = UI_CreateUI(timer_x+10,timer_y+45,&FONT[index_font],Render_TP_TR_TextWithPic);
		//属性初始化
	UI[InUI_TP_TR_Start].value_num = 1;		//用value_num来表示 1启动 / 0暂停
	UI[InUI_TP_TR_Start].parameter = 5;
	UI[InUI_TP_TR_Start].index_start = InUI_TP_Timer_F;
	UI[InUI_TP_TR_Start].index_end = InUI_TP_TR_Reset;
	Other_StringCpy(UI[InUI_TP_TR_Start].value_text,"start");
		//字体(状态)绑定
	FONT[++index_font] = TFTF_CreateFont((const char*)font_ASCII_PIXEL_2412,24,12,TFTF_RGB888To565(0xFFFFFF),TFTF_RGB888To565(0xf73859));
	UI[InUI_TP_TR_Start].d_font[1] = &FONT[index_font];
	FONT[++index_font] = TFTF_CreateFont((const char*)qyf_pic01_stop_2424,24,24,UI[InUI_TP_TR_Start].d_font[1]->ft_color,UI[InUI_TP_TR_Start].d_font[1]->bk_color);
	UI[InUI_TP_TR_Start].d_font[2] = &FONT[index_font];			//用于绑定图标
		//交互绑定
	UI[InUI_TP_TR_Start].Func_StateRule = StateRule_TP_TR_Start;
	UI[InUI_TP_TR_Start].Func_Event_DOWN = DOWN_TP_TR_Start;
	//reset
	FONT[++index_font] = TFTF_CreateFont((const char*)font_ASCII_PIXEL_2412,24,12,TFTF_RGB888To565(0xeaf6f6),TFTF_RGB888To565(0x66bfbf));
	UI[InUI_TP_TR_Reset] = UI_CreateUI(timer_x+98,timer_y+45,&FONT[index_font],Render_TextWithLine);	//5+12*5+24+8=97
		//属性初始化
	UI[InUI_TP_TR_Reset].parameter = 5;
	Other_StringCpy(UI[InUI_TP_TR_Reset].value_text,"Reset");
	UI[InUI_TP_TR_Reset].index_start = InUI_TP_Timer_F;
	UI[InUI_TP_TR_Reset].index_end = InUI_TP_TR_Reset;
		//交互绑定
	UI[InUI_TP_TR_Reset].Func_Event_DOWN = DOWN_TP_TR_Reset;
	//set
	FONT[++index_font] = TFTF_CreateFont((const char*)qyf_pic01_setting_2424,24,24,TFTF_RGB888To565(0xeaf6f6),TFTF_RGB888To565(0x66bfbf));
	UI[InUI_TP_TR_Set] = UI_CreateUI(timer_x+163,timer_y+45,&FONT[index_font],Render_Pic);
		//交互绑定
	for(int i=InUI_TP_TR_Start;i<=InUI_TP_TR_Set;i++)
	{
		UI[i].Func_Event_LEFT = LEFT_CursorMove;
		UI[i].Func_Event_RIGHT = RIGHT_CursorMove;
	}
	UI[InUI_TP_TR_Start].Func_Event_LEFT = NULL_UI_Func;
	UI[InUI_TP_TR_Set].Func_Event_RIGHT = RIGHT_TP_TR_Set;
	UI[InUI_TP_TR_Set].Func_Event_DOWN = DOWN_TP_TR_Set;
	
	QY_Printf("\r\n Init_TP_Timer 创建完成，字体占用%d~%d \r\n",InFT_Timer_Start,index_font);
}
/*  时钟  */
void Init_TP_Clock(void)
{
	uint16_t clock_x = 0,clock_y = 92;
	uint16_t index_font = InFT_Clock_Start-1;
	//外边框
	FONT[++index_font] = TFTF_CreateFont((const char*)font_ASCII_PIXEL_2412,0,0,TFTF_RGB888To565(0xfc5c9c),TFTF_RGB888To565(0xfc5c9c));
	UI[InUI_TP_Clock_F] = UI_CreateUI(clock_x,clock_y,&FONT[index_font],Render_TP_Timer_F);
	//时分秒
	for(int i=0;i<4;i++)
	{
		FONT[++index_font] = TFTF_CreateFont((const char*)font_ASCII_PIXEL_3216,32,16,0,0xFFFF);
		UI[InUI_TP_ClockH+i] = UI_CreateUI(clock_x+10+48*i,clock_y+10,&FONT[index_font],Render_Clock_Num);
		UI[InUI_TP_ClockH+i].parameter = 2;
		UI[InUI_TP_ClockH+i].value_num = 0;
	}
	UI[InUI_TP_ClockH].value_num = 10;
	UI[InUI_TP_ClockMin].value_num = 41;
		//属性初始化
	for(int i=InUI_TP_ClockH;i<=InUI_TP_ClockSign;i++)
	{
		UI[i].index_start = InUI_TP_ClockH;
		UI[i].index_end = InUI_TP_ClockSign;
	}
	//交互
	for(int i=0;i<4;i++)
	{
		UI[InUI_TP_ClockH+i].Func_Event_DOWN = DOWN_TP_ClockNum;
		UI[InUI_TP_ClockH+i].Func_Event_UP = UP_TP_ClockNum;
		UI[InUI_TP_ClockH+i].Func_Event_LEFT = LEFT_CursorMove;
		UI[InUI_TP_ClockH+i].Func_Event_RIGHT = RIGHT_CursorMove;
	}
	UI[InUI_TP_ClockH].Func_Event_LEFT = LR_TP_toSetClock;
	UI[InUI_TP_ClockMS].Func_Event_RIGHT = LR_TP_toSetClock;
	//中间的标点
	FONT[++index_font] = TFTF_CreateFont((const char*)font_ASCII_PIXEL_3216,32,16,0,0xFFFF);
	UI[InUI_TP_ClockSign] = UI_CreateUI(clock_x+42,clock_y+10,&FONT[index_font],Render_TP_ClockSign);
	//下面的调试入口
	FONT[++index_font] = TFTF_CreateFont((const char*)font_ASCII_PIXEL_2412,24,12,TFTF_RGB888To565(0x756c83),TFTF_RGB888To565(0xb9e1dc));
	UI[InUI_TP_SetClock] = UI_CreateUI(clock_x+10,clock_y+45,&FONT[index_font],Render_TP_TR_TextWithPic);
	Other_StringCpy(UI[InUI_TP_SetClock].value_text,"Set_Clock.");
	UI[InUI_TP_SetClock].parameter = 10;
	FONT[++index_font] = TFTF_CreateFont((const char*)qyf_pic01_setting_2424,24,24,TFTF_RGB888To565(0x756c83),TFTF_RGB888To565(0xb9e1dc));
	UI[InUI_TP_SetClock].d_font[2] = &FONT[index_font];
		//交互
	UI[InUI_TP_SetClock].Func_Event_LEFT = LEFT_TP_SetClock;
	UI[InUI_TP_SetClock].Func_Event_RIGHT = RIGHT_TP_SetClock;
	UI[InUI_TP_SetClock].Func_Event_DOWN = DOWN_TP_SetClock;
	
	QY_Printf("\r\n Init_TP_Timer 创建完成，字体占用%d~%d \r\n",InFT_Clock_Start,index_font);
}
/*  github图标  */
void Init_TP_Icon(void)
{
	uint16_t index_font = InFT_TP_Icon_Start-1;
	FONT[++index_font] = TFTF_CreateFont((const char*)qyf_pic01_github_6464,64,64,TFTF_RGB888To565(0xb9e1dc),TFTF_RGB888To565(0xf38181));
	UI[InUI_TP_ICON] = UI_CreateUI(201,0,&FONT[index_font],Render_Pic);
	UI[InUI_TP_ICON].Func_Event_LEFT = LEFT_TP_ICON;
	UI[InUI_TP_ICON].Func_Event_RIGHT = RIGHT_TP_ICON;
	
	QY_Printf("\r\n Init_TP_Icon 创建完成，字体占用%d~%d \r\n",InFT_TP_Icon_Start,index_font);
}
/*  一直刷新的数字  */
void Init_TP_Num(void)
{
	FONT[InFT_TP_Num] = TFTF_CreateFont((const char*)font_ASCII_PIXEL_3216,32,16,TFTF_RGB888To565(0xfeffe4),TFTF_RGB888To565(0x2eb872));
	UI[InUI_TP_Num] = UI_CreateUI(10,180,&FONT[InFT_TP_Num],Render_Clock_Num);
	UI[InUI_TP_Num].parameter = 6;
	UI[InUI_TP_Num].value_num = 0;
		//交互
	UI[InUI_TP_Num].Func_Event_DOWN = DOWN_TP_ClockNum;
	UI[InUI_TP_Num].Func_Event_UP = UP_TP_ClockNum;
	UI[InUI_TP_Num].Func_Event_LEFT = LEFT_TP_Num;
}
/*  一直刷新的字符  */
void Init_TP_Char(void)
{
	uint16_t index_font = InFT_TP_Char_Start-1;
	uint16_t x_char = 200,y_char=100;
	//字符本身
	FONT[++index_font] = TFTF_CreateFont((const char*)font_ASCII_PIXEL_3216,32,16,TFTF_RGB888To565(0xf8ecfd),TFTF_RGB888To565(0x7a08fa));
	UI[InUI_TP_Char] = UI_CreateUI(x_char,y_char,&FONT[index_font],Render_Text);
	UI[InUI_TP_Char].parameter = 1;
	UI[InUI_TP_Char].value_text[0] = ';';
	UI[InUI_TP_Char].value_text[1] = '\0';
	//暂停/开始图标
	FONT[++index_font] = TFTF_CreateFont((const char*)qyf_pic01_stop_2424,24,24,TFTF_RGB888To565(0xc7f5fe),TFTF_RGB888To565(0x071a52));
	UI[InUI_TP_C_Icon] = UI_CreateUI(x_char+20,y_char+8,&FONT[index_font],Render_Pic);
	//按键
	FONT[++index_font] = TFTF_CreateFont((const char*)font_ASCII_PIXEL_2412,24,12,TFTF_RGB888To565(0xc7f5fe),TFTF_RGB888To565(0x071a52));
	UI[InUI_TP_C_Botton] = UI_CreateUI(x_char,y_char+36,&FONT[index_font],Render_Text);
	UI[InUI_TP_C_Botton].parameter = 5;
	UI[InUI_TP_C_Botton].value_num = 1;
	Other_StringCpy(UI[InUI_TP_C_Botton].value_text,"Stop_");
		//属性初始化
	for(int i=InUI_TP_Char;i<=InUI_TP_C_Botton;i++)
	{
		UI[i].index_start = InUI_TP_Char;
		UI[i].index_end = InUI_TP_C_Botton;
	}
		//交互
	UI[InUI_TP_C_Botton].Func_Event_DOWN = DOWN_TP_TR_Start;
	UI[InUI_TP_C_Botton].Func_StateRule = StateRule_TP_C_Botton;
	UI[InUI_TP_C_Botton].Func_Event_RIGHT = RIGHT_TP_C_Botton;
	UI[InUI_TP_C_Botton].Func_Event_LEFT =  LEFT_TP_C_Botton;
	
	QY_Printf("\r\n Init_TP_Char 创建完成，字体占用%d~%d \r\n",InFT_TP_Char_Start,index_font);
}

/*  给外面用的接口  */

void Init_UI_TestPage(void)
{
	//背景板
	UI[InUI_TP_Background] = UI_CreateUI(0,0,&FONT[0],Render_TP_Background);
	//秒表初始化
	Init_TP_Timer();
	//时钟
	Init_TP_Clock();
	//github图标
	Init_TP_Icon();
	//一直刷新的数字
	Init_TP_Num();
	//一直刷新的字符
	Init_TP_Char();
	
}
void Init_Page_TestPage(void)
{
	uint16_t page1_ui_indexs[] = {
		InUI_TP_Background
		,InUI_TP_Hour		
		,InUI_TP_Minute		
		,InUI_TP_Second		
		,InUI_TP_MS			
		,InUI_TP_t_HtM		
		,InUI_TP_t_MtS		
		,InUI_TP_t_StMS		
		,InUI_TP_Timer_F		
		,InUI_TP_TR_Reset	
		,InUI_TP_TR_Start	
		,InUI_TP_TR_Set
		,InUI_TP_ClockH		
		,InUI_TP_ClockMin	
		,InUI_TP_ClockS		
		,InUI_TP_ClockMS		
		,InUI_TP_ClockSign	
		,InUI_TP_Clock_F		
		,InUI_TP_SetClock	
		,InUI_TP_ICON
		,InUI_TP_Num
		,InUI_TP_Char
		,InUI_TP_C_Icon
		,InUI_TP_C_Botton
	};
	UI_CreatePage(&PAGE[InPG_TestPage],page1_ui_indexs,sizeof(page1_ui_indexs)/sizeof(uint16_t),&UI[InUI_TP_TR_Start]);
}
void TestPage_ClockCircle(void)
{
	//秒表数值变化
	if(UI[InUI_TP_TR_Start].value_num!=0)
	{
		UI[InUI_TP_MS].value_num++;
		if(UI[InUI_TP_MS].value_num>=100)
		{
			UI[InUI_TP_MS].value_num = 0;
			if(++UI[InUI_TP_Second].value_num>=60)
			{
				UI[InUI_TP_Second].value_num = 0;
				if(++UI[InUI_TP_Minute].value_num>=60)
				{
					UI[InUI_TP_Minute].value_num = 0;
					++UI[InUI_TP_Hour].value_num;
					UI_AddRender(&UI[InUI_TP_Hour]);
				}
				UI_AddRender(&UI[InUI_TP_Minute]);
			}
			UI_AddRender(&UI[InUI_TP_Second]);
		}
//		if(UI[InUI_TP_MS].value_num%7==0)			//可以限制字符刷新速度
		{
			UI_AddRender(&UI[InUI_TP_MS]);
		}
	}
	//时钟数值变化
	if(++UI[InUI_TP_ClockMS].value_num>=100)
	{
		UI[InUI_TP_ClockMS].value_num = 0;
		if(++UI[InUI_TP_ClockS].value_num>=60)
		{
			UI[InUI_TP_ClockS].value_num = 0;
			if(++UI[InUI_TP_ClockMin].value_num>=60)
			{
				UI[InUI_TP_ClockMin].value_num = 0;
				UI[InUI_TP_ClockH].value_num++;
				UI_AddRender(&UI[InUI_TP_ClockH]);
			}
			UI_AddRender(&UI[InUI_TP_ClockMin]);
		}
		UI_AddRender(&UI[InUI_TP_ClockS]);
	}
//	if(UI[InUI_TP_ClockMS].value_num%7==0)
	{
		UI_AddRender(&UI[InUI_TP_ClockMS]);
	}
	UI[InUI_TP_Num].value_num++;
	//一直刷新的数字
	UI_AddRender(&UI[InUI_TP_Num]);
	//字符变化
	if(UI[InUI_TP_C_Botton].value_num!=0)
	{
		if(++(UI[InUI_TP_Char].value_text[0])>=' '+95)
		{
			UI[InUI_TP_Char].value_text[0] = ' ';
		}
		UI_AddRender(&UI[InUI_TP_Char]);	
	}
	
}
















