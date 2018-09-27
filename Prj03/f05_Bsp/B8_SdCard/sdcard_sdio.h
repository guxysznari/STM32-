// ============================================================================
// Copyright (C) 2017-2018  All Rights Reserved
// 模块描述: SD卡驱动(基于SDIO)
// 模块版本: V1.00
// 创建人员: GuXY
// 创建时间: 2017-04-18
// ============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// ============================================================================
#ifndef __SDCARD_SDIO_H__
#define __SDCARD_SDIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "mySys.h"
// ============================================================================
#define SD_TIMEOUT      ((UINT32)100000000)         //超时时间
#define SD_DMA_MODE    	( 0 )		                //1：DMA模式，0：查询模式   
#define CN_SDIO_DATABUF_LEN  (512)                  //SDIO接口通信数据buf大小
// ============================================================================
extern SD_HandleTypeDef        SDCARD_Handler;     //SD卡句柄
extern HAL_SD_CardInfoTypedef  SDCardInfo;         //SD卡信息结构体
// ============================================================================
UINT8 SD_Init(void);
UINT8 SD_GetCardInfo(HAL_SD_CardInfoTypedef *cardinfo);
UINT8 SD_ReadDisk(UINT8* pbyBuf,UINT32 dwSector,UINT32 dwCnt);
UINT8 SD_WriteDisk(UINT8 *pbyBuf,UINT32 dwSector,UINT32 dwCnt);
UINT8 SD_ReadBlocks_DMA(UINT32 *pdwBuf,UINT64 uSector,UINT32 dwBlockSize,UINT32 dwCnt);
UINT8 SD_WriteBlocks_DMA(UINT32 *pdwBuf,UINT64 uSector,UINT32 dwBlockSize,UINT32 dwCnt);
// ============================================================================
#ifdef __cplusplus
}
#endif
#endif//__SDCARD_SDIO_H__

