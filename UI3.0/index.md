# 介绍

“-”   
 *2025/8/16-16:04.秦羽*

## 文件总览

### ***qy_ascii_font.h*** : 字库   
### ***qy_pic.h*** : 图库   
### ***TFT_UI.h/c*** : UI框架的主要组成和实现，不含显示函数和预制UI
```c
//可单独使用
/*  结构体  */
typedef struct tft_font{
    ...
}tft_font;//字体
typedef struct tft_obj{
    ...
}tft_obj;//页面上的元素
typedef struct tft_ui{
    ...
}tft_ui;//可以交互的UI
typedef struct tft_pointer{
    ...
}tft_pointer;//光标
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
```
### ***TFT_font.h/c*** : 简单的屏幕显示函数
```c
//依赖TFT_UI
/*  关于形状的部分  */
void TFTF_DrawRect(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint16_t color);
void TFTF_DrawFrame(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint16_t color,int8_t thick);
void TFTF_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color,int8_t thick);

/*  关于取模显示的部分  */
void TFTF_Single_01Pic(uint16_t x,uint16_t y,uint8_t pic01,uint8_t color_index);
void TFTF_Single_Char(uint16_t x,uint16_t y,char _char,uint8_t font,uint8_t color_index);

/*  关于文本/数字的部分  */
void TFTF_ShowNum(uint16_t x,uint16_t y,uint32_t num,uint8_t font,uint8_t color_index,int8_t digits);
void TFTF_ShowString(uint16_t x,uint16_t y,const char* text,uint8_t font,uint8_t color_index,int8_t NumOfChar);
```
### ***UI_Instance.h/c & UI_INS_Func.h/c*** : 一些定义好的预制UI
```c
//依赖TFT_UI和TFT_font   
//数字
tft_obj 	INS_Obj_Num(uint16_t x,uint16_t y,uint8_t digit,uint8_t color,uint8_t font_index,uint32_t value_num);
tft_ui 		INS_UI_Num_Normal(tft_obj* obj,int8_t front_ui,int8_t next_ui);
tft_ui 		INS_UI_Num_Digit(tft_obj* obj,uint8_t default_digit,int8_t front_ui,int8_t next_ui);
void 		INS_Status_Num_Normal(tft_obj* obj,uint16_t n_1,uint8_t color1,uint16_t n_2,uint8_t color2,uint16_t n_3,uint8_t color3);

//文本
tft_obj 	INS_Obj_Text(uint16_t x,uint16_t y,const char* text,uint8_t digit,uint8_t color,uint8_t font_index);
tft_ui 		INS_UI_Text(tft_obj* obj,int8_t front_ui,int8_t next_ui);
void 		INS_Status_Text(tft_obj* o,const char* words,uint16_t color,uint8_t font);

//图片
tft_obj INS_Obj_Pic(uint16_t x,uint16_t y,uint8_t color,uint8_t pic_index);
tft_ui INS_UI_Pic(tft_obj* o,int8_t front_ui,int8_t next_ui);
void INS_Status_Pic(tft_obj* o,uint8_t color,uint8_t pic_index);
```

## 移植

### ***屏幕显示***
需要实现TFT_font中的接口
```c
/*	填充方向:+------------>
 *			|			 x
 *			|	1  2  3
 *			|	|  |  |
 *			|	|  |  |
 *			|	V  V  V
 *			v y
 */
/**@brief  设定显示位置
  *@add    这里宽和高和X/Yend大概是Xend=width+x-1，Yend=height+y-1
  */
static void TFTF_SetRect(uint16_t x,uint16_t y,uint16_t width,uint16_t height)
{
	TFT_SetRect(x,y,width,height);
}
/**@brief  发送单个像素
  */
static void TFTF_Pixel(uint16_t rgb565)
{
	TFT_SendColor(rgb565);
}
```    
<hr>

### ***UI架构***
需要把`TFT_UI.h/c`中的`Init_UI`和`Circle_Render`接入程序中
```c
#include "TFT_UI.h"
int main(void)
{
    ...
    Screen_Init();  //屏幕初始化
    ...
    Init_UI();      //UI架构初始化
    ...
    while(1)
    {
        ...
        Circle_Render();        //渲染
        ...
    }

}
```    
<hr>

### ***字库/图库接入***
在`TFT_UI.h`中通过`#define InFT_ASC/PIC_Name_3216`指定字体下标
```c
//来自TFT_UI.h
//字体定义  Index_Font_ASCII/Picture_名称_高度宽度
#define InFT_ASC_PIXEL_2412		0
#define InFT_ASC_PIXEL_3216		1
#define InFT_PIC_Set_3232		2
#define	InFT_PIC_Cloud_3232		3
#define InFT_PIC_Github_3232	4
```      
之后在`TFT_UI.c`完成全局变量绑定
```c
//来自TFT_UI.c   
/**@brief  字体绑定，创建字体绑定到全局变量
  */
static void IF_UI_FontInit(void)
{
	FONT[InFT_ASC_PIXEL_2412]	= UI_CreateFont((const char*)font_ASCII_PIXEL_2412,24,12);
	FONT[InFT_ASC_PIXEL_3216] 	= UI_CreateFont((const char*)font_ASCII_PIXEL_3216,32,16);
	FONT[InFT_PIC_Set_3232] 	= UI_CreateFont((const char*)qyf_pic01_setting_3232,32,32);
	FONT[InFT_PIC_Github_3232] 	= UI_CreateFont((const char*)qyf_pic01_github_3232,32,32);
	FONT[InFT_PIC_Cloud_3232] 	= UI_CreateFont((const char*)qyf_pic01_cloud_3232,32,32);
}
```    
<hr>

### ***操作接口***
需要用具体硬件的 **按键键值** 或者 **串口调试的传入值** 进行操作绑定      
绑定`CURSOR.ui->Func_Event_UP/DOWN/LEFT/RIGHT`     
或者其他添加到`tft_ui(TFT_UI.h)`的函数    
用`switch`或者`if`判断进行操作    
```c
//用上位机键盘发送串口值实现调试的例子		
switch(buff)
{
case 'w':
	CURSOR.ui->Func_Event_UP(CURSOR.ui);
	break;
case 's':
	CURSOR.ui->Func_Event_DOWN(CURSOR.ui);
	break;
case 'a':
	CURSOR.ui->Func_Event_LEFT(CURSOR.ui);
	break;
case 'd':
	CURSOR.ui->Func_Event_RIGHT(CURSOR.ui);
	break;
case 'f':
    CURSOR.ui->Func_Event_F(CURSOR.ui);
	break;
}
```
<hr>

### ***测试***
来自`UI_Instance.h/c`中的`Page_TEST`是已经创建好的可交互测试界面    
内容不多  ~~(毕竟中午之前都还正在写....)~~    
将`Page_TEST`与页面全局变量绑定`(TFT_UI.c)`后即可显示
```c
//来自TFT_UI.c
static uint8_t IF_UI_PageInit(void)
{
	Page_In[InPG_Test_Ins] = Page_TEST;
	
	return InPG_Test_Ins;
}
```

## 具体使用

### ***页面创建（包含UI创建）***
```c
//来自TFT_UI.c
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

void Example_Render_aaa(tft_obj* o)；
void Example_LEFT_aaa(tft_ui* u)；
void Example_DOWN_aaa(tft_ui* u)；

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

//例程中绑定的显示/操作函数
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


//在UI_Instance.h/c中有快速创建 数字/字符/图片 的函数 INS_Obj_Num/Text/Pic(创建OBJ) INS_UI_Num/Text/Pic(创建UI)
//不过是今天(2025/8/16)刚写好的....等下整理下....
```    
<hr>    

### 预制UI
来自`UI_Instance.h/c`（需要`UI_INS_Func.h/c`支持）    
为了快速创建简单可用的一些显示和简单交互的内容     
在`UI_Instance.h/c`中提供了一些创建好的函数     
可以快速创建页面中的 *数字/字符/图片* 以及将要加入的 进度条/滑块/菜单 等等
```c
//来自UI_Instance.h
//数字
tft_obj 	INS_Obj_Num(uint16_t x,uint16_t y,uint8_t digit,uint8_t color,uint8_t font_index,uint32_t value_num);
tft_ui 		INS_UI_Num_Normal(tft_obj* obj,int8_t front_ui,int8_t next_ui);
tft_ui 		INS_UI_Num_Digit(tft_obj* obj,uint8_t default_digit,int8_t front_ui,int8_t next_ui);
void 		INS_Status_Num_Normal(tft_obj* obj,uint16_t n_1,uint8_t color1,uint16_t n_2,uint8_t color2,uint16_t n_3,uint8_t color3);

//文本
tft_obj 	INS_Obj_Text(uint16_t x,uint16_t y,const char* text,uint8_t digit,uint8_t color,uint8_t font_index);
tft_ui 		INS_UI_Text(tft_obj* obj,int8_t front_ui,int8_t next_ui);
void 		INS_Status_Text(tft_obj* o,const char* words,uint16_t color,uint8_t font);

//图片
tft_obj INS_Obj_Pic(uint16_t x,uint16_t y,uint8_t color,uint8_t pic_index);
tft_ui INS_UI_Pic(tft_obj* o,int8_t front_ui,int8_t next_ui);
void INS_Status_Pic(tft_obj* o,uint8_t color,uint8_t pic_index);
```

