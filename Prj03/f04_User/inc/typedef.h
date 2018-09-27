//****************************************************************************************
//Copyright(c) 2011-2011 -------------------------
//文 件 名：type.h
//功能说明：
//开发人员：GuXY
//创建时间：20170110
//文件版本：v1.00
//****************************************************************************************
//程序修改记录（最新的放在最前面）
//<版本号><修改日期><修改人员><修改功能描述>
//========================================================================================
#ifndef __TYPE_H__
#define __TYPE_H__

#ifdef __cplusplus
extern "C" {
#endif
 
#ifndef PROGRAM_TYPE    
    typedef   signed  char       SINT8;
    typedef   signed  short int  SINT16;
    typedef   signed  int        SINT32;
    typedef   signed  long long  SINT64;    
    
    typedef unsigned  char       BYTE;
    typedef unsigned  char       BOOLEAN;   
    typedef unsigned  char       UINT8;
    typedef unsigned  short int  UINT16;
    typedef unsigned  int        UINT32;
    typedef unsigned  long long  UINT64;
    
    typedef   float   FLOAT32;

    typedef   __IO    SINT8      VSINT8;
    typedef   __IO    SINT16     VSINT16;
    typedef   __IO    SINT32     VSINT32;
                                 
    typedef   __IO    UINT8      VUINT8;
    typedef   __IO    UINT16     VUINT16;
    typedef   __IO    UINT32     VUINT32;
                                 
    typedef   __I     SINT8      VSCINT8; 
    typedef   __I     SINT16     VSCINT16;
    typedef   __I     SINT32     VSCINT32;
                                 
    typedef   __I     UINT8      VUCINT8; 
    typedef   __I     UINT16     VUCINT16;
    typedef   __I     UINT32     VUCINT32;
                                 
    typedef   const   SINT8      SCINT8; 
    typedef   const   SINT16     SCINT16;
    typedef   const   SINT32     SCINT32;
                                 
    typedef   const   UINT8      UCINT8; 
    typedef   const   UINT16     UCINT16;
    typedef   const   UINT32     UCINT32;
   
#endif    
    
#ifndef FALSE
    #define FALSE               0
#endif
#ifndef TRUE
    #define TRUE                1
#endif

#ifndef NULL
    #define NULL                0
#endif

#ifndef BOOL
    #define BOOL                UINT8
#endif

#ifndef STATUS
    #define STATUS              int
#endif

#ifndef ERROR
    #define ERROR               (-1)
#endif

#ifndef OK
    #define OK                  (0)
#endif

#ifdef __cplusplus
}
#endif
#endif//__TYPE_H__
