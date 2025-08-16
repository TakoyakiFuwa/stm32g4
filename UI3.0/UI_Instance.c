#include "UI_Instance.h"
#include "TFT_UI.h"
#include "TFT_Font.h"
#include "UI_INS_Func.h"

/*	在TFT_UI.h/c框架上简单写了一些预制UI
 *	用函数简单调用下就能有，而且有绑定好了光标操作函数
 *	但是功能是写死的，而且效率不确定
 *	建议还是自己用TFT_UI.h/c的函数自己创建UI
 *				——2025/8/16-15:20.秦羽
 */

extern uint16_t		COLOR[];
extern tft_font 	FONT[];	
extern tft_obj		OBJ[];	
extern tft_ui		UI[];		
extern tft_pointer 	CURSOR;		


/*  以下是文本接口  */
// 文本的value_num 	右4位(16,0xF)表示digit 再3位(0x30)表示当前最多有多少状态
//					左16位表示前跳转(0xFF00 0000)的UI下标和后跳转(0x00FF 0000)的UI下标
/**@brief  创建一个用于显示的文本
  *@param  x,y 		坐标(如果底层是tft_font.h/c，左上角)
  *@param  text		要显示的文本，字符数低于char[10]
  *@param  digit	字符位数
  *@param  color	颜色下标，例如InCor_Black<<4|InCor_White白底黑字
  *@param  font_index	字体下标，例如InFT_ASC_PIXEL_3216,建议定义在TFT_UI.h
  *@retval tft_obj	建议用全局变量OBJ[]来接收
  */
tft_obj INS_Obj_Text(uint16_t x,uint16_t y,const char* text,uint8_t digit,uint8_t color,uint8_t font_index)
{
	tft_obj o = UI_CreateOBJ(x,y,font_index,color,INS_Render_Text);
	int i=0;
	for(int i=0;i<=9;i++)
	{
		o.value_text[i] = '\0';
	}
	for(;text[i]!='\0';i++)
	{
		if(i==10)
		{
			break;
		}
		o.value_text[i] = text[i];
	}
	if(i!=10)
	{
		o.value_text[i]='\0';
	}
	if(digit>=10)//文本位数
	{
		digit=10;
	}
	o.value_num |= digit;
	//最多状态数
	o.value_num &= 0xFFFFFFCF; 
	
	return o;
}
/**@brief  为一个字符串OBJ绑定默认UI
  *@param  obj 需要提前用INS_OBJ_Text(...)创建OBJ(全局变量)绑定在这里
  *@param  front_ui/next_ui		调用ui[].Func_Event_Left/Right时光标跳转的UI的下标，建议绑定定义好的InUI_XXXX
  *@retval tft_ui 建议用全局变量UI[]来接受
  *@add	   默认绑定左右键是更换光标指向，上下键是更换状态
  *		   状态可通过INS_Status_Text来绑定更多状态(字符/颜色/字体)
  */
tft_ui INS_UI_Text(tft_obj* obj,int8_t front_ui,int8_t next_ui)
{
	tft_ui ui = UI_CreateUI(obj);
	//文本前后跳转 前0xFF00 0000  后0x00FF 0000
	uint32_t index = front_ui<<24;
	obj->value_num |= index;
	index = next_ui<<16;
	obj->value_num |= index;
	//操作绑定
	ui.Func_Event_LEFT = INS_LEFT_Text;
	ui.Func_Event_RIGHT = INS_RIGHT_Text;
	ui.Func_Event_DOWN = INS_DOWN_Text;
	ui.Func_Event_UP = INS_UP_Text;
	
	return ui;
}
/**@brief  为一个字符串OBJ添加更多状态
  *@param  o		需要用INS_OBJ_Text(...)创建OBJ(全局变量)绑定在这里
  *@param  words	新增的状态文本，可能只能容纳两三个字符
  *@param  color	新增的状态颜色
  *@param  font		新增的状态字体，建议字体尺寸相同
  *@retval void
  *@add	   添加状态后，可以使用OBJ.status = 0~3 来更换状态
  */
void INS_Status_Text(tft_obj* o,const char* words,uint16_t color,uint8_t font)
{
	//读出当前状态数量
	uint8_t status_max = ((o->value_num&0x30)>>4);
	if(status_max<3)
	{
		status_max++;
	}
	//写回当前数量状态
	o->value_num &= 0xFFFFFFCF; 
	o->value_num |= status_max<<4;
	//字体/颜色更改
	o->fonts[status_max] = font;
	o->colors[status_max] = color;
	//添加文本
	int i=0;
	for(;i<9;i++)
	{//找到最后一个\0的位置
		if(o->value_text[i]=='\0')
		{
			status_max--;
			if(status_max==0)
			{
				break;
			}
		}
	}
	i++;
	for(int j=0;words[j]!='\0';j++)
	{//在后面添加文本
		if(i==10)
		{
			break;
		}
		o->value_text[i++] = words[j];
	}
	if(++i<=9)
	{//添加\0
		o->value_text[i]='\0';
	}
	//更改渲染方式
	o->Func_Render_N = INS_Render_Text;
}


/*  以下是数字接口  */
	//在num中 value_text的 	9->digit 8->next_ui 7->front_ui
	//						4/5->color3 3/2->color2 1/0->color1
//可以按位操作的数字不带有其他状态		5->选中标志位(0没选中，1选中没调整，2进入调整)	
//									6->当前操作的位（从右向左数） 4默认选中的位
//可以按位操作的数字
/**@brief  创建一个最简单的显示数字
  *@param  x,y 		坐标(左上角)
  *@param  digit	位数
  *@param  color	颜色，例如白底黑字 InCor_Black<<4|InCor_White
  *@param  font_index	字体下标，例如InFT_ASC_PIXEL_3216，建议定义在TFT_UI.h中
  *@param  value_num	初始值
  *@retval tft_obj	返回创建好的显示数字
  */
tft_obj INS_Obj_Num(uint16_t x,uint16_t y,uint8_t digit,uint8_t color,uint8_t font_index,uint32_t value_num)
{
	tft_obj obj = UI_CreateOBJ(x,y,font_index,color,INS_Render_Num);
	obj.value_num = value_num;
	obj.value_text[9] = digit;
	
	return obj;
}
/**@brief  可以交互的数字
  *@param  obj		需要用INS_Obj_Num创建绑定全局变量OBJ[]
  *@param  front_ui/next_ui		通过UI.Func_Event_Left/Right进行光标左右跳转的UI下标
  *@retval tft_ui 	创建好的ui
  *@add	   默认左右操作是更换光标绑定，上下操作是调整数值
  */
tft_ui INS_UI_Num_Normal(tft_obj* obj,int8_t front_ui,int8_t next_ui)
{
	tft_ui ui;
	ui = UI_CreateUI(obj);
	ui.obj->value_text[7] = front_ui;
	ui.obj->value_text[8] = next_ui;
	ui.Func_Event_DOWN 	= INS_DOWN_Num;
	ui.Func_Event_UP	= INS_UP_Num;
	ui.Func_Event_LEFT  = INS_LEFT_Normal;
	ui.Func_Event_RIGHT = INS_RIGHT_Normal;
	
	return ui;
}
/**@brief  可以按位操作的数字
  *@param  obj		需要用INS_Obj_Num创建绑定全局变量OBJ[]
  *@param  default_digit	进行位调整时的光标位	
  *@param  front_ui/next_ui	通过UI.Func_Event_Left/Right进行光标左右跳转的UI下标
  *@retval tft_ui	创建好的UI
  *@add	   左右操作是更换UI,上下操作进入位调整
  *		   进入位调整后，左右是更换要调整的位
  */
tft_ui INS_UI_Num_Digit(tft_obj* obj,uint8_t default_digit,int8_t front_ui,int8_t next_ui)
{
	tft_ui ui = INS_UI_Num_Normal(obj,front_ui,next_ui);
	//按位操作的特殊渲染方式
	ui.obj->Func_Render_N = INS_Render_Num_Digit;
	ui.Func_Event_DOWN 	= INS_DOWN_Num_Digit;
	ui.Func_Event_UP	= INS_UP_Num_Digit;
	ui.Func_Event_LEFT  = INS_LEFT_Num_Digit;
	ui.Func_Event_RIGHT = INS_RIGHT_Num_Digit;
	//标志位处理 6->当前选中的位
	//			5->选中标志位 1没有调整 2进入调整
	//			4->当前选中的位_默认位置
	obj->value_text[6] = default_digit;
	obj->value_text[4] = default_digit;
	obj->value_text[5] = 1;
	
	return ui;
}
/**@brief  数字颜色与数字值相关联
  *@param  obj			需要用INS_Obj_Num创建绑定全局变量OBJ[]
  *@param  n_1/2/3 		不同颜色范围
  *@param  color1/2/3	不同状态的颜色
  *@retval void
  *@add	   obj.value_num<n_1时显示INS_Obj_Num的参数颜色，n_1<obj.value_num<n_2时显示color1
  *		   n_2<obj.value_num<n_3显示color2，obj.value_num>n_3时显示color3
  */
void INS_Status_Num_Normal(tft_obj* obj,uint16_t n_1,uint8_t color1,uint16_t n_2,uint8_t color2,uint16_t n_3,uint8_t color3)
{
	obj->Func_Render_N = INS_Render_ColorNun;
	if(n_1==0)
	{
		return;
	}
	obj->value_text[0] = n_1>>8;
	obj->value_text[1] = n_1&0xFF;
	obj->colors[1] = color1;
	if(n_2==0)
	{
		return;
	}
	obj->value_text[2] = n_2>>8;
	obj->value_text[3] = n_2&0xFF;
	obj->colors[2] = color2;
	if(n_3==0)
	{
		return;
	}
	obj->value_text[4] = n_3>>8;
	obj->value_text[5] = n_3&0xFF;
	obj->colors[3] = color3;
}


/*  以下是图片接口  */
/**@brief  创建一个仅显示的图片
  *@param  x/y		位置
  *@param  color	颜色，例如白底黑字InCor_Black<<4|InCor_White
  *@param  pic_index	图片，InFT_PIC_XXXX建议定义在TFT_UI.h中
  */
tft_obj INS_Obj_Pic(uint16_t x,uint16_t y,uint8_t color,uint8_t pic_index)
{
	tft_obj o = UI_CreateOBJ(x,y,pic_index,color,INS_Render_Pic);
	o.value_text[9] = 0;
	return o;
}
/**@brief  为图片绑定调整和状态变换
  *@param  o	需要用INS_Obj_Pic创建绑定全局变量OBJ[]
  *@param  front_ui/next_ui	通过UI.Func_Event_Left/Right进行光标左右跳转的UI下标
  *@retval tft_ui	创建好的UI
  *@add	   左右键进行光标跳转，上下键可以更换用INS_Status_Pic绑定的其他状态(图片)
  */
tft_ui INS_UI_Pic(tft_obj* o,int8_t front_ui,int8_t next_ui)
{
	tft_ui u = INS_UI_Num_Normal(o,front_ui,next_ui);
	u.Func_Event_UP = INS_UP_Pic;
	u.Func_Event_DOWN = INS_DOWN_Pic;
	
	return u;
}
/**@brief  绑定更多图片和颜色
  *@param  o		需要用INS_Obj_Pic创建绑定全局变量OBJ[]
  *@param  color	添加新的颜色
  *@param  pic_index 图片，InFT_PIC_XXX建议定义在TFT_UI.h中
  *@retval void
  *@add	   添加新图片后，就算没有绑定UI，也可用UI_INS_Func.h中的INS_UP_Pic/INS_DOWN_Pic来更换状态/图片
  */
void INS_Status_Pic(tft_obj* o,uint8_t color,uint8_t pic_index)
{
	if(o->value_text[9]+1<=3)
	{
		o->value_text[9]++;
	}
	uint8_t index = o->value_text[9];
	o->colors[index] = color;
	o->fonts[index] = pic_index;
	
}


//测试内容
//就是一个页面，
//可以用"Page_In[xxx] = Ins_TEST_Page;return xxx;"移植下做测试
tft_ui* Page_TEST(void)
{
	//这里写的很不方便，
	//应该在.h文件中#define InOBJ_XXX 0
	//然后用OBJ[InOBJ_XXX]和UI[InUI_XXX]
	//直接用OBJ[0]会在后期调整时带来很大麻烦，而且意义不明
	//不过现在只是在测试.....
	OBJ[0] = INS_Obj_Num(30,30,3,InCor_Dawn<<4|InCor_Moon,InFT_ASC_PIXEL_3216,32);
	INS_Status_Num_Normal(&OBJ[0],200,InCor_BigRed<<4|InCor_Blue,400,InCor_Black<<4|InCor_Gold,600,InCor_Orange<<4|InCor_Black);
	UI[0] = INS_UI_Num_Normal(&OBJ[0],4,1);
	OBJ[1] = INS_Obj_Num(30+16*4,30,3,InCor_Dawn<<4|InCor_Moon,InFT_ASC_PIXEL_3216,0);
	UI[1] = INS_UI_Num_Normal(&OBJ[1],0,2);
	OBJ[2] = INS_Obj_Num(30+16*8,30,5,InCor_Black<<4|InCor_Gold,InFT_ASC_PIXEL_3216,2312);
	UI[2] = INS_UI_Num_Digit(&OBJ[2],2,1,3);
	
	OBJ[3] = INS_Obj_Text(30,80,"Hello",8,0x2C,InFT_ASC_PIXEL_3216);
	UI[3] = INS_UI_Text(&OBJ[3],2,4);
	
	OBJ[4] = INS_Obj_Num(30,120,9,0x1D,InFT_ASC_PIXEL_2412,892384);
	UI[4] = INS_UI_Num_Digit(&OBJ[4],7,3,5);
	
	OBJ[5] = INS_Obj_Text(30,180,"ON",3,InCor_Black<<4|InCor_GREEN,InFT_ASC_PIXEL_3216);
	INS_Status_Text(&OBJ[5],"OFF",InCor_Moon<<4|InCor_Red,InFT_ASC_PIXEL_3216);
	UI[5] = INS_UI_Text(&OBJ[5],4,6);
	
	
	OBJ[6] = INS_Obj_Text(100,180,"A",3,InCor_Black<<4|InCor_GREEN,InFT_ASC_PIXEL_3216);
	INS_Status_Text(&OBJ[6],"B",InCor_Moon<<4|InCor_Red,InFT_ASC_PIXEL_3216);
	INS_Status_Text(&OBJ[6],"C",InCor_Moon<<4|InCor_Red,InFT_ASC_PIXEL_3216);
	INS_Status_Text(&OBJ[6],"txt",InCor_Moon<<4|InCor_Red,InFT_ASC_PIXEL_3216);
	UI[6] = INS_UI_Text(&OBJ[6],5,7);
	
	OBJ[7] = INS_Obj_Pic(180,90,0xF3,InFT_PIC_Github_3232);
	INS_Status_Pic(&OBJ[7],0xA4,InFT_PIC_Cloud_3232);
	INS_Status_Pic(&OBJ[7],0x7D,InFT_PIC_Set_3232);
	UI[7] = INS_UI_Pic(&OBJ[7],6,0);
	
	
	
	//页面初始化
	uint8_t nums[] = {0,1,2,3,4,5,6,7};
	UI_RenderOBJs(nums,sizeof(nums)/sizeof(uint8_t));
	return &UI[5];
}












