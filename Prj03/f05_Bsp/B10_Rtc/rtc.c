
// ============================================================================
// Copyright (C) 2017-2018  All Rights Reserved
// 模块描述: RTC处理组件
// 模块版本: V1.00
// 创建人员: GuXY
// 创建时间: 2017-04-18
// ============================================================================
#include "rtc.h" 

// ============================================================================
RTC_HandleTypeDef RTC_Handler;  //RTC句柄
// ============================================================================
//函数功能:RTC时间设置
//输入参数:时,分,秒,byTimeFormat:RTC_HOURFORMAT12_AM/RTC_HOURFORMAT12_PM
//返回参数:SUCEE(1)-成功,ERROR(0)-失败 
// ============================================================================
HAL_StatusTypeDef RTC_Set_Time(UINT8 byHour,UINT8 byMin,UINT8 bySec,UINT8 byTimeFormat)
{
	RTC_TimeTypeDef RTC_TimeStructure;
	
	RTC_TimeStructure.Hours          = byHour;
	RTC_TimeStructure.Minutes        = byMin;
	RTC_TimeStructure.Seconds        = bySec;
	RTC_TimeStructure.TimeFormat     = byTimeFormat;
	RTC_TimeStructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    RTC_TimeStructure.StoreOperation = RTC_STOREOPERATION_RESET;
    
	return HAL_RTC_SetTime(&RTC_Handler,&RTC_TimeStructure,RTC_FORMAT_BIN);	
}
// ============================================================================
//函数功能:RTC日期设置
//输入参数:年(0~99),月(1~12),日(0~31)week:星期(1~7,0,非法!)
//返回参数:SUCEE(1)-成功,ERROR(0)-失败 
// ============================================================================
HAL_StatusTypeDef RTC_Set_Date(UINT8 byYear,UINT8 byMonth,UINT8 byDate,UINT8 byWeek)
{
	RTC_DateTypeDef RTC_DateStructure;
    
	RTC_DateStructure.Year    = byYear;
	RTC_DateStructure.Month   = byMonth;
	RTC_DateStructure.Date    = byDate;
	RTC_DateStructure.WeekDay = byWeek;
    
	return HAL_RTC_SetDate(&RTC_Handler,&RTC_DateStructure,RTC_FORMAT_BIN);
}
// ============================================================================
//函数功能:RTC初始化函数
//输入参数:无
//返回参数:0-成功;2-失败
// ============================================================================
UINT8 Rtc_Init(void)
{      
	RTC_Handler.Instance            = RTC;
    RTC_Handler.Init.HourFormat     = RTC_HOURFORMAT_24;//RTC设置为24小时格式 
    RTC_Handler.Init.AsynchPrediv   = 0X7F;             //RTC异步分频系数(1~0X7F)
    RTC_Handler.Init.SynchPrediv    = 0XFF;             //RTC同步分频系数(0~7FFF)   
    RTC_Handler.Init.OutPut         = RTC_OUTPUT_DISABLE;     
    RTC_Handler.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    RTC_Handler.Init.OutPutType     = RTC_OUTPUT_TYPE_OPENDRAIN;
    
    if(HAL_RTC_Init(&RTC_Handler)!=HAL_OK)
    {
        return 2;
    }
      
    if(HAL_RTCEx_BKUPRead(&RTC_Handler,RTC_BKP_DR0)!=0X5050)    //是否第一次配置
    { 
        RTC_Set_Time(14,45,56,RTC_HOURFORMAT12_PM);	            //设置时间 
		RTC_Set_Date(18,3,9,5);		                            //设置日期
        HAL_RTCEx_BKUPWrite(&RTC_Handler,RTC_BKP_DR0,0X5050);   //标记已经初始化过了
    }
    return 0;
}
// ============================================================================
//函数功能:RTC时钟配置
//输入参数:RTC句柄
//返回参数:无
// ============================================================================
void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc)
{
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

    __HAL_RCC_PWR_CLK_ENABLE();//使能电源时钟PWR
	HAL_PWR_EnableBkUpAccess();//取消备份区域写保护
    
    RCC_OscInitStruct.LSEState       = RCC_LSE_ON;            //RTC使用LSE
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_NONE;
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE;//LSE配置
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;   //外设为RTC
    PeriphClkInitStruct.RTCClockSelection    = RCC_RTCCLKSOURCE_LSE;//RTC时钟源为LSE
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
        
    __HAL_RCC_RTC_ENABLE();//RTC时钟使能
}
// ============================================================================
//函数功能:设置闹钟时间(按星期闹铃,24小时制)
//输入参数:星期(1~7)
//返回参数:无
// ============================================================================
void RTC_Set_AlarmA(UINT8 byWeek,UINT8 byHour,UINT8 byMin,UINT8 bySec)
{ 
    RTC_AlarmTypeDef RTC_AlarmSturuct;
    
    RTC_AlarmSturuct.AlarmTime.Hours      = byHour;     //时
    RTC_AlarmSturuct.AlarmTime.Minutes    = byMin;      //分
    RTC_AlarmSturuct.AlarmTime.Seconds    = bySec;      //秒
    RTC_AlarmSturuct.AlarmTime.SubSeconds = 0;
    RTC_AlarmSturuct.AlarmTime.TimeFormat = RTC_HOURFORMAT12_AM;
    
    RTC_AlarmSturuct.AlarmMask            = RTC_ALARMMASK_NONE;             //精确匹配星期，时分秒
    RTC_AlarmSturuct.AlarmSubSecondMask   = RTC_ALARMSUBSECONDMASK_NONE;
    RTC_AlarmSturuct.AlarmDateWeekDaySel  = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY;//按星期
    RTC_AlarmSturuct.AlarmDateWeekDay     = byWeek;                         //星期
    RTC_AlarmSturuct.Alarm                = RTC_ALARM_A;                    //闹钟A
    
    HAL_RTC_SetAlarm_IT(&RTC_Handler,&RTC_AlarmSturuct,RTC_FORMAT_BIN);
    HAL_NVIC_SetPriority(RTC_Alarm_IRQn,0x01,0x02); //抢占优先级1,子优先级2
    HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
}
// ============================================================================
//周期性唤醒定时器设置  
//dwWclockSetVal:  @ref RTCEx_Wakeup_Timer_Definitions
//#define RTC_WAKEUPCLOCK_RTCCLK_DIV16        ((uint32_t)0x00000000)
//#define RTC_WAKEUPCLOCK_RTCCLK_DIV8         ((uint32_t)0x00000001)
//#define RTC_WAKEUPCLOCK_RTCCLK_DIV4         ((uint32_t)0x00000002)
//#define RTC_WAKEUPCLOCK_RTCCLK_DIV2         ((uint32_t)0x00000003)
//#define RTC_WAKEUPCLOCK_CK_SPRE_16BITS      ((uint32_t)0x00000004)
//#define RTC_WAKEUPCLOCK_CK_SPRE_17BITS      ((uint32_t)0x00000006)
// ============================================================================
//函数功能:cnt:自动重装载值.减到0,产生中断.
//输入参数:无
//返回参数:无
// ============================================================================
void RTC_Set_WakeUp(UINT32 dwWclockSetVal,UINT16 wCnt)
{ 
    __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&RTC_Handler, RTC_FLAG_WUTF);//清除RTC WAKE UP的标志
	
	HAL_RTCEx_SetWakeUpTimer_IT(&RTC_Handler,wCnt,dwWclockSetVal);//设置重装载值和时钟 
	
    HAL_NVIC_SetPriority(RTC_WKUP_IRQn,0x02,0x02);                //抢占优先级1,子优先级2
    HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
}
// ============================================================================
//函数功能:RTC闹钟中断服务函数
//输入参数:无
//返回参数:无
// ============================================================================
void RTC_Alarm_IRQHandler(void)
{
    HAL_RTC_AlarmIRQHandler(&RTC_Handler);
}
// ============================================================================
//函数功能:RTC闹钟A中断处理回调函数
//输入参数:无
//返回参数:无
// ============================================================================
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{

}
// ============================================================================
//函数功能:RTC WAKE UP中断服务函数
//输入参数:无
//返回参数:无
// ============================================================================
void RTC_WKUP_IRQHandler(void)
{
    HAL_RTCEx_WakeUpTimerIRQHandler(&RTC_Handler); 
}
// ============================================================================
//函数功能:RTC WAKE UP中断处理
//输入参数:无
//返回参数:无
// ============================================================================
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{

}
// ============================================================================

