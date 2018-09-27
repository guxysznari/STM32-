// ============================================================================
// Copyright (C) 2017-2018  All Rights Reserved
// 模块描述: usmart_str.h
// 模块版本: V1.00
// 创建人员: GuXY
// 创建时间: 2017-04-18
// ============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// ============================================================================
#ifndef __USMART_STR_H
#define __USMART_STR_H	 

#ifdef __cplusplus
extern "C" {
#endif

#include "mySys.h"
  
UINT8  usmart_get_parmpos(UINT8 byNum);						//得到某个参数在参数列里面的起始位置
UINT8  usmart_strcmp(UINT8 *pbyStr1,UINT8 *pbyStr2);					//对比两个字符串是否相等
UINT8  usmart_str2num(UINT8 *pbyStr,UINT32 *pdwRes);					//字符串转为数字
UINT8  usmart_get_cmdname(UINT8 *pbyStr,UINT8 *pbyCmdName,UINT8 *pbyNLen,UINT8 byMaxLen);//从str中得到指令名,并返回指令长度
UINT8  usmart_get_fname(UINT8 *pbyStr,UINT8 *pbyFName,UINT8 *pbyNum,UINT8 *pbyRVal);		//从str中得到函数名
UINT8  usmart_get_aparm(UINT8 *pbyStr,UINT8 *pbyFParm,UINT8 *pbyType); 	//从str中得到一个函数参数
UINT8  usmart_get_fparam(UINT8*pbyStr,UINT8 *pbyArn);  			//得到str中所有的函数参数.
UINT32 usmart_pow(UINT8 byM,UINT8 byN);							//M^N次方


#ifdef __cplusplus
}
#endif


#endif//__USMART_STR_H

