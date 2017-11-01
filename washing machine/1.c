/*
 * @Author: Guoquan Wei 1940359148@qq.com 
 * @Date: 2017-10-16 16:09:25 
 * @Last Modified by: Guoquan Wei
 * @Last Modified time: 2017-10-16 21:20:43
 */
#include <reg52.h>
#define uchar unsigned char
#define uint unsigned int
#define comm 0
#define dat 1

uchar waterlevel = 2;
//水位要求，初始水位为高水位
uchar mode, status;
//mode 工作模式，标准经济等等    status 当前工作状态，进水出水等等
bit forwardtmp, reversetmp;
//存储正转反转的状态

uchar workflag = 0;

uchar modearray[16] = "标准经济单洗排水";
uchar statusarray[20] = "进水出水洗涤漂洗脱水";

sbit standardled = P0 ^ 2;
sbit economicled = P0 ^ 3;
sbit singlewashled = P0 ^ 4;
sbit drainmodeled = P0 ^ 5;
//工作模式指示灯

sbit cap = P0 ^ 6;
sbit waterlowswitch = P0 ^ 7;
sbit watermidswitch = P1 ^ 0;
sbit waterhighswitch = P1 ^ 1;
// 机器盖子，和三个水位开关

sbit waterlow = P1 ^ 2;
sbit watermid = P1 ^ 3;
sbit waterhigh = P1 ^ 4;
// 三个水位指示灯

sbit washled = P1 ^ 5;
sbit rinsingled = P1 ^ 6;
sbit drainled = P1 ^ 7;
// 洗涤 漂洗 脱水指示灯

sbit waterin = P2 ^ 0;
sbit waterout = P2 ^ 1;
//进水 出水指示灯

sbit modechose = P2 ^ 2;
sbit levelchose = P2 ^ 3;
sbit switch1 = P2 ^ 4;

sbit forward = P2 ^ 5;
sbit reverse = P2 ^ 6;

//---------引脚定义-------------------------------------------------------------------------------------------------
sbit sid = P0 ^ 1;
sbit sclk = P0 ^ 0;

//-------------------------------------------
void delay_LCD(unsigned int t)
{
	while (t--)
		;
}
//-------------------------------------------
void wr_lcd(unsigned char dat_comm, unsigned char content)
{
	unsigned char i, j;
	sclk = 0;
	sid = 1;
	for (i = 0; i < 5; i++)
	{
		sclk = 1;
		sclk = 0;
	}
	sid = 0;
	sclk = 1;
	sclk = 0;
	if (dat_comm)
		sid = 1; //data
	else
		sid = 0; //command
	sclk = 1;
	sclk = 0;
	sid = 0;
	sclk = 1;
	sclk = 0;
	for (j = 0; j < 2; j++)
	{
		for (i = 0; i < 4; i++)
		{
			content = content << 1;
			sid = CY;
			sclk = 1;
			sclk = 0;
		}
		sid = 0;
		for (i = 0; i < 4; i++)
		{
			sclk = 1;
			sclk = 0;
		}
	}
}
//-------------------------------------------
void init_lcd(void)
{
	wr_lcd(comm, 0x30); /*30---基本指令动作*/
	wr_lcd(comm, 0x01); /*清屏，地址指针指向00H*/
	delay_LCD(100);
	wr_lcd(comm, 0x06); /*光标的移动方向*/
	wr_lcd(comm, 0x0c); /*开显示，关游标*/
	wr_lcd(comm, 0x02);
	wr_lcd(comm, 0x14);
}
/*---------------显示汉字或字符----------------*/
void ch_disp_any(unsigned char line, unsigned char row, unsigned char num, unsigned char *chn)
{
	unsigned char i;
	unsigned char j[5] = {0, 0x80, 0x90, 0x88, 0x98};
	wr_lcd(comm, 0x30);
	wr_lcd(comm, j[line] + row - 1);
	for (i = 0; i < num; i++)
		wr_lcd(dat, chn[i]);
}

//----毫秒延时--------------------------
void delayms(unsigned int t)
{
	unsigned char i;
	while (t--)
	{
		i = 123;
		while (i--)
			;
	}
}

void delaymotorms(int x)
{
	int i, j;
	for (i = 0; i < x; i++)
		for (j = 0; j < 110; j++)
		{
			if (workflag == 1)
			{
				if (switch1 == 0)
				{
					delayms(10);
					if (switch1 == 0)
					{
						while (switch1 == 0)
						{
							;
						}
						workflag = 2;
						ch_disp_any(4, 6, 6, "暂停  ");
						forwardtmp = forward;
						reversetmp = reverse;
						forward = reverse = 0;
						while (1)
						{
							if (switch1 == 0)
							{
								delayms(10);
								if (switch1 == 0)
								{
									while (switch1 == 0)
									{
										;
									}
									workflag = 1;
									ch_disp_any(4, 6, 6, "启动  ");
									forward = forwardtmp;
									reverse = reversetmp;
									break;
								}
							}
						}
					}
				}
				if (cap == 1)
				{
					delayms(10);
					if (cap == 1)
					{
						workflag = 2;
						ch_disp_any(4, 6, 6, "暂停  ");
						forwardtmp = forward;
						reversetmp = reverse;
						forward = reverse = 0;
					}
					while (1)
					{
						if (cap == 0)
						{
							delayms(10);
							if (cap == 0)
							{
								workflag = 1;
								ch_disp_any(4, 6, 6, "启动  ");
								forward = forwardtmp;
								reverse = reversetmp;
								break;
							}
						}
					}
				}
			}
		}
}

void waterinmachine()
{
	status = 0;
	waterin = 0;
	ch_disp_any(3, 6, 4, statusarray + status * 4);
	while (~waterin)
	{
		switch (waterlevel)
		{
		case 0:
			if (waterlowswitch == 0 && watermidswitch == 1 && waterhighswitch == 1)
			{
				waterin = 1;
			}
			break;

		case 1:
			if (waterlowswitch == 0 && watermidswitch == 0 && waterhighswitch == 1)
			{
				waterin = 1;
			}
			break;

		case 2:
			if (waterlowswitch == 0 && watermidswitch == 0 && waterhighswitch == 0)
			{
				waterin = 1;
			}
			break;

		default:
			break;
		}
	}
}

void wateroutmachine()
{
	status = 1;
	waterout = 0;
	ch_disp_any(3, 6, 4, statusarray + status * 4);
	delayms(2000);
	waterout = 1;
	ch_disp_any(3, 6, 4, "    ");
}

void forwardact(char during)
{
	forward = 1;
	reverse = 0;
	delaymotorms(during * 1000);
	forward = 0;
	reverse = 0;
}

void reverseact(char during)
{
	forward = 0;
	reverse = 1;
	delaymotorms(during * 1000);
	forward = 0;
	reverse = 0;
}

void washing()
{
	status = 2;
	ch_disp_any(3, 6, 4, statusarray + status * 4);
	washled = 0;
	forwardact(2);
	delayms(500);
	reverseact(2);
	delayms(500);
	washled = 1;
}

void rinsing()
{
	status = 3;
	ch_disp_any(3, 6, 4, statusarray + status * 4);
	rinsingled = 0;
	forwardact(2);
	delayms(500);
	reverseact(2);
	delayms(500);
	rinsingled = 1;
}

void dehydration()
{
	status = 4;
	ch_disp_any(3, 6, 4, statusarray + status * 4);
	drainled = 0;
	waterout = 0;
	forwardact(4);
	waterout = 1;
	drainled = 1;
	ch_disp_any(3, 6, 4, "    ");
}

void main()
{
	init_lcd();
	ch_disp_any(1, 2, 12, "全自动洗衣机");
	ch_disp_any(2, 1, 9, "洗衣方式:");
	ch_disp_any(3, 1, 9, "洗衣状态:");
	ch_disp_any(4, 1, 9, "运行状态:");
	standardled = 0;
	waterhigh = 0;
	ch_disp_any(2, 6, 4, modearray + 4 * mode);
	while (1)
	{
		if (modechose == 0 && workflag == 0)
		{
			delayms(10);
			if (modechose == 0)
			{
				while (~modechose)
				{
					;
				}
				mode++;
				if (mode > 3)
				{
					mode = 0;
				}
				ch_disp_any(2, 6, 4, modearray + 4 * mode);
				standardled = economicled = singlewashled = drainmodeled = 1;
				switch (mode)
				{
				case 0:
					standardled = 0;
					break;

				case 1:
					economicled = 0;
					break;

				case 2:
					singlewashled = 0;
					break;

				case 3:
					drainmodeled = 0;
					break;

				default:
					break;
				}
			}
		}
		if (levelchose == 0 && workflag == 0)
		{
			delayms(10);
			if (levelchose == 0)
			{
				while (~levelchose)
				{
					;
				}
				waterlevel++;
				if (waterlevel > 2)
				{
					waterlevel = 0;
				}
				waterhigh = watermid = waterlow = 1;
				switch (waterlevel)
				{
				case 0:
					waterlow = 0;
					break;

				case 1:
					watermid = 0;
					break;

				case 2:
					waterhigh = 0;
					break;

				default:
					break;
				}
			}
		}
		if (cap == 1 && switch1 == 0)
		{
			delayms(10);
			if (switch1 == 0)
			{
				ch_disp_any(4, 6, 6, "关机盖");
			}
		}
		else if (cap == 0 && switch1 == 0)
		{
			delayms(10);
			if (switch1 == 0)
			{
				while (switch1 == 0)
				{
					;
				}
				workflag = 1;
				ch_disp_any(4, 6, 6, "启动  ");
				switch (mode)
				{
				case 0:
					waterinmachine();
					washing();
					washing();
					wateroutmachine();
					waterinmachine();
					rinsing();
					wateroutmachine();
					waterinmachine();
					washing();
					wateroutmachine();
					dehydration();
					workflag = 0;
					break;

				case 1:
					waterinmachine();
					washing();
					wateroutmachine();
					waterinmachine();
					rinsing();
					wateroutmachine();
					dehydration();
					workflag = 0;
					break;

				case 2:
					waterinmachine();
					washing();
					washing();
					wateroutmachine();
					workflag = 0;
					break;

				case 3:
					dehydration();
					workflag = 0;
					break;

				default:
					break;
				}
				ch_disp_any(4, 6, 6, "停止  ");
			}
		}
	}
}
