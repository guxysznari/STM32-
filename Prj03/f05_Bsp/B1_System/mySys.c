// ============================================================================
// Copyright (C) 2017-2018  All Rights Reserved
// 模块描述: 系统时钟初始化,时钟设置/中断管理/GPIO设置等
// 模块版本: V1.00
// 创建人员: GuXY
// 创建时间: 2017-04-18
// ============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// ============================================================================

#include "mySys.h"
// ============================================================================
#define CN_PLL_M  (25 )//主PLL和音频PLL分频系数(PLL之前的分频),取值范围:2~63.
#define CN_PLL_N  (360)//主PLL倍频系数(PLL倍频),取值范围:64~432.
#define CN_PLL_P  (RCC_PLLP_DIV2)//系统时钟的主PLL分频系数(PLL之后的分频),取值范围:2,4,6,8.(仅限这4个值!)
#define CN_PLL_Q  (7)//USB/SDIO/随机数产生器等的主PLL分频系数(PLL之后的分频),取值范围:2~15.

// ============================================================================
// 函数功能:系统时钟配置
// Fvco(VCO频率)=Fs*(plln/pllm);
// SYSCLK(系统时钟频率)=Fvco/pllp=Fs*(plln/(pllm*pllp));
// Fusb(USB,SDIO,RNG等的时钟频率)=Fvco/pllq=Fs*(plln/(pllm*pllq));
// Fs:PLL输入时钟频率,可以是HSI,HSE等. 
// 外部晶振为25M的时候,推荐值:plln=360,pllm=25,pllp=2,pllq=8.
// 得到:Fvco=25*(360/25)=360Mhz,SYSCLK=360/2=180Mhz,Fusb=360/8=45Mhz
// ============================================================================
void SystemClock_Config(void)
{    
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;
    
    //使能PWR时钟
    __HAL_RCC_PWR_CLK_ENABLE(); 
    
    /* 下面这个设置用来设置调压器输出电压级别，以便在器件未以最大频率工作
    时使性能与功耗实现平衡，此功能只有STM32F42xx和STM32F43xx器件有 */
    
    //设置调压器输出电压级别1
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;  //时钟源为HSE
    RCC_OscInitStruct.HSEState       = RCC_HSE_ON;              //打开HSE
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;              //打开PLL
    RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;       //PLL时钟源选择HSE
    RCC_OscInitStruct.PLL.PLLM = CN_PLL_M; //主PLL和音频PLL分频系数(PLL之前的分频),取值范围:2~63.
    RCC_OscInitStruct.PLL.PLLN = CN_PLL_N; //主PLL倍频系数(PLL倍频),取值范围:64~432.  
    RCC_OscInitStruct.PLL.PLLP = CN_PLL_P; //系统时钟的主PLL分频系数(PLL之后的分频),取值范围:2,4,6,8.(仅限这4个值!)
    RCC_OscInitStruct.PLL.PLLQ = CN_PLL_Q; //USB/SDIO/随机数产生器等的主PLL分频系数(PLL之后的分频),取值范围:2~15.
    //初始化
    if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler();
    }    
    //开启Over-Driver功能
    if(HAL_PWREx_EnableOverDrive() != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler();
    }   
    //选中PLL作为系统时钟源并且配置HCLK,PCLK1和PCLK2
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;//设置系统时钟时钟源为PLL
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;//AHB分频系数为1
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  //APB1分频系数为4
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  //APB2分频系数为2 
    //同时设置FLASH延时周期为5WS，也就是6个CPU周期。
    if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler();
    }
}

// ============================================================================
// @brief  This function is executed in case of error occurrence.
// @param  None
// @retval None
// ============================================================================
void Error_Handler(void)
{
    /* User may add here some code to deal with this error */
    while(1)
    {
        
    }
}
// ============================================================================
#ifdef  USE_FULL_ASSERT
//当编译提示出错的时候此函数用来报告错误的文件和所在行
//file：指向源文件
//line：指向在文件中的行数
void assert_failed(uint8_t* file, uint32_t line)
{ 
	while (1)
	{
	}
}
#endif
// ============================================================================

// ============================================================================
// THUMB指令不支持汇编内联
// ============================================================================
// 函数功能:采用如下方法实现执行汇编指令WFI  
// 输入参数:无
// 返 回 值:无
// ============================================================================
__asm void WFI_SET(void)
{
	WFI;		  
}
// ============================================================================
// 函数功能:关闭所有中断(但是不包括fault和NMI中断)
// 输入参数:无
// 返 回 值:无
// ============================================================================
__asm void INTX_DISABLE(void)
{
	CPSID   I
	BX      LR	  
}
// ============================================================================
// 函数功能:开启所有中断
// 输入参数:无
// 返 回 值:无
// ============================================================================
__asm void INTX_ENABLE(void)
{
	CPSIE   I
	BX      LR  
}
// ============================================================================
// 函数功能:设置栈顶地址
// 输入参数:addr:栈顶地址
// 返 回 值:无
// ============================================================================
__asm void MSR_MSP(UINT32 addr) 
{
	MSR MSP, r0 			//set Main Stack value
	BX r14
}
// ============================================================================

