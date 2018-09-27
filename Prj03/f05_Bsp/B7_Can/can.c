// ============================================================================
// Copyright (C) 2017-2018  All Rights Reserved
// 模块描述: CAN通信驱动
// 模块版本: V1.00
// 创建人员: GuXY
// 创建时间: 2017-04-18
// ============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// ============================================================================
#include "can.h"
#include "usart.h"
#include "delay.h"

// ============================================================================
CAN_HandleTypeDef   CAN1_Handler;   //CAN1句柄
CanTxMsgTypeDef     TxMessage;      //发送消息
CanRxMsgTypeDef     RxMessage;      //接收消息
// ============================================================================
// 函数功能:CAN初始化
// 输入参数: 波特率=Fpclk1/((tbs1+tbs2+1)*brp); 波特率:45M/((6+8+1)*6)=500Kbps
// 返 回 值:0-初始化OK;其他-初始化失败; 
// ============================================================================
UINT8 CAN1_Mode_Init(void)
{
    CAN_FilterConfTypeDef  CAN1_FilerConf;
    
    CAN1_Handler.Instance       = CAN1; 
    CAN1_Handler.pTxMsg         = &TxMessage;           //发送消息
    CAN1_Handler.pRxMsg         = &RxMessage;           //接收消息
    CAN1_Handler.Init.Prescaler = 6;                    //分频系数(Fdiv)为brp+1
    CAN1_Handler.Init.Mode      = CAN_MODE_LOOPBACK;    //模式设置 
    CAN1_Handler.Init.SJW       = CAN_SJW_1TQ;          //重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位 CAN_SJW_1TQ~CAN_SJW_4TQ
    CAN1_Handler.Init.BS1       = CAN_BS1_8TQ;          //tbs1范围CAN_BS1_1TQ~CAN_BS1_16TQ
    CAN1_Handler.Init.BS2       = CAN_BS2_6TQ;          //tbs2范围CAN_BS2_1TQ~CAN_BS2_8TQ
    CAN1_Handler.Init.TTCM      = DISABLE;              //非时间触发通信模式 
    CAN1_Handler.Init.ABOM      = DISABLE;              //软件自动离线管理
    CAN1_Handler.Init.AWUM      = DISABLE;              //睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
    CAN1_Handler.Init.NART      = ENABLE;               //禁止报文自动传送 
    CAN1_Handler.Init.RFLM      = DISABLE;              //报文不锁定,新的覆盖旧的 
    CAN1_Handler.Init.TXFP      = DISABLE;              //优先级由报文标识符决定 
	
    if(HAL_CAN_Init(&CAN1_Handler)!=HAL_OK)
    {
        return 1;   //初始化
    }
    
    CAN1_FilerConf.FilterIdHigh         = 0X0000;           //32位ID
    CAN1_FilerConf.FilterIdLow          = 0X0000;
    CAN1_FilerConf.FilterMaskIdHigh     = 0X0000;           //32位MASK
    CAN1_FilerConf.FilterMaskIdLow      = 0X0000;  
    CAN1_FilerConf.FilterFIFOAssignment = CAN_FILTER_FIFO0; //过滤器0关联到FIFO0
    CAN1_FilerConf.FilterNumber         = 0;                //过滤器0
    CAN1_FilerConf.FilterMode           = CAN_FILTERMODE_IDMASK;
    CAN1_FilerConf.FilterScale          = CAN_FILTERSCALE_32BIT;
    CAN1_FilerConf.FilterActivation     = ENABLE;           //激活滤波器0
    CAN1_FilerConf.BankNumber           = 14;
	
    if(HAL_CAN_ConfigFilter(&CAN1_Handler,&CAN1_FilerConf)!=HAL_OK)
    {
        return 2;//滤波器初始化
    }
	
    return 0;
}
// ============================================================================
// 函数功能:CAN底层驱动，引脚配置，时钟配置，中断配置,HAL_CAN_Init()函数调用
// 输入参数:hcan:CAN句柄
// 返 回 值:
// ============================================================================
void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_CAN1_CLK_ENABLE();                
    __HAL_RCC_GPIOA_CLK_ENABLE();			   
	
    GPIO_Initure.Pin        = GPIO_PIN_11|GPIO_PIN_12;  
    GPIO_Initure.Mode       = GPIO_MODE_AF_PP;          //推挽复用
    GPIO_Initure.Pull       = GPIO_PULLUP;              //上拉
    GPIO_Initure.Speed      = GPIO_SPEED_FAST;          //快速
    GPIO_Initure.Alternate  = GPIO_AF9_CAN1;            //复用为CAN1
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);                 //初始化
    
#if CAN1_RX0_INT_ENABLE
    //FIFO0消息挂起中断允许.      
    __HAL_CAN_ENABLE_IT(&CAN1_Handler,CAN_IT_FMP0);
    //CAN1->IER|=1<<1;		//FIFO0消息挂起中断允许.
    //抢占优先级1，子优先级2
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn,1,2);   
    //使能中断
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);          
#endif	
}

#if CAN1_RX0_INT_ENABLE                         
// ============================================================================
// 函数功能:CAN中断服务函数
// 输入参数:
// 返 回 值:
// ============================================================================
void CAN1_RX0_IRQHandler(void)
{
    HAL_CAN_IRQHandler(&CAN1_Handler);
}
// ============================================================================
// 函数功能:CAN中断服务函数
// 输入参数:CAN句柄
// 返 回 值:
// ============================================================================
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{
    UINT8 byLoop;

    byLoop=0;
    //重新开启FIF00消息挂号中断
    __HAL_CAN_ENABLE_IT(&CAN1_Handler,CAN_IT_FMP0);
    
    printf("id:%d\r\n" ,CAN1_Handler.pRxMsg->StdId);
    printf("ide:%d\r\n",CAN1_Handler.pRxMsg->IDE);
    printf("rtr:%d\r\n",CAN1_Handler.pRxMsg->RTR);
    printf("len:%d\r\n",CAN1_Handler.pRxMsg->DLC);
    for(byLoop=0;byLoop<8;byLoop++)
    {
        printf("rxbuf[%d]:%d\r\n",byLoop,CAN1_Handler.pRxMsg->Data[byLoop]);
    }
}
#endif	
// ============================================================================
// 函数功能:发送一组数据
// 输入参数:
// 返 回 值:0-成功;其他-失败;
// ============================================================================
UINT8 CAN1_Send_Msg(tagCanTxMsg *ptCanTxMsg)
{	
    UINT16 wLoop;
    
    CAN1_Handler.pTxMsg->StdId = ptCanTxMsg->dwMsgID;        //标准标识符
    CAN1_Handler.pTxMsg->ExtId = ptCanTxMsg->dwMsgID;        //扩展标识符(29位)
    CAN1_Handler.pTxMsg->IDE   = ptCanTxMsg->dwIDE;          //使用标准帧-CAN_ID_STD;扩展帧-CAN_ID_EXT
    CAN1_Handler.pTxMsg->RTR   = ptCanTxMsg->dwRTR;          //数据帧-CAN_RTR_DATA
    CAN1_Handler.pTxMsg->DLC   = ptCanTxMsg->dwDLC;          //数据长度             
    for(wLoop=0;wLoop<(ptCanTxMsg->dwDLC);wLoop++)
    {
        CAN1_Handler.pTxMsg->Data[wLoop]=ptCanTxMsg->pbyMsg[wLoop];
    }
    if(HAL_CAN_Transmit(&CAN1_Handler,10)!=HAL_OK)
    {
        return 1;  
    }
    return 0;       
}
// ============================================================================
// 函数功能:接收数据查询
// 输入参数:数据缓存区;	 
// 返 回 值:0-无数据被收到;其他-接收的数据长度;
// ============================================================================
UINT8 CAN1_Receive_Msg(UINT8 *pbyBuf)
{		   		   
 	UINT32 dwLoop;
    
    if(HAL_CAN_Receive(&CAN1_Handler,CAN_FIFO0,0)!=HAL_OK)
    {
        return 0;//接收数据，超时时间设置为0    
    }
    for(dwLoop=0;dwLoop<CAN1_Handler.pRxMsg->DLC;dwLoop++)
    {
        pbyBuf[dwLoop]=CAN1_Handler.pRxMsg->Data[dwLoop];
    }
	return CAN1_Handler.pRxMsg->DLC;	
}
// ============================================================================

