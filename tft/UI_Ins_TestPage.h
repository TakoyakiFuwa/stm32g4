#ifndef __UI_INS_TESTPAGE_H__
#define __UI_INS_TESTPAGE_H__


/*  字体		21~80*/
#define 	InFT_Timer_Start 	21		//计时器字体下标起点
#define 	InFT_Timer_End 		45		//计时器字体下标终点
#define		InFT_Clock_Start	46		//时钟字体下标起点
#define		InFT_Clock_End		53		//时钟字体下标终点
#define		InFT_TP_Icon_Start	60		//Github图标
#define		InFT_TP_Num			70		//一直刷新的数字
#define		InFT_TP_Char_Start	71		//一直刷新的字符


/*  UI  	51~100*/
	//作为秒表组
#define		InUI_TP_Hour		51		//左右来回切换 上下调整数值
#define		InUI_TP_Minute		52
#define		InUI_TP_Second		53
#define		InUI_TP_MS			54		
#define		InUI_TP_t_HtM		55		//小时和分钟之间的冒号
#define		InUI_TP_t_MtS		56		//分钟和秒之间的冒号
#define		InUI_TP_t_StMS		57		//秒和毫秒之间的冒号
#define		InUI_TP_Timer_F		58		//秒表外边框				外圈3内圈2
#define		InUI_TP_TR_Start	59		//秒表启动/关闭 侧边使用一个图标
#define		InUI_TP_TR_Reset	60		//秒表复位
#define		InUI_TP_TR_Set		61		//设置秒表数值(给颜色调试一个接口)
	//作为时钟组
#define		InUI_TP_ClockH		62		//左右来回切换 上下调整数值
#define		InUI_TP_ClockMin	63
#define		InUI_TP_ClockS		64
#define		InUI_TP_ClockMS		65		
#define		InUI_TP_ClockSign	66		//三个冒号做成一个UI
#define		InUI_TP_Clock_F		67		//时钟外边框
#define		InUI_TP_SetClock	68		//时钟按键 "Set clock time."
	//图标
#define		InUI_TP_ICON		69
	//一直刷新的数字				
#define		InUI_TP_Num			70
	//一直刷新的字符
#define		InUI_TP_Char		71		//字符本身
#define		InUI_TP_C_Icon		72		//停止/开始图标
#define		InUI_TP_C_Botton	73		//开始/停止按键


/*  PAGE  1 */
#define		InPG_TestPage		1


//接口
void Init_UI_TestPage(void);
void Init_Page_TestPage(void);
void TestPage_ClockCircle(void);

#endif
