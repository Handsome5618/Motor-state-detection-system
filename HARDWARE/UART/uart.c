#include "uart.h"
#include "stdio.h"
/*  上位机测数据    */

uint8_t Serial_Rxdata;

void USART1_Init(void)
{
	//USART1_TX:PA 9   
	//USART1_RX:PA10
	GPIO_InitTypeDef GPIO_InitStructure;     //串口端口配置结构体变量
	USART_InitTypeDef USART_InitStructure;   //串口参数配置结构体变量
	NVIC_InitTypeDef NVIC_InitStructure;     //串口中断配置结构体变量

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);   //打开PA端口时钟

    //USART1_TX   PA9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;          		 //PA9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  		 //设定IO口的输出速度为50MHz
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	   		 //复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);             	 	 //初始化PA9
    //USART1_RX	  PA10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;             //PA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //上拉输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);                 //初始化PA10 

    //USART 初始化设置
	USART_InitStructure.USART_BaudRate = 115200;                  //串口波特率为115200
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;   //字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;        //一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;           //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   //无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	                  //收发模式
    USART_Init(USART1, &USART_InitStructure);                     //初始化串口1
	
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //使能中断

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	
	//USART1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;  //抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		    //子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			      //IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	                          //根据指定的参数初始化VIC寄存器


    USART_Cmd(USART1, ENABLE);                     //使能串口1

  	//如下语句解决第1个字节无法正确发送出去的问题
	USART_ClearFlag(USART1, USART_FLAG_TC);        //清串口1发送标志
		
}

void USART1_IRQHandler(void)			 
{
	if( USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET)  	   //接收中断  
	{
		Serial_Rxdata = USART_ReceiveData(USART1);
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);   //清除中断标志
	}
	switch(Serial_Rxdata)
	{
		case 0x11:
			GPIO_ResetBits(GPIOA,GPIO_Pin_4);
			break;
		case 0x10:
			GPIO_SetBits(GPIOA,GPIO_Pin_4);
			break;
		case 0x21:
			GPIO_ResetBits(GPIOA,GPIO_Pin_5);
			break;
		case 0x20:
			GPIO_SetBits(GPIOA,GPIO_Pin_5);
			break;
		case 0x31:
			GPIO_ResetBits(GPIOA,GPIO_Pin_6);
			break;
		case 0x30:
			GPIO_SetBits(GPIOA,GPIO_Pin_6);
			break;
		case 0x41:
			GPIO_ResetBits(GPIOA,GPIO_Pin_7);
			break;
		case 0x40:
			GPIO_SetBits(GPIOA,GPIO_Pin_7);
			break;
		default:
			break;	
	}
}
