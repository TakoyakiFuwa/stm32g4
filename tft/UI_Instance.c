#include "UI_Instance.h"
#include "TFT_font.h"		//提供tft_font结构体和(数字/字符/图片)显示函数
#include "TFT_UI.h"			//提供UI显示的具体架构
#include "UI_Index.h"		//提供光标的下标

//页面构成
#include "UI_Ins_ColorBox.h"	//调色框页面
#include "UI_Ins_TestPage.h"	//测试页面

/*	这个文件的很多内容相当于"搭积木"
 *	代码或者函数大多和屏幕上某个UI绑定，
 *	不需要后续使用
 *	所以就不写注释了
 *			——2025/8/6-17:50.秦羽
 */
/*	这些文件的关系是:
 *																	
 *	TFT_ILI9341.h/c——————————————(驱动)————————>TFT_font.h/c		 UI_Index.h
 *		|											|			 	  |
 *		|									(提供字体和数字显示) 	 (提供统一下标)
 *	(屏幕初始化)										|			      |
 *		|											v			      v
 *		|							UI_Ins_XXXX.h/c+UI_Ins_ColorBox.h/c+UI_Ins_TestPage.h/c<——(提供字库/图片库)——qy_ascii_font.h+qy_pic.h
 *		|									^								|
 *		|									|						(构建具体的UI和Page)
 *		|						  (提供UI/Page创建的工具)						|
 *		v									|								v
 *	  main.c<——(架构初始化/屏幕刷新接口)——TFT_UI.h/c<——(提供UI/Page初始化)——UI_Instance.h/c		
 *
 *	创建一个新页面的步骤大概是:
 *			创建新的 UI_Ins_XXXX.h/c 文件 ——> 在.h文件内确定好font/ui/page的下标作为#define ——>搬运extern UI_CURSOR/UI/FONT/font_xxx全局变量 —
 *			—> 在.c文件内通过 UI_CreateXX() 实例化具体font/ui ——> 实现Init_UI_XXXX()和Init_Page_XXXX()后添加到UI_Instance.c —
 *			—> 将 UI_Ins_XXXX.h 文件中的#define移动到UI_Index.h便于管理
 *	
 *			——2025/8/9-16:44.秦羽
 */

//来自TFT_UI.h
extern tft_pointer 	UI_CURSOR;				//光标
extern tft_font 	FONT[300];				//字体
extern tft_ui 		UI[100];				//UI
extern tft_page 	PAGE[20];				//页面

/*  字体，赋值给光标  */
extern const char font_ASCII_PIXEL_2412[][36];

/*  对外接口  */
/**@brief  所有UI实例化的接口
  *@param  void
  *@retval tft_font 给光标用的字体，主要是颜色
  *@add	   并不是UI初始化，不应该直接被调用
  *		   只是Init_UI架构的一部分
  *		   初始化应该用Init_UI();
  */
tft_font* INS_Init_UI(void)
{
	//	InFT_Cursor(0)	光标字体
	FONT[InFT_Cursor] = TFTF_CreateFont((const char*)font_ASCII_PIXEL_2412,24,12,TFTF_RGB888To565(0xfce38a),TFTF_RGB888To565(0x522546));
	//调色框UI的初始化
	Init_UI_ColorBox();
	//测试页面UI初始化
	Init_UI_TestPage();
	
	  
	//返回光标字体
	return &FONT[InFT_Cursor];
}
/**@brief  页面实例化的接口
  *@param  void
  *@retval tft_page 初始化完成后最先跳转的页面
  *@add	   同样不应该被直接调用
  */
tft_page* INS_Init_Page(void)
{
	//页面配置
	//	调色框页		UI_Ins_ColorBox
	Init_Page_ColorBox();
	//	测试页		UI_Ins_TestPage
	Init_Page_TestPage();
	
	
	//初始化完成后进入测试页
	return &PAGE[InPG_TestPage];
}


//测试接口
void INS_Test(void)
{
	Init_UI();
	while(1)
	{
		HAL_Delay(10);
		CircleRender_UI();
	
		
		TestPage_ClockCircle();
	}
}





