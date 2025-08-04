#ifndef __TFT_UI_H__
#define __TFT_UI_H__
#include "stdint.h"
#include "TFT_font.h"

/*	(还是对链表念念不忘)
 *	变化数据的渲染打算用队列来实现
 *	通过UI_TryRender(tftu_ui* ui)来添加到渲染队列
 *//*这里真正实现的函数就是UI_AddRender*/

//page，屏幕的当前位置
//更改页面时重新刷新屏幕，并更改UI的显示属性
typedef struct tftu_page{
	uint16_t ui_index[100];		//该页面绑定的UI下标，999为不绑定，详见UI_CreatePage
	struct tftu_ui* start_ui;	//光标换页时，在这页第一个关联的UI
}tftu_page;
//UI，页面(page)的基础组件
typedef struct tftu_ui{
	//基础属性
	uint16_t 	x;
	uint16_t 	y;
	tftf_font* 	font;
	int8_t 		is_present;			//是否显示
	int8_t		readyto_present;	//已经在显示队列中,避免重复添加
	char		notes[20];			//备注
	
	//值(value)，可扩展
	uint32_t 	value_num;
	char 		value_text[20];	
	
	//考虑代码复用性的参数，可能会删
	uint16_t 	parameter;	//一个纯粹的参数，考虑提高按键交互函数的代码复用性
							//比如->在显示数字中，这个参数可作为数字的位数
							//		例如，有两个显示数字的UI要实现
							//		一个UI显示两位数，一个UI显示三位数
							//		那么这两个的UI渲染函数中可能会用到void TFTF_ShowNum(x,y,num,font,digits)
							//		其中，digits(位数) 就需要分别填2和3
							//		此时，如果有parameter，可以写成TFTF_ShowNum(x,y,num,font,ui->parameter);
							//应该会有更多的应用场景，也可能没有，所以考虑删除
	
	//绑定的函数方法
	void (*Func_Render_N)(struct tftu_ui* self);	//N表示不应该直接被调用
	//原因:	有关屏幕的时序要求较高，
	//		一旦被中断打断容易出现UI显示错误（软件上的屏幕坏点）
	//		所以要谨慎直接使用渲染函数
	//		本架构中只会出现在 void CircleRender_UI(void) （渲染队列）和 void UI_ChangePage(void)（页面切换中）
	//		
	//平时想要调用渲染函数可以用 void UI_AddRender(tftu_ui* u) 将要渲染的UI添加到渲染队列中
	
	//以下是用户交互函数，应当绑定具体按键（按键监听处理）
	//注意，	扩展时应当在UI_CreateUI中添加 = NULL_UI_Func
	//		原因参考NULL_UI_Func
	void (*Func_Event_UP)(struct tftu_ui* self);	//上移
	void (*Func_Event_DOWN)(struct tftu_ui* self);	//下移
	void (*Func_Event_LEFT)(struct tftu_ui* self);	//左移
	void (*Func_Event_RIGHT)(struct tftu_ui* self);	//右移
	void (*Func_Event_F)(struct tftu_ui* self);		//确认	(更换颜色)
	void (*Func_Event_Q)(struct tftu_ui* self);		//退出
}tftu_ui;
//光标，结构体不复用，仅实例化一个光标
typedef struct tftu_pointer{
	tftu_page*	ptr_page;		//光标所在的页
	tftu_ui* 	ptr_ui;			//光标所在的UI
	tftf_font* 	cursor_font;	//光标的字体
	tftf_font* 	temp_font;		//暂时保存的字体，光标离开时返回到UI
	uint16_t 	index_ofUI;		//光标所在UI的下标 同tftu_ui中的parameter属性，考虑删
}tftu_pointer;
//

void 	Init_UI(void);
void 	CircleRender_UI(void);
void 	UI_AddRender(tftu_ui* u);
tftu_ui	UI_CreateUI(uint16_t x,uint16_t y,tftf_font* font,void (*Func_Render)(struct tftu_ui* self));
void 	UI_Cursor_ChangeUI(tftu_ui* new_ui);
void 	UI_Cursor_Refresh(void);
void 	UI_CreatePage(tftu_page* page,uint16_t* ui_index,uint16_t number_ofUI,tftu_ui* start_ui);
void	UI_ChangePage(tftu_page* new_page);


//测试接口
void TFTU_Test(void);

#endif







