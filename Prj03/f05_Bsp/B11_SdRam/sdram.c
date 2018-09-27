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
#include "sdram.h"
#include "delay.h"

SDRAM_HandleTypeDef SDRAM_Handler;   //SDRAM句柄
// ============================================================================
//组件内函数声明
void  SDRAM_Init_Sequence(SDRAM_HandleTypeDef *hsdram);
UINT8 SDRAM_Send_Cmd(UINT8 byBankX,UINT8 byCmd,UINT8 byRefresh,UINT16 wRegVal);
// ============================================================================
// 函数功能:SDRAM初始化
// 输入参数:无
// 返 回 值:无
// ============================================================================
void SDRAM_Init(void)
{
    FMC_SDRAM_TimingTypeDef SDRAM_Timing;

    SDRAM_Timing.LoadToActiveDelay    = 2;  //加载模式寄存器到激活时间的延迟为2个时钟周期
    SDRAM_Timing.ExitSelfRefreshDelay = 6;  //退出自刷新延迟为6个时钟周期
    SDRAM_Timing.SelfRefreshTime      = 4;  //自刷新时间为4个时钟周期                                 
    SDRAM_Timing.RowCycleDelay        = 6;  //行循环延迟为6个时钟周期
    SDRAM_Timing.WriteRecoveryTime    = 2;  //恢复延迟为2个时钟周期
    SDRAM_Timing.RPDelay              = 2;  //行预充电延迟为2个时钟周期
    SDRAM_Timing.RCDDelay             = 2;  //行到列延迟为2个时钟周期
                                                     
    SDRAM_Handler.Instance                = FMC_SDRAM_DEVICE;                         
    SDRAM_Handler.Init.SDBank             = FMC_SDRAM_BANK1;                   //第一个SDRAM BANK
    SDRAM_Handler.Init.ColumnBitsNumber   = FMC_SDRAM_COLUMN_BITS_NUM_8;       //列数量
    SDRAM_Handler.Init.RowBitsNumber      = FMC_SDRAM_ROW_BITS_NUM_13;         //行数量
    SDRAM_Handler.Init.MemoryDataWidth    = FMC_SDRAM_MEM_BUS_WIDTH_32;        //数据宽度为32位
    SDRAM_Handler.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;      //一共4个BANK
    SDRAM_Handler.Init.CASLatency         = FMC_SDRAM_CAS_LATENCY_3;           //CAS为3
    SDRAM_Handler.Init.WriteProtection    = FMC_SDRAM_WRITE_PROTECTION_DISABLE;//失能写保护
    SDRAM_Handler.Init.SDClockPeriod      = FMC_SDRAM_CLOCK_PERIOD_2;          //SDRAM时钟为HCLK/2=180M/2=90M=11.1ns
    SDRAM_Handler.Init.ReadBurst          = FMC_SDRAM_RBURST_ENABLE;           //使能突发
    SDRAM_Handler.Init.ReadPipeDelay      = FMC_SDRAM_RPIPE_DELAY_0;           //读通道延时
    
    HAL_SDRAM_Init(&SDRAM_Handler,&SDRAM_Timing);

    //发送SDRAM初始化序列	
    SDRAM_Init_Sequence(&SDRAM_Handler);
		
	//刷新频率计数器(以SDCLK频率计数),计算方法:
	//COUNT=SDRAM刷新周期/行数-20=SDRAM刷新周期(us)*SDCLK频率(Mhz)/行数
    //我们使用的SDRAM刷新周期为64ms,SDCLK=180/2=90Mhz,行数为8192(2^13).
	//所以,COUNT=64*1000*90/8192-20=683
	HAL_SDRAM_ProgramRefreshRate(&SDRAM_Handler,683);//设置刷新频率

}
// ============================================================================
// 函数功能:发送SDRAM初始化序列
// 输入参数:无
// 返 回 值:无
// ============================================================================
void SDRAM_Init_Sequence(SDRAM_HandleTypeDef *hsdram)
{
    UINT32 dwTemp;

    //初始化
    dwTemp = 0;
    
    //时钟配置使能
    SDRAM_Send_Cmd(0,FMC_SDRAM_CMD_CLK_ENABLE,1,0); 
    //至少延时200us
    delay_us(500);  
    //对所有存储区预充电
    SDRAM_Send_Cmd(0,FMC_SDRAM_CMD_PALL,1,0);   
    //设置自刷新次数 
    SDRAM_Send_Cmd(0,FMC_SDRAM_CMD_AUTOREFRESH_MODE,8,0);   

    //配置模式寄存器,SDRAM的bit0~bit2为指定突发访问的长度，
	                      //bit3为指定突发访问的类型，
	                      //bit4~bit6为CAS值
	                      //bit7~bit8为运行模式
	                      //bit9为指定的写突发模式
	                      //bit10~bit11位保留位
	dwTemp = (UINT32)SDRAM_MODEREG_BURST_LENGTH_1   |	//设置突发长度:1(可以是1/2/4/8)
              SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |	//设置突发类型:连续(可以是连续/交错)
              SDRAM_MODEREG_CAS_LATENCY_3           |	//设置CAS值:3(可以是2/3)
              SDRAM_MODEREG_OPERATING_MODE_STANDARD |   //设置操作模式:0,标准模式
              SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;     //设置突发写模式:1,单点访问

    //设置SDRAM的模式寄存器
    SDRAM_Send_Cmd(0,FMC_SDRAM_CMD_LOAD_MODE,1,dwTemp); 
}

// ============================================================================
// 函数功能:SDRAM底层驱动，引脚配置，时钟使能
// 输入参数:SDRAM句柄
// 返 回 值:无
// ============================================================================
void HAL_SDRAM_MspInit(SDRAM_HandleTypeDef *hsdram)
{
    GPIO_InitTypeDef GPIO_Initure;

    /* Enable GPIO clocks */
    __HAL_RCC_GPIOD_CLK_ENABLE();            
    __HAL_RCC_GPIOE_CLK_ENABLE();            
    __HAL_RCC_GPIOF_CLK_ENABLE();            
    __HAL_RCC_GPIOG_CLK_ENABLE();            
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOI_CLK_ENABLE(); 

    /* Enable FMC clock */
    __HAL_RCC_FMC_CLK_ENABLE(); 
    
    GPIO_Initure.Mode      = GPIO_MODE_AF_PP;        
    GPIO_Initure.Pull      = GPIO_PULLUP;            
    GPIO_Initure.Speed     = GPIO_SPEED_HIGH;        
    GPIO_Initure.Alternate = GPIO_AF12_FMC;          

    /* GPIOD configuration */  
    GPIO_Initure.Pin = GPIO_PIN_0 |GPIO_PIN_1  |GPIO_PIN_8 |GPIO_PIN_9 |GPIO_PIN_10|\
                       GPIO_PIN_14|GPIO_PIN_15;              
    HAL_GPIO_Init(GPIOD,&GPIO_Initure);     

    /* GPIOE configuration */  
    GPIO_Initure.Pin = GPIO_PIN_0 |GPIO_PIN_1  |GPIO_PIN_7 |GPIO_PIN_8 |GPIO_PIN_9 |\
                       GPIO_PIN_10| GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|\
                       GPIO_PIN_15;              
    HAL_GPIO_Init(GPIOE,&GPIO_Initure);     

    /* GPIOF configuration */  
    GPIO_Initure.Pin = GPIO_PIN_0 |GPIO_PIN_1  |GPIO_PIN_2 |GPIO_PIN_3 |GPIO_PIN_4 |\
                       GPIO_PIN_5 |GPIO_PIN_11 |GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|\
                       GPIO_PIN_15;              
    HAL_GPIO_Init(GPIOF,&GPIO_Initure);     

    /* GPIOG configuration */  
    GPIO_Initure.Pin = GPIO_PIN_0 |GPIO_PIN_1  |GPIO_PIN_2 |GPIO_PIN_4 |GPIO_PIN_5 |\
                       GPIO_PIN_8 |GPIO_PIN_15;              
    HAL_GPIO_Init(GPIOG,&GPIO_Initure);      

    /* GPIOH configuration */  
    GPIO_Initure.Pin = GPIO_PIN_2 |GPIO_PIN_3  |GPIO_PIN_5 |GPIO_PIN_8 |GPIO_PIN_9 |\
                       GPIO_PIN_10|GPIO_PIN_11 |GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|\
                       GPIO_PIN_15;
    HAL_GPIO_Init(GPIOH, &GPIO_Initure); 

    /* GPIOI configuration */  
    GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 |GPIO_PIN_2 | GPIO_PIN_3|GPIO_PIN_4 |\
                       GPIO_PIN_5 | GPIO_PIN_6 |GPIO_PIN_7 | GPIO_PIN_9|GPIO_PIN_10;
    HAL_GPIO_Init(GPIOI, &GPIO_Initure);
}

// ============================================================================
// 函数功能:向SDRAM发送命令
// 输入参数:byBankX:0,向BANK5上面的SDRAM发送指令;1,向BANK6上面的SDRAM发送指令
// 返 回 值:0,正常;1,失败.
// ============================================================================
//byCmd:指令(0,正常模式/1,时钟配置使能/2,预充电所有存储区/3,自动刷新/4,加载模式寄存器/5,自刷新/6,掉电)
//byRefresh:自刷新次数
//wRegVal:模式寄存器的定义
UINT8 SDRAM_Send_Cmd(UINT8 byBankX,UINT8 byCmd,UINT8 byRefresh,UINT16 wRegVal)
{
    UINT32 dwTargetBank;
    FMC_SDRAM_CommandTypeDef tCmd;

    dwTargetBank = 0;
    
    if(byBankX==0)
    {
        dwTargetBank = FMC_SDRAM_CMD_TARGET_BANK1;       
    }
    else if(byBankX==1)
    {
        dwTargetBank = FMC_SDRAM_CMD_TARGET_BANK2;   
    }
    
    tCmd.CommandMode            = byCmd;             //命令
    tCmd.CommandTarget          = dwTargetBank;      //目标SDRAM存储区域
    tCmd.AutoRefreshNumber      = byRefresh;         //自刷新次数
    tCmd.ModeRegisterDefinition = wRegVal;           //要写入模式寄存器的值

    //向SDRAM发送命令
    if(HAL_SDRAM_SendCommand(&SDRAM_Handler,&tCmd,0X1000)==HAL_OK) 
    {
        return 0;  
    }
    else 
    {
        return 1;    
    }
}
// ============================================================================
// 函数功能:在指定地址(dwWriteAdr+Bank5_SDRAM_ADDR)开始,连续写入dwNum个字节.
// 输入参数:pbyBuf:字节指针;WriteAddr:要写入的地址;dwNum:要写入的字节数
// 返 回 值:无
// ============================================================================
void FMC_SDRAM_WrBuf(UINT8 *pbyBuf,UINT32 dwWriteAdr,UINT32 dwNum)
{
	for(;dwNum!=0;dwNum--)
	{
		*(VUINT8*)(Bank5_SDRAM_ADDR+dwWriteAdr) = *pbyBuf;
		dwWriteAdr++;
		pbyBuf++;
	}
}
// ============================================================================
// 函数功能:在指定地址((dwWriteAdr+Bank5_SDRAM_ADDR))开始,连续读出dwNum个字节.
// 输入参数:pbyBuf:字节指针;ReadAddr:要读出的起始地址;dwNum:要写入的字节数
// 返 回 值:无
// ============================================================================
void FMC_SDRAM_RdBuf(UINT8 *pbyBuf,UINT32 dwReadAdr,UINT32 dwNum)
{
	for(;dwNum!=0;dwNum--)
	{
		*pbyBuf++ = *(VUINT8*)( Bank5_SDRAM_ADDR + dwReadAdr );
		dwReadAdr++;
	}
}
// ============================================================================

