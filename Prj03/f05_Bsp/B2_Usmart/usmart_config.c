// ============================================================================
// Copyright (C) 2017-2018  All Rights Reserved
// 模块描述: 用户配置模块
// 模块版本: V1.00
// 创建人员: GuXY
// 创建时间: 2017-04-18
// ============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// ============================================================================
#include "usmart.h"
#include "usmart_str.h"

// ============================================================================
// 要包含所用到的函数所申明的头文件(用户添加) 
#include "delay.h"	 	
#include "mySys.h"
#include "rtc.h" 
//#include "lcd.h"
//#include "sdram.h"
//#include "ltdc.h"
								 
//extern void led_set(UINT8 bySta);
//extern void test_fun(void(*ledset)(UINT8),UINT8 bySta);	

// ============================================================================
//函数名列表初始化(用户添加)
tagUsmartFuncNameList usmart_namelist[]=
{
#if USMART_USE_WRFUNS==1 	//如果使能了读写操作
	(void*)read_addr, "UINT32  read_addr(UINT32 dwAddr);",
	(void*)write_addr,"void write_addr(UINT32 dwAddr,UINT32 dwVal)",	 
#endif		   
	(void*)delay_ms,"void delay_ms(UINT16 wMsCnt)",
 	(void*)delay_us,"void delay_us(UINT32 dwUsCnt)",	
    (HAL_StatusTypeDef*)RTC_Set_Time,"HAL_StatusTypeDef RTC_Set_Time(UINT8 byHour,UINT8 byMin,UINT8 bySec,UINT8 byTimeFormat)",
    (HAL_StatusTypeDef*)RTC_Set_Date,"HAL_StatusTypeDef RTC_Set_Date(UINT8 byYear,UINT8 byMonth,UINT8 byDate,UINT8 byWeek)",

//	(void*)led_set, "void led_set(UINT8 bySta)",
//	(void*)test_fun,"void test_fun(void(*ledset)(UINT8),UINT8 bySta)",						
};
// ============================================================================
// 函数控制管理器初始化,得到各个受控函数的名字,函数总数量
// ============================================================================
tagUsmartDev usmart_dev=
{
	usmart_namelist,
	usmart_init,
	usmart_exe,
	usmart_scan,
    usmart_cmd_rec,
	sizeof(usmart_namelist)/sizeof(tagUsmartFuncNameList),//函数数量
	0,	  	//参数数量
	0,	 	//函数ID
	1,		//参数显示类型,0,10进制;1,16进制
	0,		//参数类型.bitx:,0,数字;1,字符串	    
	0,	  	//每个参数的长度暂存表,需要MAX_PARM个0初始化
	0,		//函数的参数,需要PARM_LEN个0初始化
};   

// ============================================================================

