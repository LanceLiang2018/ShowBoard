#include "includes.h"
#include "lcd.h"
#include "TEXT.h"
#include "malloc.h"
#include "delay.h"
#include "rtc.h"
#include "led.h"

typedef struct
{
	u16 pcolor;		//������ɫ
	u16 bcolor;		//������ɫ
	u16 rcolor;		//�߿���ɫ
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

//#define GUI_TYPE_BAR	0x10	//������
//#define GUI_TYPE_TEXT	0x11	//�ı�
//#define GUI_TYPE_BUTT	0x12	//��ť

//BAR:
#define GUI_BAR_H	16		//��������

typedef struct
{
	u16 id;			//ID��
	u8 type;		//��ʽ
	u16 x;			//��ʼ����
	u16 y;
	u16 ex;			//��ֹ����
	u16 ey;
	u8 *text;		//�ı�
	CLR clr;		//��ɫ
	u8 state;		//��ǰ״̬
	u8 show;		//�Ƿ�չʾ
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

void GUI_Change(OBJ *_obj, u8 *text);	//�ı��ı�
void GUI_MoveTo(OBJ *_obj, u16 _x, u16 _y);	//�ı�OBJ����(���Ͻ�)
void GUI_Clicked(OBJ *obj);
u8 GUI_Click(u16 x, u16 y);
