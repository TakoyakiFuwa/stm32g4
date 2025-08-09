#include "UI_Ins_TestPage.h"
//#include "UI_Index.h"
#include "TFT_UI.h"

//页面组成
extern tft_pointer 	UI_CURSOR;				//光标
extern tft_font 	FONT[300];				//字体
extern tft_ui 		UI[200];				//UI
extern tft_page 	PAGE[20];				//页面

//字库
extern const char font_ASCII_PIXEL_2412[][36];
extern const char font_ASCII_PIXEL_3216[][64];
//图片库

//渲染
void Render_Page1(void)
{
	UI_CURSOR.parameter = InUI_TP_Hour;
	TFTF_DrawRect(0,SCREEN_HEIGHT-52,SCREEN_WIDTH,52,0x0);
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
void Left_TP_Hour(tft_ui* u)
{
	UI_CURSOR.parameter = InUI_TP_MS;
	UI_Cursor_ChangeUI(&UI[InUI_TP_MS]);
}
void Right_TP_MS(tft_ui* u)
{
	UI_CURSOR.parameter = InUI_TP_Hour;
	UI_Cursor_ChangeUI(&UI[InUI_TP_Hour]);
}


/*  给外面用的接口  */

void Init_UI_TestPage(void)
{
	//字体初始化
	//时间的数字 InFT_TP_Hour0 ~ InFT_TP_MS3
		//0号字体
	uint16_t temp_InFT = InFT_TP_Hour0;
	for(int i=0;i<=12;i+=4)
	{
		FONT[temp_InFT+i] = TFTF_CreateFont((const char*)font_ASCII_PIXEL_3216,32,16,TFTF_RGB888To565(0),TFTF_RGB888To565(0xFFFFFF));
	}
		//1号字体
	temp_InFT = InFT_TP_Hour1;
	for(int i=0;i<=12;i+=4)
	{
		FONT[temp_InFT+i] = TFTF_CreateFont((const char*)font_ASCII_PIXEL_3216,32,16,TFTF_RGB888To565(0xaa96da),TFTF_RGB888To565(0xeaffd0));
	}
		//2号字体
	temp_InFT = InFT_TP_Hour2;
	for(int i=0;i<=12;i+=4)
	{
		FONT[temp_InFT+i] = TFTF_CreateFont((const char*)font_ASCII_PIXEL_3216,32,16,TFTF_RGB888To565(0xb83b5e),TFTF_RGB888To565(0xf9ed69));
	}
		//3号字体
	temp_InFT = InFT_TP_Hour3;
	for(int i=0;i<=12;i+=4)
	{
		FONT[temp_InFT+i] = TFTF_CreateFont((const char*)font_ASCII_PIXEL_3216,32,16,TFTF_RGB888To565(0x6a2c70),TFTF_RGB888To565(0xf08a5d));
	}
	//冒号		InFT_TP_t_HtM ~ InFT_TP_t_StMS
	temp_InFT = InFT_TP_t_HtM;
	for(int i=0;i<3;i++)
	{
		FONT[temp_InFT+i] = TFTF_CreateFont((const char*)font_ASCII_PIXEL_3216,32,16,TFTF_RGB888To565(0),TFTF_RGB888To565(0xFFFFFF));
	}
	
	
	//页面初始化
	//时钟上的数字
	UI[InUI_TP_Hour] = UI_CreateUI(0+48*0,25,&FONT[InFT_TP_Hour0],Render_Clock_Num);
	UI[InUI_TP_Minute] = UI_CreateUI(0+48*1,25,&FONT[InFT_TP_Minute0],Render_Clock_Num);
	UI[InUI_TP_Second] = UI_CreateUI(0+48*2,25,&FONT[InFT_TP_Second0],Render_Clock_Num);
	UI[InUI_TP_MS] = UI_CreateUI(0+48*3,25,&FONT[InFT_TP_MS0],Render_Clock_Num);
		//设定位数
	for(int i=InUI_TP_Hour;i<=InUI_TP_MS;i++)
	{
		UI[i].parameter = 2;
	}
		//设定不同状态的字体
	for(int i=0;i<4;i++)
	{
		UI[InUI_TP_Hour].d_font[i] = &FONT[InFT_TP_Hour0+i];
	}
	for(int i=0;i<4;i++)
	{
		UI[InUI_TP_Minute].d_font[i] = &FONT[InFT_TP_Minute0+i];
	}
	for(int i=0;i<4;i++)
	{
		UI[InUI_TP_Second].d_font[i] = &FONT[InFT_TP_Second0+i];
	}
	for(int i=0;i<4;i++)
	{
		UI[InUI_TP_MS].d_font[i] = &FONT[InFT_TP_MS0+i];
	}
		//绑定函数
	for(int i=InUI_TP_Hour;i<=InUI_TP_MS;i++)
	{
		UI[i].value_num = 0;
		UI[i].Func_Event_LEFT = LEFT_CursorMove;
		UI[i].Func_Event_RIGHT = RIGHT_CursorMove;
		UI[i].Func_Event_DOWN = DOWN_TP_ClockNum;
		UI[i].Func_Event_UP = UP_TP_ClockNum;
		UI[i].Func_StateRule = StateRule_Clock;
	}
	UI[InUI_TP_Hour].Func_Event_LEFT = Left_TP_Hour;
	UI[InUI_TP_MS].Func_Event_RIGHT = Right_TP_MS;
	UI[InUI_TP_MS].Func_StateRule = StateRule_Clock_MS;
	//时钟数字之间的冒号
	for(int i=0;i<3;i++)
	{
		UI[InUI_TP_t_HtM+i] = UI_CreateUI(32+48*i,25,&FONT[InFT_TP_t_HtM+i],Render_Text);
		UI[InUI_TP_t_HtM+i].parameter = 1;
		UI[InUI_TP_t_HtM+i].value_text[0] = ':';
		UI[InUI_TP_t_HtM+i].value_text[1] = '\0';
	}
	for(int i=InUI_TP_Hour;i<=InUI_TP_t_StMS;i++)
	{
		UI[i].index_start = InUI_TP_Hour;
		UI[i].index_end = InUI_TP_t_StMS;
	}
}
void Init_Page_TestPage(void)
{
	uint16_t page1_ui_indexs[] = {
		InUI_TP_Hour	
		,InUI_TP_Minute	
		,InUI_TP_Second	
		,InUI_TP_MS		
		,InUI_TP_t_HtM	
		,InUI_TP_t_MtS	
		,InUI_TP_t_StMS	
	};
	UI_CreatePage(&PAGE[InPG_TestPage],page1_ui_indexs,sizeof(page1_ui_indexs)/sizeof(uint16_t),&UI[InUI_TP_Hour],Render_Page1);
}
void TestPage_ClockCircle(void)
{
	if(++UI[InUI_TP_MS].value_num>=100)
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
	
	UI_AddRender(&UI[InUI_TP_MS]);

}
















