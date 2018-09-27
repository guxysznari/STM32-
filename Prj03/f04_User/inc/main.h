// ============================================================================
// Copyright (C) 2017-2018  All Rights Reserved
// 模块描述: 主函数(标准化软件工程)
// 模块版本: V1.00
// 创建人员: GuXY
// 创建时间: 2017-04-18
// ============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// ============================================================================
#ifndef __MAIN_H
#define __MAIN_H

//----------------------------------------------------------------------------
#include "stm32f4xx_hal.h"
#include "stdio.h"
#include "stm324x9i_eval.h"
#include "stm324x9i_eval_io.h"
#include "mySys.h"

//----------------------------------------------------------------------------
//网络通信协议选择
//#define MACRO_TCP_CLIENT
//#define MACRO_TCP_SERVER
#define MACRO_UDP_CLIENT
//#define MACRO_UDP_SERVER
//----------------------------------------------------------------------------
/* Definition for USARTx clock resources */
#define USARTx                           USART1
#define USARTx_CLK_ENABLE()              __HAL_RCC_USART1_CLK_ENABLE();
#define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE() 

#define USARTx_FORCE_RESET()             __HAL_RCC_USART1_FORCE_RESET()
#define USARTx_RELEASE_RESET()           __HAL_RCC_USART1_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USARTx_TX_PIN                    GPIO_PIN_9
#define USARTx_TX_GPIO_PORT              GPIOA  
#define USARTx_TX_AF                     GPIO_AF7_USART1
#define USARTx_RX_PIN                    GPIO_PIN_10
#define USARTx_RX_GPIO_PORT              GPIOA 
#define USARTx_RX_AF                     GPIO_AF7_USART1
//----------------------------------------------------------------------------
//端口设置
#define TCP_CLIENT_PORT     ( 8000 )
#define TCP_SERVER_PORT     ( 8080 )

#define UDP_CLIENT_PORT     ( 6000 )  
#define UDP_SERVER_PORT     ( 6060 )  
//----------------------------------------------------------------------------
//目的IP地址
#define DEST_IP_ADDR0       ( 192  )
#define DEST_IP_ADDR1       ( 168  )
#define DEST_IP_ADDR2       ( 0    )
#define DEST_IP_ADDR3       ( 11   )

//----------------------------------------------------------------------------
//本地静态IP
#define IP_ADDR0            ( 192  )
#define IP_ADDR1            ( 168  )
#define IP_ADDR2            ( 0    )
#define IP_ADDR3            ( 10   )
   
//子网掩码
#define NETMASK_ADDR0       ( 255  )
#define NETMASK_ADDR1       ( 255  )
#define NETMASK_ADDR2       ( 255  )
#define NETMASK_ADDR3       ( 0    )

//网关
#define GW_ADDR0            ( 192  )
#define GW_ADDR1            ( 168  )
#define GW_ADDR2            ( 0    )
#define GW_ADDR3            ( 1    )
//----------------------------------------------------------------------------

#endif /* __MAIN_H */

