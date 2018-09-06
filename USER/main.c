#include "main.h"

u8 status = 0;

//START 任务
//设置任务优先级
#define START_TASK_PRIO      			10 //开始任务的优先级设置为最低?
//设置任务堆栈大小
#define START_STK_SIZE  				64*2
//任务堆栈	
OS_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *pdata);	
 			   
//LED0任务
//设置任务优先级
#define LED0_TASK_PRIO       			12
//设置任务堆栈大小
#define LED0_STK_SIZE  		    		64*2
//任务堆栈	
OS_STK LED0_TASK_STK[LED0_STK_SIZE];
//任务函数
void led0_task(void *pdata);


//LED1任务
//设置任务优先级
#define LED1_TASK_PRIO       			11
//设置任务堆栈大小
#define LED1_STK_SIZE  					64*2
//任务堆栈
OS_STK LED1_TASK_STK[LED1_STK_SIZE];
//任务函数
void led1_task(void *pdata);

//GUI任务
//设置任务优先级
#define GUI_TASK_PRIO       			9
//设置任务堆栈大小
#define GUI_STK_SIZE  		    		64*2
//任务堆栈	
OS_STK GUI_TASK_STK[GUI_STK_SIZE];
//任务函数
void gui_task(void *pdata);

//TaskBar任务
//设置任务优先级
#define BAR_TASK_PRIO       			4
//设置任务堆栈大小
#define BAR_STK_SIZE  		    		64*2
//任务堆栈	
OS_STK BAR_TASK_STK[BAR_STK_SIZE];
//任务函数
void taskbar_task(void *pdata);

const u8 cnt = 0xAA;

int main(void)
{	
	u8 res;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	delay_init();	    //延时函数初始化	  
	uart_init(115200);	 	//串口初始化为115200
 	LED_Init();		  			//初始化与LED连接的硬件接口
	KEY_Init();					//初始化按键
	RTC_Init();
	LCD_Init();			   		//初始化LCD     
	W25QXX_Init();				//初始化W25Q128
 	my_mem_init(SRAMIN);		//初始化内部内存池
	exfuns_init();				//为fatfs相关变量申请内存  
	//RTC_Set(2018, 9, 14, 16, 53, 00);
	//W25QXX_Erase_Chip();
 	//res = f_mount(fs[0],"0:",1); 		//挂载SD卡 
	//while (res)
	//	res = f_mount(fs[0],"0:",1); 		//挂载SD卡 
	res = f_mount(fs[1],"1:",0); 		//挂载FLASH
	printf("FLASH res:%d\r\n", res);
	printf("count:0x%X\r\n", (int)&cnt);
	if (res)
	{
		printf("Formating FLASH...\r\n");
		res = f_mkfs("1:", 0, 512);
		if (res)
			printf("Format ERR\r\n");
		else
			printf("Format OK\r\n");
	}
	res = font_init();
	if (res)
	{
		update_font(0, 0, 16, "0:");
	}
	printf("Inited\r\n");
	//Show_Str(0, 200, 240, 320, "UCOS测试", 12, 0);
	//POINT_COLOR = BLACK;
	//Show_Str(0, 12, 240, 320, "UCOS测试", 16, 0);
	//mf_scan_files("0:");
	mf_scan_files("1:");
	//res = 0;
	//mf_showfree(&res);
	res = 1;
	mf_showfree(&res);
	OSInit();   
 	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//创建起始任务
	OSStart();	  	 
}
	  
//开始任务
void start_task(void *pdata)
{
    OS_CPU_SR cpu_sr=0;
	
	pdata = pdata; 
  OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)    
 	OSTaskCreate(led0_task,(void *)0,(OS_STK*)&LED0_TASK_STK[LED0_STK_SIZE-1],LED0_TASK_PRIO);
 	OSTaskCreate(led1_task,(void *)0,(OS_STK*)&LED1_TASK_STK[LED1_STK_SIZE-1],LED1_TASK_PRIO);
	OSTaskCreate(gui_task,(void *)0,(OS_STK*)&GUI_TASK_STK[GUI_STK_SIZE-1],GUI_TASK_PRIO);
	GUI_Init();
	OSTaskCreate(taskbar_task,(void *)0,(OS_STK*)&BAR_TASK_STK[BAR_STK_SIZE-1],BAR_TASK_PRIO);
	OSTaskCreate(TP_task,(void *)0,(OS_STK*)&TP_TASK_STK[TP_STK_SIZE-1],TP_TASK_PRIO);
	
	if (TP_Init())
		printf("TP_Adjust OK\r\n");
	else
		printf("TP_Adjusted\r\n");
	
	LED2 = 1;
	
	OSTaskSuspend(START_TASK_PRIO);	//挂起起始任务.
	OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
	
}

//在\n后面加上空格(屈服。。)
#define STR_INDEX "\
Hello，大家好！\n 这个展示板是柳铁一中创客汇的展示项目！\n \
创客汇是一个创客的嘉年华\n \
无论你是大神还是小白\n \
都可以在这里吸取营养\n \
野蛮生长\n \
成为你想成为的\n \
让我们思维的火花在这里迸射\n \
点燃整个世界。\n \
我们的地址：科技楼一楼创客工作室"

#define STR_JOIN_IN "\
你可以选择网上报名或者来社团提交报名表。\n \
报名表在社团门口自由取用。\n \
报名网站：http://www.ltyzckh.xyz/join_in"

#define STR_CONTACT "\
官方网站：http://www.ltyzckh.xyz\n \
\
社长QQ：1301816134\n \
研究部长QQ：1352040930\n \
如果发现了bug，欢迎向我们报告。"

#define STR_SOURCE "\
GtHub：https://github.com/LanceLiang2018/ShowBoard/\n \
本工程全部开源，基于UC/OS的分时操作系统。\n \
欢迎fork/star~"

enum
{
	PAGE_INDEX = 0,
	PAGE_JOIN_IN,
	PAGE_CONTACT,
	PAGE_SOURCE
};

OBJ* list[128];
u8 last = 0;
u8 page = PAGE_INDEX;
u8 last_page = PAGE_JOIN_IN;

void Flow(void)
{
	u8 tmp[128];
	u8 t = 0;
	u16 x = 240;
	u16 dx = 24;
	u16 i;
	//FInd all shown
	for (i=0; i<last; i++)
		if (list[i]->show == 1)
			tmp[t++] = i;
	
	for (i=0; i<t; i++)
		list[tmp[t]]->x += 240;
		
	while (x > 0)
	{
		for (i=0; i<t; i++)
		{
			GUI_MoveTo(list[tmp[i]], list[tmp[i]]->x-dx, list[tmp[i]]->y);
			//GUI_Redraw(list[tmp[i]]);
			GUI_Draw_One(list[tmp[i]]);
		}
		//20 fps??
		x -= dx;
		delay_ms(10);
	}
	//for (i=0; i<last; i++)
	//	GUI_Redraw(list[i]);
}


void Flush(void)
{
	u8 i;
	for (i=0; i<last; i++)
		GUI_Show(list[i], 0);	
	switch(page)
	{
		case PAGE_INDEX:
		{
			GUI_Show(list[1], 1);
			GUI_Show(list[2], 1);
			GUI_Show(list[3], 1);
			GUI_Show(list[4], 1);
			break;
		}
		case PAGE_JOIN_IN:
		{
			GUI_Show(list[0], 1);
			GUI_Show(list[5], 1);
			break;
		}
		case PAGE_CONTACT:
		{
			GUI_Show(list[0], 1);
			GUI_Show(list[6], 1);
			break;
		}
		case PAGE_SOURCE:
		{
			GUI_Show(list[0], 1);
			GUI_Show(list[7], 1);
			break;
		}
		default:
			break;
	}
	if (last_page != page)
	{
		last_page = page;
		//Flow();
		for (i=0; i<last; i++)
			GUI_Redraw(list[i]);
	}
}

void bReturnClick(void)
{
	page = PAGE_INDEX;
	Flush();
}
void bJoinInClick(void)
{
	page = PAGE_JOIN_IN;
	Flush();
}
void bContactClick(void)
{
	page = PAGE_CONTACT;
	Flush();
}
void bSourceClick(void)
{
	page = PAGE_SOURCE;
	Flush();
}




//LED0任务
void led0_task(void *pdata)
{
	OBJ* bReturn = GUI_Create(TYPE_BUTT, 64*0, 32*9, 64*4, 32, "返回", bReturnClick);
	OBJ* bJoinIn = GUI_Create(TYPE_BUTT, 80*0, 32*9, 80, 32, "加入我们", bJoinInClick);
	OBJ* bContact = GUI_Create(TYPE_BUTT, 80*1, 32*9, 80, 32, "联系方式", bContactClick);
	OBJ* bSource = GUI_Create(TYPE_BUTT, 80*2, 32*9, 80, 32, "查看源码", bSourceClick);
	OBJ* tIndex = GUI_Create(TYPE_TEXT, 0, 16, 240, 320-32*1-16, STR_INDEX, NULL);
	OBJ* tJoinIn = GUI_Create(TYPE_TEXT, 0, 16, 240, 320-32*1-16, STR_JOIN_IN, NULL);
	OBJ* tContact = GUI_Create(TYPE_TEXT, 0, 16, 240, 320-32*1-16, STR_CONTACT, NULL);
	OBJ* tSource = GUI_Create(TYPE_TEXT, 0, 16, 240, 320-32*1-16, STR_SOURCE, NULL);
	
	list[last++] = bReturn;		//0
	list[last++] = bJoinIn;		//1
	list[last++] = bContact;	//2
	list[last++] = bSource;		//3
	list[last++] = tIndex;		//4
	list[last++] = tJoinIn;		//5
	list[last++] = tContact;	//6
	list[last++] = tSource;		//7
	Flush();
	while (1)
	{
		//LED0 = !LED0;
		Flush();
		delay_ms(1000);
	}	
}

void EnterSleepMode()
{
	LED2 = 0;
	LCD_DisplayOff();
	delay_ms(3000);
	PWR_EnterSTANDBYMode();
}

//LED1任务
void led1_task(void *pdata)
{
	u32 i = 0;
	while (1)
	{
		//delay_ms(1000 * 60);
		//delay_ms(1000 * 60);
		//delay_ms(1000 * 3);
		for (i=0; i<1000 * 60 / 60; i++)
		{
			if (status == 1)
			{
				status = 0;
				break;
			}
			if (i >= 1000 * 60 / 60 - 100)
				EnterSleepMode();
			delay_ms(60);
		}
	}
}
