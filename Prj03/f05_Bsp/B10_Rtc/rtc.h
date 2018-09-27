// ============================================================================
// Copyright (C) 2017-2018  All Rights Reserved
// 模块描述: RTC组件
// 模块版本: V1.00
// 创建人员: GuXY
// 创建时间: 2017-04-18
// ============================================================================
// ============================================================================
#ifndef  _USER_RTC_H_
#define  _USER_RTC_H_

#ifdef __cplusplus
extern "C" {
#endif
// ============================================================================
#include "mySys.h"

// ============================================================================
extern RTC_HandleTypeDef RTC_Handler;  //RTC句柄

// ============================================================================   
extern UINT8  Rtc_Init(void);
extern HAL_StatusTypeDef RTC_Set_Time(UINT8 byHour,UINT8 byMin,UINT8 bySec,UINT8 byTimeFormat);
extern HAL_StatusTypeDef RTC_Set_Date(UINT8 byYear,UINT8 byMonth,UINT8 byDate,UINT8 byWeek);

// ============================================================================

#ifdef __cplusplus
}
#endif

#endif // _USER_RTC_H_

