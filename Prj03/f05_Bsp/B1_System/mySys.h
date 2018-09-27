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
#ifndef __SYS_H__
#define __SYS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx.h"
#include "stm32f4xx_hal_conf.h"
#include "Typedef.h"
#include "Logicgate.h"

// ============================================================================
#define SYSTEM_SUPPORT_OS		0		//系统文件夹是否支持OS(0:不支持os,1:支持os)
// ============================================================================
//定义一些常用的数据类型短关键字 
typedef int8_t     s8;
typedef int16_t    s16;
typedef int32_t    s32;

typedef uint8_t    u8;
typedef int16_t    u16;
typedef int32_t    u32;

typedef __IO  int8_t    vs8;
typedef __IO  int16_t   vs16;
typedef __IO  int32_t   vs32;

typedef __IO  uint8_t   vu8;
typedef __IO  uint16_t  vu16;
typedef __IO  uint32_t  vu32;

typedef __I   int8_t    vsc8;   
typedef __I   int16_t   vsc16; 
typedef __I   int32_t   vsc32; 

typedef __I   uint8_t   vuc8;  
typedef __I   uint16_t  vuc16; 
typedef __I   uint32_t  vuc32;  

typedef const int8_t    sc8;  
typedef const int16_t   sc16;  
typedef const int32_t   sc32; 

typedef const uint8_t   uc8; 
typedef const uint16_t  uc16;  
typedef const uint32_t  uc32;  
// ============================================================================
//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+20) //0x40020014
#define GPIOB_ODR_Addr    (GPIOB_BASE+20) //0x40020414 
#define GPIOC_ODR_Addr    (GPIOC_BASE+20) //0x40020814 
#define GPIOD_ODR_Addr    (GPIOD_BASE+20) //0x40020C14 
#define GPIOE_ODR_Addr    (GPIOE_BASE+20) //0x40021014 
#define GPIOF_ODR_Addr    (GPIOF_BASE+20) //0x40021414    
#define GPIOG_ODR_Addr    (GPIOG_BASE+20) //0x40021814   
#define GPIOH_ODR_Addr    (GPIOH_BASE+20) //0x40021C14    
#define GPIOI_ODR_Addr    (GPIOI_BASE+20) //0x40022014 
#define GPIOJ_ODR_Addr    (GPIOJ_BASE+20) //0x40022414
#define GPIOK_ODR_Addr    (GPIOK_BASE+20) //0x40022814

#define GPIOA_IDR_Addr    (GPIOA_BASE+16) //0x40020010 
#define GPIOB_IDR_Addr    (GPIOB_BASE+16) //0x40020410 
#define GPIOC_IDR_Addr    (GPIOC_BASE+16) //0x40020810 
#define GPIOD_IDR_Addr    (GPIOD_BASE+16) //0x40020C10 
#define GPIOE_IDR_Addr    (GPIOE_BASE+16) //0x40021010 
#define GPIOF_IDR_Addr    (GPIOF_BASE+16) //0x40021410 
#define GPIOG_IDR_Addr    (GPIOG_BASE+16) //0x40021810 
#define GPIOH_IDR_Addr    (GPIOH_BASE+16) //0x40021C10 
#define GPIOI_IDR_Addr    (GPIOI_BASE+16) //0x40022010 
#define GPIOJ_IDR_Addr    (GPIOJ_BASE+16) //0x40022410 
#define GPIOK_IDR_Addr    (GPIOK_BASE+16) //0x40022810 

//位带操作,IO口操作宏定义
#define MACRO_MEM_ADDR(Addr)           ( *((volatile unsigned long  *)(Addr)) ) 
#define MACRO_BIT_BAND(Addr, BitNum)   ( (Addr & 0xF0000000)+0x2000000+((Addr &0xFFFFF)<<5)+(BitNum<<2) ) 
#define MACRO_BIT_ADDR(Addr, BitNum)   ( MACRO_MEM_ADDR(MACRO_BIT_BAND(Addr, BitNum)) ) 

//IO口操作,只对单一的IO口,确保n的值小于16
#define MACRO_PA_Out(n)   MACRO_BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define MACRO_PA_In(n)    MACRO_BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 
#define MACRO_PB_Out(n)   MACRO_BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define MACRO_PB_In(n)    MACRO_BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 
#define MACRO_PC_Out(n)   MACRO_BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define MACRO_PC_In(n)    MACRO_BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 
#define MACRO_PD_Out(n)   MACRO_BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define MACRO_PD_In(n)    MACRO_BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 
#define MACRO_PE_Out(n)   MACRO_BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
#define MACRO_PE_In(n)    MACRO_BIT_ADDR(GPIOE_IDR_Addr,n)  //输入
#define MACRO_PF_Out(n)   MACRO_BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
#define MACRO_PF_In(n)    MACRO_BIT_ADDR(GPIOF_IDR_Addr,n)  //输入
#define MACRO_PG_Out(n)   MACRO_BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define MACRO_PG_In(n)    MACRO_BIT_ADDR(GPIOG_IDR_Addr,n)  //输入
#define MACRO_PH_Out(n)   MACRO_BIT_ADDR(GPIOH_ODR_Addr,n)  //输出 
#define MACRO_PH_In(n)    MACRO_BIT_ADDR(GPIOH_IDR_Addr,n)  //输入
#define MACRO_PI_Out(n)   MACRO_BIT_ADDR(GPIOI_ODR_Addr,n)  //输出 
#define MACRO_PI_In(n)    MACRO_BIT_ADDR(GPIOI_IDR_Addr,n)  //输入
#define MACRO_PJ_Out(n)   MACRO_BIT_ADDR(GPIOJ_ODR_Addr,n)  //输出 
#define MACRO_PJ_In(n)    MACRO_BIT_ADDR(GPIOJ_IDR_Addr,n)  //输入
#define MACRO_PK_Out(n)   MACRO_BIT_ADDR(GPIOK_ODR_Addr,n)  //输出 
#define MACRO_PK_In(n)    MACRO_BIT_ADDR(GPIOK_IDR_Addr,n)  //输入

// ============================================================================
extern void SystemClock_Config(void);//时钟系统配置
extern void Error_Handler(void);
// ============================================================================
//以下为汇编函数
void  WFI_SET(void);		//执行WFI指令
void  INTX_DISABLE(void);   //关闭所有中断
void  INTX_ENABLE(void);	//开启所有中断
void  MSR_MSP(UINT32 addr);	//设置堆栈地址 

// ============================================================================

#ifdef __cplusplus
}
#endif
#endif//__SYS_H__

