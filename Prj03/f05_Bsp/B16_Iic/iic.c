// ============================================================================
// Copyright (C) 2017-2018  All Rights Reserved
// 模块描述: IIC实现组件
// 模块版本: V1.00
// 创建人员: GuXY
// 创建时间: 2017-04-18
// ============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// ============================================================================
#include "iic.h"
#include "delay.h"

// ============================================================================
#define I2C1_ADDRESS1           ( 0x3A       )
#define I2C1_SPEED              ( 100000     )                
// ============================================================================
I2C_HandleTypeDef I2C1_Handler;
// ============================================================================
//IIC所有操作函数
void  IIC1_MspInit(void);                   //初始化IIC的IO口				 
// ============================================================================
// 函数功能:IIC初始化
// 输入参数:无
// 返 回 值:无
// ============================================================================
void IIC1_Init(void)
{
    if(HAL_I2C_GetState(&I2C1_Handler) == HAL_I2C_STATE_RESET)
    {
        I2C1_Handler.Instance             = I2C1;
        I2C1_Handler.Init.ClockSpeed      = I2C1_SPEED;
        I2C1_Handler.Init.DutyCycle       = I2C_DUTYCYCLE_2;
        I2C1_Handler.Init.OwnAddress1     = I2C1_ADDRESS1;
        I2C1_Handler.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
        I2C1_Handler.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
        I2C1_Handler.Init.OwnAddress2     = 0;
        I2C1_Handler.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
        I2C1_Handler.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;

        IIC1_MspInit();
        HAL_I2C_Init(&I2C1_Handler);
    }
}
// ============================================================================
// 函数功能:初始化IIC的IO口			
// 输入参数:无
// 返 回 值:无
// ============================================================================
void IIC1_MspInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStruct;  
    
    __HAL_RCC_GPIOB_CLK_ENABLE();
    
    GPIO_InitStruct.Pin       = GPIO_PIN_6;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* Enable I2C clock */
    __HAL_RCC_I2C1_CLK_ENABLE();
    
    /* Force the I2C peripheral clock reset */  
    __HAL_RCC_I2C1_FORCE_RESET(); 

    /* Release the I2C peripheral clock reset */  
    __HAL_RCC_I2C1_RELEASE_RESET(); 

    /* Enable and set I2Cx Interrupt to a lower priority */
    HAL_NVIC_SetPriority(I2C1_EV_IRQn, 0x05, 0);
    HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);

    /* Enable and set I2Cx Interrupt to a lower priority */
    HAL_NVIC_SetPriority(I2C1_ER_IRQn, 0x05, 0);
    HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
}
// ============================================================================
// 函数功能:IIC1写数据
// 输入参数:无
// 返 回 值:无
// ============================================================================
void IIC1_Write_Byte(UINT8 byAddr, UINT8 byReg, UINT8 byValue)
{
    HAL_I2C_Mem_Write(&I2C1_Handler, byAddr, (UINT16)byReg, I2C_MEMADD_SIZE_8BIT, &byValue, 1, 100); 
}
// ============================================================================
// 函数功能:IIC1读数据
// 输入参数:无
// 返 回 值:所读取的值
// ============================================================================
UINT8 IIC1_Read_Byte(UINT8 byAddr, UINT8 byReg)
{
    UINT8 byValue=0;
    
    HAL_I2C_Mem_Read(&I2C1_Handler, byAddr, (UINT16)byReg, I2C_MEMADD_SIZE_8BIT, &byValue, 1, 1000); 

    return byValue;
}
// ============================================================================
// 函数功能:IIC1写多个数据
// 输入参数:无
// 返 回 值:无
// ============================================================================
void IIC1_Write_Bytes(UINT8 byAddr, UINT8 byReg, UINT16 wMemAdr,UINT8 *pbyBuf,UINT16 wLen)
{
    HAL_I2C_Mem_Write(&I2C1_Handler, byAddr, (UINT16)byReg, wMemAdr, pbyBuf, wLen, 1000);
}
// ============================================================================
// 函数功能:IIC1读多个数据
// 输入参数:无
// 返 回 值:所读取的值
// ============================================================================
void IIC1_Read_Bytes(UINT8 byAddr, UINT8 byReg,UINT16 wMemAdr,UINT8 *pbyBuf,UINT16 wLen)
{
    HAL_I2C_Mem_Read(&I2C1_Handler, byAddr, (UINT16)byReg, wMemAdr, pbyBuf, wLen, 1000);
}
// ============================================================================

