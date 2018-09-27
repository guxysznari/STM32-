// ============================================================================
// Copyright (C) 2017-2018  All Rights Reserved
// 模块描述: SDRAM驱动
// 模块版本: V1.00
// 创建人员: GuXY
// 创建时间: 2017-04-18
// ============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// ============================================================================
#ifndef __SDRAM_H__
#define __SDRAM_H__

#include "mySys.h"

extern SDRAM_HandleTypeDef SDRAM_Handler;//SDRAM句柄
#define Bank5_SDRAM_ADDR    ((UINT32)(0XC0000000)) //SDRAM开始地址

//SDRAM配置参数
#define SDRAM_MODEREG_BURST_LENGTH_1             ((UINT16)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((UINT16)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((UINT16)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((UINT16)0x0004)

#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((UINT16)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((UINT16)0x0008)

#define SDRAM_MODEREG_CAS_LATENCY_2              ((UINT16)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((UINT16)0x0030)

#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((UINT16)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((UINT16)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((UINT16)0x0200)

// ============================================================================
extern void SDRAM_Init(void);//SDRAM初始化
extern void FMC_SDRAM_WrBuf(UINT8 *pbyBuf,UINT32 dwWriteAdr,UINT32 dwNum);//SDRAM写
extern void FMC_SDRAM_RdBuf(UINT8 *pbyBuf,UINT32 dwReadAdr ,UINT32 dwNum);//SDRAM读
// ============================================================================
#endif  //__SDRAM_H__

