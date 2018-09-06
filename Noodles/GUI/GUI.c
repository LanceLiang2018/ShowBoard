#include "GUI.h"
#include "touch.h"

OBJ *g_obj = NULL;
u16 g_top = 0;
u16 g_id = 0;
const CLR g_clr[3] = 			//目前只有3种type
{													//字体，背景，边框
	{WHITE,	BLACK,	BLACK},	//BAR(0)
	{BLACK,	WHITE,	GREEN},	//TEXT(1)
	{BLACK,	LGRAY,	BLACK},	//BUTT(2)
};

void GUI_Init(void)
{
	//OS_CPU_SR cpu_sr=0;
	g_obj = (OBJ*)mymalloc(SRAMIN, OBJ_MAX*sizeof(OBJ));	//给Obj分配内存(暂时不用free了)
	//OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)
	
	//OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
	//GUI_Create(0, 0, 128, 32, "OBJ0", clr);
}

OBJ* GUI_Create(
	u8 type,
	u16 _x, u16 _y, 
	u16 _ex, u16 _ey, 
	u8 *_text,
	void (*OnClick)(void))
{
	OS_CPU_SR cpu_sr=0;
	OS_ENTER_CRITICAL();
	if (g_top+1 >= OBJ_MAX)
		return NULL;
	
	g_obj[g_top].type = type;
	g_obj[g_top].x = _x;
	//if (type != TYPE_BAR)
	//	_y += GUI_BAR_H;		//坐标偏移
	//else
	//					;		//坐标不偏移
	//画图时偏移
	g_obj[g_top].y = _y;
	g_obj[g_top].ex = _ex + _x;
	g_obj[g_top].ey = _ey + _y;
	g_obj[g_top].text = _text;
	//g_obj[g_top].clr = _clr;	//删除_clr，用type指定
	g_obj[g_top].id = g_id;
	g_obj[g_top].state = GUI_REDRAW;
	g_obj[g_top].show = 0;
	
	g_obj[g_top].clr = g_clr[type];
	g_obj[g_top].OnClick = OnClick;
	
	g_top++;				//注意这里栈顶指向[g_top-1]
	g_id++;
	
	OS_EXIT_CRITICAL();
	return &g_obj[g_top-1];
}
/*
	OS_CPU_SR cpu_sr=0;
	OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)
	OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
*/
u8 GUI_Del(OBJ* _obj)
{
	u16 i;
	OS_CPU_SR cpu_sr=0;
	for (i=0; i<g_top; i++)
	{
		if (_obj->id == g_obj[i].id)	//用ID确认
		{
			OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)
			LCD_Fill(g_obj[i].x, g_obj[i].y, g_obj[i].ex, g_obj[i].ey-1, g_obj[i].clr.bcolor);
			OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
			g_obj[i] = g_obj[g_top-1];
			g_top--;
			return 0;			//找到了立即返回
		}
	}
	return 1;			//找不到
}

void GUI_Change(OBJ *_obj, u8 *_text)		//改变文本
{
	_obj->text = _text;
}

void GUI_MoveTo(OBJ *_obj, u16 _x, u16 _y)	//改变OBJ坐标(左上角)
{
	OS_CPU_SR cpu_sr=0;
	OS_ENTER_CRITICAL();
	//if (_obj->type == TYPE_BAR)
		//LCD_Fill(_obj->x, _obj->y, _obj->ex+1, _obj->ey+1, _obj->clr.bcolor);
	//else
		LCD_Fill(_obj->x, _obj->y, _obj->ex+1, _obj->ey, _obj->clr.bcolor);
	OS_EXIT_CRITICAL();
	_obj->ex += _x - _obj->x;
	_obj->ey += _y - _obj->y;
	_obj->x = _x;
	_obj->y = _y;
}

u16 fps = 0;
void gui_task(void *pdata)
{
	u16 i;
	
	while (1)
	{
		for (i=0; i<g_top; i++)
		{
			if (g_obj[i].state == GUI_REDRAW)
			{
				GUI_Draw_One(&g_obj[i]);
				g_obj[i].state = GUI_OK;
			}
				
		}
		fps++;
		delay_ms(50);
	}
}

void GUI_Draw_One(OBJ *_obj)
{
	u16 len = 0;
	OS_CPU_SR cpu_sr=0;
	//LCD_Clear(WHITE);					//太慢了！
	OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)
	if (_obj->show == 1)
	{
		LCD_Fill(_obj->x, _obj->y, _obj->ex, _obj->ey-1, _obj->clr.bcolor);
		POINT_COLOR = _obj->clr.rcolor;
		LCD_DrawRectangle(_obj->x, _obj->y, _obj->ex, _obj->ey-1);

		POINT_COLOR = _obj->clr.pcolor;
		BACK_COLOR = _obj->clr.bcolor;
		switch (_obj->type)
		{
			case TYPE_BUTT:
				status = 1;
				len = strlen((const char*)_obj->text);
				Show_Str((_obj->x+_obj->ex)/2-len*16/2/2, 
						//_obj->y+16/2, 
						(_obj->y+_obj->ey)/2-16/2, 
						_obj->ex, 
						_obj->ey, 
						_obj->text, 16, 1);
				break;
			case TYPE_BAR:
			case TYPE_TEXT:
				Show_Str(_obj->x, _obj->y, _obj->ex, _obj->ey, _obj->text, 16, 1);
				break;
			default:
				break;
		}
	}
	OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
}

void GUI_Redraw_One(OBJ *_obj)
{
	if (_obj->show == 1)
	{
		LCD_Fill(_obj->x, _obj->y, _obj->ex, _obj->ey, _obj->clr.bcolor);
		POINT_COLOR = _obj->clr.rcolor;
		LCD_DrawRectangle(_obj->x, _obj->y, _obj->ex, _obj->ey);
		POINT_COLOR = _obj->clr.pcolor;
		Show_Str(_obj->x+1, _obj->y+1, _obj->ex, _obj->ey, _obj->text, 16, 1);
	}
}

void GUI_Clicked(OBJ *obj)
{
	u16 tmp;
	tmp = obj->clr.bcolor;
	obj->clr.bcolor = obj->clr.pcolor;
	obj->clr.pcolor = tmp;
	obj->state = GUI_REDRAW;
}

u8 GUI_Click(u16 x, u16 y)
{
	u16 i;
	for (i=0; i<g_top; i++)
	{
		if (g_obj[i].show == 0)
			continue;
		if (g_obj[i].x<x&&g_obj[i].ex>x&&g_obj[i].y<y&&g_obj[i].ey>y)
		{
			if (g_obj[i].type == TYPE_BUTT || g_obj[i].type == TYPE_BAR)
			{
				GUI_Clicked(&g_obj[i]);
				if (g_obj[i].type == TYPE_BUTT)
					g_obj[i].OnClick();
			}
			return i+1;
		}
	}
	return 0;
}

void taskbar_task(void *pdata)
{
	OBJ *bar;
	u8 *Buff;
	
	Buff = (u8*)mymalloc(SRAMIN, 32*sizeof(u8));
	strcpy((char*)Buff, "12:00");
	//bar = GUI_Create(TYPE_BAR, 240-9*8, 0, 9*8, 16, Buff);
	bar = GUI_Create(TYPE_BAR, 0, 0, 240, 16, Buff, NULL);
	GUI_Show(bar, 1);
	myfree(SRAMIN, Buff);
	//bar = GUI_Create(0, 0, 20, 20, Buff, clr);
	
	while (1)
	{
		printf("FPS:%d\r\n", fps);
		fps = 0;
		RTC_Get();
		//printf("%d:%d\r\n", calendar.hour, calendar.min);
		sprintf((char*)Buff, "                    %%%02d %02d:%02d" , my_mem_perused(SRAMIN), calendar.hour, calendar.min);
		GUI_Show(bar, 1);
		GUI_Redraw(bar);			//激发更新
		GUI_Change(bar, Buff);
		delay_ms(1000);
	}
}	
