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
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "mySys.h"
#include "stdio.h"	

#define CN_BUFSIZE_RX               1       //缓存大小
#define CN_USART_RX_LEN  			200  	//定义最大接收字节数 200
#define CN_USART1_RX_EN 			1		//使能（1）/禁止（0）串口1接收

// ============================================================================
extern UINT8    USART_RX_BUF[CN_USART_RX_LEN];  //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern UINT8    aRxBuffer[CN_BUFSIZE_RX];       //HAL库USART接收Buffer
extern UINT16   USART_RX_STA;         		    //接收状态标记	
extern UART_HandleTypeDef UART1_Handler;        //UART句柄

// ============================================================================
extern void uart_init(UINT32 dwBound);

#ifdef __cplusplus
}
#endif

#endif//__USART_H__
