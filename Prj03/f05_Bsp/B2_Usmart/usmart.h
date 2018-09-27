// ============================================================================
// Copyright (C) 2017-2018  All Rights Reserved
// 模块描述: Usmart.h
// 模块版本: V1.00
// 创建人员: GuXY
// 创建时间: 2017-04-18
// ============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// ============================================================================
#ifndef __USMART_H
#define __USMART_H

#ifdef __cplusplus
extern "C" {
#endif

#include "usmart_str.h"

// ============================================================================
// USMART资源占用情况@MDK 3.80A@2.0版本：
// FLASH:4K~K字节(通过USMART_USE_HELP和USMART_USE_WRFUNS设置)
// SRAM:最少的情况下72字节
// SRAM计算公式:   SRAM=PARM_LEN+72-4  其中PARM_LEN必须大于等于4.
// 应该保证堆栈不小于100个字节.
// ============================================================================
// 用户配置参数
#define MAX_FNAME_LEN 		30	//函数名最大长度										   
#define MAX_PARM 			10	//最大为10个参数 ,修改此参数,必须修改usmart_exe与之对应.
#define PARM_LEN 			200	//所有参数之和的长度不超过PARM_LEN个字节,注意串口接收部分要与之对应(不小于PARM_LEN)

#define USMART_ENTIMX_SCAN 	1	//使用TIM的定时中断来扫描SCAN函数,如果设置为0,需要自己实现隔一段时间扫描一次scan函数.
								//(注意:如果要用runtime统计功能,必须设置USMART_ENTIMX_SCAN为1)
#define USMART_USE_HELP		1	//使用帮助，该值设为0，可以节省近700个字节，但是将导致无法显示帮助信息。
#define USMART_USE_WRFUNS	1	//使能读写函数,读取任何地址的值,写寄存器的值.

// ============================================================================
#define USMART_OK 			0  //无错误
#define USMART_FUNCERR 		1  //函数错误
#define USMART_PARMERR 		2  //参数错误
#define USMART_PARMOVER 	3  //参数溢出
#define USMART_NOFUNCFIND 	4  //未找到匹配函数

#define SP_TYPE_DEC      	0  //10进制参数显示
#define SP_TYPE_HEX       	1  //16进制参数显示
// ============================================================================
//函数名列表	
typedef struct
{
	void * func;			//函数指针
	const UINT8* name;		//函数名(查找串)	 
}tagUsmartFuncNameList;

//usmart控制管理器
typedef struct 
{
	tagUsmartFuncNameList *ptFuns;	    //函数名指针

	void   (*init)(UINT8);				//初始化
	void   (*exe)(void); 				//执行 
	void   (*scan)(void);               //扫描
	UINT8  (*cmd_rec)(UINT8 *str);	    //识别函数名及参数
	UINT8  fnum; 				  		//函数数量
	UINT8  pnum;                        //参数数量
	UINT8  id;							//函数id
	UINT8  sptype;						//参数显示类型(非字符串参数):0,10进制;1,16进制;
	UINT8  plentbl[MAX_PARM];  		    //每个参数的长度暂存表
	UINT8  parm[PARM_LEN];  			//函数的参数
	UINT8  runtimeflag;					//0,不统计函数执行时间;1,统计函数执行时间,注意:此功能必须在USMART_ENTIMX_SCAN使能的时候,才有用
	UINT16 parmtype;					//参数的类型
	UINT32 runtime;					    //运行时间,单位:0.1ms,最大延时时间为定时器CNT值的2倍*0.1ms
}tagUsmartDev;

extern tagUsmartFuncNameList usmart_namelist[];	//在usmart_config.c里面定义
extern tagUsmartDev usmart_dev;		            //在usmart_config.c里面定义

void    usmart_init(UINT8 bySysclk);    //初始化
void    usmart_exe(void);		        //执行
void    usmart_scan(void);              //扫描
void    write_addr(UINT32 dwAddr,UINT32 dwVal);//在指定地址写入指定的值
void    usmart_reset_runtime(void);     //复位运行时间
UINT8   usmart_cmd_rec(UINT8 *pbyStr);	//识别
UINT32  read_addr(UINT32 dwAddr);	    //读取指定地址的值
UINT32  usmart_get_runtime(void);	    //获取运行时间

#ifdef __cplusplus
}
#endif

#endif//__USMART_H

