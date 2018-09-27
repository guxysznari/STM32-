// ============================================================================
// Copyright (C) 2017-2018  All Rights Reserved
// 模块描述: LED处理组件
// 模块版本: V1.00
// 创建人员: GuXY
// 创建时间: 2017-04-18
// ============================================================================
#include "led.h" 

//*****************************************************************************
//函数功能:Led初始化函数
//输入参数:无
//返回参数:无
//*****************************************************************************
void Led_Init(void)
{ 
    GPIO_InitTypeDef GPIO_Initure;
    
    /* GPIOG Peripheral clock enable */
    __HAL_RCC_GPIOG_CLK_ENABLE();
				
		/* Configure LED in output pushpull mode */
	GPIO_Initure.Pin   = CN_LED1_PIN | CN_LED2_PIN | CN_LED3_PIN | CN_LED4_PIN;
		GPIO_Initure.Mode  = GPIO_MODE_OUTPUT_PP;
		GPIO_Initure.Pull  = GPIO_NOPULL;
		GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;	
		HAL_GPIO_Init(GPIOG, &GPIO_Initure);	

    HAL_GPIO_WritePin(GPIOG,CN_LED1_PIN,GPIO_PIN_SET); 
    HAL_GPIO_WritePin(GPIOG,CN_LED2_PIN,GPIO_PIN_SET); 
    HAL_GPIO_WritePin(GPIOG,CN_LED3_PIN,GPIO_PIN_SET); 
    HAL_GPIO_WritePin(GPIOG,CN_LED4_PIN,GPIO_PIN_SET); 
}
// ============================================================================
