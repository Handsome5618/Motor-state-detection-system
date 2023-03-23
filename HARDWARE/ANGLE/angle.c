#include "angle.h"
#include "inv_mpu.h"
#include "delay.h"

typedef union
{
	uint8_t Angle[4];
	float angle;
}Angle;

Angle Angle_Pitch;
Angle Angle_Yaw;
Angle Angle_Roll;

void Send_Angle(void)
{
	mpu_dmp_get_data(&Angle_Pitch.angle,&Angle_Yaw.angle,&Angle_Roll.angle);
	USART_SendData(USART1,Angle_Pitch.angle);
//	while(USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET);
//	USART_SendData(USART1,Angle_Yaw.angle);
//	while(USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET);
//	USART_SendData(USART1,Angle_Roll.angle);
//	while(USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET);
}
