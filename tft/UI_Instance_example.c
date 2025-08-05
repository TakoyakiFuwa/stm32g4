#include "UI_Instance_example.h"
#include "TFT_font.h"		//提供tft_font结构体和(数字/字符/图片)显示函数
#include "TFT_UI.h"			//提供UI显示的具体架构
#include "TFT_ILI9341.h"
#include "qy_printf.h"

//来自TFT_UI.h
extern tft_pointer 	UI_CURSOR;				//光标
extern tft_font 	FONT[100];				//字体
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

//当前屏幕大小
#define SCREEN_WIDTH 	320
#define SCREEN_HEIGHT 	240

//颜色修改面板的page/ui缓冲区
tft_page* page_color_changed 	= &PAGE[0];
tft_ui*	  ui_color_changed 		= &UI[0];

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
/*关于UI 想在中间插入很难做 应该把这些做成宏定义或者变量
 *	0	光标
 *	1	颜色选择框			//不会被光标选中	50*320  左侧示例图片(框)
 *	2	颜色框-示例图片/数字	//32*32	坐标相对与颜色选择框 +9，+9 边框+4,+4,42,42,3 上/下按照页面ui的index开始调整 右进入前景色
 *	3	颜色框-示例文本		//不会被光标选中 120*24 当前要更改的内容
 *	4	颜色框-前景色		//front 60*24 下键选中固定颜色0 左键选中示例图片/数字 右键选中前景色 
 *	5	颜色框-背景色		//back_ 60*24 下键选中固定颜色0	左键选中前景色 右键选中字体并换成字体选择UI
 *	6	颜色框-字体			//font_ 60*24 下键选中字体选择	左键选中背景色并换成颜色框UI
 *	7	颜色框 	-固定颜色0	//16*16 上键返回前景色/背景色 左键UI_index-1 右键UI_index+1 下键确认
 *	8		 	-固定颜色1	//颜色值保存在value中
 *	9		 	-固定颜色2
 *	10		 	-固定颜色3
 *	11		 	-固定颜色4
 *	12		 	-固定颜色5	
 *	13		 	-固定颜色6	
 *	14		 	-固定颜色7	
 *	15		 	-不定的颜色	//下键跳出RGB565输入框
 *	16			-旧颜色 	
 *	17	颜色框-字体选择		//24*120左右更换字体 下键确认 上键返回并不更改
 *	18	颜色(rgb565)0x	0	
 *	19					1
 *	20					2
 *	21					3
 *	22					4
 *	23					5
 *	24					6
 *	25					7
 *	26	颜色确认					T 重新刷新当前页，并跳出颜色栏
 *	27	颜色取消					F 重新刷新当前页，并跳出颜色栏
 *	28	退出颜色配置
 *
 *	......
 *
 *			//打算用一个时钟做页面测试
 *	80	时
 *	81	分
 *	82	秒
 *	83	毫秒
 *	84	时/分：
 *	85	分/秒：
 *	86	秒/毫秒
 *		
 *
 */
/*关于页面 
 *	0 颜色选择页
 *
 *
 *
 */

/*  工具函数  */
void INS_StringCpy(char* target,const char* _string)
{
	int i=0;
	for(;_string[i]!='\0';i++)
	{
		target[i] = _string[i];
	}
	target[i] = '\0';
}



/* 初始化相关 */
	/*  渲染部分  */
//颜色选择框->示例图片
void Render_UI2(tft_ui* u)
{
	TFTF_Single_01Pic(u->x+9,u->y+9,FONT[3]);
	TFTF_DrawFrame(u->x+4,u->y+4,42,42,0,2);
}
//颜色选择框->当前UI文本
void Render_TEXT(tft_ui* u)
{//UI->3/4/5/6
	TFTF_ShowString(UI[1].x+u->x,UI[1].y+u->y,u->value_text,*(u->font),u->parameter);
}
void Render_FixedColor(tft_ui* u)
{
	TFTF_DrawRect(UI[1].x+u->x+4,UI[1].y+u->y+4,16,16,u->parameter);
	TFTF_DrawFrame(UI[1].x+u->x+2,UI[1].y+u->y+2,20,20,u->font->bk_color,2);
}
void Render_UnsureColor(tft_ui* u)
{
	TFT_SetRect(UI[1].x+u->x+4,UI[1].y+u->y+4,16,16);
	for(int i=0;i<32;i++)
	{
		for(int j=0;j<8;j++)
		{
			TFT_SendColor(UI[7+j].parameter);
		}
	}
	
	TFTF_DrawFrame(UI[1].x+u->x+2,UI[1].y+u->y+2,20,20,u->font->bk_color,3);
}


//页面渲染
void Render_Page0(void)
{
	//内框
	TFTF_DrawRect(UI[1].x,UI[1].y,320,50,UI[1].font->bk_color);
	TFTF_DrawRect(UI[1].x+50,UI[1].y,1,50,UI[1].font->ft_color);
	TFTF_DrawRect(UI[1].x+50,UI[1].y+25,270,1,UI[1].font->ft_color);
	//外边框
	TFTF_DrawFrame(UI[1].x,UI[1].y,320,50,UI[1].font->ft_color,1);
}
void Render_Page1(void)
{
	
}
	
	/*  按键监听部分  */
tft_ui* tochanged_ui = &UI[2]; //在设置中将要修改的UI
uint8_t color_block_index = 12;
void LEFT_ColorBlock(tft_ui* u)
{//UI[7~16]
	if(color_block_index<=7)
	{
		color_block_index = 17;
	}
	color_block_index--;
	UI_Cursor_ChangeUI(&UI[color_block_index]);
}
void RIGHT_ColorBlock(tft_ui* u)
{//UI[7~16]
	if(color_block_index>=16)
	{
		color_block_index = 6;
	}
	color_block_index++;
	UI_Cursor_ChangeUI(&UI[color_block_index]);
	for(int i=4;i<=6;i++)
	{
		UI_AddRender(&UI[i]);
	}
}
void LEFT_UI28(tft_ui* u)
{
	UI_Cursor_ChangeUI(&UI[6]);
	color_block_index = 6;
}
void DOWN_UI28(tft_ui* u)
{//退出颜色处理
	INS_ExitColorChange();
}
void RIGHT_UI6(tft_ui* u)
{
	UI_Cursor_ChangeUI(&UI[28]);
}


uint8_t color_type_index = 4;
void UP_ColorBlock(tft_ui* u)
{//UI[7~16],向上跳转到FT/BK(menu)
	UI_Cursor_ChangeUI(&UI[color_type_index]);
}
void DOWN_ColorMenu(tft_ui* u)
{//UI[4,5],向下跳转到ColorBlock(menu)
	for(int i=7;i<=16;i++)
	{
		UI[i].font = &FONT[4];
		UI_AddRender(&UI[i]);
	}
	UI[16].font = &FONT[2];
	for(int i=4;i<=6;i++)
	{
		UI[i].font = &FONT[1];
		UI_AddRender(&UI[i]);
	}
	if(color_type_index==4)
	{
		UI[16].parameter = tochanged_ui->font->ft_color;
	}
	else if(color_type_index==5)
	{
		UI[16].parameter = tochanged_ui->font->bk_color;
	}
	UI_Cursor_ChangeUI(&UI[16]);
	UI[color_type_index].font = &FONT[2];
	color_block_index = 16;
}
void LEFT_ColorMenu(tft_ui* u)
{//UI[4,5,6]
	if(color_type_index<=4)
	{
		color_type_index = 7;
	}
	color_type_index--;
	UI_Cursor_ChangeUI(&UI[color_type_index]);
}
void RIGHT_ColorMenu(tft_ui* u)
{//UI[4,5,6]
	if(color_type_index>=6)
	{
		color_type_index = 3;
	}
	color_type_index++;
	UI_Cursor_ChangeUI(&UI[color_type_index]);
}
void DOWN_ColorBlock(tft_ui* u)
{
	for(int i=7;i<=16;i++)
	{
		UI[i].font = &FONT[4];
		UI_AddRender(&UI[i]);
	}
	UI_CURSOR.temp_font = &FONT[2];
	if(color_type_index==4)
	{
		UI[2].font->ft_color = u->parameter;
		ui_color_changed->font->ft_color = u->parameter;
	}
	else if(color_type_index==5)
	{
		UI[2].font->bk_color = u->parameter;
		ui_color_changed->font->bk_color = u->parameter;
	}
	UI_AddRender(&UI[2]);
	UI_AddRender(UI_CURSOR.ptr_ui);
	UI_AddRender(ui_color_changed);
}

//时钟页面测试
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

void UP_ClockNum(tft_ui* u)
{
	u->value_num++;
	UI_AddRender(u);
}
void DOWN_ClockNum(tft_ui* u)
{
	u->value_num--;
	UI_AddRender(u);
}

/*  对外接口  */
//固定初始化
tft_font* INS_Init_UI(void)
{
	//字体配置
	//	0	光标
	FONT[0] = TFTF_CreateFont((const char*)font_ASCII_PIXEL_2412,24,12,TFT_RGB888To565(0xfce38a),TFT_RGB888To565(0x522546));
	//	1	颜色选择框的文本和数字
	FONT[1] = TFTF_CreateFont((const char*)font_ASCII_PIXEL_2412,24,12,TFT_RGB888To565(0x40514e),TFT_RGB888To565(0xa6e3e9));
	//	2	颜色选择框-选中配色
	FONT[2] = TFTF_CreateFont((const char*)font_ASCII_PIXEL_2412,24,12,TFT_RGB888To565(0xfcbad3),TFT_RGB888To565(0xf38181));
	//  3	颜色选择框-示例图片
	FONT[3] = TFTF_CreateFont((const char*)qyf_pic01_cloud_3232,32,32,0,0xFFFF);
	//	4	颜色选择框-颜色外圈的框
	FONT[4] = TFTF_CreateFont((const char*)font_ASCII_PIXEL_3216,32,16,TFT_RGB888To565(0xc9d6df),TFT_RGB888To565(0xa6e3e9));
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
	
	
	//UI配置
	//	0	光标
	UI[0] = UI_CreateUI(0,0,&FONT[0],NULL_UI_Func);
	//	1	颜色选择框			//不会被光标选中	50*320  左侧示例图片(框)
	UI[1] = UI_CreateUI(0,240-50,&FONT[1],NULL_UI_Func);
	//	2	颜色框-示例图片/数字	//32*32	坐标相对与颜色选择框 +9，+9 边框+4,+4,42,42,3 上/下按照页面ui的index开始调整 右进入前景色
	UI[2] = UI_CreateUI(0,240-50,&FONT[3],Render_UI2);
	//	3	颜色框-当前UI备注		//不会被光标选中 120*24 当前要更改的内容
	UI[3] = UI_CreateUI(51,1,&FONT[1],Render_TEXT);
	UI[3].parameter = 10;
	INS_StringCpy(UI[3].value_text,"UI_NOTES");
	//	4	颜色框-前景色		//front 36*24 下键选中固定颜色0 左键选中示例图片/数字 右键选中前景色 
	UI[4] = UI_CreateUI(SCREEN_WIDTH-36*4,1,&FONT[2],Render_TEXT);
	UI[4].parameter = 2;
	INS_StringCpy(UI[4].value_text,"ft");
	//	5	颜色框-背景色		//back_ 36*24 下键选中固定颜色0	左键选中前景色 右键选中字体并换成字体选择UI
	UI[5] = UI_CreateUI(SCREEN_WIDTH-36*3,1,&FONT[1],Render_TEXT);
	UI[5].parameter = 2;
	INS_StringCpy(UI[5].value_text,"bk");
	//	6	颜色框-字体			//font_ 36*24 下键选中字体选择	左键选中背景色并换成颜色框UI
	UI[6] = UI_CreateUI(SCREEN_WIDTH-36*2,1,&FONT[1],Render_TEXT);
	UI[6].parameter = 2;
	INS_StringCpy(UI[6].value_text,"t_");
	for(int i=4;i<=6;i++)
	{
		UI[i].Func_Event_DOWN = DOWN_ColorMenu;
		UI[i].Func_Event_LEFT = LEFT_ColorMenu;
		UI[i].Func_Event_RIGHT = RIGHT_ColorMenu;
	}
	UI[6].Func_Event_DOWN = NULL_UI_Func;
	UI[6].Func_Event_RIGHT = RIGHT_UI6;
	//	28	退出颜色选择
	UI[28] = UI_CreateUI(SCREEN_WIDTH-36,1,&FONT[1],Render_TEXT);
	UI[28].parameter = 2;
	INS_StringCpy(UI[28].value_text,"Q_");
	UI[28].Func_Event_LEFT = LEFT_UI28;
	UI[28].Func_Event_DOWN = DOWN_UI28;
	//	7	颜色框 	-固定颜色0	//外尺寸24*24 颜色16*16 颜色框2 上键返回前景色/背景色 左键UI_index-1 右键UI_index+1 下键确认
	UI[7] = UI_CreateUI(54,26,&FONT[4],Render_FixedColor);
	UI[7].parameter = TFT_RGB888To565(0xff461f);
	//	8		 	-固定颜色1	//颜色值保存在value中
	UI[8] = UI_CreateUI(54+26*1,26,&FONT[4],Render_FixedColor);
	UI[8].parameter = TFT_RGB888To565(0x1685a9);
	//	9		 	-固定颜色2
	UI[9] = UI_CreateUI(54+26*2,26,&FONT[4],Render_FixedColor);
	UI[9].parameter = TFT_RGB888To565(0xffa631);
	//	10		 	-固定颜色3
	UI[10] = UI_CreateUI(54+26*3,26,&FONT[4],Render_FixedColor);
	UI[10].parameter = TFT_RGB888To565(0x00bc12);
	//	11		 	-固定颜色4
	UI[11] = UI_CreateUI(54+26*4,26,&FONT[4],Render_FixedColor);
	UI[11].parameter = TFT_RGB888To565(0xFFFFFF);
	//	12		 	-固定颜色5	
	UI[12] = UI_CreateUI(54+26*5,26,&FONT[4],Render_FixedColor);
	UI[12].parameter = TFT_RGB888To565(0xaa96da);
	//	13		 	-固定颜色6	
	UI[13] = UI_CreateUI(54+26*6,26,&FONT[4],Render_FixedColor);
	UI[13].parameter = TFT_RGB888To565(0xabdd8);
	//	14		 	-固定颜色7	
	UI[14] = UI_CreateUI(54+26*7,26,&FONT[4],Render_FixedColor);
	UI[14].parameter = TFT_RGB888To565(0x323232);
	//	15		 	-不定的颜色	//下键跳出RGB565输入框
	UI[15] = UI_CreateUI(54+26*8,26,&FONT[4],Render_UnsureColor);
	//	16			-旧颜色 	
	UI[16] = UI_CreateUI(54+26*9,26,&FONT[2],Render_FixedColor);
	UI[16].parameter = UI[1].font->ft_color;
	for(int i=7;i<=16;i++)
	{
		UI[i].Func_Event_LEFT = LEFT_ColorBlock;
		UI[i].Func_Event_RIGHT = RIGHT_ColorBlock;
		UI[i].Func_Event_UP = UP_ColorBlock;
		UI[i].Func_Event_DOWN = DOWN_ColorBlock;
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
	
	
	return &FONT[0];
}
tft_page* INS_Init_Page(void)
{
	//页面配置
	//	0 颜色选择页
	uint16_t indexs_ofPage0[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,28};
	UI_CreatePage(&PAGE[0],indexs_ofPage0,sizeof(indexs_ofPage0)/sizeof(uint16_t),&UI[4],Render_Page0);
	//	1 时钟页
	uint16_t indexs_ofPage1[] = {80,81,82,83,84,85,86};
	UI_CreatePage(&PAGE[1],indexs_ofPage1,sizeof(indexs_ofPage1)/sizeof(uint16_t),&UI[80],Render_Page1);
	
	//PAGE[0]颜色选择页
//	return &PAGE[0];
	//PAGE[1]时钟页
	return &PAGE[1];
}



void INS_EnterColorChange(void)
{//打开颜色选择面板
	page_color_changed = UI_CURSOR.ptr_page;
	ui_color_changed = UI_CURSOR.ptr_ui;
	UI[2].font->bk_color = ui_color_changed->font->bk_color;
	UI[2].font->ft_color = ui_color_changed->font->ft_color;
	UI_ChangePage(&PAGE[0]);
	for(int i=0;i<100;i++)
	{
		if(page_color_changed->ui_index[i]==999)
		{
			break;
		}
		UI[page_color_changed->ui_index[i]].is_present = 1;
		
	}
}
void INS_ExitColorChange(void)
{
	UI_ChangePage(page_color_changed);
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
				UI_AddRender(&UI[80]);
			}
			UI_AddRender(&UI[81]);
		}
		UI_AddRender(&UI[82]);
	}
	UI_AddRender(&UI[83]);
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





