// ============================================================================
// Copyright (C) 2017-2018  All Rights Reserved
// 模块描述: 延时处理函数
// 模块版本: V1.00
// 创建人员: GuXY
// 创建时间: 2017-04-18
// ============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// ============================================================================
#include "delay.h"
#include "mySys.h"
// ============================================================================
#if SYSTEM_SUPPORT_OS
#include "includes.h"					
#endif
// ============================================================================
#if SYSTEM_SUPPORT_OS		
    static UINT16 g_wFacMs=0;				        //ms延时倍乘数,在os下,代表每个节拍的ms数
#endif
static UINT32 g_dwFacUs=0;							//us延时倍乘数

// ============================================================================
#if SYSTEM_SUPPORT_OS						
//仅作UCOSII和UCOSIII的支持,其他OS,请自行参考着移植
//delay_osrunning:用于表示OS当前是否正在运行,以决定是否可以使用相关函数
//delay_ostickspersec:用于表示OS设定的时钟节拍,delay_init将根据这个参数来初始哈systick
//delay_osintnesting:用于表示OS中断嵌套级别,因为中断里面不可以调度,delay_ms使用该参数来决定如何运行
// ============================================================================
#ifdef 	OS_CRITICAL_METHOD						//要支持UCOSII				
#define delay_osrunning		OSRunning			//OS是否运行标记,0-不运行;1-运行
#define delay_ostickspersec	OS_TICKS_PER_SEC	//OS时钟节拍,即每秒调度次数
#define delay_osintnesting 	OSIntNesting		//中断嵌套级别,即中断嵌套次数
#endif
#ifdef 	CPU_CFG_CRITICAL_METHOD					//要支持UCOSIII	
#define delay_osrunning		OSRunning			//OS是否运行标记,0-不运行;1-运行
#define delay_ostickspersec	OSCfg_TickRate_Hz	//OS时钟节拍,即每秒调度次数
#define delay_osintnesting 	OSIntNestingCtr		//中断嵌套级别,即中断嵌套次数
#endif

// ============================================================================
// 函数功能:us级延时时,关闭任务调度(防止打断us级延迟),用于锁定OS任务调度,禁止调度
// ============================================================================
void delay_osschedlock(void)
{
#ifdef CPU_CFG_CRITICAL_METHOD   			//使用UCOSIII
	OS_ERR err; 
	OSSchedLock(&err);						//UCOSIII的方式,禁止调度，防止打断us延时
#else										//否则UCOSII
	OSSchedLock();							//UCOSII的方式,禁止调度，防止打断us延时
#endif
}
// ============================================================================
// 函数功能:us级延时时,恢复任务调度,用于解锁OS任务调度,重新开启调度
// ============================================================================
void delay_osschedunlock(void)
{	
#ifdef CPU_CFG_CRITICAL_METHOD   			//使用UCOSIII
	OS_ERR err; 
	OSSchedUnlock(&err);					//UCOSIII的方式,恢复调度
#else										//否则UCOSII
	OSSchedUnlock();						//UCOSII的方式,恢复调度
#endif
}
// ============================================================================
// 函数功能:调用OS自带的延时函数延时,用于OS延时,可以引起任务调度.
// 输入参数:dwTicks-延时节拍数
// 返 回 值:无
// ============================================================================
void delay_ostimedly(UINT32 dwTicks)
{
#ifdef CPU_CFG_CRITICAL_METHOD
	OS_ERR err; 
	OSTimeDly(dwTicks,OS_OPT_TIME_PERIODIC,&err); //UCOSIII延时采用周期模式
#else
	OSTimeDly(dwTicks);						    //UCOSII延时
#endif 
}
// ============================================================================
// 功能描述:systick中断服务函数,使用OS时用到
// ============================================================================
void SysTick_Handler(void)
{	
    HAL_IncTick();
	if(delay_osrunning==1)					//OS开始跑了,才执行正常的调度处理
	{
		OSIntEnter();						//进入中断
		OSTimeTick();       				//调用ucos的时钟服务程序               
		OSIntExit();       	 				//触发任务切换软中断
	}
}
// ============================================================================
#endif //end-SYSTEM_SUPPORT_OS
// ============================================================================
// 函数功能:初始化延迟函数,当使用ucos的时候,此函数会初始化ucos的时钟节拍
// 输入参数:bySysClk-系统时钟频率,SYSTICK的时钟固定为AHB时钟
// 返 回 值:无
// ============================================================================
void delay_init(UINT8 bySysClk)
{
#if SYSTEM_SUPPORT_OS 						
	UINT32 dwReload;//reload为24位寄存器,最大值:16777216,在180M下,约合0.745s左右	
#endif

    //SysTick频率为HCLK
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

    g_dwFacUs = bySysClk;						

#if SYSTEM_SUPPORT_OS 						//如果需要支持OS.
	dwReload = bySysClk;					    //每秒钟的计数次数 单位为K
	//根据delay_ostickspersec设定溢出时间
	dwReload = dwReload * (1000000/delay_ostickspersec);	
	g_wFacMs = 1000/delay_ostickspersec;		//代表OS可以延时的最少单位	   
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;  //开启SYSTICK中断
	SysTick->LOAD  = dwReload; 					//每1/OS_TICKS_PER_SEC秒中断一次	
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;   //开启SYSTICK
#endif

}								    

// ============================================================================
#if SYSTEM_SUPPORT_OS 						
// ============================================================================
// 函数功能:us级延时函数
// 输入参数:要延时的us数(0~190887435)(最大值即2^32/g_dwFacUs@fac_us=22.5)	
// 返 回 值:无
// ============================================================================
void delay_us(UINT32 dwUsCnt)
{		
	UINT32 dwTicks;
	UINT32 dwTOld,dwTNow,dwTCnt;
	UINT32 dwReload;
	
    dwTCnt   = 0;
    dwReload = SysTick->LOAD;           //LOAD的值  
    dwTicks  = dwUsCnt * g_dwFacUs; 	//需要的节拍数 
	delay_osschedlock();				//阻止OS调度，防止打断us延时
	dwTOld = SysTick->VAL;        		//刚进入时的计数器值
	
	while(1)
	{
		dwTNow = SysTick->VAL;	
		if(dwTNow != dwTOld)
		{	    
			if(dwTNow < dwTOld)
			{
                dwTCnt = dwTCnt + ( dwTOld - dwTNow );  
            }
			else 
			{
                dwTCnt = dwTCnt + ( dwReload - dwTNow + dwTOld );     
            }
			dwTOld=dwTNow;
			if(dwTCnt >= dwTicks)
			{
                break;          //时间超过/等于要延迟的时间,则退出.
            }
		}  
	};
	delay_osschedunlock();					//恢复OS调度											    
}
// ============================================================================
// 函数功能:ms级延时函数
// 输入参数:要延时的ms数(0~65535)
// 返 回 值:无
// ============================================================================
void delay_ms(UINT16 wMsCnt)
{	
	if(delay_osrunning&&delay_osintnesting==0)//如果OS已经在跑了,并且不是在中断里面(中断里面不能任务调度)	    
	{		 
		if(wMsCnt>=g_wFacMs)						//延时的时间大于OS的最少时间周期 
		{ 
   			delay_ostimedly(wMsCnt/g_wFacMs);	//OS延时
		}
		wMsCnt%=g_wFacMs;						//OS已经无法提供这么小的延时了,采用普通方式延时    
	}
	delay_us((UINT32)(wMsCnt*1000));				//普通方式延时
}

// ============================================================================
#else  //else-SYSTEM_SUPPORT_OS
// ============================================================================
// 函数功能:us级延时函数
// 输入参数:nus为要延时的us数,0~190887435(最大值即2^32/g_dwFacUs@fac_us=22.5)	 
// 返 回 值:无
// ============================================================================
void delay_us(UINT32 dwUsCnt)
{		
	UINT32 dwTicks;
	UINT32 dwTOld,dwTNow,dwTCnt;
	UINT32 dwReload;
    
    dwTCnt   = 0;
	dwReload = SysTick->LOAD;         // LOAD的值
	dwTicks  = dwUsCnt * g_dwFacUs;   // 需要的节拍数 
	dwTOld   = SysTick->VAL;          // 刚进入时的计数器值
	
	while(1)
	{
		dwTNow = SysTick->VAL;	
        
		if(dwTNow != dwTOld)
		{	    
			if(dwTNow<dwTOld)
			{
                dwTCnt = dwTCnt + ( dwTOld - dwTNow );
            }
			else 
			{
                dwTCnt = dwTCnt + ( dwReload - dwTNow + dwTOld );       
            }

            dwTOld = dwTNow;
            
            //时间超过/等于要延迟的时间,则退出
			if(dwTCnt >= dwTicks)
            {
                break;         
            }
		}  
	};
}

// ============================================================================
// 函数功能:ms级延时函数
// 输入参数:要延时的ms数
// 返 回 值:无
// ============================================================================
void delay_ms(UINT16 wMsCnt)
{
	UINT32 dwLoop;
	for(dwLoop=0;dwLoop<wMsCnt;dwLoop++)
    {
        delay_us(1000);
    }
}
// ============================================================================
#endif//end-SYSTEM_SUPPORT_OS
// ============================================================================

