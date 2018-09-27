// ============================================================================
// Copyright (C) 2017-2018  All Rights Reserved
// 模块描述: ethernet通信PHY驱动
// 模块版本: V1.00
// 创建人员: GuXY
// 创建时间: 2017-04-18
// ============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// ============================================================================
#ifndef __PHY_H__
#define __PHY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "mySys.h"

// ============================================================================
extern UINT8 *g_pbyRxBuf; 							//以太网底层驱动接收buffers指针 
extern UINT8 *g_pbyTxBuf; 							//以太网底层驱动发送buffers指针
extern ETH_HandleTypeDef    ETH_Handler;            //以太网句柄
extern ETH_DMADescTypeDef  *DMARxDscrTab;			//以太网DMA接收描述符数据结构体指针
extern ETH_DMADescTypeDef  *DMATxDscrTab;			//以太网DMA发送描述符数据结构体指针 
extern ETH_DMADescTypeDef  *DMATxDescToSet;			//DMA发送描述符追踪指针
extern ETH_DMADescTypeDef  *DMARxDescToGet; 		//DMA接收描述符追踪指针 
// ============================================================================
UINT8   PHY_Init(void);
UINT8   PHY_Get_Speed(void);
UINT32  PHY_ReadPHY(UINT16 wReg);
void    PHY_WritePHY(UINT16 wReg,UINT16 wValue);
UINT8   ETH_MACDMA_Config(void);
UINT8   ETH_Mem_Malloc(void);
void    ETH_Mem_Free(void);
UINT32  ETH_GetRxPktSize(ETH_DMADescTypeDef *DMARxDesc);
// ============================================================================

#ifdef __cplusplus
}
#endif

#endif//__PHY_H__

