#ifndef __UI_INS_TESTPAGE_H__
#define __UI_INS_TESTPAGE_H__


/*  字体		21~80*/
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



/*  UI  	51~100*/
#define		InUI_TP_Hour		51		//左右来回切换 上下调整数值
#define		InUI_TP_Minute		52
#define		InUI_TP_Second		53
#define		InUI_TP_MS			54		
#define		InUI_TP_t_HtM		55		//小时和分钟之间的冒号
#define		InUI_TP_t_MtS		56		//分钟和秒之间的冒号
#define		InUI_TP_t_StMS		57		//秒和毫秒之间的冒号



/*  PAGE  1 */
#define		InPG_TestPage		1


//接口
void Init_UI_TestPage(void);
void Init_Page_TestPage(void);
void TestPage_ClockCircle(void);

#endif
