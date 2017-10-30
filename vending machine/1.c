/*
 * @Author: Guoquan Wei 1940359148@qq.com 
 * @Date: 2017-10-15 18:51:30 
 * @Last Modified by:   Guoquan Wei 
 * @Last Modified time: 2017-10-15 18:51:30 
 */

#include <reg52.h>
#include <string.h>
#define comm 0
#define dat 1
#define uchar unsigned char
#define uint unsigned int
sbit sid = P3 ^ 0; 
sbit sclk = P3 ^ 7;
#define KEY P2

#define drinkkey P0
#define drinkled P1

sbit drawbackled = P3 ^ 2;
sbit getled = P3 ^ 3;

float price[8] = {3.0, 2.0, 1.5, 5.0, 3.0, 3.5, 5.5, 1.0};
uchar pricearray[3];
//用来存放价格转化成的字符串
uchar drinknamearray[32] = "七喜可乐牛奶锐澳芬达雪碧酸奶白水";
uchar drinkcode;
uchar moneykey;
float paymoney, change;
uchar num;

uchar drinkkeytmp;
bit selectedflag, moneyflag;

// 按下的饮料按键


void sprintf1(char *p,float num)
{
	int a,b;
	a=(int)(num*10.0)/10+48;
	b=(int)(num*10.0)%10+48;
	*p=a;
	*(p+1)='.';
	*(p+2)=b;
}

void inittimer()
{
	TMOD = 0x21;
	TH0 = (65536 - 45872) / 256;
	TL0 = (65536 - 45872) % 256;
	EA = 1;
	ET0 = 1;
	TR0 = 1;
}

//-------------------------------------------
void delay_key(unsigned int t)
{
	while (t--)
		;
}
//-------------------------------------------
unsigned char Keyboard(void)
{
	unsigned char x, y, z = 0, jz;
	KEY = 0xf0;		 //低四位置低，即行接低
	if (KEY != 0xf0) //是否有按键按下
	{
		delay_key(10);   //去抖
		if (KEY != 0xf0) //确实有按键按下
		{
			x = KEY;	//读取按键所在的行
			KEY = 0x0f; //高四位置低，即列接低
			y = KEY;	//读取按键所在的列
			z = x | y;  //按键的坐标为行+列
			while (KEY != 0x0f)
				; //等待按键抬起
		}
	}
	switch (z)
	{
	case 0xee:
	{
		jz = 0;
		break;
	} //按键值显示为0
	case 0xde:
	{
		jz = 1;
		break;
	} //按键值显示为1
	case 0xbe:
	{
		jz = 2;
		break;
	}
	case 0x7e:
	{
		jz = 3;
		break;
	}

	case 0xed:
	{
		jz = 4;
		break;
	}
	case 0xdd:
	{
		jz = 5;
		break;
	}
	case 0xbd:
	{
		jz = 6;
		break;
	}
	case 0x7d:
	{
		jz = 7;
		break;
	}

	case 0xeb:
	{
		jz = 8;
		break;
	}
	case 0xdb:
	{
		jz = 9;
		break;
	}
	case 0xbb:
	{
		jz = 10;
		break;
	}
	case 0x7b:
	{
		jz = 11;
		break;
	}

	case 0xe7:
	{
		jz = 12;
		break;
	}
	case 0xd7:
	{
		jz = 13;
		break;
	}
	case 0xb7:
	{
		jz = 14;
		break;
	}
	case 0x77:
	{
		jz = 15;
		break;
	}
	default:
	{
		jz = 16;
		break;
	} // 没有按键按下，返回16
	}
	return jz;
}

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
void delay_ms(unsigned int t)
{
	unsigned char i;
	while (t--)
	{
		i = 123;
		while (i--)
			;
	}
}

void speech_delay(unsigned int t)
{
	unsigned char i;
	while (t--)
	{
		i=123;
		while (i--)
		{
			;
		}
	}
}

void init_voice()
{
	TL1=0xfa;
	TH1=0xfa;
	TMOD=0x20;
	SCON=0x50;
	PCON=0x80;
	EA=0;
	REN=1;
	TI=0;
	RI=0;
	TR1=1;
}

void speech(unsigned char *text,unsigned char length)
{
	unsigned char head[5];
	unsigned char ecc=0;
	unsigned int i=0;
	head[0]=0xfd;
	head[1]=0x00;
	head[2]=length+3;
	head[3]=0x01;
	head[4]=0x01;
	for (i=0 ; i<5 ; i++ )
	{
		ecc=ecc^(head[i]);
		SBUF=head[i];
		while (TI==0)
		{
			;
		}
		TI=0;
	}
	for (i=0 ; i<length ; i++ )
	{
		ecc=ecc^(text[i]);
		SBUF=text[i];
		while (TI==0)
		{
			;
		}
		TI=0;
	}
	SBUF=ecc;
	while (TI==0)
	{
		;
	}
	TI=0;
	speech_delay(length*240);
}


void main()
{
	uchar i;
	init_lcd();
	init_voice();
	inittimer();
	ch_disp_any(1, 1, 9, "选择物品:");
	ch_disp_any(2, 1, 9, "应投金额:");
	ch_disp_any(3, 1, 9, "实投金额:");
	ch_disp_any(4, 1, 9, "购物状态:");
	drinkkey=0xff;
	while (1)
	{
	
		if (drinkkey != 0xff)
		{
			delay_ms(10);
			if (drinkkey != 0xff)
			{
				switch (drinkkey)
				{
				case 0xfe:
					drinkcode = 0;
					drinkkeytmp=drinkkey;
					break;

				case 0xfd:
					drinkcode = 1;
					drinkkeytmp=drinkkey;
					break;

				case 0xfb:
					drinkcode = 2;
					drinkkeytmp=drinkkey;
					break;

				case 0xf7:
					drinkcode = 3;
					drinkkeytmp=drinkkey;
					break;

				case 0xef:
					drinkcode = 4;
					drinkkeytmp=drinkkey;
					break;

				case 0xdf:
					drinkcode = 5;
					drinkkeytmp=drinkkey;
					break;

				case 0xbf:
					drinkcode = 6;
					drinkkeytmp=drinkkey;
					break;

				case 0x7f:
					drinkcode = 7;
					drinkkeytmp=drinkkey;
					break;

				default:
					break;
				}

				selectedflag = 1;
				ch_disp_any(1, 6, 4, drinknamearray + 4 * drinkcode);
				sprintf1(pricearray, price[drinkcode]);
				ch_disp_any(2, 6, 3, pricearray);
				ch_disp_any(2, 8, 2, "元");
				ch_disp_any(4, 6, 6, "请投币");
				speech(drinknamearray + 4 * drinkcode, 4);
				speech(pricearray, 4);
				speech("元请投币", 8);
			}
		}

		moneykey = Keyboard();
		if (moneykey < 11&&selectedflag==1)
		{
			paymoney = 0.5 * (moneykey + 1);
			sprintf1(pricearray,paymoney);
			ch_disp_any(3, 6, 3, pricearray);
			ch_disp_any(3, 8, 2, "元");
			speech("实投金额",8);
			speech(pricearray,4);
			speech("元",2);
			moneyflag = 1;
		}
		else if (moneykey == 12&&moneyflag==1)
		{
			moneyflag=0;
			if (paymoney == price[drinkcode])
			{
				ch_disp_any(4, 1, 16, "购物状态: 请取物");
				speech("请取物",6);
				selectedflag=0;
				drinkled=drinkkeytmp;
				for (i = 0; i < 20; i++)
				{
					getled = ~getled;
					delay_ms(200);
				}
				getled=1;
				delay_ms(1000);
				drinkled=0xff;
				ch_disp_any(1,1,16,"选择物品:       ");
				ch_disp_any(2, 1, 16, "应投金额:       ");
				ch_disp_any(3, 1, 16, "实投金额:       ");
				ch_disp_any(4,1,16,"购物状态:       ");
			}
			else if (paymoney < price[drinkcode])
			{
				ch_disp_any(4, 1, 6, "退币: ");
				ch_disp_any(4, 4, 3, pricearray);
				ch_disp_any(4, 6, 6, "元    ");
				speech("退币",4);
				speech(pricearray,4);
				speech("元",2);
				for (i = 0; i < 20; i++)
				{
					drawbackled = ~drawbackled;
					delay_ms(200);
				}
				drawbackled=1;
				delay_ms(500);
				ch_disp_any(3, 1, 16, "实投金额:       ");
				ch_disp_any(4, 1, 16, "购物状态: 请投币");
			}
			else if (paymoney > price[drinkcode])
			{
				change = paymoney - price[drinkcode];
				sprintf1(pricearray,change);
				ch_disp_any(4, 1, 6, "找零:");
				ch_disp_any(4, 4, 3, pricearray);
				ch_disp_any(4, 6, 6, "请取物");
				selectedflag=0;
				drinkled=drinkkeytmp;
				speech("找零",4);
				speech(pricearray,4);
				speech("元",2);
				speech("请取物",6);
				for (i = 0; i < 20; i++)
				{
					drawbackled = ~drawbackled;
					getled=~getled;
					delay_ms(200);
				}
				drawbackled=getled=1;
				delay_ms(1000);
				drinkled=0xff;
				ch_disp_any(1, 1, 16, "选择物品:       ");
				ch_disp_any(2, 1, 16, "应投金额:       ");
				ch_disp_any(3, 1, 16, "实投金额:       ");
				ch_disp_any(4, 1, 16, "购物状态:       ");
			}
		}
		else if (moneykey == 15)
		{
			if (selectedflag == 1 && moneyflag == 1)
			{
				moneyflag = 0;
				ch_disp_any(4, 1, 6, "退币: ");
				ch_disp_any(4, 4, 3, pricearray);
				ch_disp_any(4, 6, 6, "元    ");
				speech("退币",4);
				speech(pricearray,4);
				speech("元",2);
				for (i = 0; i < 20; i++)
				{
					drawbackled = ~drawbackled;
					delay_ms(200);
				}
				drawbackled=1;
				delay_ms(500);
				moneyflag=0;
				ch_disp_any(3, 1, 16, "实投金额:       ");
				ch_disp_any(4, 1, 16, "购物状态: 请投币");
			}
			else if (selectedflag == 1 && moneyflag == 0)
			{
				selectedflag=0;
				drinkled=0xff;
				ch_disp_any(1, 1, 16, "选择物品:       ");
				ch_disp_any(2, 1, 16, "应投金额:       ");
				ch_disp_any(3, 1, 16, "实投金额:       ");
				ch_disp_any(4, 1, 16, "购物状态:       ");
			}
		}
	}
}

void t0_time() interrupt 1

{
	TH0 = (65536 - 45872) / 256;
	TL0 = (65536 - 45872) % 256;
	//selectedflag=1时候，要求闪烁，所以num在==1时才加，且num=10和20时进行led的亮灭操作，使其闪烁
	if(selectedflag==1)
	num++;
	if (num == 10 && selectedflag == 1)
	{
		drinkled = drinkkeytmp;
	}
	else if (num >= 20 && selectedflag == 1)
	{
		drinkled = 0xff;
		num = 0;
	}
}
//num 制零一般放到中断里
