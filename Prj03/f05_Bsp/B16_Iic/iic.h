// ============================================================================
// Copyright (C) 2017-2018  All Rights Reserved
// 模块描述: IIC实现组件
// 模块版本: V1.00
// 创建人员: GuXY
// 创建时间: 2017-04-18
// ============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// ============================================================================
#ifndef __IIC_H__
#define __IIC_H__

#include "mySys.h"

// ============================================================================
//接口函数
extern void  IIC1_Init(void);			
extern void  IIC1_Write_Byte(UINT8 byAddr, UINT8 byReg, UINT8 byValue);  
extern UINT8 IIC1_Read_Byte(UINT8 byAddr, UINT8 byReg);
extern void  IIC1_Write_Bytes(UINT8 byAddr, UINT8 byReg, UINT16 wMemAdr,UINT8 *pbyBuf,UINT16 wLen);
extern void  IIC1_Read_Bytes(UINT8 byAddr, UINT8 byReg,UINT16 wMemAdr,UINT8 *pbyBuf,UINT16 wLen);    
// ============================================================================

#endif  //__IIC_H__

