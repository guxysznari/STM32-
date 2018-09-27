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
#include "malloc.h"	   

// ============================================================================
//内存池(32字节对齐)
__align(32) UINT8 tabMem1Base[MEM1_MAX_SIZE];													//内部SRAM内存池
__align(32) UINT8 tabMem2Base[MEM2_MAX_SIZE] __attribute__((at(0XC0000000)));					//外部SDRAM内存池
__align(32) UINT8 tabMem3Base[MEM3_MAX_SIZE] __attribute__((at(0X10000000)));					//内部CCM内存池

//内存管理表
UINT32 tabMem1MapBase[MEM1_ALLOC_TABLE_SIZE];													//内部SRAM内存池MAP
UINT32 tabMem2MapBase[MEM2_ALLOC_TABLE_SIZE] __attribute__((at(0XC0000000+MEM2_MAX_SIZE)));	    //外部SRAM内存池MAP
UINT32 tabMem3MapBase[MEM3_ALLOC_TABLE_SIZE] __attribute__((at(0X10000000+MEM3_MAX_SIZE)));	    //内部CCM内存池MAP

//内存管理参数	   
const UINT32 tabMemTblSize[SRAMBANK] = { MEM1_ALLOC_TABLE_SIZE,MEM2_ALLOC_TABLE_SIZE,
                                         MEM3_ALLOC_TABLE_SIZE };	//内存表大小
const UINT32 tabMemBlkSize[SRAMBANK] = { MEM1_BLOCK_SIZE, MEM2_BLOCK_SIZE,
                                         MEM3_BLOCK_SIZE };         //内存分块大小
const UINT32 tabMemSumSize[SRAMBANK] = { MEM1_MAX_SIZE,MEM2_MAX_SIZE,
                                         MEM3_MAX_SIZE };           //内存总大小
// ============================================================================
void   mymemset(void *,UINT8,UINT32);   //设置内存
void   mymemcpy(void *,void *,UINT32);  //复制内存     
void   my_mem_init(UINT8);				//内存管理初始化函数(外/内部调用)
UINT8  my_mem_free(UINT8,UINT32);	    //内存释放(内部调用)
UINT16 my_mem_perused(UINT8) ;			//获得内存使用率(外/内部调用) 
UINT32 my_mem_malloc(UINT8,UINT32);	    //内存分配(内部调用)
// ============================================================================
//内存管理控制器
tagMallocDev tMallcoDev=
{
	my_mem_init,						            //内存初始化
	my_mem_perused,						            //内存使用率
	tabMem1Base,                                    //内存池
	tabMem2Base,
	tabMem3Base,			
	tabMem1MapBase,                                 //内存管理状态表
	tabMem2MapBase,
	tabMem3MapBase,   
	0,0,0,  		 					            //内存管理未就绪
};
// ============================================================================
// 函数功能:复制内存
// 输入参数:目的地址,源地址,需要复制的内存长度(字节为单位)
// 返 回 值:
// ============================================================================
void mymemcpy(void *pDes,void *pSrc,UINT32 dwLen)  
{  
    UINT8 *pbyXDes = pDes;
	UINT8 *pbyXSrc = pSrc; 
    
    while(dwLen--)
    {
        *pbyXDes++ = *pbyXSrc++;  
    }
}  
// ============================================================================
// 函数功能:设置内存
// 输入参数:*pBaseAdr:内存首地址, bySetVal:要设置的值,count:需要设置的内存大小(字节为单位)
// 返 回 值:
// ============================================================================
void mymemset(void *pBaseAdr,UINT8 bySetVal,UINT32 dwCnt)  
{  
    UINT8 *pbyXS = pBaseAdr; 
    
    while(dwCnt--)
    {
        *pbyXS ++= bySetVal;  
    }
}
// ============================================================================
// 函数功能:内存管理初始化  
// 输入参数:byMemX:所属内存块
// 返 回 值:
// ============================================================================
void my_mem_init(UINT8 byMemX)  
{  
    //内存状态表数据清零  
    mymemset(tMallcoDev.memmap[byMemX],0,tabMemTblSize[byMemX]*4);
    
    //内存管理初始化OK  
 	tMallcoDev.memrdy[byMemX] = 1;								
}
// ============================================================================
// 函数功能:获取内存使用率
// 输入参数:所属内存块
// 返 回 值:使用率(扩大了10倍,0~1000,代表0.0%~100.0%)
// ============================================================================
UINT16 my_mem_perused(UINT8 byMemX)  
{  
    UINT32 dwUsedFlg=0;  
    UINT32 dwLoop;  

    for(dwLoop=0;dwLoop<tabMemTblSize[byMemX];dwLoop++)  
    {  
        if(tMallcoDev.memmap[byMemX][dwLoop])
        {
            dwUsedFlg++; 
        }
    } 
    return (dwUsedFlg*1000)/(tabMemTblSize[byMemX]);  
} 
// ============================================================================
// 函数功能:内存分配(内部调用)
// 输入参数:byMemX:所属内存块;dwSize:要分配的内存大小(字节)
// 返 回 值:0XFFFFFFFF,代表错误;其他,内存偏移地址 
// ============================================================================
UINT32 my_mem_malloc(UINT8 byMemX,UINT32 dwSize)  
{  
    SINT32 iOffset=0;  
    UINT32 dwNMemB;	  //需要的内存块数  
	UINT32 dwCMemB=0; //连续空内存块数
    UINT32 dwLoop; 
    
    if(!tMallcoDev.memrdy[byMemX])
    {
        //未初始化,先执行初始化 
        tMallcoDev.init(byMemX);
    }
    
    if(dwSize==0)
    {
        return 0XFFFFFFFF;//不需要分配
    }
    
    //获取需要分配的连续内存块数
    dwNMemB=dwSize/tabMemBlkSize[byMemX];  	

    if(dwSize%tabMemBlkSize[byMemX])
    {
        dwNMemB++;  
    }
    
    //搜索整个内存控制区  
    for(iOffset=tabMemTblSize[byMemX]-1;iOffset>=0;iOffset--)
    {     
		if(!tMallcoDev.memmap[byMemX][iOffset])
		{
            //连续空内存块数增加
            dwCMemB++;
        }
		else 
		{
            //连续内存块清零
            dwCMemB=0;                                
        }
        
        //找到了连续nmemb个空内存块
		if(dwCMemB==dwNMemB)							
		{
            //标注内存块非空 
            for(dwLoop=0;dwLoop<dwNMemB;dwLoop++)  					
            {  
                tMallcoDev.memmap[byMemX][iOffset+dwLoop]=dwNMemB;  
            }  
            //返回偏移地址  
            return (iOffset*tabMemBlkSize[byMemX]);
		}
    }  
    //未找到符合分配条件的内存块  
    return 0XFFFFFFFF;
}  
// ============================================================================
// 函数功能:释放内存(内部调用) 
// 输入参数:byMemX:所属内存块offset:内存地址偏移
// 返 回 值:0,释放成功;1,释放失败;  
// ============================================================================
UINT8 my_mem_free(UINT8 byMemX,UINT32 iOffset)  
{  
    UINT32 dwLoop;  
    UINT32 dwIndex;
    UINT32 dwNMemB;

    dwIndex = 0;
    dwNMemB = 0;
    
    //未初始化,先执行初始化
    if(!tMallcoDev.memrdy[byMemX])
	{
		tMallcoDev.init(byMemX);    
        //未初始化  
        return 1;
    }  
    //偏移在内存池内. 
    if(iOffset<tabMemSumSize[byMemX])
    {  
        dwIndex = iOffset/tabMemBlkSize[byMemX];		//偏移所在内存块号码  
        dwNMemB = tMallcoDev.memmap[byMemX][dwIndex];	//内存块数量

        //内存块清零
        for(dwLoop=0;dwLoop<dwNMemB;dwLoop++)  						
        {  
            tMallcoDev.memmap[byMemX][dwIndex+dwLoop]=0;  
        }  
        
        return 0;  
        
    }
    else 
    {
        //偏移超区了
        return 2;
    }
}  

// ============================================================================
// 函数功能:释放内存(外部调用) 
// 输入参数:byMemX:所属内存块ptr:内存首地址 
// 返 回 值:无
// ============================================================================
void myfree(UINT8 byMemX,void *pMemBaseAdr)  
{  
	UINT32 dwOffset;   
    
	if(pMemBaseAdr==NULL)
	{
        return;//地址为0.  
    }
 	dwOffset=(UINT32)pMemBaseAdr-(UINT32)tMallcoDev.membase[byMemX];     
    //释放内存     
    my_mem_free(byMemX,dwOffset);	 
}  

// ============================================================================
// 函数功能:分配内存(外部调用)
// 输入参数:byMemX:所属内存块size:内存大小(字节)
// 返 回 值:新分配到的内存首地址.
// ============================================================================
void *mymalloc(UINT8 byMemX,UINT32 dwSize)  
{  
    UINT32 dwOffset;   
    
	dwOffset=my_mem_malloc(byMemX,dwSize); 
    
    if(dwOffset==0XFFFFFFFF)
    {
        return NULL;  
    }
    else
    {
        return (void*)((UINT32)tMallcoDev.membase[byMemX] + dwOffset);  
    }
}  

// ============================================================================
// 函数功能:重新分配内存(外部调用)
// 输入参数:byMemX:所属内存块*pMemBaseAdr:旧内存首地址size:要分配的内存大小(字节)
// 返 回 值:新分配到的内存首地址.
// ============================================================================
void *myrealloc(UINT8 byMemX,void *pMemBaseAdr,UINT32 dwSize)  
{  
    UINT32 dwOffset;    
    
    dwOffset=my_mem_malloc(byMemX,dwSize);   
    
    if(dwOffset==0XFFFFFFFF)
    {
        return NULL;     
    }
    else  
    {  									
        //拷贝旧内存内容到新内存     
	    mymemcpy((void*)((UINT32)tMallcoDev.membase[byMemX]+dwOffset),pMemBaseAdr,dwSize);	
        //释放旧内存
        myfree(byMemX,pMemBaseAdr);  	
        //返回新内存首地址
        return (void*)((UINT32)tMallcoDev.membase[byMemX]+dwOffset);  				
    }  
}
// ============================================================================

