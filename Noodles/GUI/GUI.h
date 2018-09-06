#include "includes.h"
#include "lcd.h"
#include "TEXT.h"
#include "malloc.h"
#include "delay.h"
#include "rtc.h"
#include "led.h"

typedef struct
{
	u16 pcolor;		//字体颜色
	u16 bcolor;		//背景颜色
	u16 rcolor;		//边框颜色
}CLR;

//STATES:
#define GUI_OK		0x00
#define GUI_REDRAW	0x01
#define GUI_Redraw(x)	x->state = GUI_REDRAW
#define GUI_Show(x, y)	x->show = y

typedef enum
{
	TYPE_BAR = 0,		//(0)
	TYPE_TEXT,			//(1)
	TYPE_BUTT,			//(2)
}GUI_TYPE;

//#define GUI_TYPE_BAR	0x10	//任务条
//#define GUI_TYPE_TEXT	0x11	//文本
//#define GUI_TYPE_BUTT	0x12	//按钮

//BAR:
#define GUI_BAR_H	16		//任务条宽

typedef struct
{
	u16 id;			//ID号
	u8 type;		//格式
	u16 x;			//起始坐标
	u16 y;
	u16 ex;			//终止坐标
	u16 ey;
	u8 *text;		//文本
	CLR clr;		//颜色
	u8 state;		//当前状态
	u8 show;		//是否展示
	void (*OnClick)(void);
}OBJ;

extern const CLR g_clr[3];
extern OBJ *g_obj;
extern u16 g_top;
extern u16 g_id;
extern u8 status;

#define OBJ_MAX		64

void GUI_Init(void);
OBJ* GUI_Create(u8 type, u16 _x, u16 _y, u16 _ex, u16 _ey, u8 *_text, void (*OnClick)(void));
u8 GUI_Del(OBJ* _obj);
void GUI_Draw_One(OBJ *_obj);

void GUI_Change(OBJ *_obj, u8 *text);	//改变文本
void GUI_MoveTo(OBJ *_obj, u16 _x, u16 _y);	//改变OBJ坐标(左上角)
void GUI_Clicked(OBJ *obj);
u8 GUI_Click(u16 x, u16 y);
