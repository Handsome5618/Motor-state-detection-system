#include "oled.h"
#include "olediic.h"
#include "word.h"

u8 OLED_GRAM[144][8];

/***************************************************
函数功能:OLED的初始化
***************************************************/
void OLED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	  //使能A端口时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_1;	  //PA1->IIC_SCL  PA2->IIC_SDA
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; 		    //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       //速度50MHz
 	GPIO_Init(GPIOA, &GPIO_InitStructure);	                //初始化PA1,PA2
 	GPIO_SetBits(GPIOA,GPIO_Pin_1|GPIO_Pin_2);              //PA1,PA2置高

	OLED_WR_Byte(0xAE,OLED_CMD);//---关闭OLED面板
	OLED_WR_Byte(0x00,OLED_CMD);//---设置列低地址
	OLED_WR_Byte(0x10,OLED_CMD);//---设置列高地址
	OLED_WR_Byte(0x40,OLED_CMD);//---设置起始行地址设置映射RAM显示起始行（0x00~0x3F）
	OLED_WR_Byte(0x81,OLED_CMD);//---设置对比控制寄存器
	OLED_WR_Byte(0xCF,OLED_CMD);//---设置亮度
	OLED_WR_Byte(0xA1,OLED_CMD);//---设置分段/列映射     0xa0左右反置 0xa1正常
	OLED_WR_Byte(0xC8,OLED_CMD);//---设置COM列/行扫描方向   0xc0上下反置 0xc8正常
	OLED_WR_Byte(0xA6,OLED_CMD);//---设置正常显示
	OLED_WR_Byte(0xA8,OLED_CMD);//---设置复用比（1到64）
	OLED_WR_Byte(0x3f,OLED_CMD);//---1/64负载
	OLED_WR_Byte(0xD3,OLED_CMD);//---设置显示偏移映射RAM计数器（0x00~0x3F）
	OLED_WR_Byte(0x00,OLED_CMD);//---不抵消
	OLED_WR_Byte(0xd5,OLED_CMD);//---设置显示时钟分频比/振荡器频率
	OLED_WR_Byte(0x80,OLED_CMD);//---s设置分频比，将时钟设置为100帧/秒
	OLED_WR_Byte(0xD9,OLED_CMD);//---设置预充电时间
	OLED_WR_Byte(0xF1,OLED_CMD);//---将预充电设置为15个时钟，放电设置为1个时钟
	OLED_WR_Byte(0xDA,OLED_CMD);//---设置com引脚硬件配置
	OLED_WR_Byte(0x12,OLED_CMD);//---
	OLED_WR_Byte(0xDB,OLED_CMD);//---设置 vcomh
	OLED_WR_Byte(0x30,OLED_CMD);//---设置VCOM取消选择级别
	OLED_WR_Byte(0x20,OLED_CMD);//---设置页面寻址模式  (0x00/0x01/0x02)
	OLED_WR_Byte(0x02,OLED_CMD);//---
	OLED_WR_Byte(0x8D,OLED_CMD);//---设置电荷泵启用/禁用
	OLED_WR_Byte(0x14,OLED_CMD);//---设置（0x10）禁用
	OLED_Clear();
	OLED_WR_Byte(0xAF,OLED_CMD);
}


/**************************************************
函数功能:发送一个字节
入口参数:mode:数据/命令标志 
        0表示命令
        1表示数据
**************************************************/
void OLED_WR_Byte(u8 dat,u8 mode)
{
	I2C_Start();
	Send_Byte(0x78);
	I2C_WaitAck();
	if(mode){Send_Byte(0x40);}
  else{Send_Byte(0x00);}
	I2C_WaitAck();
	Send_Byte(dat);
	I2C_WaitAck();
	I2C_Stop();
}


/**************************************************
函数功能:反转屏幕显示
入口参数:i=0:正常显示;
         i=1:反转显示-----蓝变黑，黑变蓝                      
**************************************************/
void OLED_ColorTurn(u8 i)
{
	if(i==0)
		{
			OLED_WR_Byte(0xA6,OLED_CMD);//正常显示
		}
	if(i==1)
		{
			OLED_WR_Byte(0xA7,OLED_CMD);//反色显示
		}
}


/**************************************************
函数功能:屏幕旋转180度
入口参数:i=0:正常显示
         i=1:反转显示
**************************************************/
void OLED_DisplayTurn(u8 i)
{
	if(i==0)
		{
			OLED_WR_Byte(0xC8,OLED_CMD);//正常显示
			OLED_WR_Byte(0xA1,OLED_CMD);
		}
	if(i==1)
		{
			OLED_WR_Byte(0xC0,OLED_CMD);//反转显示
			OLED_WR_Byte(0xA0,OLED_CMD);
		}
}


/**************************************************
函数功能:开启OLED显示
**************************************************/
void OLED_DisPlay_On(void)
{
	OLED_WR_Byte(0x8D,OLED_CMD);//电荷泵使能
	OLED_WR_Byte(0x14,OLED_CMD);//开启电荷泵
	OLED_WR_Byte(0xAF,OLED_CMD);//点亮屏幕
}


/**************************************************
函数功能:关闭OLED显示
**************************************************/
void OLED_DisPlay_Off(void)
{
	OLED_WR_Byte(0x8D,OLED_CMD);//电荷泵使能
	OLED_WR_Byte(0x10,OLED_CMD);//关闭电荷泵
	OLED_WR_Byte(0xAE,OLED_CMD);//关闭屏幕
}


/**************************************************
函数功能:更新显存到OLED(即刷新刚写入的数据,刷新屏幕)
**************************************************/
void OLED_Refresh(void)
{
	u8 i,n;
	for(i=0;i<8;i++)
	{
		OLED_WR_Byte(0xb0+i,OLED_CMD); //设置行起始地址
		OLED_WR_Byte(0x00,OLED_CMD);   //设置低列起始地址
		OLED_WR_Byte(0x10,OLED_CMD);   //设置高列起始地址
		I2C_Start();
		Send_Byte(0x78);
		I2C_WaitAck();
		Send_Byte(0x40);
		I2C_WaitAck();
		for(n=0;n<128;n++)
		{
			Send_Byte(OLED_GRAM[n][i]);
			I2C_WaitAck();
		}
		I2C_Stop();
  }
}
/***************************************************
函数功能:清除OLED屏幕
***************************************************/
void OLED_Clear(void)
{
	u8 i,n;
	for(i=0;i<8;i++)
	{
	   for(n=0;n<128;n++)
			{
			 OLED_GRAM[n][i]=0;//清除所有数据
			}
  }
	OLED_Refresh();//更新显示
}


/***************************************************
函数功能:在指定的(x,y)坐标处画一个点
入口参数:
        x:0-127
        y:0-63
        t:1:填充;  0:清空
***************************************************/
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 i,m,n;
	i=y/8;
	m=y%8;
	n=1<<m;
	if(t){OLED_GRAM[x][i]|=n;}
	else
	{
		OLED_GRAM[x][i]=~OLED_GRAM[x][i];
		OLED_GRAM[x][i]|=n;
		OLED_GRAM[x][i]=~OLED_GRAM[x][i];
	}
}


/****************************************************
函数功能:从(x1,y1)到(x2,y2)画一条直线
入口参数:
        x1,x2:0-127
        y1,y2:0-63
        mode:1:填充  0:清空
*****************************************************/
void OLED_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2,u8 mode)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1;
	uRow=x1;//画线起点坐标
	uCol=y1;
	if(delta_x>0)incx=1; //设置单步方向 
	else if (delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if (delta_y==0)incy=0;//水平线 
	else {incy=-1;delta_y=-delta_x;}
	if(delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y;
	for(t=0;t<distance+1;t++)
	{
		OLED_DrawPoint(uRow,uCol,mode);//画点
		xerr+=delta_x;
		yerr+=delta_y;
		if(xerr>distance)
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}


/****************************************************
函数功能:以(x,y)为圆心,r为半径画一个圆
入口参数:
        x:0-127
        y:0-63
*****************************************************/
void OLED_DrawCircle(u8 x,u8 y,u8 r)
{
	int a, b,num;
    a = 0;
    b = r;
    while(2 * b * b >= r * r)      
    {
        OLED_DrawPoint(x + a, y - b,1);
        OLED_DrawPoint(x - a, y - b,1);
        OLED_DrawPoint(x - a, y + b,1);
        OLED_DrawPoint(x + a, y + b,1);
 
        OLED_DrawPoint(x + b, y + a,1);
        OLED_DrawPoint(x + b, y - a,1);
        OLED_DrawPoint(x - b, y - a,1);
        OLED_DrawPoint(x - b, y + a,1);
        
        a++;
        num = (a * a + b * b) - r*r;//计算画的点离圆心的距离
        if(num > 0)
        {
            b--;
            a--;
        }
    }
}


/****************************************************
函数功能:在(x,y)处显示一个字符,包括部分字符
入口参数:
        x:0-127
        y:0-63
        size:选择字体:6*8  6*12  8*16  12*24
        mode:0:反色显示  1:正常显示
*****************************************************/
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size1,u8 mode)
{
	u8 i,m,temp,size2,chr1;
	u8 x0=x,y0=y;
	if(size1==8)size2=6;
	else size2=(size1/8+((size1%8)?1:0))*(size1/2);  //得到字体一个字符对应点阵集所占的字节数
	chr1=chr-' ';  //计算偏移后的值
	for(i=0;i<size2;i++)
	{
		if(size1==8)
			  {temp=asc2_0806[chr1][i];} //调用0806字体
		else if(size1==12)
        {temp=asc2_1206[chr1][i];} //调用1206字体
		else if(size1==16)
        {temp=asc2_1608[chr1][i];} //调用1608字体
		else if(size1==24)
        {temp=asc2_2412[chr1][i];} //调用2412字体
		else return;
		for(m=0;m<8;m++)
		{
			if(temp&0x01)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp>>=1;
			y++;
		}
		x++;
		if((size1!=8)&&((x-x0)==size1/2))
		{x=x0;y0=y0+8;}
		y=y0;
  }
}


/****************************************************
函数功能:以(x,y)为起点,显示一个字符串
入口参数:
        x:0-127
        y:0-63
        size:选择字体:6*8  6*12  8*16  12*24
        *chr:字符串起始地址
        mode:0:反色显示  1:正常显示
*****************************************************/
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 size1,u8 mode)
{
	while((*chr>=' ')&&(*chr<='~'))//判断是不是非法字符!
	{
		OLED_ShowChar(x,y,*chr,size1,mode);
		if(size1==8)x+=6;
		else x+=size1/2;
		chr++;
  }
}


/****************************************************
函数功能:计算m^n,即m*m*m*m*m*m*m*m*m*m*m*m*m....(n个)
入口参数:
        m:底数
        n:指数
*****************************************************/
u32 OLED_Pow(u8 m,u8 n)
{
	u32 result=1;
	while(n--)
	{
	  result*=m;
	}
	return result;
}


/****************************************************
函数功能:以(x.y)为起始坐标显示数字
入口参数:
        x:0-127
        y:0-63
        num :要显示的数字
        len :数字的位数
        size:字体大小
        mode:0,反色显示;1,正常显示
*****************************************************/
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size1,u8 mode)
{
	u8 t,temp,m=0;
	if(size1==8)m=2;
	for(t=0;t<len;t++)
	{
		temp=(num/OLED_Pow(10,len-t-1))%10;
			if(temp==0)
			{
				OLED_ShowChar(x+(size1/2+m)*t,y,'0',size1,mode);
      }
			else 
			{
			  OLED_ShowChar(x+(size1/2+m)*t,y,temp+'0',size1,mode);
			}
  }
}


/****************************************************
函数功能:以(x.y)为起始坐标显示汉字
入口参数:
        x:0-127
        y:0-63
        num :汉字对应的序号
        size:字体大小
        mode:0,反色显示;1,正常显示
*****************************************************/
void OLED_ShowChinese(u8 x,u8 y,u8 num,u8 size1,u8 mode)
{
	u8 m,temp;
	u8 x0=x,y0=y;
	u16 i,size3=(size1/8+((size1%8)?1:0))*size1;  //得到字体一个字符对应点阵集所占的字节数
	for(i=0;i<size3;i++)
	{
		if(size1==16)
				{temp=Hzk1[num][i];}//调用16*16字体
		else if(size1==24)
				{temp=Hzk2[num][i];}//调用24*24字体
		else if(size1==32)       
				{temp=Hzk3[num][i];}//调用32*32字体
		else if(size1==64)
				{temp=Hzk4[num][i];}//调用64*64字体
		else return;
		for(m=0;m<8;m++)
		{
			if(temp&0x01)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp>>=1;
			y++;
		}
		x++;
		if((x-x0)==size1)
		{x=x0;y0=y0+8;}
		y=y0;
	}
}


/****************************************************
函数功能:显示汉字的个数(滚屏显示)
入口参数:
        num 显示汉字的个数
        space 每一遍显示的间隔
        mode:0,反色显示;1,正常显示
*****************************************************/
void OLED_ScrollDisplay(u8 num,u8 space,u8 mode)
{
	u8 i,n,t=0,m=0,r;
	while(1)
	{
		if(m==0)
		{
	    OLED_ShowChinese(128,24,t,16,mode); //写入一个汉字保存在OLED_GRAM[][]数组中
			t++;
		}
		if(t==num)
			{
				for(r=0;r<16*space;r++)      //显示间隔
				 {
					for(i=1;i<144;i++)
						{
							for(n=0;n<8;n++)
							{
								OLED_GRAM[i-1][n]=OLED_GRAM[i][n];
							}
						}
           OLED_Refresh();
				 }
        t=0;
      }
		m++;
		if(m==16){m=0;}
		for(i=1;i<144;i++)   //实现左移
		{
			for(n=0;n<8;n++)
			{
				OLED_GRAM[i-1][n]=OLED_GRAM[i][n];
			}
		}
		OLED_Refresh();
	}
}


/****************************************************
函数功能:以(x,y)为起点坐标显示图片 (图片取字模后存放在bmp.h)
入口参数:
         x,y：起点坐标
         sizex:图片的长度
         sizey:图片的宽度
         BMP[]：要写入的图片数组
         mode:0,反色显示;1,正常显示
*****************************************************/
void OLED_ShowPicture(u8 x,u8 y,u8 sizex,u8 sizey,u8 BMP[],u8 mode)
{
	u16 j=0;
	u8 i,n,temp,m;
	u8 x0=x,y0=y;
	sizey=sizey/8+((sizey%8)?1:0);
	for(n=0;n<sizey;n++)
	{
		 for(i=0;i<sizex;i++)
		 {
				temp=BMP[j];
				j++;
				for(m=0;m<8;m++)
				{
					if(temp&0x01)OLED_DrawPoint(x,y,mode);
					else OLED_DrawPoint(x,y,!mode);
					temp>>=1;
					y++;
				}
				x++;
				if((x-x0)==sizex)
				{
					x=x0;
					y0=y0+8;
				}
				y=y0;
     }
	 }
}

void OLED_ShowFloat(u8 x,u8 y,float num,u8 len,u8 size,u8 mode)
{
	float Abs_num;
	int unit,tens,hundreds,one_decimal,two_decimal;
	Abs_num = My_abs(num);
	hundreds = (int)Abs_num/100;    					//求百位
	tens = ((int)Abs_num/10) % 10;  					//求十位
  unit = Abs_num-hundreds*100	-tens*10;   	//求个位
	one_decimal=((Abs_num-(int)Abs_num))*10;
	two_decimal=(int)((Abs_num-(int)Abs_num)*100)%10;
	if(num>=0)
	{
	OLED_ShowChar(x,y,'+',size,mode);
	OLED_ShowNum(x+8,y,hundreds,1,size,mode);
	OLED_ShowNum(x+16,y,tens,1,size,mode);
	OLED_ShowNum(x+24,y,unit,1,size,mode);
	OLED_ShowChar(x+32,y,'.',size,mode);
	OLED_ShowNum(x+40,y,one_decimal,1,size,mode);
	OLED_ShowNum(x+48,y,two_decimal,1,size,mode);
	OLED_Refresh();
	}
	else if(num<0)
	{
	OLED_ShowChar(x,y,'-',size,mode);
	OLED_ShowNum(x+8,y,hundreds,1,size,mode);
	OLED_ShowNum(x+16,y,tens,1,size,mode);
	OLED_ShowNum(x+24,y,unit,1,size,mode);
	OLED_ShowChar(x+32,y,'.',size,mode);
	OLED_ShowNum(x+40,y,one_decimal,1,size,mode);
	OLED_ShowNum(x+48,y,two_decimal,1,size,mode);
	OLED_Refresh();
	}
}


float My_abs(float a)
{
	if(a>=0) return a;
	else return(-a);
}
