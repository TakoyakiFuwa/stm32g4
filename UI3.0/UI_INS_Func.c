#include "UI_INS_Func.h"
#include "TFT_UI.h"
#include "TFT_font.h"

/*	这里的函数都是随手写随手绑定随手扔掉的  
 *	功能写的比较死，复用性不高，而且乱七八糟
 *	像是我宿舍抽屉里的垃圾袋/胶布/螺丝刀/钉子
 *	有时间会考虑过来整理
 *			——2025/8/16-15:18.秦羽
 */


extern uint16_t		COLOR[];
extern tft_font 	FONT[];	
extern tft_obj		OBJ[];	
extern tft_ui		UI[];		
extern tft_pointer 	CURSOR;		


//数字部分
	//在num中 value_text的 	9->digit 8->next_ui 7->front_ui
	//						4/5->color3 3/2->color2 1/0->color1
//可以按位操作的数字不带有其他状态		5->选中标志位(0没选中，1选中没调整，2进入调整)	
//									6->当前操作的位（从右向左数） 4默认选中的位
//可以按位操作的数字
void INS_Render_Num_Digit(tft_obj* o)
{
	tft_font font = FONT[o->fonts[o->status]];
	
	if(o->value_text[5]==2)
	{//某一位渲染
		uint32_t value = o->value_num;
		uint8_t digit_cursor = o->value_text[9]-1-o->value_text[6];
		for(int i=o->value_text[9]-1;i>=0;i--)
		{
			if(i==digit_cursor)
			{
				TFTF_Single_Char(o->x+i*font.width,o->y,value%10+'0',o->fonts[o->status],o->colors[o->status]);
			}
			else
			{
				TFTF_Single_Char(o->x+i*font.width,o->y,value%10+'0',o->fonts[o->status],CURSOR.temp_colors[o->status]);
			}
			value/=10;
		}	
			
	}
	else
	{//整体渲染
		TFTF_ShowNum(o->x,o->y,o->value_num,o->fonts[o->status],o->colors[o->status],o->value_text[9]);
	}
}
void INS_Render_Num(tft_obj* o)
{
	TFTF_ShowNum(o->x,o->y,o->value_num,o->fonts[o->status],o->colors[o->status],o->value_text[9]);
}
void INS_Render_ColorNun(tft_obj* o)
{
	o->status = 0;
	for(int i=5;i>=1;i-=2)
	{	
		if(o->value_num >=  (o->value_text[i-1]<<8|o->value_text[i]))
		{
			o->status = i/2+1;
			break;
		}
	}
	INS_Render_Num(o);
}
void INS_LEFT_Normal(tft_ui* u)
{
	uint8_t index = u->obj->value_text[7];
	UI_CursorChange(&UI[index]);
}
void INS_RIGHT_Normal(tft_ui* u)
{
	uint8_t index = u->obj->value_text[8];
	UI_CursorChange(&UI[index]);
}
void INS_DOWN_Num(tft_ui* u)
{
	if(u->obj->value_num>0)
	{
		u->obj->value_num--;
	}
	UI_Render(u->obj);
}
void INS_UP_Num(tft_ui* u)
{
	u->obj->value_num++;
	UI_Render(u->obj);
}
void INS_LEFT_Num_Digit(tft_ui* u)
{
	tft_obj* o = u->obj;
	if(o->value_text[5]==2)
	{
		if(++o->value_text[6]>o->value_text[9]-1)//退出调整状态
		{
			o->value_text[5] = 0;
		}
		UI_Render(o);
	}
	else
	{//跳转到其他UI
		INS_LEFT_Normal(u);
	}
}
void INS_RIGHT_Num_Digit(tft_ui* u)
{
	tft_obj* o = u->obj;
	if(o->value_text[5]==2)
	{
		if(o->value_text[6]==0)//退出调整状态
		{
			o->value_text[5] = 0;
		}
		else
		{
			o->value_text[6]--;
		}
		UI_Render(o);
	}
	else
	{//跳转到其他UI
		INS_RIGHT_Normal(u);
	}
}
void INS_UP_Num_Digit(tft_ui* u)
{
	tft_obj* o = u->obj;
	if(o->value_text[5]==2)
	{//调整某一位数字
		uint32_t digit_count = 1;
		for(int i=0;i<o->value_text[6];i++)
		{
			digit_count*=10;
		}
		if(digit_count+o->value_num<994967295)
		{
			o->value_num+=digit_count;
		}
	}
	else
	{//进入调整状态
		o->value_text[5] = 2;
		o->value_text[6] = o->value_text[4];  
	}
	UI_Render(o);
}
void INS_DOWN_Num_Digit(tft_ui* u)
{
	tft_obj* o = u->obj;
	if(o->value_text[5]==2)
	{//调整某一位数字
		uint32_t digit_count = 1;
		for(int i=0;i<o->value_text[6];i++)
		{
			digit_count*=10;
		}
		if(o->value_num>=digit_count)
		{
			o->value_num-=digit_count;
		}
	}
	else
	{//进入调整状态
		o->value_text[5] = 2;
		o->value_text[6] = o->value_text[4];  
	}
	UI_Render(o);
}



//文本部分
// value_num 	右4位(16,0xF)表示digit (0x30)表示当前最多有多少状态
//				左16位表示前跳转(0xFF00 0000)和后跳转(0x00FF 0000)
void INS_Render_Text(tft_obj* o)
{
	char* words = o->value_text;
	uint8_t status = o->status;
	for(int i=0;i<9;i++)
	{
		if(status == 0)
		{
			words = &words[i];
			break;
		}
		if(words[i]=='\0')
		{
			status--;
		}
	}	
	TFTF_ShowString(o->x,o->y,words,o->fonts[o->status],o->colors[o->status],(o->value_num&0xF));
}
void INS_LEFT_Text(tft_ui* u)
{
	uint8_t index = (u->obj->value_num)>>24;
	UI_CursorChange(&UI[index]);
}
void INS_RIGHT_Text(tft_ui* u)
{
	uint8_t index = (u->obj->value_num)>>16;
	UI_CursorChange(&UI[index]);
}
/*
void INS_UP_Text_Plus(tft_ui* u)
{
	INS_UP_Text(u);
	switch(u->obj->status)
	{
	case 0:
		operate_text_status0();
		break;
	case 1:
		operate_text_status1();
		break;
	case 2:
		operate_text_status2();
		break;
	}
}
 */
void INS_UP_Text(tft_ui* u)
{
	uint8_t status_max = ((u->obj->value_num&0x30)>>4);
	if(u->obj->status<status_max)
	{
		u->obj->status++;
	}
	else
	{
		u->obj->status = 0;
	}
	UI_Render(u->obj);
}
void INS_DOWN_Text(tft_ui* u)
{
	if(u->obj->status>0)
	{
		u->obj->status--;
	}
	else
	{
		u->obj->status = ((u->obj->value_num&0x30)>>4);
	}
	UI_Render(u->obj);
}

//图片
//8->next_ui 7->front_ui 9最大字体
void INS_Render_Pic(tft_obj* obj)
{
	TFTF_Single_01Pic(obj->x,obj->y,obj->fonts[obj->status],obj->colors[obj->status]);
}
void INS_UP_Pic(tft_ui* u)
{
	if(u->obj->status<u->obj->value_text[9])
	{
		u->obj->status++;
	}		
	else
	{
		u->obj->status = 0;
	}
	UI_Render(u->obj);
}
void INS_DOWN_Pic(tft_ui* u)
{
	if(u->obj->status>0)
	{
		u->obj->status--;
	}
	else
	{
		u->obj->status = u->obj->value_text[9];
	}
	UI_Render(u->obj);
}


