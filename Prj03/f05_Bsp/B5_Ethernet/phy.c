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
#include "phy.h"
#include "delay.h"
#include "malloc.h"
#include "lwip_comm.h"

// ============================================================================
UINT8 *g_pbyRxBuf; 					//以太网底层驱动接收buffers指针 
UINT8 *g_pbyTxBuf; 					//以太网底层驱动发送buffers指针
ETH_HandleTypeDef   ETH_Handler;    //以太网句柄
ETH_DMADescTypeDef *DMARxDscrTab;	//以太网DMA接收描述符数据结构体指针
ETH_DMADescTypeDef *DMATxDscrTab;	//以太网DMA发送描述符数据结构体指针 

// ============================================================================
// 函数功能:PHY芯片初始化 
// 输入参数:无
// 返 回 值:0,成功;其他,失败
// ============================================================================
UINT8 PHY_Init(void)
{      
    UINT8 byMACAddr[6];
    
    INTX_DISABLE();                         //关闭所有中断，复位过程不能被打断！
//    PCF8574_WriteBit(ETH_RESET_IO,1);       //硬件复位
    delay_ms(100);
//    PCF8574_WriteBit(ETH_RESET_IO,0);       //复位结束
    delay_ms(100);
    INTX_ENABLE();                          //开启所有中断

    byMACAddr[0] = lwipdev.mac[0]; 
	byMACAddr[1] = lwipdev.mac[1]; 
	byMACAddr[2] = lwipdev.mac[2];
	byMACAddr[3] = lwipdev.mac[3];   
	byMACAddr[4] = lwipdev.mac[4];
	byMACAddr[5] = lwipdev.mac[5];
        
	ETH_Handler.Instance             = ETH;
    ETH_Handler.Init.MACAddr         = byMACAddr;                   //MAC地址  
    ETH_Handler.Init.AutoNegotiation = ETH_AUTONEGOTIATION_ENABLE;  //使能自协商模式 
    ETH_Handler.Init.Speed           = ETH_SPEED_100M;              //速度100M,如果开启了自协商模式，此配置就无效
    ETH_Handler.Init.DuplexMode      = ETH_MODE_FULLDUPLEX;         //全双工模式，如果开启了自协商模式，此配置就无效
    ETH_Handler.Init.MediaInterface  = ETH_MEDIA_INTERFACE_MII;     //MII接口  
    ETH_Handler.Init.RxMode          = ETH_RXPOLLING_MODE;          //查询接收模式 
    ETH_Handler.Init.ChecksumMode    = ETH_CHECKSUM_BY_HARDWARE;    //硬件帧校验  
    ETH_Handler.Init.PhyAddress      = DP83848_PHY_ADDRESS;         //PHY芯片地址  

    if(HAL_ETH_Init(&ETH_Handler)==HAL_OK)
    {
        return 0;   //成功
    }
    else
    {
        return 1;  //失败
    }
}

// ============================================================================
// 函数功能:ETH底层驱动，时钟使能，引脚配置,此函数会被HAL_ETH_Init()调用
// 输入参数:heth:以太网句柄
// 返 回 值:
// ============================================================================
void HAL_ETH_MspInit(ETH_HandleTypeDef *heth)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* Enable GPIOs clocks */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOI_CLK_ENABLE(); 

       
    /*网络引脚设置 MII接口 
    ETH_MII_TX_CLK -------------------> PC3
    ETH_MII_TX_EN --------------------> PG11
    ETH_MII_TXD0 ---------------------> PG13
    ETH_MII_TXD1 ---------------------> PG14
    ETH_MII_TXD2 ---------------------> PC2
    ETH_MII_TXD3 ---------------------> PB8
    ETH_MII_RXD0 ---------------------> PC4
    ETH_MII_RXD1 ---------------------> PC5
    ETH_MII_RXD2 ---------------------> PH6
    ETH_MII_RXD3 ---------------------> PH7
    ETH_MII_RX_ER --------------------> PI10 (not configured)        
    ETH_MII_RX_DV --------------------> PA7
    ETH_MII_RX_CLK -------------------> PA1
    ETH_MII_COL ----------------------> PH3  (not configured)
    ETH_MII_CRS ----------------------> PA0  (not configured)
    ETH_MDC --------------------------> PC1
    ETH_MDIO -------------------------> PA2
    
    ETH_PPS_OUT ----------------------> PB5  (not configured)
    */
    /* Configure PA1, PA2 and PA7 */
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pull = GPIO_NOPULL; 
    GPIO_InitStructure.Alternate = GPIO_AF11_ETH;
    GPIO_InitStructure.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_7;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure PB5 and PB8 */
    GPIO_InitStructure.Pin = GPIO_PIN_5 | GPIO_PIN_8;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    /* Configure PC1, PC2, PC3, PC4 and PC5 */
    GPIO_InitStructure.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* Configure PG11, PG14 and PG13 */
    GPIO_InitStructure.Pin =  GPIO_PIN_11 | GPIO_PIN_13 | GPIO_PIN_14;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);
    
    /* Configure PH6, PH7 */
    GPIO_InitStructure.Pin =  GPIO_PIN_6 | GPIO_PIN_7;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStructure);

    /* Enable ETHERNET clock  */
    __HAL_RCC_ETH_CLK_ENABLE();
      
//    HAL_NVIC_SetPriority(ETH_IRQn,1,0);         //网络中断优先级应该高一点
//    HAL_NVIC_EnableIRQ(ETH_IRQn);
}
// ============================================================================
// 函数功能:读取PHY寄存器值
// 输入参数:
// 返 回 值:
// ============================================================================
UINT32 PHY_ReadPHY(UINT16 wReg)
{
    UINT32 dwRegVal;
    
    HAL_ETH_ReadPHYRegister(&ETH_Handler,wReg,&dwRegVal);
    
    return dwRegVal;
}
// ============================================================================
// 函数功能:向PHY芯片指定寄存器写入值
// 输入参数:reg:要写入的寄存器value:要写入的值
// 返 回 值:
// ============================================================================
void PHY_WritePHY(UINT16 wReg,UINT16 wValue)
{
    UINT32 dwTemp;

    dwTemp = (UINT32)wValue;
    
    HAL_ETH_ReadPHYRegister(&ETH_Handler,wReg,&dwTemp);
}
// ============================================================================
// 函数功能:得到PHY芯片的速度模式
// 输入参数:
// 返 回 值:001:10M半双工101:10M全双工010:100M半双工110:100M全双工,其他:错误.
// ============================================================================
UINT8 PHY_Get_Speed(void)
{
	UINT8 bySpeed;
    
	bySpeed = ((PHY_ReadPHY(31)&0x1C)>>2); //从PHY的31号寄存器中读取网络速度和双工模式

    return bySpeed;
}

extern void lwip_pkt_handle(void);		//在lwip_comm.c里面定义
// ============================================================================
// 函数功能:中断服务函数
// 输入参数:
// 返 回 值:
// ============================================================================
void ETH_IRQHandler(void)
{
    while(ETH_GetRxPktSize(ETH_Handler.RxDesc))   
    {
        lwip_pkt_handle();//处理以太网数据，即将数据提交给LWIP
    }
    //清除中断标志位
    __HAL_ETH_DMA_CLEAR_IT(&ETH_Handler,ETH_DMA_IT_NIS);    //清除DMA中断标志位
    __HAL_ETH_DMA_CLEAR_IT(&ETH_Handler,ETH_DMA_IT_R);      //清除DMA接收中断标志位
}
// ============================================================================
// 函数功能:获取接收到的帧长度
// 输入参数:DMARxDesc:接收DMA描述符
// 返 回 值:接收到的帧长度
// ============================================================================
UINT32  ETH_GetRxPktSize(ETH_DMADescTypeDef *DMARxDesc)
{
    UINT32 dwFrameLength;

    dwFrameLength = 0;
    
    if(((DMARxDesc->Status&ETH_DMARXDESC_OWN)==(uint32_t)RESET) &&
      (( DMARxDesc->Status&ETH_DMARXDESC_ES )==(uint32_t)RESET) &&
      (( DMARxDesc->Status&ETH_DMARXDESC_LS )!=(uint32_t)RESET)) 
    {
        dwFrameLength=((DMARxDesc->Status&ETH_DMARXDESC_FL)>>ETH_DMARXDESC_FRAME_LENGTHSHIFT);
    }
    return dwFrameLength;
}
// ============================================================================
// 函数功能:为ETH底层驱动申请内存
// 输入参数:
// 返 回 值:0,正常;其他,失败
// ============================================================================
UINT8 ETH_Mem_Malloc(void)
{ 
	DMARxDscrTab = mymalloc(SRAMIN,ETH_RXBUFNB*sizeof(ETH_DMADescTypeDef));//申请内存
	DMATxDscrTab = mymalloc(SRAMIN,ETH_TXBUFNB*sizeof(ETH_DMADescTypeDef));//申请内存  
	g_pbyRxBuf = mymalloc(SRAMIN,ETH_RX_BUF_SIZE*ETH_RXBUFNB);	//申请内存
	g_pbyTxBuf = mymalloc(SRAMIN,ETH_TX_BUF_SIZE*ETH_TXBUFNB);	//申请内存
	
	if(!(UINT32)&DMARxDscrTab||!(UINT32)&DMATxDscrTab||!(UINT32)&g_pbyRxBuf||!(UINT32)&g_pbyTxBuf)
	{
		ETH_Mem_Free();
		return 1;	//申请失败
	}	
	return 0;		//申请成功
}
// ============================================================================
// 函数功能:释放ETH 底层驱动申请的内存
// 输入参数:
// 返 回 值:
// ============================================================================
void ETH_Mem_Free(void)
{ 
	myfree(SRAMIN,DMARxDscrTab);//释放内存
	myfree(SRAMIN,DMATxDscrTab);//释放内存
	myfree(SRAMIN,g_pbyRxBuf);	//释放内存
	myfree(SRAMIN,g_pbyTxBuf);	//释放内存  
}
// ============================================================================

