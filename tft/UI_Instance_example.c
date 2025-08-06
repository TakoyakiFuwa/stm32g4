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
/*关于UI 打算做成宏定义
 */
//InUI->Index UI
#define		InUI_Cursor			0
#define 	InUI_ColorBox		1		//不会被光标选中					51*WIDTH 	是更改颜色的界面  
#define 	InUI_CB_Sample		2		//调色框中最左侧的样本图片  		51*51		是颜色示例
#define 	InUI_CB_UIIndex		3		//调色框第一行的UI的下标			36*24		要更改的UI的下标	右键进入前景色 上下键更改当前要修改的UI
#define 	InUI_CB_Type_FT		4		//调色框第一行"FT" front前景色	60*24 		下键选中固定颜色0 左键选中示例图片/数字 右键选中背景色 
#define 	InUI_CB_Type_BK		5		//调色框第一行"BK" back背景色		60*24 		下键选中固定颜色0	左键选中前景色 右键选中字体并换成字体选择UI
#define 	InUI_CB_Type_F_		6		//调色框第一行"F_" font字体		60*24 		下键选中字体选择	左键选中背景色并换成颜色框UI
#define     InUI_CB_Type_Q_		7		//调色框第一行"Q_" 退出			60*24		下键退出并重新刷新当前页 左键返回F_
#define 	InUI_CB_ColorFix0	8		//调色框第二行固定颜色0~7			28*24 		上键返回前景色/背景色 左键UI_index-1 右键UI_index+1 下键确认
#define 	InUI_CB_ColorFix1	9				//颜色值保存在parameter中
#define 	InUI_CB_ColorFix2	10	
#define 	InUI_CB_ColorFix3	11	
#define 	InUI_CB_ColorFix4	12	
#define 	InUI_CB_ColorFix5	13		
#define 	InUI_CB_ColorFix6	14		
#define 	InUI_CB_ColorFix7	15		
#define     InUI_CB_ColorUS		16		//调色框第二行倒数第二个UnSure	16*16		下键进入RGB888输入框
#define     InUI_CB_ColorOld	17		//调色框第二行，保存字体之前的颜色	16*16	
#define		InUI_CB_RGB_0x		18		//调色框第二行的RGB888输入框0x	28*24		仅是'0x'文本，不会被选中
#define		InUI_CB_RGB_5		19		//调色框第二行RGB888的最高位		28*24		左右更换RGB888输入位置 上下更换数值
#define		InUI_CB_RGB_4		20
#define		InUI_CB_RGB_3		21
#define		InUI_CB_RGB_2		22
#define		InUI_CB_RGB_1		23
#define		InUI_CB_RGB_0		24
#define		InUI_CB_RGB_True	25		//RGB888 确认					28*24		下键重新刷新
#define		InUI_CB_RGB_False	26		//RGB888 取消
#define 	InUI_CB_Line2_BK	27		//调色框第二行背景
/*关于UI 想在中间插入很难做 应该把这些做成宏定义或者变量
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
 *	29	RGB888输入		0x	//6位数字
 *	30					0
 *	31					1
 *	32					2
 *	33					3
 *	34					4
 *	35					5
 *	36					T	确定
 *	37					F	取消
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

/*  复用性较高的函数，更改和使用时应该谨慎  */
void Render_Num(tft_ui* u)
{
	TFTF_ShowNum(u->x,u->y,u->value_num,*u->font,u->parameter);
}
void LEFT_CursorMove(tft_ui* u)
{
	UI_Cursor_ChangeUI(&UI[--UI_CURSOR.parameter]);
}
void RIGHT_CursorMove(tft_ui* u)
{
	UI_Cursor_ChangeUI(&UI[++UI_CURSOR.parameter]);
}



/*  调色框  */
	/*  调色框-渲染部分  */
//颜色选择框->示例图片
void Render_CB_Sample(tft_ui* u)
{
	TFTF_Single_01Pic(u->x+9,u->y+9,FONT[3]);
	TFTF_DrawFrame(u->x+4,u->y+4,42,42,0,2);
}
//颜色选择框->当前UI文本
void Render_CB_TEXT(tft_ui* u)
{
	TFTF_ShowString(UI[InUI_ColorBox].x+u->x,UI[InUI_ColorBox].y+u->y,u->value_text,*(u->font),u->parameter);
}
void Render_CB_UIIndex(tft_ui* u)
{
	TFTF_ShowNum(UI[InUI_ColorBox].x+u->x,UI[InUI_ColorBox].y+u->y,u->value_num,*(u->font),u->parameter);
}
void Render_CB_ColorFix(tft_ui* u)
{
	TFTF_DrawRect(UI[InUI_ColorBox].x+u->x+4,UI[InUI_ColorBox].y+u->y+4,16,16,u->parameter);
	TFTF_DrawFrame(UI[InUI_ColorBox].x+u->x+2,UI[InUI_ColorBox].y+u->y+2,20,20,u->font->bk_color,2);
}
void Render_CB_ColorUS(tft_ui* u)
{
	TFT_SetRect(UI[InUI_ColorBox].x+u->x+4,UI[InUI_ColorBox].y+u->y+4,16,16);
	for(int i=0;i<32;i++)
	{
		for(int j=InUI_CB_ColorFix0;j<=InUI_CB_ColorFix7;j++)
		{
			TFT_SendColor(UI[j].parameter);
		}
	}
	
	TFTF_DrawFrame(UI[InUI_ColorBox].x+u->x+2,UI[InUI_ColorBox].y+u->y+2,20,20,u->font->bk_color,3);
}
void Render_CB_RGB(tft_ui* u)
{
	if(u->value_num<10)
	{
		TFTF_ShowNum(UI[InUI_ColorBox].x+u->x,UI[InUI_ColorBox].y+u->y,u->value_num,*u->font,1);
	}
	else
	{
		char _char[2];
		_char[0] = (char)('a'+u->value_num-10);
		_char[1] = '\0';
		TFTF_ShowString(UI[InUI_ColorBox].x+u->x,UI[InUI_ColorBox].y+u->y,(const char*)_char,*u->font,1);
	}
}
void Render_CB_Line2_BK(tft_ui* u)
{
	TFTF_DrawRect(UI[InUI_ColorBox].x+u->x,UI[InUI_ColorBox].y+u->y,SCREEN_WIDTH,24,FONT[1].bk_color);
}
	/*  调色框-交互部分  */
uint8_t index_cb_type;
void DOWN_CB_Type(tft_ui* u)
{
	index_cb_type = UI_CURSOR.parameter;
	switch(UI_CURSOR.parameter)
	{
	case InUI_CB_Type_BK:
	case InUI_CB_Type_FT:
			//第二行固定颜色
		for(int i=InUI_CB_ColorFix0;i<=InUI_CB_ColorOld;i++)
		{
			UI[i].font = &FONT[4];
			UI_AddRender(&UI[i]);
		}
		UI[InUI_CB_ColorOld].font = &FONT[2];
			//重新渲染第一行
		for(int i=InUI_CB_Type_BK;i<=InUI_CB_Type_Q_;i++)
		{
			UI[i].font = &FONT[1];
			UI_AddRender(&UI[i]);
		}
		UI[InUI_CB_ColorOld].parameter = UI_CURSOR.temp_ui->font->ft_color;
		if(UI_CURSOR.parameter==InUI_CB_Type_BK)
		{
			UI[InUI_CB_ColorOld].parameter = UI_CURSOR.temp_ui->font->bk_color;
		}
		UI_AddRender(&UI[InUI_CB_ColorOld]);
		//更改UI绑定
		UI_CURSOR.parameter = InUI_CB_ColorOld;
		UI_CURSOR.temp_font = &FONT[2];
		UI_Cursor_ChangeUI(&UI[InUI_CB_ColorOld]);
		break;
	case InUI_CB_Type_Q_:
		INS_ExitColorChange();
		break;
	default:
		QY_Printf("\r\n UI_Instance_example缺乏绑定 \r\n");
	}
}
void DOWN_CB_Color(tft_ui* u)
{
	for(int i=InUI_CB_ColorFix0;i<=InUI_CB_ColorOld;i++)
	{
		UI[i].font = &FONT[4];
		UI_AddRender(&UI[i]);
	}
	UI_CURSOR.temp_font = &FONT[2];
	if(index_cb_type==InUI_CB_Type_FT)
	{
		UI[InUI_CB_Sample].font->ft_color = u->parameter;
		UI_CURSOR.temp_ui->font->ft_color = u->parameter;
	}
	else if(index_cb_type==InUI_CB_Type_BK)
	{
		UI[InUI_CB_Sample].font->bk_color = u->parameter;
		UI_CURSOR.temp_ui->font->bk_color = u->parameter;
	}
	UI_AddRender(&UI[InUI_CB_Sample]);
	UI_Cursor_ChangeUI(UI_CURSOR.ptr_ui);
	UI_AddRender(UI_CURSOR.temp_ui);
}
void UP_CB_Color(tft_ui* u)
{
	UI_CURSOR.parameter = index_cb_type;
	UI_Cursor_ChangeUI(&UI[index_cb_type]);
	UI_CURSOR.temp_font = &FONT[1];
}
uint16_t cb_rgb_false_color = 0;
void DOWN_CB_ColorUS(tft_ui* u)
{//更换成RGB888输入
	//InUI_CB_ColorUS变为选中状态
	for(int i=InUI_CB_ColorFix0;i<=InUI_CB_ColorOld;i++)
	{
		UI[i].font = &FONT[4];
	}
	UI_CURSOR.temp_font = &FONT[2];
	//记录旧颜色，用于CB_RGB_FALSE
	if(index_cb_type==InUI_CB_Type_FT)
	{
		cb_rgb_false_color = UI[InUI_CB_Sample].font->ft_color;
	}
	else if(index_cb_type==InUI_CB_Type_BK)
	{
		cb_rgb_false_color = UI[InUI_CB_Sample].font->bk_color;
	}
	//把旧颜色放在InUI_CB_RGB0~5中显示
	uint32_t oc = cb_rgb_false_color;
	uint32_t red = (oc&0xF800);//1111 1000 0000 0000
	uint32_t green = (oc&0x7E0);//0000 0111 1110 0000
	uint32_t blue = (oc&0x1F);//0000 0000 0001 1111
	red<<=8;
	green<<=5;
	blue<<=3;
	oc = (red|green|blue);
	for(int i=InUI_CB_RGB_0;i>=InUI_CB_RGB_5;i--)
	{
		UI[i].value_num=oc%16;
		oc/=16;
	}
	//刷新显示面板
		//刷背景
	UI[InUI_CB_Line2_BK].is_present = 1;
	UI[InUI_CB_Line2_BK].readyto_present = 0;
	UI_AddRender(&UI[InUI_CB_Line2_BK]);
	UI[InUI_CB_Line2_BK].is_present = 0;
	for(int i=InUI_CB_RGB_0x;i<=InUI_CB_RGB_False;i++)
	{
		UI[i].is_present = 1;
		UI[i].readyto_present = 0;
		UI_AddRender(&UI[i]);
	}
	//UI指向新位置
	UI[InUI_CB_ColorUS].is_present = 0;
	UI_Cursor_ChangeUI(&UI[InUI_CB_RGB_5]);
	UI_CURSOR.parameter = InUI_CB_RGB_5;
}
static void Other_CB_RGB(void)
{
	//获取RGB888
	uint32_t rgb888 = 0;
	for(int i=InUI_CB_RGB_5;i<=InUI_CB_RGB_0;i++)
	{
		rgb888 *= 16;
		rgb888 += UI[i].value_num;
	}
		//改成RGB565
	rgb888 = TFT_RGB888To565(rgb888);
	//更改示例UI颜色
	if(index_cb_type==InUI_CB_Type_FT)
	{
		UI[InUI_CB_Sample].font->ft_color = rgb888;
		UI_CURSOR.temp_ui->font->ft_color = rgb888;
	}
	else if(index_cb_type==InUI_CB_Type_BK)
	{
		UI[InUI_CB_Sample].font->bk_color = rgb888;
		UI_CURSOR.temp_ui->font->bk_color = rgb888;
	}
	//重新刷新UI
	UI_AddRender(&UI[InUI_CB_Sample]);
	UI_AddRender(UI_CURSOR.temp_ui);
}
void DOWN_CB_RGB(tft_ui* u)
{
	//更改本身数据
	if(u->value_num==0)
	{
		u->value_num = 15;
	}
	else
	{
		u->value_num--;
	}
	UI_Cursor_Refresh();
	//颜色更改
	Other_CB_RGB();
}
void UP_CB_RGB(tft_ui* u)
{
	//更改本身数据
	if(u->value_num==0xF)
	{
		u->value_num = 0;
	}
	else
	{			
		u->value_num++;
	}
	UI_Cursor_Refresh();
	//颜色更改
	Other_CB_RGB();
}
static void Other_CB_RGB_TrueOrFalse(void)
{
	//调色框第二行换回固定颜色栏
	UI[InUI_CB_Line2_BK].is_present =1;
	UI[InUI_CB_Line2_BK].readyto_present =0;
	UI_AddRender(&UI[InUI_CB_Line2_BK]);
	UI[InUI_CB_Line2_BK].is_present =0;
	for(int i=InUI_CB_ColorFix0;i<=InUI_CB_ColorOld;i++)
	{
		UI_AddRender(&UI[i]);
	}
	UI_CURSOR.parameter = InUI_CB_ColorUS;
	UI[InUI_CB_ColorUS].is_present = 1;
	UI[InUI_CB_ColorUS].readyto_present = 0;
	UI_Cursor_ChangeUI(&UI[InUI_CB_ColorUS]);
}

void DOWN_CB_RGB_True(tft_ui* u)
{
	u->is_present = 0;
	Other_CB_RGB_TrueOrFalse();
}
void DOWN_CB_RGB_False(tft_ui* u)
{
	u->is_present = 0;
	if(index_cb_type==InUI_CB_Type_FT)
	{
		UI[InUI_CB_Sample].font->ft_color = cb_rgb_false_color;
		UI_CURSOR.temp_ui->font->ft_color = cb_rgb_false_color;
	}
	else if(index_cb_type==InUI_CB_Type_BK)
	{
		UI[InUI_CB_Sample].font->bk_color = cb_rgb_false_color;
		UI_CURSOR.temp_ui->font->bk_color = cb_rgb_false_color;
	}
	UI_AddRender(&UI[InUI_CB_Sample]);
	UI_AddRender(UI_CURSOR.temp_ui);
	Other_CB_RGB_TrueOrFalse();
}


/* 初始化相关 */
	/*页面渲染*/
void Render_Page0(void)
{
	//内框
	TFTF_DrawRect(UI[InUI_ColorBox].x,UI[InUI_ColorBox].y,320,51,UI[InUI_ColorBox].font->bk_color);
	TFTF_DrawRect(UI[InUI_ColorBox].x+51,UI[InUI_ColorBox].y,1,51,UI[InUI_ColorBox].font->ft_color);
	TFTF_DrawRect(UI[InUI_ColorBox].x+51,UI[InUI_ColorBox].y+25,270,1,UI[InUI_ColorBox].font->ft_color);
	//外边框
	TFTF_DrawFrame(UI[InUI_ColorBox].x,UI[InUI_ColorBox].y,320,51,UI[InUI_ColorBox].font->ft_color,1);
}
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
	//		光标
	UI[InUI_Cursor] = UI_CreateUI(0,0,&FONT[0],NULL_UI_Func);
	//		颜色选择框			//不会被光标选中	51*320  左侧示例图片(框)
	UI[InUI_ColorBox] = UI_CreateUI(0,240-51,&FONT[1],NULL_UI_Func);
	//		颜色框-示例图片/数字	//不会被光标选中 32*32	坐标相对与颜色选择框 +9，+9 边框+4,+4,42,42,3 
	UI[InUI_CB_Sample] = UI_CreateUI(0,240-51,&FONT[3],Render_CB_Sample);
	//		颜色框-当前UI备注		//36*24 要更改的UI的下标	右键进入前景色 上下键更改当前要修改的UI
	UI[InUI_CB_UIIndex] = UI_CreateUI(53,1,&FONT[1],Render_CB_UIIndex);
	UI[InUI_CB_UIIndex].parameter = 3;
	UI[InUI_CB_UIIndex].value_num = 0;
	//		颜色框-前景色		//front 36*24 下键选中固定颜色0 左键选中示例图片/数字 右键选中前景色 
	UI[InUI_CB_Type_FT] = UI_CreateUI(SCREEN_WIDTH-36*4,1,&FONT[1],Render_CB_TEXT);
	UI[InUI_CB_Type_FT].parameter = 2;
	INS_StringCpy(UI[InUI_CB_Type_FT].value_text,"ft");
	//		颜色框-背景色		//back_ 36*24 下键选中固定颜色0	左键选中前景色 右键选中字体并换成字体选择UI
	UI[InUI_CB_Type_BK] = UI_CreateUI(SCREEN_WIDTH-36*3,1,&FONT[1],Render_CB_TEXT);
	UI[InUI_CB_Type_BK].parameter = 2;
	INS_StringCpy(UI[InUI_CB_Type_BK].value_text,"bk");
	//		颜色框-字体			//font_ 36*24 下键选中字体选择	左键选中背景色并换成颜色框UI
	UI[InUI_CB_Type_F_] = UI_CreateUI(SCREEN_WIDTH-36*2,1,&FONT[1],Render_CB_TEXT);
	UI[InUI_CB_Type_F_].parameter = 2;
	INS_StringCpy(UI[InUI_CB_Type_F_].value_text,"t_");
	//		退出颜色选择
	UI[InUI_CB_Type_Q_] = UI_CreateUI(SCREEN_WIDTH-36,1,&FONT[1],Render_CB_TEXT);
	UI[InUI_CB_Type_Q_].parameter = 2;
	INS_StringCpy(UI[InUI_CB_Type_Q_].value_text,"Q_");
	for(int i=InUI_CB_UIIndex;i<=InUI_CB_Type_Q_;i++)
	{
		UI[i].Func_Event_DOWN = DOWN_CB_Type;
		UI[i].Func_Event_LEFT = LEFT_CursorMove;
		UI[i].Func_Event_RIGHT = RIGHT_CursorMove;
	}
	UI[InUI_CB_UIIndex].Func_Event_LEFT = NULL_UI_Func;
	UI[InUI_CB_Type_Q_].Func_Event_RIGHT = NULL_UI_Func;
	//		颜色框 	-固定颜色0	//外尺寸24*24 颜色16*16 颜色框2 上键返回前景色/背景色 左键UI_index-1 右键UI_index+1 下键确认
	UI[InUI_CB_ColorFix0] = UI_CreateUI(54,26,&FONT[4],Render_CB_ColorFix);
	UI[InUI_CB_ColorFix0].parameter = TFT_RGB888To565(0xff461f);
	//			 	-固定颜色1	//颜色值保存在value中
	UI[InUI_CB_ColorFix1] = UI_CreateUI(54+26*1,26,&FONT[4],Render_CB_ColorFix);
	UI[InUI_CB_ColorFix1].parameter = TFT_RGB888To565(0x1685a9);
	//			 	-固定颜色2
	UI[InUI_CB_ColorFix2] = UI_CreateUI(54+26*2,26,&FONT[4],Render_CB_ColorFix);
	UI[InUI_CB_ColorFix2].parameter = TFT_RGB888To565(0xffa631);
	//			 	-固定颜色3
	UI[InUI_CB_ColorFix3] = UI_CreateUI(54+26*3,26,&FONT[4],Render_CB_ColorFix);
	UI[InUI_CB_ColorFix3].parameter = TFT_RGB888To565(0x00bc12);
	//			 	-固定颜色4
	UI[InUI_CB_ColorFix4] = UI_CreateUI(54+26*4,26,&FONT[4],Render_CB_ColorFix);
	UI[InUI_CB_ColorFix4].parameter = TFT_RGB888To565(0xFFFFFF);
	//			 	-固定颜色5	
	UI[InUI_CB_ColorFix5] = UI_CreateUI(54+26*5,26,&FONT[4],Render_CB_ColorFix);
	UI[InUI_CB_ColorFix5].parameter = TFT_RGB888To565(0xaa96da);
	//			 	-固定颜色6	
	UI[InUI_CB_ColorFix6] = UI_CreateUI(54+26*6,26,&FONT[4],Render_CB_ColorFix);
	UI[InUI_CB_ColorFix6].parameter = TFT_RGB888To565(0xabdd8);
	//			 	-固定颜色7	
	UI[InUI_CB_ColorFix7] = UI_CreateUI(54+26*7,26,&FONT[4],Render_CB_ColorFix);
	UI[InUI_CB_ColorFix7].parameter = TFT_RGB888To565(0x323232);
	//			 	-不定的颜色	//下键跳出RGB565输入框
	UI[InUI_CB_ColorUS] = UI_CreateUI(54+26*8,26,&FONT[4],Render_CB_ColorUS);
	//				-旧颜色 	
	UI[InUI_CB_ColorOld] = UI_CreateUI(54+26*9,26,&FONT[2],Render_CB_ColorFix);
	UI[InUI_CB_ColorOld].parameter = UI[InUI_ColorBox].font->ft_color;
	for(int i=InUI_CB_ColorFix0;i<=InUI_CB_ColorOld;i++)
	{
		UI[i].Func_Event_LEFT  = LEFT_CursorMove;
		UI[i].Func_Event_RIGHT = RIGHT_CursorMove;
		UI[i].Func_Event_DOWN  = DOWN_CB_Color;
		UI[i].Func_Event_UP	   = UP_CB_Color;
	}
	UI[InUI_CB_ColorFix0].Func_Event_LEFT = NULL_UI_Func;
	UI[InUI_CB_ColorOld].Func_Event_RIGHT = NULL_UI_Func;
	UI[InUI_CB_ColorUS].Func_Event_DOWN = DOWN_CB_ColorUS;
	//		InUI_CB_RGB_0x				//调色框第二行的RGB888输入框0x	28*24		仅是'0x'文本，不会被选中	
	UI[InUI_CB_RGB_0x] = UI_CreateUI(54,26,&FONT[1],Render_CB_TEXT);
	UI[InUI_CB_RGB_0x].parameter = 2;
	INS_StringCpy(UI[InUI_CB_RGB_0x].value_text,"0x");
	//		InUI_CB_RGB_5				//调色框第二行RGB888的最高位					左右更换RGB888输入位置 上下更换数值		
	UI[InUI_CB_RGB_5] = UI_CreateUI(68+16*1,26,&FONT[1],Render_CB_RGB);
	//		InUI_CB_RGB_4		
	UI[InUI_CB_RGB_4] = UI_CreateUI(68+16*2,26,&FONT[1],Render_CB_RGB);
	//		InUI_CB_RGB_3		
	UI[InUI_CB_RGB_3] = UI_CreateUI(68+16*3,26,&FONT[1],Render_CB_RGB);
	//		InUI_CB_RGB_2		
	UI[InUI_CB_RGB_2] = UI_CreateUI(68+16*4,26,&FONT[1],Render_CB_RGB);
	//		InUI_CB_RGB_1		
	UI[InUI_CB_RGB_1] = UI_CreateUI(68+16*5,26,&FONT[1],Render_CB_RGB);
	//		InUI_CB_RGB_0		
	UI[InUI_CB_RGB_0] = UI_CreateUI(68+16*6,26,&FONT[1],Render_CB_RGB);
	//		InUI_CB_RGB_True			//RGB888 确认
	UI[InUI_CB_RGB_True] = UI_CreateUI(68+16*6+24,26,&FONT[4],Render_CB_ColorFix);
	UI[InUI_CB_RGB_True].parameter = TFT_RGB888To565(0x00bc12);
	//		InUI_CB_RGB_False			//RGB888 取消
	UI[InUI_CB_RGB_False] = UI_CreateUI(68+16*6+50,26,&FONT[4],Render_CB_ColorFix);
	UI[InUI_CB_RGB_False].parameter = TFT_RGB888To565(0xc93756);
	//	 	InUI_CB_Line2_BK			//调色框第二行背景
	UI[InUI_CB_Line2_BK] = UI_CreateUI(52,26,&FONT[1],Render_CB_Line2_BK);
	for(int i=InUI_CB_RGB_5;i<=InUI_CB_RGB_False;i++)
	{
		UI[i].Func_Event_RIGHT = RIGHT_CursorMove;
		UI[i].Func_Event_LEFT = LEFT_CursorMove;
		UI[i].Func_Event_DOWN = DOWN_CB_RGB;
		UI[i].Func_Event_UP = UP_CB_RGB;
	}
	UI[InUI_CB_RGB_5].Func_Event_LEFT = NULL_UI_Func;
	UI[InUI_CB_RGB_False].Func_Event_RIGHT = NULL_UI_Func;
	UI[InUI_CB_RGB_False].Func_Event_UP = NULL_UI_Func;
	UI[InUI_CB_RGB_False].Func_Event_DOWN = DOWN_CB_RGB_False;
	UI[InUI_CB_RGB_True].Func_Event_UP = NULL_UI_Func;
	UI[InUI_CB_RGB_True].Func_Event_DOWN = DOWN_CB_RGB_True;
	
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
	uint16_t indexs_ofPage0[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17};
	UI_CreatePage(&PAGE[0],indexs_ofPage0,sizeof(indexs_ofPage0)/sizeof(uint16_t),&UI[InUI_CB_Type_FT],Render_Page0);
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
	if(UI_CURSOR.ptr_page==&PAGE[0])
	{
		return;
	}
	//获取要更改的UI的下标
	for(int i=0;i<100;i++)
	{
		if(UI_CURSOR.ptr_page->ui_index[i]==999)
		{
			break;
		}
		if(&UI[UI_CURSOR.ptr_page->ui_index[i]] == UI_CURSOR.ptr_ui)
		{
			UI[InUI_CB_UIIndex].value_num = UI_CURSOR.ptr_page->ui_index[i];
			break;
		}
	}
	//做页面跳转的准备
	uint8_t color_box_up = 0;	//判断调色器位置
	page_color_changed = UI_CURSOR.ptr_page;
	UI_CURSOR.parameter = InUI_CB_Type_FT;
	UI_CURSOR.temp_ui = UI_CURSOR.ptr_ui;
	//确定调色器位置
	if(UI[InUI_ColorBox].y!=0&&UI_CURSOR.temp_ui->y>UI[InUI_ColorBox].y || 
	   UI_CURSOR.temp_ui->y+UI_CURSOR.temp_ui->font->size_height>UI[InUI_ColorBox].y)
	{
		UI[InUI_ColorBox].y = 0;
		color_box_up = 1;
	}
	else 
	{
		UI[InUI_ColorBox].y = SCREEN_HEIGHT-51;
	}
	//CB_Sample
	UI[InUI_CB_Sample].font->bk_color = UI_CURSOR.temp_font->bk_color;
	UI[InUI_CB_Sample].font->ft_color = UI_CURSOR.temp_font->ft_color;
	//PAGE[0]->调色器页面
	UI_ChangePage(&PAGE[0]);
	for(int i=0;i<100;i++)
	{
		if(page_color_changed->ui_index[i]==999)
		{
			break;
		}
		//我都不敢想这段代码要怎么读
		//避免页面刷新内容和调色器冲突
		if(color_box_up==1 && UI[page_color_changed->ui_index[i]].y<UI[InUI_CB_Sample].y+51)//调色器在上面
		{//调色器在上面 并且 当前UI的y比调色器的下底边高
			continue;
		}
		else if(color_box_up==0 && UI[page_color_changed->ui_index[i]].y+UI[page_color_changed->ui_index[i]].font->size_height>UI[InUI_CB_Sample].y)
		{//调色器在下面 并且 当前UI下底边比调色器的y(上顶)低
			continue;
		}
		UI[page_color_changed->ui_index[i]].is_present = 1;
	}
	UI_AddRender(UI_CURSOR.temp_ui);
}
void INS_ExitColorChange(void)
{
	for(int i=0;i<100;i++)
	{
		if(page_color_changed->ui_index[i]==999)
		{
			break;
		}
		UI[page_color_changed->ui_index[i]].is_present = 0;
		UI[page_color_changed->ui_index[i]].readyto_present = 0;
	}
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





