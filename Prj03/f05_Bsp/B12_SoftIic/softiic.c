// ============================================================================
// Copyright (C) 2017-2018  All Rights Reserved
// 模块描述: IIC软件实现组件
// 模块版本: V1.00
// 创建人员: GuXY
// 创建时间: 2017-04-18
// ============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// ============================================================================
#include "softiic.h"
#include "delay.h"

// ============================================================================
//IIC所有操作函数
void  SoftIIC_Init(void);                   //初始化IIC的IO口				 
void  SoftIIC_Start(void);				    //发送IIC开始信号
void  SoftIIC_Stop(void);	  			    //发送IIC停止信号
void  SoftIIC_Ack(void);					//IIC发送ACK信号
void  SoftIIC_NAck(void);				    //IIC不发送ACK信号
void  SoftIIC_Send_Byte(UINT8);			    //IIC发送一个字节
UINT8 SoftIIC_Read_Byte(UINT8);             //IIC读取一个字节
UINT8 SoftIIC_Wait_Ack(void); 				//IIC等待ACK信号
// ============================================================================
// 函数功能:IIC初始化
// 输入参数:无
// 返 回 值:无
// ============================================================================
void SoftIIC_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOB_CLK_ENABLE();   
    
    //PH4,5初始化设置
    GPIO_Initure.Pin    = GPIO_PIN_6|GPIO_PIN_9;
    GPIO_Initure.Mode   = GPIO_MODE_OUTPUT_PP;      //推挽输出
    GPIO_Initure.Pull   = GPIO_PULLUP;              //上拉
    GPIO_Initure.Speed  = GPIO_SPEED_FAST;          //快速
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
    
    MACRO_IIC_SDA = 1;
    MACRO_IIC_SCL = 1;  
}
// ============================================================================
// 函数功能:产生IIC起始信号
// 输入参数:无
// 返 回 值:无
// ============================================================================
void SoftIIC_Start(void)
{
	MACRO_SDA_OUT();    
	MACRO_IIC_SDA=1;	  	  
	MACRO_IIC_SCL=1;
	delay_us(4);
 	MACRO_IIC_SDA=0;
	delay_us(4);
	MACRO_IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
}
// ============================================================================
// 函数功能:产生IIC停止信号
// 输入参数:无
// 返 回 值:无
// ============================================================================
void SoftIIC_Stop(void)
{
	MACRO_SDA_OUT();
	MACRO_IIC_SCL=0;
	MACRO_IIC_SDA=0;
 	delay_us(4);
	MACRO_IIC_SCL=1; 
	MACRO_IIC_SDA=1;//发送I2C总线结束信号
	delay_us(4);							   	
}

// ============================================================================
// 函数功能:等待应答信号到来
// 输入参数:无
// 返 回 值:1-失败,0-成功
// ============================================================================
UINT8 SoftIIC_Wait_Ack(void)
{
	UINT8 byTimeOut;
    
	MACRO_SDA_IN();      //SDA设置为输入  
	MACRO_IIC_SDA=1;
    delay_us(1);	   
	MACRO_IIC_SCL=1;
    delay_us(1);	

    byTimeOut = 0;
    
	while(MACRO_READ_SDA)
	{
		byTimeOut++;
        
		if(byTimeOut>250)
		{
			SoftIIC_Stop();
			return 1;
		}
	}
	MACRO_IIC_SCL=0;//时钟输出0 	   
	return 0;  
} 
// ============================================================================
// 函数功能:产生ACK应答
// 输入参数:无
// 返 回 值:无
// ============================================================================
void SoftIIC_Ack(void)
{
	MACRO_IIC_SCL=0;
	MACRO_SDA_OUT();
	MACRO_IIC_SDA=0;
	delay_us(2);
	MACRO_IIC_SCL=1;
	delay_us(2);
	MACRO_IIC_SCL=0;
}
// ============================================================================
// 函数功能:不产生ACK应答		  
// 输入参数:无
// 返 回 值:无
// ============================================================================
void SoftIIC_NAck(void)
{
	MACRO_IIC_SCL=0;
	MACRO_SDA_OUT();
	MACRO_IIC_SDA=1;
	delay_us(2);
	MACRO_IIC_SCL=1;
	delay_us(2);
	MACRO_IIC_SCL=0;
}	
// ============================================================================
// 函数功能:IIC发送一个字节
// 输入参数:无
// 返 回 值:1-有应答,0-无应答(返回从机有无应答)	
// ============================================================================
void SoftIIC_Send_Byte(UINT8 byTxData)
{                        
    UINT8 byLoop;   
    
	MACRO_SDA_OUT(); 
    
    //拉低时钟开始数据传输
    MACRO_IIC_SCL=0;
    
    for(byLoop=0;byLoop<8;byLoop++)
    {              
        MACRO_IIC_SDA=(byTxData&0x80)>>7;
        byTxData<<=1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
		MACRO_IIC_SCL=1;
		delay_us(2); 
		MACRO_IIC_SCL=0;	
		delay_us(2);
    }	 
} 	  
// ============================================================================
// 函数功能:读1个字节
// 输入参数:无
// 返 回 值:无
// ============================================================================
UINT8 SoftIIC_Read_Byte(UINT8 byAck)
{
	UINT8 byLoop,byRecv=0;
    
    //SDA设置为输入
	MACRO_SDA_IN();
    
    for(byLoop=0;byLoop<8;byLoop++)
	{
        MACRO_IIC_SCL=0; 
        delay_us(2);
		MACRO_IIC_SCL=1;
        byRecv<<=1;
        if(MACRO_READ_SDA)
        {
            byRecv++;   
        }
		delay_us(1); 
    }	
    
    if(!byAck)
    {
        SoftIIC_NAck();//发送nACK
    }
    else
    {
        SoftIIC_Ack(); //发送ACK   
    }
    
    return byRecv;
}
// ============================================================================

