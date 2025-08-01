#include "TFT_text.h"
#include "TFT_font.h"

/*	打算在原有的TFT_font（对单个字符/图片进行取模显示）库上
 *	进行进一步写来显示 字符串/数字/窗口 的库
 *				——2025/8/
 */


/*  字库  */
//#include "qy_font.h"
extern const char qyf_EN_PIXEL_2412[][36];
extern const char qyf_EN_GOTHIC_2412[][36];
extern uint16_t QYF_COLOR_BLUE   ;
extern uint16_t QYF_COLOR_RED    ;
extern uint16_t QYF_COLOR_YELLOW ;
extern uint16_t QYF_COLOR_GREEN  ;

//数字字体
typedef struct tftt_num_font{
	const char* font_lib;	//字体库
	uint8_t 	size_height;		//字体库的高度
	uint16_t 	ft_color;	//前景色 (front-color字体色)
	uint16_t	bk_color;	//背景色 (back-color背景色)
}tftt_num_font;
//ascii码
typedef struct tftt_ascii_font{
	const char* font_lib;	//字体库
	uint8_t 	size_height;		//字体库的高度
	uint16_t 	ft_color;	//前景色 (front-color字体色)
	uint16_t	bk_color;	//背景色 (back-color背景色)
}tftt_ascii_font;








void TFTT_Test(void)
{
//	TFTF_Rect(0,0,300,300,0xad55);
//	TFTF_ENChar(10,20,24,qyf_EN_GOTHIC_2412['5'-' '],QYF_COLOR_BLUE,QYF_COLOR_RED);
}


