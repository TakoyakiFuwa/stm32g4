#ifndef __UI_INDEX_H__
#define __UI_INDEX_H__

/*	全局变量 (TFT_UI.c)
extern tft_pointer 	UI_CURSOR;				//光标
extern tft_font 	FONT[300];				//字体
extern tft_ui 		UI[200];				//UI
extern tft_page 	PAGE[20];				//页面
 */

/*  字体  */
	//空字体				0		TFT_UI.h/c
#define		InFT_Null			0
	//光标字体			1		UI_Instance.h/c
#define		InFT_Cursor			1
	//调色框用到的字体	2~20	UI_Ins_ColorBox.h/c
#define		InFT_ColorBox		2		//调色框
#define		InFT_CB_On			3		//选中的配色
#define		InFT_CB_Sample		4		//示例图片	
#define		InFT_CB_Block		5		//色块
#define		InFT_ICON_0			6		//图标-更换要调整的UI
#define		InFT_ICON_1			7		//图标-更换色块组
#define		InFT_CB_FONT		8		//字体示例
//#define	InFT_CB_End			20		//ColorBox字体最大数量
	//测试页面用到的字体	21~80	UI_Ins_TestPage.h/c
	/*
#define		InFT_TP_Hour0		21
#define		InFT_TP_Hour1		22
#define		InFT_TP_Hour2		23
#define		InFT_TP_Hour3		24
#define		InFT_TP_Minute0		25
#define		InFT_TP_Minute1		26
#define		InFT_TP_Minute2		27
#define		InFT_TP_Minute3		28
#define		InFT_TP_Second0		29
#define		InFT_TP_Second1		30
#define		InFT_TP_Second2		31
#define		InFT_TP_Second3		32
#define		InFT_TP_MS0			33
#define		InFT_TP_MS1			34
#define		InFT_TP_MS2			35
#define		InFT_TP_MS3			36
#define		InFT_TP_t_HtM		37		//小时和分钟之间的冒号
#define		InFT_TP_t_MtS		38		//分钟和秒之间的冒号
#define		InFT_TP_t_StMS		39		//秒和毫秒之间的冒号
	*/
//#define	InFT_TP_End			80		//TestPage字体最大数量



/*  UI  */
	//空UI				0		TFT_UI.h/c
#define		InUI_Null			0
	//调色框的组成内容	1~50	UI_Ins_ColorBox.h/c
#define 	InUI_ColorBox		1		//不会被光标选中					51*WIDTH 	是更改颜色的界面  
#define 	InUI_CB_Sample		2		//调色框中最左侧的样本图片  		51*51		是颜色示例
#define		InUI_CB_Choose		3
#define 	InUI_CB_Font0		4		//调色框第一行，0号字体			20*24		每个UI有四种字体
#define 	InUI_CB_Font1		5		//调色框第一行，1号字体
#define 	InUI_CB_Font2		6		//调色框第一行，2号字体
#define 	InUI_CB_Font3		7		//调色框第一行，3号字体
#define 	InUI_CB_Type_FT		8		//调色框第一行"FT" front前景色	60*24 		下键选中固定颜色0 左键选中示例图片/数字 右键选中背景色 
#define 	InUI_CB_Type_BK		9		//调色框第一行"BK" back背景色		60*24 		下键选中固定颜色0	左键选中前景色 右键选中字体并换成字体选择UI
#define 	InUI_CB_Type_F_		10		//调色框第一行"F_" font字体		60*24 		下键选中字体选择	左键选中背景色并换成颜色框UI
#define     InUI_CB_Type_Q_		11		//调色框第一行"Q_" 退出			60*24		下键退出并重新刷新当前页 左键返回F_
#define 	InUI_CB_ColorFix0	12		//调色框第二行固定颜色0~7			28*24 		上键返回前景色/背景色 左键UI_index-1 右键UI_index+1 下键确认
#define 	InUI_CB_ColorFix1	13				//颜色值保存在parameter中
#define 	InUI_CB_ColorFix2	14	
#define 	InUI_CB_ColorFix3	15	
#define 	InUI_CB_ColorFix4	16	
#define 	InUI_CB_ColorFix5	17		
#define 	InUI_CB_ColorFix6	18		
#define 	InUI_CB_ColorFix7	19		
#define     InUI_CB_ColorChange	20		//调色框第二行，更换第二行颜色选项	16*16		下键更换自身图标，隐藏/打开输入框入口，左右键函数发生变化
#define     InUI_CB_RGB			21		//调色框第二行，打开输入框			16*16		下键进入RGB888输入框	
#define		InUI_CB_RGB_0x		22		//调色框第二行的RGB888输入框0x	28*24		仅是'0x'文本，不会被选中
#define		InUI_CB_RGB_5		23		//调色框第二行RGB888的最高位		28*24		左右更换RGB888输入位置 上下更换数值
#define		InUI_CB_RGB_4		24
#define		InUI_CB_RGB_3		25
#define		InUI_CB_RGB_2		26
#define		InUI_CB_RGB_1		27
#define		InUI_CB_RGB_0		28
#define		InUI_CB_RGB_True	29		//RGB888 确认					28*24		下键重新刷新
#define		InUI_CB_RGB_False	30		//RGB888 取消
#define 	InUI_CB_Line2_BK	31		//调色框第二行背景
#define		InUI_CB_FONTChange	32		//调色框第二行，更改字体			120*24		左右键更换字体，下键确认，上键返回
//#define	InUI_CB_End			50		//分配给ColorBox页面的UI就到50
	//测试页面		51~100		UI_Ins_TestPage.h/c
	/*
#define		InUI_TP_Hour		51		//左右来回切换 上下调整数值
#define		InUI_TP_Minute		52
#define		InUI_TP_Second		53
#define		InUI_TP_MS			54		
#define		InUI_TP_t_HtM		55		//小时和分钟之间的冒号
#define		InUI_TP_t_MtS		56		//分钟和秒之间的冒号
#define		InUI_TP_t_StMS		57		//秒和毫秒之间的冒号
	*/
//#define 	InUI_TP_End			100		//分配给TestPage页面的UI就到100



/*  页面  */
#define		InPG_ColorBox		0
//#define		InPG_TestPage		1



#endif
