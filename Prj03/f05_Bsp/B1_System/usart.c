// ============================================================================
// Copyright (C) 2017-2018  All Rights Reserved
// 模块描述: 串口打印处理函数
// 模块版本: V1.00
// 创建人员: GuXY
// 创建时间: 2017-04-18
// ============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// ============================================================================

#include "usart.h"
#include "delay.h"
// ============================================================================
//使用os,包括下面的头文件
#if SYSTEM_SUPPORT_OS
#include "includes.h"					
#endif

// ============================================================================
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
//#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)	
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (UINT8) ch;      
	return ch;
}
#endif 

// ============================================================================
#if CN_USART1_RX_EN   //如果使能了接收
//串口1中断服务程序,注意,读取USARTx->SR能避免莫名其妙的错误   	
UINT8  aRxBuffer[CN_BUFSIZE_RX];//HAL库使用的串口接收缓冲
UINT8  USART_RX_BUF[CN_USART_RX_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
UINT16 USART_RX_STA=0;          // 接收状态标记	
                                // bit15，	接收完成标志
                                // bit14，	接收到0x0d
                                // bit13~0，接收到的有效字节数目

UART_HandleTypeDef UART1_Handler; //UART句柄

// ============================================================================
// 函数功能:初始化IO 串口1 
// 输入参数:Bound-波特率
// 返 回 值:无
// ============================================================================
void uart_init(UINT32 dwBound)
{	
	//UART初始化设置
	UART1_Handler.Instance          = USART1;					//USART1
	UART1_Handler.Init.BaudRate     = dwBound;				    //波特率
	UART1_Handler.Init.WordLength   = UART_WORDLENGTH_8B;       //字长为8位数据格式
	UART1_Handler.Init.StopBits     = UART_STOPBITS_1;	        //一个停止位
	UART1_Handler.Init.Parity       = UART_PARITY_NONE;		    //无奇偶校验位
	UART1_Handler.Init.HwFlowCtl    = UART_HWCONTROL_NONE;      //无硬件流控
	UART1_Handler.Init.Mode         = UART_MODE_TX_RX;		    //收发模式
	HAL_UART_Init(&UART1_Handler);					            //使能UART1
	
	HAL_UART_Receive_IT(&UART1_Handler, (UINT8 *)aRxBuffer, CN_BUFSIZE_RX);//该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量
  
}
// ============================================================================
// 函数功能:UART底层初始化，时钟使能，引脚配置，中断配置,此函数会被HAL_UART_Init()调用
// 输入参数:huart-串口句柄
// 返 回 值:无
// ============================================================================
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    //GPIO端口设置
	GPIO_InitTypeDef GPIO_Initure;
	
	if(huart->Instance==USART1)//如果是串口1，进行串口1 MSP初始化
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();			        //使能GPIOA时钟
		__HAL_RCC_USART1_CLK_ENABLE();			        //使能USART1时钟
	
		GPIO_Initure.Pin        = GPIO_PIN_9;			//PA9
		GPIO_Initure.Mode       = GPIO_MODE_AF_PP;		//复用推挽输出
		GPIO_Initure.Pull       = GPIO_PULLUP;			//上拉
		GPIO_Initure.Speed      = GPIO_SPEED_FAST;		//高速
		GPIO_Initure.Alternate  = GPIO_AF7_USART1;	    //复用为USART1
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	        //初始化PA9

		GPIO_Initure.Pin=GPIO_PIN_10;			        //PA10
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	        //初始化PA10
		
#if CN_USART1_RX_EN
		HAL_NVIC_EnableIRQ(USART1_IRQn);				//使能USART1中断通道
		HAL_NVIC_SetPriority(USART1_IRQn,3,3);			//抢占优先级3，子优先级3
#endif	
	}

}
// ============================================================================
// 函数功能:传输完成回调函数
// 输入参数:
// 返 回 值:
// ============================================================================
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==USART1)//如果是串口1
	{
		if((USART_RX_STA&0x8000)==0)//接收未完成
		{
			if(USART_RX_STA&0x4000)//接收到了0x0d
			{
				if(aRxBuffer[0]!=0x0a)USART_RX_STA=0;//接收错误,重新开始
				else USART_RX_STA|=0x8000;	//接收完成了 
			}
			else //还没收到0X0D
			{	
				if(aRxBuffer[0]==0x0d)
				{
                    USART_RX_STA |= 0x4000;
                }
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF] = aRxBuffer[0] ;
					USART_RX_STA++;
					if(USART_RX_STA > (CN_USART_RX_LEN-1))
					{
                        USART_RX_STA=0;//接收数据错误,重新开始接收    
                    }
				}		 
			}
		}
	}
}
// ============================================================================
// 函数功能:串口1中断服务程序
// 输入参数:无
// 返 回 值:无
// ============================================================================
void USART1_IRQHandler(void)                	
{ 
	UINT32 dwTimeout=0;
#if SYSTEM_SUPPORT_OS	 	//使用OS
	OSIntEnter();    
#endif

	//调用HAL库中断处理公用函数
	HAL_UART_IRQHandler(&UART1_Handler);	
	
	dwTimeout = 0;
    //等待就绪
    while (HAL_UART_GetState(&UART1_Handler) != HAL_UART_STATE_READY)
	{
        dwTimeout++;
        //超时处理
        if(dwTimeout > HAL_MAX_DELAY)
        {
            break;      
        }
	}
     
	dwTimeout = 0;
    //一次处理完成之后，重新开启中断并设置RxXferCount为1
	while(HAL_UART_Receive_IT(&UART1_Handler, (UINT8 *)aRxBuffer, CN_BUFSIZE_RX) != HAL_OK)
	{
        dwTimeout++; 
        //超时处理
        if(dwTimeout>HAL_MAX_DELAY)
        {
            break; 
        }
	}
#if SYSTEM_SUPPORT_OS	 	//使用OS
    OSIntExit();  											 
#endif
} 
#endif	

/*下面代码我们直接把中断控制逻辑写在中断服务函数内部。*/
// ============================================================================
// 函数功能:串口1中断服务程序
// 输入参数:
// 返 回 值:
// ============================================================================
//void USART1_IRQHandler(void)                	
//{ 
//	UINT8 Res;
//#if SYSTEM_SUPPORT_OS	 	//使用OS
//	OSIntEnter();    
//#endif
//	if((__HAL_UART_GET_FLAG(&UART1_Handler,UART_FLAG_RXNE)!=RESET))  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
//	{
//        HAL_UART_Receive(&UART1_Handler,&Res,1,1000); 
//		if((USART_RX_STA&0x8000)==0)//接收未完成
//		{
//			if(USART_RX_STA&0x4000)//接收到了0x0d
//			{
//				if(Res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
//				else USART_RX_STA|=0x8000;	//接收完成了 
//			}
//			else //还没收到0X0D
//			{	
//				if(Res==0x0d)USART_RX_STA|=0x4000;
//				else
//				{
//					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
//					USART_RX_STA++;
//					if(USART_RX_STA>(CN_USART_RX_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
//				}		 
//			}
//		}   		 
//	}
//	HAL_UART_IRQHandler(&UART1_Handler);	
//#if SYSTEM_SUPPORT_OS	 	//使用OS
//	OSIntExit();  											 
//#endif
//} 
//#endif	

