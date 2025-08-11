#include "UI_Ins_ColorBox.h"
#include "TFT_UI.h"			//基本架构
#include "TFT_font.h"		//提供字体tft_font
#include "UI_Index.h"		//提供UI和字体下标
#include "TFT_ILI9341.h"	//提供RGB888To565
#include "qy_printf.h"		//提供QY_Printf

/*	把调色框专门写了一个文件
 *	接口很简单，UI初始化和Page初始化
 *	用ColorBox_Enter打开调色框
 *	用ColorBox_Exit退出
 *	1000行代码，真不敢想象
 *	
 *
 */

extern tft_pointer 	UI_CURSOR;				//光标
extern tft_font 	FONT[300];				//字体
extern tft_ui 		UI[200];				//UI
extern tft_page 	PAGE[20];				//页面
extern tft_font 	null_font;				//表示未绑定font size_width=size_height = 999;

/* 字体库 */
	//字体
extern const char font_ASCII_PIXEL_2412[][36];
extern const char font_ASCII_PIXEL_3216[][64];
//extern const char font_ASCII_PIXEL_4020[][100];
//extern const char font_ASCII_NI7SEG_2412[][36];
//extern const char font_ASCII_NI7SEG_3216[][64];
//extern const char font_ASCII_NI7SEG_4020[][100];
extern const char font_ASCII_Gothic_2412[][36];
extern const char font_ASCII_Gothic_3216[][64];
//extern const char font_ASCII_Gothic_4020[][100];
			//有关字体增减的函数配置
			//static void Other_CB_FONTChange(tft_ui* u)
			//static void Other_ChangeFont(tft_font* f)
			//void LEFT/RIGHT_CB_FONTChange(tft_ui* u)
			//int8_t color_changed_font_type
	//图片
extern const char qyf_pic01_cloud_3232[];
extern const char qyf_pic01_object_2424[];
extern const char qyf_pic01_down_2424[];
extern const char qyf_pic01_up_2424[];

/*  颜色库  */
uint16_t COLOR_FIXED[8];
uint16_t COLOR_RGB565[8];

/*  临时存储的全局变量  */
	//颜色修改面板的page缓冲区
tft_page* color_changed_page 	= &PAGE[0];
	//当前要修改UI的哪个字体0~3号字体
int8_t 	  color_changed_font_Index 	= 0;
	//是否修改当前整个UI组
int8_t 	  color_changed_combination = 1;
	//当前修改的UI的Index
uint16_t  color_changed_ui_index = 0;
	//临时存储要修改的字体
tft_font*	color_changed_font[4][16];
tft_font	color_changed_font_temp[16];
	//第二行处于 0-固定色块/1-RGB色块
uint16_t cb_colorchange = 0;
	//当前要修改UI字体的什么属性（InUI_CB_FT-前景色/InUI_CB_BK-背景色/InUI_CB_F_-字体）
uint8_t index_cb_type;
	//字体类型 (0-PIXEL_LCD7/1-NI7SEG/2-MS_Gothic)
int8_t  color_changed_font_type;	


/*  调色框  */
/*  调色框-渲染部分  */
//页面渲染
void Render_CB_Background(tft_ui* u)
{
	//内框
	TFTF_DrawRect(UI[InUI_ColorBox].x,UI[InUI_ColorBox].y,320,51,UI[InUI_ColorBox].font->bk_color);
	TFTF_DrawRect(UI[InUI_ColorBox].x+51,UI[InUI_ColorBox].y,1,51,UI[InUI_ColorBox].font->ft_color);
	TFTF_DrawRect(UI[InUI_ColorBox].x+51,UI[InUI_ColorBox].y+25,270,1,UI[InUI_ColorBox].font->ft_color);
	//外边框
	TFTF_DrawFrame(UI[InUI_ColorBox].x,UI[InUI_ColorBox].y,320,51,UI[InUI_ColorBox].font->ft_color,1);
}
//颜色选择框->示例图片
void Render_CB_Sample(tft_ui* u)
{
	TFTF_Single_01Pic(u->x+9,UI[InUI_ColorBox].y+9,FONT[InFT_CB_Sample]);
	TFTF_DrawFrame(u->x+4,u->y+4,42,42,0,2);
}
//常规图片渲染
void Render_CB_Pic(tft_ui* u)
{
	TFTF_Single_01Pic(UI[InUI_ColorBox].x+u->x,UI[InUI_ColorBox].y+u->y,*u->font);
}
//颜色选择框->当前UI文本
void Render_CB_TEXT(tft_ui* u)
{
	TFTF_ShowString(UI[InUI_ColorBox].x+u->x,UI[InUI_ColorBox].y+u->y,u->value_text,*(u->font),u->parameter);
}
//色块的渲染
void Render_CB_ColorFix(tft_ui* u)
{
	TFTF_DrawRect(UI[InUI_ColorBox].x+u->x+4,UI[InUI_ColorBox].y+u->y+4,16,16,u->parameter);
	TFTF_DrawFrame(UI[InUI_ColorBox].x+u->x+2,UI[InUI_ColorBox].y+u->y+2,20,20,u->font->bk_color,2);
}
//RGB888输入框的入口，色块区中彩色的色块的渲染 US原来是UnSure的缩写
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
//输入RGB888时的数字
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
//调色框第二行的背景，用于覆盖
void Render_CB_Line2_BK(tft_ui* u)
{
	TFTF_DrawRect(UI[InUI_ColorBox].x+u->x,UI[InUI_ColorBox].y+u->y,SCREEN_WIDTH,u->parameter,FONT[InFT_ColorBox].bk_color);
}


/*  调色框-交互部分  */
//从调色框第二行返回调色框第一行
void UP_CB_Color(tft_ui* u)
{
	UI_CURSOR.parameter = index_cb_type;
	UI_Cursor_ChangeUI(&UI[index_cb_type]);
	UI_CURSOR.temp_font = &FONT[InFT_ColorBox];
}
//调色框第二行的箭头图标，更换色块类型(固定色块/RGB888色块)
void DOWN_CB_ColorChange(tft_ui* u)
{
	uint16_t j=0;
	if(cb_colorchange==0)//处于固定颜色页
	{
		cb_colorchange = 1;
		u->Func_Event_UP = DOWN_CB_ColorChange;
		//换色块
		for(int i=InUI_CB_ColorFix0;i<=InUI_CB_ColorFix7;i++)
		{
			UI[i].parameter = COLOR_RGB565[j++];
			UI_AddRender(&UI[i]);
		}
		//更改自身图标
		UI_CURSOR.cursor_font->font_lib = (const char*)qyf_pic01_up_2424;
		UI_CURSOR.temp_font->font_lib = (const char*)qyf_pic01_up_2424;
		UI_AddRender(u);
		u->Func_Event_RIGHT = RIGHT_CursorMove;
		//添加RGB888输入接口
		UI[InUI_CB_RGB].is_present = 1;
		UI[InUI_CB_RGB].readyto_present = 0;
		UI_AddRender(&UI[InUI_CB_RGB]);
		return;
	}
	u->Func_Event_UP = UP_CB_Color;	
	cb_colorchange = 0;
	//渲染背景
	UI[InUI_CB_Line2_BK].is_present = 1;
	UI[InUI_CB_Line2_BK].readyto_present = 0;
	UI_AddRender(&UI[InUI_CB_Line2_BK]);
	UI[InUI_CB_Line2_BK].is_present = 0;
	//换色块
	for(int i=InUI_CB_ColorFix0;i<=InUI_CB_ColorFix7;i++)
	{
		UI[i].parameter = COLOR_FIXED[j++];
		UI_AddRender(&UI[i]);
	}
	//更改自身图标
	UI_CURSOR.cursor_font->font_lib = (const char*)qyf_pic01_down_2424;
	UI_CURSOR.temp_font->font_lib = (const char*)qyf_pic01_down_2424;
	u->readyto_present = 0;
	UI_AddRender(u);
	u->Func_Event_RIGHT = NULL_UI_Func;
}
//选择要修改的字体属性(前景色/背景色/字体)
void DOWN_CB_Type(tft_ui* u)
{
	index_cb_type = UI_CURSOR.parameter;
	switch(UI_CURSOR.parameter)
	{
	case InUI_CB_Type_BK:
	case InUI_CB_Type_FT:
		//重新渲染第一行
		for(int i=InUI_CB_Type_BK;i<=InUI_CB_Type_Q_;i++)
		{
			UI[i].font = &FONT[InFT_ColorBox];
			UI_AddRender(&UI[i]);
		}
		//更改UI绑定
		UI_CURSOR.parameter = InUI_CB_ColorChange;
		UI_CURSOR.temp_font = &FONT[InFT_CB_On];
		UI_Cursor_ChangeUI(&UI[InUI_CB_ColorChange]);
		//渲染处理第二行
		for(int i=InUI_CB_ColorFix0;i<=InUI_CB_ColorFix7;i++)
		{
			UI[i].font = &FONT[InFT_ColorBox];
		}
		UI[InUI_CB_ColorFix0].font = &FONT[InFT_CB_On];
		cb_colorchange =1;
		DOWN_CB_ColorChange(&UI[InUI_CB_ColorChange]);
		break;
	case InUI_CB_Type_F_:
		//做一下字体判定
		if(UI_CURSOR.temp_ui->font->font_lib == (const char*)font_ASCII_PIXEL_2412 ||
		   UI_CURSOR.temp_ui->font->font_lib == (const char*)font_ASCII_PIXEL_3216 )
//		   UI_CURSOR.temp_ui->font->font_lib == (const char*)font_ASCII_PIXEL_4020 )
		{
			UI[InUI_CB_FONTChange].font->font_lib = (const char*)font_ASCII_PIXEL_2412;
			color_changed_font_type = 0;
		}
//		else if(UI_CURSOR.temp_ui->font->font_lib == (const char*)font_ASCII_NI7SEG_2412 ||
//				UI_CURSOR.temp_ui->font->font_lib == (const char*)font_ASCII_NI7SEG_3216 )
//				UI_CURSOR.temp_ui->font->font_lib == (const char*)font_ASCII_NI7SEG_4020 )
//		{
//			UI[InUI_CB_FONTChange].font->font_lib = (const char*)font_ASCII_NI7SEG_2412;
//			color_changed_font_type = 1;
//		}
		else if(UI_CURSOR.temp_ui->font->font_lib == (const char*)font_ASCII_Gothic_2412 ||
				UI_CURSOR.temp_ui->font->font_lib == (const char*)font_ASCII_Gothic_3216 )
//				UI_CURSOR.temp_ui->font->font_lib == (const char*)font_ASCII_Gothic_4020 )
		{
			UI[InUI_CB_FONTChange].font->font_lib = (const char*)font_ASCII_Gothic_2412;
			color_changed_font_type = 2;
		}
			//清空第二行
		UI[InUI_CB_Line2_BK].is_present = 1;
		UI[InUI_CB_Line2_BK].readyto_present = 0;
		UI_AddRender(&UI[InUI_CB_Line2_BK]);
		UI[InUI_CB_Line2_BK].is_present = 0;
			//字体选项
		UI[InUI_CB_FONTChange].is_present = 1;
		UI[InUI_CB_FONTChange].readyto_present = 0;
		UI_AddRender(&UI[InUI_CB_FONTChange]);
		//更改UI绑定
		UI_Cursor_ChangeUI(&UI[InUI_CB_FONTChange]);
		break;
	case InUI_CB_Type_Q_:
		ColorBox_Exit();
		break;
	default:
		QY_Printf("\r\n UI_Instance_example缺乏绑定 \r\n");
	}
}
//确定切换颜色
void Other_ChangeColor(uint16_t rgb565,uint16_t index)
{
	uint16_t changed_index = index - UI_CURSOR.temp_ui->index_start;
	if(index_cb_type==InUI_CB_Type_FT)
	{
		UI[InUI_CB_Sample].font->ft_color = rgb565;
		color_changed_font[color_changed_font_Index][changed_index]->ft_color = rgb565;
		UI[index].font->ft_color = rgb565;
	}
	else if(index_cb_type==InUI_CB_Type_BK)
	{
		UI[InUI_CB_Sample].font->bk_color = rgb565;
		color_changed_font[color_changed_font_Index][changed_index]->bk_color = rgb565;
		UI[index].font->bk_color = rgb565;
	}
	UI_AddRender(&UI[index]);
}
//记录色块的UI方便更改
uint16_t color_changed_colorblock_index = 0;
//将示例图片和UI更改成该色块的颜色
void DOWN_CB_Color(tft_ui* u)
{	
		//便于RGB888输入框更改颜色
	color_changed_colorblock_index = UI_CURSOR.parameter;
	//刷新调色框第二行
	for(int i=InUI_CB_ColorFix0;i<InUI_CB_ColorChange;i++)
	{
		UI[i].font = &FONT[InFT_CB_Block];
		UI_AddRender(&UI[i]);
	}
	UI_CURSOR.temp_font = &FONT[InFT_CB_On];
	//更改颜色
	if(color_changed_combination==1)
	{
		for(int i=UI_CURSOR.temp_ui->index_start;i<=UI_CURSOR.temp_ui->index_end;i++)
		{
			Other_ChangeColor(u->parameter,i);
		}
	}
	else
	{
		Other_ChangeColor(u->parameter,color_changed_ui_index);
	}
	//重新渲染
	UI_AddRender(&UI[InUI_CB_Sample]);
	UI_Cursor_ChangeUI(UI_CURSOR.ptr_ui);
}
//进入RGB888输入框
void DOWN_CB_RGBEnter(tft_ui* u)
{//更换成RGB888输入
	//把旧颜色放在InUI_CB_RGB0~5中显示
	uint32_t oc = 0;//Old Color
	if(index_cb_type==InUI_CB_Type_FT)
	{
		oc = UI[InUI_CB_Sample].font->ft_color;
	}
	else
	{
		oc = UI[InUI_CB_Sample].font->bk_color; 
	}
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
	UI[InUI_CB_RGB].is_present = 0;
	UI_Cursor_ChangeUI(&UI[InUI_CB_RGB_5]);
	UI_CURSOR.parameter = InUI_CB_RGB_5;
}
//把RGB888数字实时更新到示例图片和要修改的UI上
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
	if(color_changed_combination==1)
	{
		for(int i=UI_CURSOR.temp_ui->index_start;i<=UI_CURSOR.temp_ui->index_end;i++)
		{
			Other_ChangeColor(rgb888,i);
		}
	}
	else
	{
		Other_ChangeColor(rgb888,color_changed_ui_index);
	}
	//重新刷新UI
	UI_AddRender(&UI[InUI_CB_Sample]);
	UI_AddRender(UI_CURSOR.temp_ui);
}
//调整RGB888数字
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
//调整RGB888数字
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
//确认/取消输出RGB888数据
static void Other_CB_RGB_TrueOrFalse(void)
{
	//修改全局变量->自定义颜色
	if(index_cb_type==InUI_CB_Type_FT)
	{
		COLOR_RGB565[color_changed_colorblock_index-InUI_CB_ColorFix0] = UI[InUI_CB_Sample].font->ft_color;
		UI[color_changed_colorblock_index].parameter = UI[InUI_CB_Sample].font->ft_color;
	}
	else
	{
		COLOR_RGB565[color_changed_colorblock_index-InUI_CB_ColorFix0] = UI[InUI_CB_Sample].font->bk_color;
		UI[color_changed_colorblock_index].parameter = UI[InUI_CB_Sample].font->bk_color;
	}
	//调色框第二行换回固定颜色栏
	UI[InUI_CB_Line2_BK].is_present =1;
	UI[InUI_CB_Line2_BK].readyto_present =0;
	UI_AddRender(&UI[InUI_CB_Line2_BK]);
	UI[InUI_CB_Line2_BK].is_present =0;
	UI[InUI_CB_RGB].is_present = 1;
	UI[InUI_CB_RGB].readyto_present = 0;
	for(int i=InUI_CB_ColorFix0;i<=InUI_CB_RGB;i++)
	{
		UI_AddRender(&UI[i]);
	}
	//更换到当前绑定的色块
	UI_CURSOR.parameter = color_changed_colorblock_index;
	UI_Cursor_ChangeUI(&UI[color_changed_colorblock_index]);
}
//确认RGB888颜色更改
void DOWN_CB_RGB_True(tft_ui* u)
{
	u->is_present = 0;
	Other_CB_RGB_TrueOrFalse();
}
//取消颜色RGB888更改
void DOWN_CB_RGB_False(tft_ui* u)
{
	u->is_present = 0;
	if(index_cb_type==InUI_CB_Type_FT)
	{
		UI[InUI_CB_Sample].font->ft_color = UI[color_changed_colorblock_index].parameter;
	}
	else
	{
		UI[InUI_CB_Sample].font->bk_color = UI[color_changed_colorblock_index].parameter;
	}
	Other_CB_RGB_TrueOrFalse();
	if(color_changed_combination==1)
	{
		for(int i=UI_CURSOR.temp_ui->index_start;i<=UI_CURSOR.temp_ui->index_end;i++)
		{
			Other_ChangeColor(UI[color_changed_colorblock_index].parameter,i);
		}
	}
	else
	{
		Other_ChangeColor(UI[color_changed_colorblock_index].parameter,color_changed_ui_index);
	}
}
//挑选当前UI要更改的状态
void DOWN_CB_Font(tft_ui* u)
{
	color_changed_font_Index = u->value_num;
	//选中UI的处理
		//选中多个UI
	if(color_changed_combination==1)
	{
		uint16_t ui_index = 0;
		for(int i=UI_CURSOR.temp_ui->index_start;i<=UI_CURSOR.temp_ui->index_end;i++)
		{
			UI[i].font->bk_color = color_changed_font[color_changed_font_Index][ui_index]->bk_color;
			UI[i].font->ft_color = color_changed_font[color_changed_font_Index][ui_index]->ft_color;
			UI[i].font->font_lib = color_changed_font[color_changed_font_Index][ui_index]->font_lib;
			UI_AddRender(&UI[i]);
			ui_index++;
		}
	}
	else//单个UI处理
	{
				//将当前状态(color_changed_font_Index)的字体复制上
		uint16_t backup_font_index = color_changed_ui_index - UI_CURSOR.temp_ui->index_start;
		UI_CURSOR.temp_ui->font->bk_color = color_changed_font[color_changed_font_Index][backup_font_index]->bk_color;
		UI_CURSOR.temp_ui->font->ft_color = color_changed_font[color_changed_font_Index][backup_font_index]->ft_color;
		UI_CURSOR.temp_ui->font->font_lib = color_changed_font[color_changed_font_Index][backup_font_index]->font_lib;	
	}
	//更改示例图片
	UI[InUI_CB_Sample].font->bk_color = UI_CURSOR.temp_ui->font->bk_color;
	UI[InUI_CB_Sample].font->ft_color = UI_CURSOR.temp_ui->font->ft_color;
	UI_AddRender(&UI[InUI_CB_Sample]);
	UI_AddRender(UI_CURSOR.temp_ui);
	//自身颜色改为选中状态
	for(int i=InUI_CB_Font0;i<=InUI_CB_Font3;i++)
	{
		UI[i].font = &FONT[InFT_ColorBox];
		UI_AddRender(&UI[i]);
	}
	UI_CURSOR.temp_font = &FONT[InFT_CB_On];
	UI_Cursor_ChangeUI(UI_CURSOR.ptr_ui);
}
//将UI组恢复
static void Other_ChangeCombination(void)
{
	color_changed_combination = 1;
	uint16_t index_start=UI_CURSOR.temp_ui->index_start;
	uint16_t index_end	=UI_CURSOR.temp_ui->index_end;
	uint16_t index_i;
	for(int i=index_start;i<=index_end;i++)
	{
		index_i = i-index_start;
		for(int j=0;j<4;j++)
		{
			color_changed_font[j][index_i] = UI[i].d_font[j];
		}
		color_changed_font_temp[index_i].font_lib = UI[i].font->font_lib;
		color_changed_font_temp[index_i].size_height = UI[i].font->size_height;
		color_changed_font_temp[index_i].size_width = UI[i].font->size_width;
		color_changed_font_temp[index_i].bk_color = UI_CURSOR.cursor_font->bk_color;
		color_changed_font_temp[index_i].ft_color = UI_CURSOR.cursor_font->ft_color;
		UI[i].font = &color_changed_font_temp[index_i];
		for(int j=0;j<4;j++)
		{
			UI[i].d_font[j] = &color_changed_font_temp[index_i];
		}
		UI[i].is_present = 1;
		UI[i].readyto_present = 0;
		UI_AddRender(&UI[i]);
	}
}
//更换当前想要更改颜色的UI
void DOWN_CB_Choose(tft_ui* u)
{
	uint16_t s = UI_CURSOR.temp_ui->index_start;
	uint16_t e = UI_CURSOR.temp_ui->index_end;
	uint16_t index = color_changed_ui_index - s;
	if(color_changed_combination==1)
	{
		color_changed_combination=0;
		//把字体还给其他元素，重新渲染后禁止渲染
		for(int i=s+1;i<=e;i++)
		{
			for(int j=0;j<4;j++)
			{
				UI[i].d_font[j] = color_changed_font[j][i-s];
			}
		}
		for(int i=s+1;i<=e;i++)
		{
			UI[i].font = UI[i].d_font[0];
			UI[i].Func_StateRule(&UI[i]);
			UI_AddRender(&UI[i]);
			UI[i].is_present = 0;
		}
		//更改选中本UI
		UI_CURSOR.temp_ui = &UI[s];
		color_changed_font_temp[0].bk_color = UI_CURSOR.cursor_font->bk_color;
		color_changed_font_temp[0].ft_color = UI_CURSOR.cursor_font->ft_color;
		UI_AddRender(UI_CURSOR.temp_ui);
		color_changed_ui_index = s;
		return;
	}
	//指向单个UI
			//最后一个UI的特殊处理
	else if(color_changed_ui_index==UI_CURSOR.temp_ui->index_end)
	{
		//恢复上一个UI
		for(int i=0;i<4;i++)
		{
			UI_CURSOR.temp_ui->d_font[i] = color_changed_font[i][index];
		}
		UI_CURSOR.temp_ui->font = UI_CURSOR.temp_ui->d_font[0];
		UI_CURSOR.temp_ui->Func_StateRule(UI_CURSOR.temp_ui);
		UI_AddRender(UI_CURSOR.temp_ui);
		UI_CURSOR.temp_ui->is_present = 0;
		//选中整体
		Other_ChangeCombination();
		return;
	}
	else
	{
		//恢复上一个UI
		for(int i=0;i<4;i++)
		{
			UI_CURSOR.temp_ui->d_font[i] = color_changed_font[i][index];
		}
		UI_CURSOR.temp_ui->font = UI_CURSOR.temp_ui->d_font[0];
		UI_CURSOR.temp_ui->Func_StateRule(UI_CURSOR.temp_ui);
		UI_AddRender(UI_CURSOR.temp_ui);
		UI_CURSOR.temp_ui->is_present = 0;
	}
		//指向下一个UI
	color_changed_ui_index++;
	index++;
	UI_CURSOR.temp_ui = &UI[color_changed_ui_index];
			//创建颜色备份
	for(int i=0;i<4;i++)
	{
		color_changed_font[i][index] = UI_CURSOR.temp_ui->d_font[i];
	}
	color_changed_font_temp[index].font_lib 		= UI_CURSOR.temp_ui->font->font_lib;
	color_changed_font_temp[index].size_height 	= UI_CURSOR.temp_ui->font->size_height;
	color_changed_font_temp[index].size_width 	= UI_CURSOR.temp_ui->font->size_width;
	color_changed_font_temp[index].bk_color = UI_CURSOR.cursor_font->bk_color;	
	color_changed_font_temp[index].ft_color = UI_CURSOR.cursor_font->ft_color;
	UI_CURSOR.temp_ui->font = &color_changed_font_temp[index];
	for(int j=0;j<4;j++)
	{
		UI_CURSOR.temp_ui->d_font[j] = &color_changed_font_temp[index];
	}
	UI_CURSOR.temp_ui->is_present = 1;
	UI_CURSOR.temp_ui->readyto_present = 0;
	UI_AddRender(UI_CURSOR.temp_ui);
}
//调色框中风扇图标绑定的函数
void RIGHT_CB_Choose(tft_ui* u)
{
	RIGHT_CursorMove(u);
	DOWN_CB_Font(&UI[InUI_CB_Font0]);
}
//关于字体更改的函数
static void Other_ChangeFont(tft_font* f);
void DOWN_CB_FONTChange(tft_ui* u)
{
	//UI组处理
	uint16_t s=UI_CURSOR.temp_ui->index_start;
	uint16_t e=UI_CURSOR.temp_ui->index_end;
	uint16_t temp_index = color_changed_ui_index-s;
	if(color_changed_combination==1)
	{
		for(int i=s;i<=e;i++)
		{
			temp_index = i-s;
			Other_ChangeFont(UI[i].font);
			Other_ChangeFont(color_changed_font[color_changed_font_Index][temp_index]);
			UI_AddRender(&UI[i]);
		}
	}
	//单独UI处理
	else
	{
		Other_ChangeFont(UI_CURSOR.temp_ui->font);
		Other_ChangeFont(color_changed_font[color_changed_font_Index][temp_index]);
		UI_AddRender(&UI[color_changed_ui_index]);
	}
}
//根据全局变量更改字体
static void Other_ChangeFont(tft_font* f)
{
	if(f->size_height!=2*f->size_width)
	{
		Other_StringCpy(UI[InUI_CB_FONTChange].value_text,"  __Not_Text_!__  ");
		UI_AddRender(&UI[InUI_CB_FONTChange]);
		return;
	}
	switch(f->size_height)
	{
	case 24:
		switch(color_changed_font_type)
		{
		case 0:f->font_lib = (const char*)font_ASCII_PIXEL_2412;	break;
//		case 1:f->font_lib = (const char*)font_ASCII_NI7SEG_2412;	break;
		case 1:f->font_lib = (const char*)font_ASCII_Gothic_2412;	break;
		}
		break;
	case 32:
		switch(color_changed_font_type)
		{
		case 0:f->font_lib = (const char*)font_ASCII_PIXEL_3216;	break;
//		case 1:f->font_lib = (const char*)font_ASCII_NI7SEG_3216;	break;
		case 1:f->font_lib = (const char*)font_ASCII_Gothic_3216;	break;
		}
		break;
	case 40:
		break;
	default:
		Other_StringCpy(UI[InUI_CB_FONTChange].value_text,"  __Not_Text_!__  ");
		UI_AddRender(&UI[InUI_CB_FONTChange]);
		return;
	}
}
//调色框第二行-字体选择
static void Other_CB_FONTChange(tft_ui* u)
{
	switch(color_changed_font_type)
	{
	case 0:
		Other_StringCpy(u->value_text,"<  _PIXEL_LCD7_  >");
		break;
//	case 1:
//		Other_StringCpy(u->value_text,"<  ___NI7SEG___  >");
//		break;
	case 1:
		Other_StringCpy(u->value_text,"<  _MS_Gothic__  >");
		break;
	}
}
void RIGHT_CB_FONTChange(tft_ui* u)
{
	if(++color_changed_font_type>1)
	{
		color_changed_font_type = 0;
	}
	Other_CB_FONTChange(u);
	Other_ChangeFont(u->font);
	UI_Cursor_Refresh();
}
void LEFT_CB_FONTChange(tft_ui* u)
{
	if(--color_changed_font_type<0)
	{
		color_changed_font_type = 1;
	}
	Other_CB_FONTChange(u);
	Other_ChangeFont(u->font);
	UI_Cursor_Refresh();
}



/*  接口函数  */

/**@brief  打开调色框
  */
void ColorBox_Enter(void)
{
	if(UI_CURSOR.ptr_page==&PAGE[0])
	{//调色框已经显示
		return;
	}
	//做页面跳转的准备（关于全局变量）
	color_changed_page = UI_CURSOR.ptr_page;
	color_changed_colorblock_index = InUI_CB_ColorFix0;
	color_changed_ui_index = UI_CURSOR.ptr_ui->index_start;
	if(color_changed_ui_index==0)	//当前UI没有绑定所在的UI组，重新进行绑定
	{
		for(int i=0;UI[color_changed_page->ui_index[i]].is_present!=100;i++)
		{
			if(i==100)
			{
				QY_Printf("\r\n UI_INS_ColorBox中ColorBox_Enter触发异常 \r\n");
				break;
			}
			if(&UI[color_changed_page->ui_index[i]]==UI_CURSOR.ptr_ui)
			{
				color_changed_ui_index = color_changed_page->ui_index[i];
				UI_CURSOR.ptr_ui->index_start = color_changed_ui_index;
				UI_CURSOR.ptr_ui->index_end = color_changed_ui_index;
				break;
			}
		}
	}
	color_changed_font_Index = 0;
	UI_CURSOR.parameter = InUI_CB_Choose;
	UI_CURSOR.temp_ui = UI_CURSOR.ptr_ui;
	for(int i=InUI_CB_Font1;i<=InUI_CB_Font3;i++)
	{
		UI[i].font = &FONT[InFT_ColorBox];
	}
	UI[InUI_CB_Font0].d_font[0] = &FONT[InFT_CB_On];
	//CB_Sample
	UI[InUI_CB_Sample].font->bk_color = UI_CURSOR.temp_font->bk_color;
	UI[InUI_CB_Sample].font->ft_color = UI_CURSOR.temp_font->ft_color;
	//进行调色器位置判断
	if(UI[InUI_ColorBox].y!=0 && UI_CURSOR.temp_ui->y+UI_CURSOR.temp_ui->font->size_height>UI[InUI_ColorBox].y)
	{
		UI[InUI_ColorBox].y = 0;
	}
	else
	{
		UI[InUI_ColorBox].y =SCREEN_HEIGHT-51;
	}
	//PAGE[0]->调色器页面
	UI_ChangePage(&PAGE[0]);
		//字体备份,并重新渲染UI组
	if(UI_CURSOR.temp_ui->index_start!=0)
	{
		Other_ChangeCombination();
	}
	else
	{
		color_changed_combination = 0;
	}
	UI_AddRender(UI_CURSOR.temp_ui);
}
/**@brief  退出调色框
  */
void ColorBox_Exit(void)
{
	//将字体归还
	uint16_t font_index = 0;
	for(int i=UI_CURSOR.temp_ui->index_start;i<=UI_CURSOR.temp_ui->index_end;i++)
	{
		for(int j=0;j<4;j++)
		{
			UI[i].d_font[j] = color_changed_font[j][font_index];
		}
		UI[i].font = UI[i].d_font[0];
		font_index++;
	}
	UI_ChangePage(color_changed_page);
}
/**@brief  初始化
  */
void Init_UI_ColorBox(void)
{
	//ColorBox全局变量初始化
	for(int i=0;i<16;i++)
	{
		color_changed_font_temp[i] = TFTF_CreateFont((const char*)font_ASCII_PIXEL_2412,24,12,TFT_RGB888To565(0xfce38a),TFT_RGB888To565(0x522546));
		for(int j=0;j<4;j++)
		{
			color_changed_font[j][i] = &color_changed_font_temp[i];
		}
	}
	//颜色初始化
	uint32_t color_rgb888_a[] = {0xff4e20,0x40de5a,0xe29c45,0x4b5cc4,0xeaff56,0xff0097,0xfffbf0,0x392f41};
	uint32_t color_rgb888_b[] = {0x0c8918,0xd9b611,0xbbcdc5,0x574266,0x8c4356,0x057748,0xe9bb1d,0xc0ebd7};
	for(int i=0;i<8;i++)
	{
		COLOR_FIXED[i] = TFT_RGB888To565(color_rgb888_a[i]);
		COLOR_RGB565[i] = TFT_RGB888To565(color_rgb888_b[i]);
	}
	
	//字体配置
	//	1	颜色选择框的文本和数字
	FONT[InFT_ColorBox] = TFTF_CreateFont((const char*)font_ASCII_PIXEL_2412,24,12,TFT_RGB888To565(0x40514e),TFT_RGB888To565(0xa6e3e9));
	//	2	颜色选择框-选中配色
	FONT[InFT_CB_On] = TFTF_CreateFont((const char*)font_ASCII_PIXEL_2412,24,12,TFT_RGB888To565(0xfcbad3),TFT_RGB888To565(0xf38181));
	//  3	颜色选择框-示例图片
	FONT[InFT_CB_Sample] = TFTF_CreateFont((const char*)qyf_pic01_cloud_3232,32,32,0,0xFFFF);
	//	4	颜色选择框-颜色外圈的框
	FONT[InFT_CB_Block] = TFTF_CreateFont((const char*)font_ASCII_PIXEL_3216,32,16,TFT_RGB888To565(0xc9d6df),FONT[InFT_ColorBox].bk_color);
	//	5	颜色选择框-更换要调整的UI(图标)
	FONT[InFT_ICON_0] = TFTF_CreateFont((const char*)qyf_pic01_object_2424,24,24,FONT[InFT_ColorBox].ft_color,FONT[InFT_ColorBox].bk_color);
	//	6	颜色选择框-更换色块组(图标)
	FONT[InFT_ICON_1] = TFTF_CreateFont((const char*)qyf_pic01_down_2424,24,24,FONT[InFT_ColorBox].ft_color,FONT[InFT_ColorBox].bk_color);
	//	7	颜色选择框-第二行更换字体
	FONT[InFT_CB_FONT] = TFTF_CreateFont((const char*)font_ASCII_PIXEL_2412,24,12,FONT[InFT_ColorBox].ft_color,FONT[InFT_ColorBox].bk_color);
	
	//UI配置
	//		颜色选择框			//不会被光标选中	51*320  左侧示例图片(框)
	UI[InUI_ColorBox] = UI_CreateUI(0,SCREEN_HEIGHT-51,&FONT[InFT_ColorBox],NULL_UI_Func);
	//		颜色框-示例图片/数字	//不会被光标选中 32*32	坐标相对与颜色选择框 +9，+9 边框+4,+4,42,42,3 
	UI[InUI_CB_Sample] = UI_CreateUI(0,0,&FONT[InFT_CB_Sample],Render_CB_Sample);
	//		颜色框-更换当前调整的UI
	UI[InUI_CB_Choose] = UI_CreateUI(54,1,&FONT[InFT_ICON_0],Render_CB_Pic);
	//		颜色框-字体0
	UI[InUI_CB_Font0] = UI_CreateUI(SCREEN_WIDTH-170-20*3,1,&FONT[InFT_ColorBox],Render_CB_TEXT);
	UI[InUI_CB_Font0].parameter = 1;
	UI[InUI_CB_Font0].value_num = 0;
	Other_StringCpy(UI[InUI_CB_Font0].value_text,"0");
	//		颜色框-字体1
	UI[InUI_CB_Font1] = UI_CreateUI(SCREEN_WIDTH-170-20*2,1,&FONT[InFT_ColorBox],Render_CB_TEXT);
	UI[InUI_CB_Font1].parameter = 1;
	UI[InUI_CB_Font1].value_num = 1;
	Other_StringCpy(UI[InUI_CB_Font1].value_text,"1");
	//		颜色框-字体2
	UI[InUI_CB_Font2] = UI_CreateUI(SCREEN_WIDTH-170-20*1,1,&FONT[InFT_ColorBox],Render_CB_TEXT);
	UI[InUI_CB_Font2].parameter = 1;
	UI[InUI_CB_Font2].value_num = 2;
	Other_StringCpy(UI[InUI_CB_Font2].value_text,"2");
	//		颜色框-字体3
	UI[InUI_CB_Font3] = UI_CreateUI(SCREEN_WIDTH-170,1,&FONT[InFT_ColorBox],Render_CB_TEXT);
	UI[InUI_CB_Font3].parameter = 1;
	UI[InUI_CB_Font3].value_num = 3;
	Other_StringCpy(UI[InUI_CB_Font3].value_text,"3");
	for(int i=InUI_CB_Choose;i<=InUI_CB_Font3;i++)
	{
		UI[i].Func_Event_DOWN = DOWN_CB_Font;
		UI[i].Func_Event_LEFT = LEFT_CursorMove;
		UI[i].Func_Event_RIGHT = RIGHT_CursorMove;
	}
	UI[InUI_CB_Choose].Func_Event_LEFT = NULL_UI_Func;
	UI[InUI_CB_Choose].Func_Event_RIGHT = RIGHT_CB_Choose;
	UI[InUI_CB_Choose].Func_Event_DOWN = DOWN_CB_Choose;
	//		颜色框-前景色		//front 36*24 下键选中固定颜色0 左键选中示例图片/数字 右键选中前景色 
	UI[InUI_CB_Type_FT] = UI_CreateUI(SCREEN_WIDTH-36*4,1,&FONT[InFT_ColorBox],Render_CB_TEXT);
	UI[InUI_CB_Type_FT].parameter = 2;
	Other_StringCpy(UI[InUI_CB_Type_FT].value_text,"ft");
	//		颜色框-背景色		//back_ 36*24 下键选中固定颜色0	左键选中前景色 右键选中字体并换成字体选择UI
	UI[InUI_CB_Type_BK] = UI_CreateUI(SCREEN_WIDTH-36*3,1,&FONT[InFT_ColorBox],Render_CB_TEXT);
	UI[InUI_CB_Type_BK].parameter = 2;
	Other_StringCpy(UI[InUI_CB_Type_BK].value_text,"bk");
	//		颜色框-字体			//font_ 36*24 下键选中字体选择	左键选中背景色并换成颜色框UI
	UI[InUI_CB_Type_F_] = UI_CreateUI(SCREEN_WIDTH-36*2,1,&FONT[InFT_ColorBox],Render_CB_TEXT);
	UI[InUI_CB_Type_F_].parameter = 2;
	Other_StringCpy(UI[InUI_CB_Type_F_].value_text,"t_");
	//		退出颜色选择
	UI[InUI_CB_Type_Q_] = UI_CreateUI(SCREEN_WIDTH-36,1,&FONT[InFT_ColorBox],Render_CB_TEXT);
	UI[InUI_CB_Type_Q_].parameter = 2;
	Other_StringCpy(UI[InUI_CB_Type_Q_].value_text,"Q_");
	for(int i=InUI_CB_Type_FT;i<=InUI_CB_Type_Q_;i++)
	{
		UI[i].Func_Event_DOWN = DOWN_CB_Type;
		UI[i].Func_Event_LEFT = LEFT_CursorMove;
		UI[i].Func_Event_RIGHT = RIGHT_CursorMove;
	}
	UI[InUI_CB_Type_Q_].Func_Event_RIGHT = NULL_UI_Func;
	//		颜色框 	-固定颜色0	//外尺寸24*24 颜色16*16 颜色框2 上键返回前景色/背景色 左键UI_index-1 右键UI_index+1 下键确认
	UI[InUI_CB_ColorFix0] = UI_CreateUI(54,26,&FONT[InFT_CB_Block],Render_CB_ColorFix);
	//			 	-固定颜色1	//颜色值保存在value中
	UI[InUI_CB_ColorFix1] = UI_CreateUI(54+26*1,26,&FONT[InFT_CB_Block],Render_CB_ColorFix);
	//			 	-固定颜色2
	UI[InUI_CB_ColorFix2] = UI_CreateUI(54+26*2,26,&FONT[InFT_CB_Block],Render_CB_ColorFix);
	//			 	-固定颜色3
	UI[InUI_CB_ColorFix3] = UI_CreateUI(54+26*3,26,&FONT[InFT_CB_Block],Render_CB_ColorFix);
	//			 	-固定颜色4
	UI[InUI_CB_ColorFix4] = UI_CreateUI(54+26*4,26,&FONT[InFT_CB_Block],Render_CB_ColorFix);
	//			 	-固定颜色5	
	UI[InUI_CB_ColorFix5] = UI_CreateUI(54+26*5,26,&FONT[InFT_CB_Block],Render_CB_ColorFix);
	//			 	-固定颜色6	
	UI[InUI_CB_ColorFix6] = UI_CreateUI(54+26*6,26,&FONT[InFT_CB_Block],Render_CB_ColorFix);
	//			 	-固定颜色7	
	UI[InUI_CB_ColorFix7] = UI_CreateUI(54+26*7,26,&FONT[InFT_CB_Block],Render_CB_ColorFix);
	for(int i=0;i<8;i++)
	{
		UI[InUI_CB_ColorFix0+i].parameter = COLOR_FIXED[i];
	}
	//				切换色块组
	UI[InUI_CB_ColorChange] = UI_CreateUI(54+26*8,26,&FONT[InFT_ICON_1],Render_CB_Pic);
	for(int i=InUI_CB_ColorFix0;i<=InUI_CB_ColorChange;i++)
	{
		UI[i].Func_Event_LEFT  = LEFT_CursorMove;
		UI[i].Func_Event_RIGHT = RIGHT_CursorMove;
		UI[i].Func_Event_DOWN  = DOWN_CB_Color;
		UI[i].Func_Event_UP	   = UP_CB_Color;
	}
	UI[InUI_CB_ColorFix0].Func_Event_LEFT = NULL_UI_Func;
	UI[InUI_CB_ColorChange].Func_Event_RIGHT = NULL_UI_Func;
	UI[InUI_CB_ColorChange].Func_Event_DOWN = DOWN_CB_ColorChange;
	//				RGB888入口
	UI[InUI_CB_RGB] = UI_CreateUI(54+26*9,26,&FONT[InFT_CB_Block],Render_CB_ColorUS);
	UI[InUI_CB_RGB].Func_Event_LEFT = LEFT_CursorMove;
	UI[InUI_CB_RGB].Func_Event_DOWN = DOWN_CB_RGBEnter;
	UI[InUI_CB_RGB].Func_Event_UP = UP_CB_Color;
	//		InUI_CB_RGB_0x				//调色框第二行的RGB888输入框0x	28*24		仅是'0x'文本，不会被选中	
	UI[InUI_CB_RGB_0x] = UI_CreateUI(54,26,&FONT[InFT_ColorBox],Render_CB_TEXT);
	UI[InUI_CB_RGB_0x].parameter = 2;
	Other_StringCpy(UI[InUI_CB_RGB_0x].value_text,"0x");
	//		InUI_CB_RGB_5				//调色框第二行RGB888的最高位					左右更换RGB888输入位置 上下更换数值		
	UI[InUI_CB_RGB_5] = UI_CreateUI(68+16*1,26,&FONT[InFT_ColorBox],Render_CB_RGB);
	//		InUI_CB_RGB_4		
	UI[InUI_CB_RGB_4] = UI_CreateUI(68+16*2,26,&FONT[InFT_ColorBox],Render_CB_RGB);
	//		InUI_CB_RGB_3		
	UI[InUI_CB_RGB_3] = UI_CreateUI(68+16*3,26,&FONT[InFT_ColorBox],Render_CB_RGB);
	//		InUI_CB_RGB_2		
	UI[InUI_CB_RGB_2] = UI_CreateUI(68+16*4,26,&FONT[InFT_ColorBox],Render_CB_RGB);
	//		InUI_CB_RGB_1		
	UI[InUI_CB_RGB_1] = UI_CreateUI(68+16*5,26,&FONT[InFT_ColorBox],Render_CB_RGB);
	//		InUI_CB_RGB_0		
	UI[InUI_CB_RGB_0] = UI_CreateUI(68+16*6,26,&FONT[InFT_ColorBox],Render_CB_RGB);
	//		InUI_CB_RGB_True			//RGB888 确认
	UI[InUI_CB_RGB_True] = UI_CreateUI(68+16*6+24,26,&FONT[InFT_CB_Block],Render_CB_ColorFix);
	UI[InUI_CB_RGB_True].parameter = TFT_RGB888To565(0x00bc12);
	//		InUI_CB_RGB_False			//RGB888 取消
	UI[InUI_CB_RGB_False] = UI_CreateUI(68+16*6+50,26,&FONT[InFT_CB_Block],Render_CB_ColorFix);
	UI[InUI_CB_RGB_False].parameter = TFT_RGB888To565(0xc93756);
	//	 	InUI_CB_Line2_BK			//调色框第二行背景
	UI[InUI_CB_Line2_BK] = UI_CreateUI(52,26,&FONT[InFT_ColorBox],Render_CB_Line2_BK);
	UI[InUI_CB_Line2_BK].parameter = 24;
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
	//		InUI_CB_FONTChange			//调色框第二行，更改字体			120*24		左右键更换字体，下键确认，上键返回
	UI[InUI_CB_FONTChange] = UI_CreateUI(74,26,&FONT[InFT_CB_FONT],Render_CB_TEXT);
	Other_StringCpy(UI[InUI_CB_FONTChange].value_text,"<  _PIXEL_LCD7_  >");
	UI[InUI_CB_FONTChange].parameter = 18;
	UI[InUI_CB_FONTChange].Func_Event_DOWN 	= DOWN_CB_FONTChange;
	UI[InUI_CB_FONTChange].Func_Event_UP 	= UP_CB_Color;
	UI[InUI_CB_FONTChange].Func_Event_RIGHT = RIGHT_CB_FONTChange;
	UI[InUI_CB_FONTChange].Func_Event_LEFT	= LEFT_CB_FONTChange;
	//		背景板
	UI[InUI_CB_Background] = UI_CreateUI(0,SCREEN_HEIGHT-51,&FONT[InFT_ColorBox],Render_CB_Background);
}
/**@brief  页面初始化
  */
void Init_Page_ColorBox(void)
{
	uint16_t indexs_ofPage0[] = {
		InUI_CB_Background
		,InUI_ColorBox		
		,InUI_CB_Sample			
		,InUI_CB_Choose			
		,InUI_CB_Font0			
		,InUI_CB_Font1			
		,InUI_CB_Font2			
		,InUI_CB_Font3			
		,InUI_CB_Type_FT			
		,InUI_CB_Type_BK			
		,InUI_CB_Type_F_			
		,InUI_CB_Type_Q_			
		,InUI_CB_ColorFix0		
		,InUI_CB_ColorFix1		
		,InUI_CB_ColorFix2		
		,InUI_CB_ColorFix3		
		,InUI_CB_ColorFix4		
		,InUI_CB_ColorFix5		
		,InUI_CB_ColorFix6		
		,InUI_CB_ColorFix7		
		,InUI_CB_ColorChange
	};
	UI_CreatePage(&PAGE[0],indexs_ofPage0,sizeof(indexs_ofPage0)/sizeof(uint16_t),&UI[InUI_CB_Choose]);
}








