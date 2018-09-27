// ============================================================================
// Copyright (C) 2017-2018  All Rights Reserved
// 模块描述: 串口通信驱动
// 模块版本: V1.00
// 创建人员: GuXY
// 创建时间: 2017-04-18
// ============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// ============================================================================

#include "rs485.h"
#include "delay.h"

// ============================================================================
UART_HandleTypeDef USART1_Handler;
UART_HandleTypeDef USART2_Handler;
UART_HandleTypeDef USART3_Handler;
// ============================================================================
UINT8  USART1_DatRxBuf[CN_USART1_RX_BUF_LEN];//用户使用接收数据缓冲
UINT8  USART1_DatRxBuf[CN_USART2_RX_BUF_LEN];//用户使用接收数据缓冲
UINT8  USART1_DatRxBuf[CN_USART3_RX_BUF_LEN];//用户使用接收数据缓冲
// ============================================================================
UINT16 g_wUsart1RxCnt;
UINT16 g_wUsart2RxCnt;
UINT16 g_wUsart3RxCnt;
// ============================================================================
void RS485_TX_Set(UINT8 byMode);
// ============================================================================
// 函数功能:初始化串口
// 输入参数:Bound-波特率
// 返 回 值:无
// ============================================================================
void uart_init(UINT8 byUsartNum,UINT32 dwBound)
{	
    g_wUsart1RxCnt = 0;
    g_wUsart2RxCnt = 0;
    g_wUsart3RxCnt = 0;
    
    switch(byUsartNum)
    {
        case 1:
            __HAL_RCC_USART1_CLK_ENABLE();                              //使能USART时钟
            //UART初始化设置
            USART1_Handler.Instance          = USART1;                   //USART
            USART1_Handler.Init.BaudRate     = dwBound;                  //波特率
            USART1_Handler.Init.WordLength   = UART_WORDLENGTH_8B;       //字长为8位数据格式
            USART1_Handler.Init.StopBits     = UART_STOPBITS_1;          //一个停止位
            USART1_Handler.Init.Parity       = UART_PARITY_NONE;         //无奇偶校验位
            USART1_Handler.Init.HwFlowCtl    = UART_HWCONTROL_NONE;      //无硬件流控
            USART1_Handler.Init.Mode         = UART_MODE_TX_RX;          //收发模式
            HAL_UART_Init(&USART1_Handler);                              //使能USART
            HAL_NVIC_EnableIRQ(USART1_IRQn);                            //使能USART1中断通道
            HAL_NVIC_SetPriority(USART1_IRQn,3,1);                      //抢占优先级3，子优先级1    
            __HAL_UART_ENABLE_IT(&USART1_Handler,UART_IT_RXNE);
            break;
        case 2:
            __HAL_RCC_USART2_CLK_ENABLE();                              //使能USART时钟
            //UART初始化设置
            USART2_Handler.Instance          = USART2;                   //USART
            USART2_Handler.Init.BaudRate     = dwBound;                  //波特率
            USART2_Handler.Init.WordLength   = UART_WORDLENGTH_8B;       //字长为8位数据格式
            USART2_Handler.Init.StopBits     = UART_STOPBITS_1;          //一个停止位
            USART2_Handler.Init.Parity       = UART_PARITY_NONE;         //无奇偶校验位
            USART2_Handler.Init.HwFlowCtl    = UART_HWCONTROL_NONE;      //无硬件流控
            USART2_Handler.Init.Mode         = UART_MODE_TX_RX;          //收发模式
            HAL_UART_Init(&USART2_Handler);                              //使能USART
            HAL_NVIC_EnableIRQ(USART2_IRQn);                            //使能USART2中断通道
            HAL_NVIC_SetPriority(USART2_IRQn,3,2);                      //抢占优先级3，子优先级2
            __HAL_UART_ENABLE_IT(&USART2_Handler,UART_IT_RXNE);
            break;
        case 3:
            __HAL_RCC_USART3_CLK_ENABLE();                              //使能USART时钟
	//UART初始化设置
            USART3_Handler.Instance          = USART3;                   //USART
            USART3_Handler.Init.BaudRate     = dwBound;                  //波特率
            USART3_Handler.Init.WordLength   = UART_WORDLENGTH_8B;       //字长为8位数据格式
            USART3_Handler.Init.StopBits     = UART_STOPBITS_1;          //一个停止位
            USART3_Handler.Init.Parity       = UART_PARITY_NONE;         //无奇偶校验位
            USART3_Handler.Init.HwFlowCtl    = UART_HWCONTROL_NONE;      //无硬件流控
            USART3_Handler.Init.Mode         = UART_MODE_TX_RX;          //收发模式
            HAL_UART_Init(&USART3_Handler);                              //使能USART
            HAL_NVIC_EnableIRQ(USART3_IRQn);                            //使能USART3中断通道
            HAL_NVIC_SetPriority(USART3_IRQn,3,3);                      //抢占优先级3，子优先级3
            __HAL_UART_ENABLE_IT(&USART3_Handler,UART_IT_RXNE);
            break;
        default:
            break;
    }
}
// ============================================================================
// 函数功能:串口1中断服务程序
// 输入参数:
// 返 回 值:
// ============================================================================
void USART1_IRQHandler(void)                	
{
    UINT8 byDataTemp; 
	
    if((__HAL_UART_GET_FLAG(&USART1_Handler, UART_FLAG_RXNE)!=RESET))  //接收
    {       
         HAL_UART_Receive(&USART1_Handler,&byDataTemp,1,1000); 
         if(g_wUsart1RxCnt<CN_USART1_RX_BUF_LEN) 
	{
            USART1_DatRxBuf[g_wUsart1RxCnt] = byDataTemp;   
            g_wUsart1RxCnt++;           
         }   
         else
         {
            g_wUsart1RxCnt=0;
         }   
	}
}
// ============================================================================
// 函数功能:串口2中断服务程序
// 输入参数:
// 返 回 值:
// ============================================================================
void USART2_IRQHandler(void)                	
{
    UINT8 byDataTemp; 

    if((__HAL_UART_GET_FLAG(&USART2_Handler, UART_FLAG_RXNE)!=RESET))  //接收
	{
         HAL_UART_Receive(&USART2_Handler,&byDataTemp,1,1000); 
         if(g_wUsart2RxCnt<CN_USART2_RX_BUF_LEN) 
		{
            USART1_DatRxBuf[g_wUsart2RxCnt] = byDataTemp;   
            g_wUsart2RxCnt++;           
         }   
         else
			{
            g_wUsart2RxCnt=0;
         }  
    } 
			}
// ============================================================================
// 函数功能:串口3中断服务程序
// 输入参数:
// 返 回 值:
// ============================================================================
void USART3_IRQHandler(void)                	
{ 
    UINT8 byDataTemp; 

    if((__HAL_UART_GET_FLAG(&USART3_Handler, UART_FLAG_RXNE)!=RESET))  //接收
			{	
         HAL_UART_Receive(&USART3_Handler,&byDataTemp,1,1000); 
         if(g_wUsart3RxCnt<CN_USART3_RX_BUF_LEN) 
				{
            USART1_DatRxBuf[g_wUsart3RxCnt] = byDataTemp;   
            g_wUsart3RxCnt++;           
                }
				else
				{
            g_wUsart3RxCnt=0;
			}
		}
	}
// ============================================================================
// 函数功能:串口1发送数据
// 输入参数:发送区首地址，发送的字节数 
// 返 回 值:
// ============================================================================
void Usart1_Send_Data(UINT8 *pbyBuf,UINT8 byLen) 
{ 
    RS485_TX_Set(1);     //设置为发送模式 
    HAL_UART_Transmit(&USART1_Handler,pbyBuf,byLen,1000);
    g_wUsart1RxCnt=0;      
    RS485_TX_Set(0);     //设置为接收模式   
}
// ============================================================================
// 函数功能:串口1接收数据
// 输入参数:接收缓存首地址，读到的数据长度 
// 返 回 值:
// ============================================================================
void Usart1_Receive_Data(UINT8 *pbyBuf,UINT8 *pbyLen) 
{ 
  UINT8 byRxLen; 
  UINT8 byLoop; 

  byLoop  = 0; 
 *pbyLen  = 0;        
  byRxLen = g_wUsart1RxCnt; 
	
  //等待10ms,连续超过10ms没有接收到一个数据,则认为接收结束 
  delay_ms(10);
  
  //接收到了数据,且接收完成了 
  if((byRxLen == g_wUsart1RxCnt) && byRxLen)
	{
    for(byLoop=0;byLoop<byRxLen;byLoop++) 
        {
      pbyBuf[byLoop] = USART1_DatRxBuf[byLoop];   
	}
     
    //记录本次数据长度 
    *pbyLen = g_wUsart1RxCnt; 
    
    //清零 
    g_wUsart1RxCnt=0;    
        }
	}
// ============================================================================
// 函数功能:串口用作RS485半双工时，发送/接收模式选择
// 输入参数:0,接收;1,发送. 
// 返 回 值:
// ============================================================================
void RS485_TX_Set(UINT8 byMode) 
{ 
    MACRO_PB_Out(13) = byMode;  //DE0
    MACRO_PG_Out(0 ) = byMode;  //DE0
    MACRO_PB_Out(12) = 1;       //HDPLX0
    MACRO_PG_Out(1 ) = 1;       //HDPLX0
} 
// ============================================================================

