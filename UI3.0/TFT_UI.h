#ifndef __TFT_UI_H__
#define __TFT_UI_H__
#include "stdint.h"

#define SCREEN_WIDTH 	320
#define SCREEN_HEIGHT 	240

//创建页面相关
/*
extern uint16_t		COLOR[];
extern tft_font 	FONT[];	
extern tft_obj		OBJ[];	
extern tft_ui		UI[];		
extern tft_pointer 	CURSOR;		
extern tft_ui*		(*Page_In[20])(void);
 */

//字体定义  Index_Font_ASCII/Picture_名称_高度宽度
#define InFT_ASC_PIXEL_2412		0
#define InFT_ASC_PIXEL_3216		1
#define InFT_PIC_Set_3232		2
#define	InFT_PIC_Cloud_3232		3
#define InFT_PIC_Github_3232	4

//颜色定义	Index_Color
#define		InCor_Black		0x0		//0x000000
#define		InCor_Blue		0x1		//0x3b2e7e	//藏蓝
#define		InCor_Red		0x2		//0x9d2933  //胭脂红	
#define		InCor_Yellow	0x3		//0xd9b611	//秋香色
#define		InCor_GREEN		0x4		//0x40de5a	//草绿
#define		InCor_Water		0x5		//0x88ada6	//水色
#define		InCor_Purple	0x6		//0x725e82	//乌色
#define		InCor_Pink		0x7		//0xf47983	//桃红
#define		InCor_Orange	0x8		//0xff8936	//橘黄
#define		InCor_Spring	0x9		//0x0eb83a	//葱青
#define		InCor_Sky		0xA		//0x70f3ff	//蔚蓝
#define		InCor_BigRed	0xB		//0xff2121 	//大红
#define		InCor_Gold		0xC		//0xeacd76	//金
#define		InCor_Dawn		0xD		//0x75664d	//黎明
#define		InCor_Moon		0xE		//0xd6ecf0	//月白
#define		InCor_White		0xF		//0xFFFFFF

//页面下标定义 Index_Page
//#define		InPG_Test1		0
//#define		InPG_Test2		1
#define		InPG_Test_Ins	2

//字体
typedef struct tft_font{
	const char* font_lib;			//字体库/图片库
	uint8_t 	height;			//单个字体/图片的高度
	uint8_t 	width;			//单个字体/图片的宽度
}tft_font;

//页面上的元素
typedef struct tft_obj{
	//基础属性
	uint16_t 	x;
	uint16_t 	y;
	int8_t 		is_present;			//是否显示 1可以显示 3正在渲染队列中 0不能显示
	int8_t		status;				//状态
	uint8_t		colors[4];
	uint8_t		fonts[4];
	//值
	uint32_t 	value_num;
	char 		value_text[10];	
	//所在的组
	int8_t		index_signal;	//1表示头 -1表示尾
	//渲染方式
	void (*Func_Render_N)(struct tft_obj* self);
}tft_obj;

//可以交互的UI
typedef struct tft_ui{
	tft_obj* obj;
	
	void (*Func_Event_UP)(struct tft_ui* self);		//上移
	void (*Func_Event_DOWN)(struct tft_ui* self);	//下移
	void (*Func_Event_LEFT)(struct tft_ui* self);	//左移
	void (*Func_Event_RIGHT)(struct tft_ui* self);	//右移
	void (*Func_Event_F)(struct tft_ui* self);		//确认	
}tft_ui;

//光标
typedef struct tft_pointer{
	tft_ui* 	ui;					//当前光标指向的UI
	uint8_t 	index_color;		//光标自身的颜色在Init_UI,光标初始化部分可以改
	uint8_t		temp_colors[4];		//光标暂时替换掉指向UI的颜色，参考UI_CursorChange()
	uint16_t 	parameter;			//参数，考虑提高扩展性用的
}tft_pointer;

/*  UI相关函数  */
void 		Init_UI(void);
void 		Circle_Render(void);
tft_obj 	UI_CreateOBJ(uint16_t x,uint16_t y,uint8_t index_font,uint8_t color,void (*Render)(struct tft_obj* u));
tft_ui 		UI_CreateUI(tft_obj* obj);
void 		UI_Render(tft_obj* obj);
void 		UI_RenderOBJs(uint8_t* index_obj,uint8_t num_obj);
void 		UI_CursorChange(tft_ui* ui);
void		UI_ChangePage(uint8_t page_index);

//其他功能函数
uint16_t 	Other_RGB888To565(uint32_t RGB888);
void 		Other_StringCpy(char* target,const char* _words);


#endif







