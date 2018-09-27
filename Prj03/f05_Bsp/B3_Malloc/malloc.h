// ============================================================================
// Copyright (C) 2017-2018  All Rights Reserved
// 模块描述: 内存管理处理模块
// 模块版本: V1.00
// 创建人员: GuXY
// 创建时间: 2017-04-18
// ============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// ============================================================================
#ifndef __MALLOC_H__
#define __MALLOC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "mySys.h" 

// ============================================================================
#ifndef NULL
#define NULL 0
#endif

//定义三个内存池
#define SRAMIN	 0		//内部内存池
#define SRAMEX   1		//外部内存池(SDRAM)
#define SRAMCCM  2		//CCM内存池(此部分SRAM仅仅CPU可以访问!!!)
#define SRAMBANK 3	    //定义支持的SRAM块数.	

//mem1内存参数设定.内部SRAM
#define MEM1_BLOCK_SIZE			64  	  						//内存块大小为64字节
#define MEM1_MAX_SIZE			100*1024  						//最大管理内存 160K
#define MEM1_ALLOC_TABLE_SIZE	MEM1_MAX_SIZE/MEM1_BLOCK_SIZE 	//内存表大小

//mem2内存参数设定.外部SDRAM
#define MEM2_BLOCK_SIZE			64  	  						//内存块大小为64字节
#define MEM2_MAX_SIZE			4096 *1024  					//最大管理内存4096K
#define MEM2_ALLOC_TABLE_SIZE	MEM2_MAX_SIZE/MEM2_BLOCK_SIZE 	//内存表大小
		 
//mem3内存参数设定.mem3处于CCM,用于管理CCM(特别注意,这部分SRAM,仅CPU可以访问!!)
#define MEM3_BLOCK_SIZE			64  	  						//内存块大小为64字节
#define MEM3_MAX_SIZE			60 *1024  						//最大管理内存60K
#define MEM3_ALLOC_TABLE_SIZE	MEM3_MAX_SIZE/MEM3_BLOCK_SIZE 	//内存表大小
		 
//内存管理控制器
typedef struct 
{
	void   (*init)(UINT8);					//初始化
	UINT16 (*perused)(UINT8);		  	    //内存使用率
	UINT8 	*membase[SRAMBANK];				//内存池 管理SRAMBANK个区域的内存
	UINT32  *memmap[SRAMBANK]; 				//内存管理状态表
	UINT8    memrdy[SRAMBANK]; 				//内存管理是否就绪
}tagMallocDev;
// ============================================================================
//用户调用函数
extern void my_mem_init(UINT8 byMemX);                                  //内存池初始化函数  
extern void myfree(UINT8 byMemX,void *pMemBaseAdr);  			        //内存释放(外部调用)
extern void *mymalloc(UINT8 byMemX,UINT32 dwSize);			            //内存分配(外部调用)
extern void *myrealloc(UINT8 byMemX,void *pMemBaseAdr,UINT32 dwSize);   //重新分配内存(外部调用)
// ============================================================================

#ifdef __cplusplus
}
#endif

#endif//__MALLOC_H__

