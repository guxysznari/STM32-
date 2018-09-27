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
#include "sdcard_sdio.h"
#include "string.h"

// ============================================================================
HAL_SD_CardInfoTypedef  SDCardInfo;                         //SD卡信息结构体
SD_HandleTypeDef        SDCARD_Handler;                     //SD卡句柄
DMA_HandleTypeDef       SDTxDMAHandler,SDRxDMAHandler;      //SD卡DMA发送和接收句柄
// ============================================================================
//SD_ReadDisk/SD_WriteDisk函数专用buf,当这两个函数的数据缓存区地址不是4字节对齐的时候,
//需要用到该数组,确保数据缓存区地址是4字节对齐的.
__align(4) UINT8 SDIO_DATA_BUFFER[CN_SDIO_DATABUF_LEN];

// ============================================================================
// 函数功能:SD卡初始化
// 输入参数:无 
// 返 回 值:0-初始化正确；其他值-初始化错误
// ============================================================================
UINT8 SD_Init(void)
{
    UINT8 bySDErr;
    
    //初始化时的时钟不能大于400KHZ 
    SDCARD_Handler.Instance                 = SDIO;
    SDCARD_Handler.Init.ClockEdge           = SDIO_CLOCK_EDGE_RISING;               //上升沿     
    SDCARD_Handler.Init.ClockBypass         = SDIO_CLOCK_BYPASS_DISABLE;            //不使用bypass模式，直接用HCLK进行分频得到SDIO_CK
    SDCARD_Handler.Init.ClockPowerSave      = SDIO_CLOCK_POWER_SAVE_DISABLE;        //空闲时不关闭时钟电源
    SDCARD_Handler.Init.BusWide             = SDIO_BUS_WIDE_1B;                     //1位数据线
    SDCARD_Handler.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;   //关闭硬件流控
    SDCARD_Handler.Init.ClockDiv            = SDIO_TRANSFER_CLK_DIV;                //SD传输时钟频率最大25MHZ
    
    bySDErr=HAL_SD_Init(&SDCARD_Handler,&SDCardInfo);
    if(bySDErr!=SD_OK)
    {
        return 1;
    }
    //使能宽总线模式
    bySDErr=HAL_SD_WideBusOperation_Config(&SDCARD_Handler,SDIO_BUS_WIDE_4B);
    if(bySDErr!=SD_OK)
    {
        return 2;
    }
    return 0;
}
// ============================================================================
// 函数功能:SDMMC底层驱动,时钟使能,引脚配置,DMA配置,此函数会被HAL_SD_Init()调用
// 输入参数:SD卡句柄
// 返 回 值:无 
// ============================================================================
void HAL_SD_MspInit(SD_HandleTypeDef *hsd)
{
    GPIO_InitTypeDef  GPIO_Initure;
    DMA_HandleTypeDef TxDMAHandler,RxDMAHandler;
    
    __HAL_RCC_GPIOC_CLK_ENABLE();   //使能GPIOC时钟
    __HAL_RCC_GPIOD_CLK_ENABLE();   //使能GPIOD时钟
    __HAL_RCC_DMA2_CLK_ENABLE();    //使能DMA2时钟 
    __HAL_RCC_SDIO_CLK_ENABLE();    //使能SDIO时钟

    //-------------------------------------------------------------------------
    //PC8,9,10,11,12
    GPIO_Initure.Pin=GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12;
    GPIO_Initure.Mode       = GPIO_MODE_AF_PP;      //推挽复用
    GPIO_Initure.Pull       = GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed      = GPIO_SPEED_HIGH;      //高速
    GPIO_Initure.Alternate  = GPIO_AF12_SDIO;       //复用为SDIO
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);             //初始化
    //-------------------------------------------------------------------------
    //PD2
    GPIO_Initure.Pin=GPIO_PIN_2;
    //初始化            
    HAL_GPIO_Init(GPIOD,&GPIO_Initure);     

//-------------------------------------------------------------------------
//使用DMA模式
#if (SD_DMA_MODE==1)          
    //配置SDMMC1中断，抢占优先级2，子优先级0              
    HAL_NVIC_SetPriority(SDMMC1_IRQn,2,0); 
    //使能SDMMC1中断
    HAL_NVIC_EnableIRQ(SDMMC1_IRQn);        
    
    //-------------------------------------------------------------------------
    //配置发送DMA
    SDRxDMAHandler.Instance                 = DMA2_Stream3;
    SDRxDMAHandler.Init.Channel             = DMA_CHANNEL_4;
    SDRxDMAHandler.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    SDRxDMAHandler.Init.PeriphInc           = DMA_PINC_DISABLE;
    SDRxDMAHandler.Init.MemInc              = DMA_MINC_ENABLE;
    SDRxDMAHandler.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    SDRxDMAHandler.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
    SDRxDMAHandler.Init.Mode                = DMA_PFCTRL;
    SDRxDMAHandler.Init.Priority            = DMA_PRIORITY_VERY_HIGH;
    SDRxDMAHandler.Init.FIFOMode            = DMA_FIFOMODE_ENABLE;
    SDRxDMAHandler.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
    SDRxDMAHandler.Init.MemBurst            = DMA_MBURST_INC4;
    SDRxDMAHandler.Init.PeriphBurst         = DMA_PBURST_INC4;

    //将接收DMA和SD卡的发送DMA连接起来
    __HAL_LINKDMA(hsd, hdmarx, SDRxDMAHandler); 
    HAL_DMA_DeInit(&SDRxDMAHandler);
    //初始化接收DMA
    HAL_DMA_Init(&SDRxDMAHandler);              
    
    //-------------------------------------------------------------------------
    //配置接收DMA 
    SDTxDMAHandler.Instance                 = DMA2_Stream6;
    SDTxDMAHandler.Init.Channel             = DMA_CHANNEL_4;
    SDTxDMAHandler.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    SDTxDMAHandler.Init.PeriphInc           = DMA_PINC_DISABLE;
    SDTxDMAHandler.Init.MemInc              = DMA_MINC_ENABLE;
    SDTxDMAHandler.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    SDTxDMAHandler.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
    SDTxDMAHandler.Init.Mode                = DMA_PFCTRL;
    SDTxDMAHandler.Init.Priority            = DMA_PRIORITY_VERY_HIGH;
    SDTxDMAHandler.Init.FIFOMode            = DMA_FIFOMODE_ENABLE;
    SDTxDMAHandler.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
    SDTxDMAHandler.Init.MemBurst            = DMA_MBURST_INC4;
    SDTxDMAHandler.Init.PeriphBurst         = DMA_PBURST_INC4;

     //将发送DMA和SD卡的发送DMA连接起来
    __HAL_LINKDMA(hsd, hdmatx, SDTxDMAHandler);
    HAL_DMA_DeInit(&SDTxDMAHandler);
    //初始化发送DMA 
    HAL_DMA_Init(&SDTxDMAHandler);              
  
    //接收DMA中断优先级
    HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 3, 0);  
    HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
    //发送DMA中断优先级
    HAL_NVIC_SetPriority(DMA2_Stream6_IRQn, 3, 0);  
    HAL_NVIC_EnableIRQ(DMA2_Stream6_IRQn);
#endif
//-------------------------------------------------------------------------
}
// ============================================================================
// 函数功能:获取卡信息
// 输入参数:卡信息存储区
// 返 回 值:错误状态
// ============================================================================
UINT8 SD_GetCardInfo(HAL_SD_CardInfoTypedef *cardinfo)
{
    UINT8 bySta;
    
    bySta = HAL_SD_Get_CardInfo(&SDCARD_Handler,cardinfo);
    return bySta;
}
// ============================================================================
//DMA模式
#if (SD_DMA_MODE==1)        
// ============================================================================
// 函数功能:通过DMA读取SD卡一个扇区
// 输入参数:buf:读数据缓存区sector:扇区地址blocksize:扇区大小(一般都是512字节)cnt:扇区个数	
// 返 回 值:0-正常;其他-错误代码;
// ============================================================================
UINT8 SD_ReadBlocks_DMA(UINT32 *pdwBuf,UINT64 uSector,UINT32 dwBlockSize,UINT32 dwCnt)
{
    UINT8 byErr;

    byErr = 0;
    
    //通过DMA读取SD卡一个扇区
    byErr = HAL_SD_ReadBlocks_DMA(&SDCARD_Handler,pdwBuf,uSector,dwBlockSize,dwCnt);
    //读取成功
    if(byErr==0)
    {
        //等待读取完成
        byErr = HAL_SD_CheckReadOperation(&SDCARD_Handler,(UINT32)SD_TIMEOUT);
    }

    return byErr;
}
// ============================================================================
// 函数功能:写SD卡
// 输入参数:buf:写数据缓存区sector:扇区地址blocksize:扇区大小(一般都是512字节)cnt:扇区个数	
// 返 回 值:0-正常;其他-错误代码;
// ============================================================================
UINT8 SD_WriteBlocks_DMA(UINT32 *pdwBuf,UINT64 uSector,UINT32 dwBlockSize,UINT32 dwCnt)
{
    UINT8 byErr;

    byErr = 0;
    
    //通过DMA写SD卡一个扇区
    byErr=HAL_SD_WriteBlocks_DMA(&SDCARD_Handler,pdwBuf,uSector,dwBlockSize,dwCnt);
    
    if(byErr==0)
    {  
        //等待读取完成
        byErr=HAL_SD_CheckWriteOperation(&SDCARD_Handler,(UINT32)SD_TIMEOUT);
    }
    return byErr;
}
// ============================================================================
// 函数功能:读SD卡
// 输入参数:buf:读数据缓存区sector:扇区地址cnt:扇区个数	
// 返 回 值:0-正常;其他-错误代码;
// ============================================================================
UINT8 SD_ReadDisk(UINT8* pbyBuf,UINT32 dwSector,UINT32 dwCnt)
{
    UINT8 bySta;
    UINT8 byLoop;
    UINT64 uSector;

    bySta   = SD_OK;
    uSector = (UINT64)dwSector;
    
    if(SDCardInfo.CardType!=STD_CAPACITY_SD_CARD_V1_1)
    {
        uSector<<=9;
    }
    if((UINT32)pbyBuf%4!=0)
    {
        for(byLoop=0;byLoop<dwCnt;byLoop++)
        {
            bySta=SD_ReadBlocks_DMA((UINT32*)SDIO_DATA_BUFFER,uSector+CN_SDIO_DATABUF_LEN*byLoop,CN_SDIO_DATABUF_LEN,1);
            memcpy(pbyBuf,SDIO_DATA_BUFFER,CN_SDIO_DATABUF_LEN);
            pbyBuf += CN_SDIO_DATABUF_LEN;
        }
    }
    else
    {
        bySta=SD_ReadBlocks_DMA((UINT32*)pbyBuf,uSector, CN_SDIO_DATABUF_LEN,dwCnt);
    }
    return bySta;
}  
// ============================================================================
// 函数功能:写SD卡
// 输入参数:buf:写数据缓存区sector:扇区地址cnt:扇区个数	
// 返 回 值:0-正常;其他-错误代码;
// ============================================================================
UINT8 SD_WriteDisk(UINT8 *pbyBuf,UINT32 dwSector,UINT32 dwCnt)
{   
    UINT8 bySta;
    UINT8 byLoop;
    UINT64 uSector;

    bySta  =SD_OK;
    uSector=(UINT64)dwSector;
    
    if(SDCardInfo.CardType!=STD_CAPACITY_SD_CARD_V1_1)
    {
        uSector<<=9;
    }
    if((UINT32)pbyBuf%4!=0)
    {
        for(byLoop=0;byLoop<dwCnt;byLoop++)
        {
            memcpy(SDIO_DATA_BUFFER,pbyBuf,CN_SDIO_DATABUF_LEN);
            //单个sector的写操作
            bySta=SD_WriteBlocks_DMA((UINT32*)SDIO_DATA_BUFFER,uSector+CN_SDIO_DATABUF_LEN*byLoop,CN_SDIO_DATABUF_LEN,1);
            pbyBuf+=CN_SDIO_DATABUF_LEN;
        }
    }
    else
    {
        //多个sector的写操作
        bySta=SD_WriteBlocks_DMA((UINT32*)pbyBuf,uSector,CN_SDIO_DATABUF_LEN,dwCnt);
    }
    return bySta;
} 
// ============================================================================
// 函数功能:SDMMC1中断服务函数
// 输入参数:无
// 返 回 值:无
// ============================================================================
void SDMMC1_IRQHandler(void)
{
    HAL_SD_IRQHandler(&SDCARD_Handler);
}
// ============================================================================
// 函数功能:DMA2数据流6中断服务函数
// 输入参数:无
// 返 回 值:无
// ============================================================================
void DMA2_Stream6_IRQHandler(void)
{
    HAL_DMA_IRQHandler(SDCARD_Handler.hdmatx);
}
// ============================================================================
// 函数功能:DMA2数据流3中断服务函数
// 输入参数:无
// 返 回 值:无
// ============================================================================
void DMA2_Stream3_IRQHandler(void)
{
    HAL_DMA_IRQHandler(SDCARD_Handler.hdmarx);
}
#else                                   //轮训模式
// ============================================================================
// 函数功能:读SD卡
// 输入参数:buf:读数据缓存区sector:扇区地址cnt:扇区个数	
// 返 回 值:0,正常;其他,错误代码;
// ============================================================================
UINT8 SD_ReadDisk(UINT8* pbyBuf,UINT32 dwSector,UINT32 dwCnt)
{
    UINT8  bySta;
    UINT8  byLoop;
    UINT64 uSector;

    bySta  =SD_OK;
    uSector=(UINT64)dwSector;
    uSector<<=9;
    
    //关闭总中断(POLLING模式,严禁中断打断SDIO读写操作!!!)
    INTX_DISABLE();
    if((UINT32)pbyBuf%4!=0)
    {
        for(byLoop=0;byLoop<dwCnt;byLoop++)
        {
            bySta=HAL_SD_ReadBlocks(&SDCARD_Handler,(UINT32*)SDIO_DATA_BUFFER,uSector+CN_SDIO_DATABUF_LEN*byLoop,CN_SDIO_DATABUF_LEN,1);//单个sector的读操作
            memcpy(pbyBuf,SDIO_DATA_BUFFER,CN_SDIO_DATABUF_LEN);
            pbyBuf+=CN_SDIO_DATABUF_LEN;
        }
    }
    else
    {
        bySta=HAL_SD_ReadBlocks(&SDCARD_Handler,(UINT32*)pbyBuf,uSector,CN_SDIO_DATABUF_LEN,dwCnt);//单个sector的读操作
    }
    //开启总中断
    INTX_ENABLE();
    return bySta;
}  
// ============================================================================
// 函数功能:写SD卡
// 输入参数:buf:写数据缓存区sector:扇区地址cnt:扇区个数	
// 返 回 值:0,正常;其他,错误代码;
// ============================================================================
UINT8 SD_WriteDisk(UINT8 *pbyBuf,UINT32 dwSector,UINT32 dwCnt)
{   
    UINT8  bySta;
    UINT8  byLoop;
    UINT64 uSector;

    bySta  =SD_OK;
    uSector=(UINT64)dwSector;
    uSector<<=9;
    //关闭总中断(POLLING模式,严禁中断打断SDIO读写操作!!!)
    INTX_DISABLE();
    if((UINT32)pbyBuf%4!=0)
    {
        for(byLoop=0;byLoop<dwCnt;byLoop++)
        {
            memcpy(SDIO_DATA_BUFFER,pbyBuf,CN_SDIO_DATABUF_LEN);
            //单个sector的写操作
            bySta=HAL_SD_WriteBlocks(&SDCARD_Handler,(UINT32*)SDIO_DATA_BUFFER,uSector+CN_SDIO_DATABUF_LEN*byLoop,CN_SDIO_DATABUF_LEN,1);
            pbyBuf+=CN_SDIO_DATABUF_LEN;
        }
    }
    else
    {
        //多个sector的写操作
        bySta=HAL_SD_WriteBlocks(&SDCARD_Handler,(UINT32*)pbyBuf,uSector,CN_SDIO_DATABUF_LEN,dwCnt);
    }
    //开启总中断
	INTX_ENABLE();
    return bySta;
}
#endif
// ============================================================================

