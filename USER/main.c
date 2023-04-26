/****************************************
PA15:AD0   PB10:IIC_SCL   PB11:IIC_SDA
PA1:IIC_SCL_OLED      PA2:IIC_SDA_OLED
****************************************/
#include "oled.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 
#include "uart.h"
#include "stdio.h"
#include "angle.h"
#include "led.h"
#include "ds18b20.h"
#include "countsensor.h"
#include "timer.h"

float T;
extern u16 Speed;

//int main(void)
//{
//	LED_Init();
//	delay_init();				//延时初始化
//	OLED_Init();
//	OLED_ColorTurn(0);          //0正常显示，1 反色显示
//	OLED_DisplayTurn(0);        //0正常显示 1 屏幕翻转显示
//	MPU_Init();				    //初始化MPU6050
//	USART1_Init();
//	CountSensor_Init();
//	Timer_Init();
//	Moto_Init();

//	while(1)
//	{
//		T = DS18B20_Get_Temp();	
//		printf("speed:  %d\r\n",(u16)Speed);
//	}
//}

int main(void)
{	 
	
	float pitch,roll,yaw; 		//欧拉角
	short aacx,aacy,aacz;		//加速度传感器原始数据
	short gyrox,gyroy,gyroz;	//陀螺仪原始数据
	short temp;                 //温度									

	LED_Init();
	delay_init();				//延时初始化
	OLED_Init();
	OLED_ColorTurn(0);          //0正常显示，1 反色显示
	OLED_DisplayTurn(0);        //0正常显示 1 屏幕翻转显示
	MPU_Init();				    //初始化MPU6050
	USART1_Init();
	CountSensor_Init();
	Timer_Init();
	Moto_Init();
	 
	OLED_ShowString(15,0,"Waiting",24,1);
	OLED_ShowString(0,28,"Initialise",24,1);
	OLED_Refresh();
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);		//使能AFIO时钟
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//禁止JTAG,从而PA15可以做普通IO使用,否则PA15不能做普通IO!!!
	
	while(mpu_dmp_init())
 	{
		OLED_Clear();
		OLED_Refresh();
	}
	OLED_Clear();
	OLED_Refresh();
	
	OLED_Init();
	OLED_ColorTurn(0);          //0正常显示，1 反色显示
	OLED_DisplayTurn(0);        //0正常显示 1 屏幕翻转显示

	OLED_ShowString(0,0,"PITCH:",12,1);
	OLED_ShowString(0,12,"ROLL:",12,1);
	OLED_ShowString(0,24,"YAW:",12,1);
	OLED_ShowString(0,36,"TEMP:",12,1);
	OLED_ShowString(0,48,"SPEED:        r/min",12,1);
	OLED_Refresh();
 	while(1)
	{
		if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0)
		{
			T = DS18B20_Get_Temp();	
			temp=MPU_Get_Temperature();					//得到温度值
			MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//得到加速度传感器数据
			MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//得到陀螺仪数据			
			
			printf("Pitch  %f\r\n",(float)pitch);
			printf("Roll  %f\r\n",(float)roll);
			printf("Yaw  %f\r\n",(float)yaw);
			printf("temp:  %f\r\n",(float)temp/100);
			printf("T:  %f\r\n",(float)T);
			printf("speed:  %d\r\n",(u16)Speed);
			printf(" \r\n");
			
			OLED_ShowFloat(50, 0,pitch,1,12,1);
			OLED_ShowFloat(50,12,roll,5,12,1);
			OLED_ShowFloat(50,24,yaw,5,12,1);
			OLED_ShowFloat(50,36,T,5,12,1);
			OLED_ShowNum(50,48,Speed,5,12,1);
			OLED_Refresh();			
		}
	} 	
}
