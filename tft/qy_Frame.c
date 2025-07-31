#include "qy_Frame.h"
#include "qy_Graph.h"
#include "qy_printf.h"

/*	打算通过链表实现ID查询
 *  然后额外一个链表实现实时刷新的内容
 *	显示的背景色取决于父级的背景色
 */
typedef enum tftf_type{
	tftf_type_text,
	tftf_type_num_var,
	tftf_type_pic,
	page
}tftf_type;
//typedef enum tftf_Xposition{
//	tftf_Xposition_center,
//	tftf_Xposition_left,
//	tftf_Xposition_right,
//	tftf_Xposition_rect  //根据矩形数据
//}tftf_Xposition;
//typedef enum tftf_Yposition{
//	tftf_Yposition_center,
//	tftf_Yposition_up,
//	tftf_Yposition_down,
//	tftf_Yposition_rect  //根据矩形数据
//}tftf_Yposition;
typedef struct tftf_obj{
	uint16_t id;		//设置id
	int8_t priority;//优先级
	tftg_rect obj_rect;		//物体位置
	tftg_rect text_rect;	//文本位置(相对obj)
	uint16_t color;
	struct tftf_obj* next;//尾指针
}tftf_obj;

tftf_obj oh;//object head

/**@brief  相关链表初始化
  *@param  void
  */
extern uint16_t D_TFT_WIDTH;
extern uint16_t D_TFT_HEIGHT;
void Init_TFTF(void)
{
	//创建表头(屏幕整体背景)
	oh.color = 0xE6D9;
	oh.id = 0;
	oh.next = &oh;
//	oh.rect = TFTG_CreateRect(0,0,D_TFT_WIDTH,D_TFT_HEIGHT);
}
//根据id获取obj
tftf_obj TFTF_FindID(uint16_t id)
{
	tftf_obj obj;
	return obj;
}
//打印obj信息
void TFTF_PrintfInfor(tftf_obj obj)
{
	
}
//按照优先级大小渲染全部内容
void TFTF_RenderAll(void)
{
	
}











