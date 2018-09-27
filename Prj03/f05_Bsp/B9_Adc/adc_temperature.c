// ============================================================================
// Copyright (C) 2017-2018  All Rights Reserved
// 模块描述: ADC内部温度读取组件
// 模块版本: V1.00
// 创建人员: GuXY
// 创建时间: 2017-04-18
// ============================================================================
#include "adc_temperature.h" 
#include "Delay.h"

// ============================================================================
ADC_HandleTypeDef ADC1_Handler;

// ============================================================================
//函数功能:ADC初始化函数
//输入参数:无
//返回参数:无
// ============================================================================
void Adc_Temperature_Init(void)
{ 
    __HAL_RCC_ADC1_CLK_ENABLE();            //使能ADC1时钟
    
    ADC1_Handler.Instance                   = ADC1;
    ADC1_Handler.Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV4;     //4分频，ADCCLK=PCLK2/4=90/4=22.5MHZ
    ADC1_Handler.Init.Resolution            = ADC_RESOLUTION_12B;           //12位模式
    ADC1_Handler.Init.DataAlign             = ADC_DATAALIGN_RIGHT;          //右对齐
    ADC1_Handler.Init.ScanConvMode          = DISABLE;                      //非扫描模式
    ADC1_Handler.Init.EOCSelection          = DISABLE;                      //关闭EOC中断
    ADC1_Handler.Init.ContinuousConvMode    = DISABLE;                      //关闭连续转换
    ADC1_Handler.Init.NbrOfConversion       = 1;                            //1个转换在规则序列中 也就是只转换规则序列1 
    ADC1_Handler.Init.DiscontinuousConvMode = DISABLE;                      //禁止不连续采样模式
    ADC1_Handler.Init.NbrOfDiscConversion   = 0;                            //不连续采样通道数为0
    ADC1_Handler.Init.ExternalTrigConv      = ADC_SOFTWARE_START;           //软件触发
    ADC1_Handler.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;//使用软件触发
    ADC1_Handler.Init.DMAContinuousRequests = DISABLE;                      //关闭DMA请求
    HAL_ADC_Init(&ADC1_Handler);                                            //初始化    
}
// ============================================================================
//函数功能:获得ADC值
//输入参数:通道值
//返回参数:转换结果
// ============================================================================
UINT16 getAdcVal(UINT32 dwChannel)   
{
    ADC_ChannelConfTypeDef ADC1_ChanConf;
    
    ADC1_ChanConf.Channel       = dwChannel;                    //通道
    ADC1_ChanConf.Rank          = 1;                            //1个序列
    ADC1_ChanConf.SamplingTime  = ADC_SAMPLETIME_480CYCLES;     //采样时间
    ADC1_ChanConf.Offset        = 0;                 
    HAL_ADC_ConfigChannel(&ADC1_Handler,&ADC1_ChanConf);        //通道配置
	
    HAL_ADC_Start(&ADC1_Handler);                               //开启ADC
	
    HAL_ADC_PollForConversion(&ADC1_Handler,10);                //轮询转换
   
	return (UINT16)HAL_ADC_GetValue(&ADC1_Handler);	            //返回最近一次ADC1规则组的转换结果
}
// ============================================================================
//函数功能:获取指定通道的转换值，取byTimes次,然后平均 
//输入参数:获取次数
//返回参数:byTimes次转换结果平均值
// ============================================================================
UINT16 getAdcAverageVal(UINT32 dwChannel,UINT8 byTimes)
{
	UINT32 dwTemp;
	UINT8  byLoop;

    dwTemp=0;
    
	for(byLoop=0;byLoop<byTimes;byLoop++)
	{
		dwTemp += getAdcVal(dwChannel);
		delay_ms(5);
	}
	return (UINT16)(dwTemp/byTimes);
} 
// ============================================================================
//函数功能:得到温度值
//输入参数:
//返回参数:温度值(扩大了100倍,单位:℃.)
// ============================================================================
SINT16 getTemperature(void)
{
	UINT32  dwAdcVal;
	SINT16  siResult;

    siResult = 0;
    //读取内部温度传感器通道,10次取平均
	dwAdcVal = getAdcAverageVal(ADC_CHANNEL_TEMPSENSOR,10);
    
	siResult = (((dwAdcVal*330*4*100)>>12)-76*4*100) + 25*100;  //转换为温度值,扩大100倍.
	
	return siResult;
}
// ============================================================================



