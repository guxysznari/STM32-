// ============================================================================
// Copyright (C) 2017-2018  All Rights Reserved
// 模块描述: 命令和参数解析
// 模块版本: V1.00
// 创建人员: GuXY
// 创建时间: 2017-04-18
// ============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// ============================================================================
#include "usmart_str.h"
#include "usmart.h"		   

// ============================================================================
// 函数功能:对比字符串
// 输入参数:字符串1指针,字符串2指针
// 返 回 值:0-相等;1-不相等;
// ============================================================================
UINT8 usmart_strcmp(UINT8 *pbyStr1,UINT8 *pbyStr2)
{
	while(1)
	{   
		if(*pbyStr1 != *pbyStr2)
        {
            return 1;
        }
		if(*pbyStr1=='\0')
        {
            break;
        }
		pbyStr1++;
		pbyStr2++;
	}
	return 0;
}
// ============================================================================
// 函数功能:把pbyStr1的内容copy到pbyStr2
// 输入参数:字符串1指针,字符串2指针
// 返 回 值:无
// ============================================================================
void usmart_strcopy(UINT8 *pbyStr1,UINT8 *pbyStr2)
{
	while(1)
	{										   
		*pbyStr2 = *pbyStr1;
		if(*pbyStr1=='\0')
        {
            break;
        }
		pbyStr1++;
		pbyStr2++;
	}
}
// ============================================================================
// 函数功能:获取字符串的长度(字节)
// 输入参数:字符串指针
// 返 回 值:字符串的长度	
// ============================================================================
UINT8 usmart_strlen(UINT8 *pbyStr)
{
	UINT8 byLen;
    
    byLen = 0;
    
	while(1)
	{							 
		if(*pbyStr=='\0')
        {
            break;
        }
		byLen++;
		pbyStr++;
	}
	return byLen;
}
// ============================================================================
// 函数功能:求幂函数(m^n函数)
// 输入参数:
// 返 回 值:m^n次方
// ============================================================================
UINT32 usmart_pow(UINT8 byM,UINT8 byN)
{
	UINT32 dwResult;

    dwResult = 1;
    
	while(byN--)
    {
        dwResult = dwResult * byM;    
    }
	return dwResult;
}
// ============================================================================
// 函数功能:把字符串转为数字,支持16进制转换,但是16进制字母必须是大写的,且格式为以0X开头的,支持负数 
// 输入参数:*pbyStr:数字字符串指针;*pdwRes:转换完的结果存放地址.
// 返 回 值:0,成功转换完成.其他,错误代码.
//          1,数据格式错误
//          2,16进制位数为0
//          3,起始格式错误
//          4,十进制位数为0
// ============================================================================
UINT8 usmart_str2num(UINT8 *pbyStr,UINT32 *pdwRes)
{
	UINT8  byNum;		
	UINT8  byHexDec;	
	UINT8  byFlag;		
	UINT8 *pbyTemp;		  
	SINT32 iNum;
	UINT32 dwTemp;

    //初始化
	pbyTemp  = pbyStr;
    byNum    = 0;		    // 数字的位数
	byHexDec = 10;	        // 默认为十进制数据
	byFlag   = 0;		    // 0,没有符号标记;1,表示正数;2,表示负数.
   *pdwRes   = 0;           // 清零.
	
	while(1)
	{
		if((*pbyTemp <= '9' && *pbyTemp >= '0')                   \
            ||((*pbyStr =='-' || *pbyStr =='+') && byNum==0)      \
            ||( *pbyTemp<='F' && *pbyTemp>='A')                   \
            ||( *pbyTemp=='X' &&  byNum==1))//参数合法
		{
			if(*pbyTemp>='A')
            {
                byHexDec = 16;  //字符串中存在字母,为16进制格式.
            }
			if(*pbyStr=='-')
            {
                byFlag = 2;
                pbyStr+= 1;
            }//偏移掉符号
			else if(*pbyStr=='+')
            {
                byFlag=1;
                pbyStr+=1;
            }//偏移掉符号
			else 
            {
                //位数增加.
                byNum++;
            }
		}
        else if(*pbyTemp=='\0')
        {
            //碰到结束符,退出.
            break;    
        }
		else
		{ 
            //不全是十进制或者16进制数据.
            return 1;              
        }
		pbyTemp++; 
	} 
    
    //重新定位到字符串开始的地址.
	pbyTemp = pbyStr;			    
	
	if(byHexDec==16)		//16进制数据
	{
		if(byNum < 3)
        {
            return 2;           //位数小于3，直接退出.因为0X就占了2个,如果0X后面不跟数据,则该数据非法.
        }
		if(*pbyTemp=='0' && (*(pbyTemp+1)=='X'))//必须以'0X'开头.
		{
			pbyTemp+= 2;	//偏移到数据起始地址.
			byNum  -= 2;//减去偏移量	 
		}
        else 
        {
            return 3;//起始头的格式不对
        }
	}
    else if(byNum==0)
    {
        return 4;//位数为0，直接退出.     
    }
	while(1)
	{
		if(byNum)
		{
            byNum--;
        }
		if(*pbyTemp<='9'&&*pbyTemp>='0')
		{
            dwTemp=*pbyTemp-'0';    //得到数字的值
        }
		else 
		{
            dwTemp=*pbyTemp-'A'+10;             //得到A~F对应的值       
        }
		*pdwRes += dwTemp * usmart_pow(byHexDec,byNum);		   
		pbyTemp++;
        
		if(*pbyTemp=='\0')
		{
            break;//数据都查完了.   
        }
	}
	if(byFlag==2)//是负数?
	{	
		iNum   =-*pdwRes; 
		*pdwRes= iNum;
	}
	return 0;//成功转换
}
// ============================================================================
// 函数功能:得到指令名
// 输入参数:*pbyStr:源字符串*cmdname:指令名*nlen:指令名长度	maxlen:最大长度(做限制,指令不可能太长的)
// 返 回 值:0,成功;其他,失败.	  
// ============================================================================
UINT8 usmart_get_cmdname(UINT8 *pbyStr,UINT8*cmdname,UINT8 *nlen,UINT8 maxlen)
{
	*nlen=0;
 	while(*pbyStr!=' '&&*pbyStr!='\0') //找到空格或者结束符则认为结束了
	{
		*cmdname=*pbyStr;
		pbyStr++;
		cmdname++;
		(*nlen)++;//统计命令长度
		if(*nlen>=maxlen)
		{
            return 1;//错误的指令
        }
	}
	*cmdname='\0';//加入结束符
	return 0;//正常返回
}
// ============================================================================
// 函数功能:获取下一个字符（当中间有很多空格的时候，此函数直接忽略空格，找到空格之后的第一个字符）
// 输入参数:pbyStr:字符串指针	
// 返 回 值:下一个字符
// ============================================================================
UINT8 usmart_search_nextc(UINT8* pbyStr)
{		   	 	
	pbyStr++;
	while(*pbyStr==' ' && pbyStr!='\0')
	{
        pbyStr++;
    }
	return *pbyStr;
} 
// ============================================================================
// 函数功能:从str中得到函数名
// 输入参数:*pbyStr:源字符串指针*pbyFName:获取到的函数名字指针*pbyNum:函数的参数个数*rval:是否需要显示返回值(0,不需要;1,需要)
// 返 回 值:0,成功;其他,错误代码.
// ============================================================================
UINT8 usmart_get_fname(UINT8 *pbyStr,UINT8 *pbyFName,UINT8 *pbyNum,UINT8 *rval)
{
	UINT8  byRes;
	UINT8  byFover;	        //括号深度
	UINT8  byOffset;  
	UINT8  byParmNum;
	UINT8  byTemp;
	UINT8  byFpName[6];     //void+X+'/0'
	UINT8  byFplCnt;        //第一个参数的长度计数器
	UINT8  byPCnt;	        //参数计数器
	UINT8  byNChar;
	UINT8 *pbyStrTemp;

    //初始化
    byFover   = 0;
    byOffset  = 0; 
    byParmNum = 0;
    byFplCnt  = 0;
    byPCnt    = 0;
    byTemp    = 1;
    
	//判断函数是否有返回值
	pbyStrTemp = pbyStr;
	while(*pbyStrTemp != '\0')//没有结束
	{
		if(*pbyStrTemp!=' ' && (byPCnt&0X7F)<5)//最多记录5个字符
		{	
			if(byPCnt==0)
			{
                byPCnt = byPCnt | 0X80;//置位最高位,标记开始接收返回值类型
            }
			if(((byPCnt&0x7f)==4) && (*pbyStrTemp!='*'))
			{
                break;//最后一个字符,必须是*
            }
			byFpName[byPCnt&0x7f] = *pbyStrTemp;//记录函数的返回值类型
			byPCnt++;
		}
        else if(byPCnt==0X85)
        {
            break;
        }
		pbyStrTemp++; 
	} 
	if(byPCnt)//接收完了
	{
		byFpName[byPCnt&0x7f] = '\0';//加入结束符
		if(usmart_strcmp(byFpName,"void")==0)
		{
            *rval = 0;//不需要返回值
        }
		else
		{
            *rval = 1;                               //需要返回值
        }
		byPCnt = 0;
	} 
	byRes      = 0;
	pbyStrTemp = pbyStr;
	while(*pbyStrTemp!='(' && *pbyStrTemp!='\0') //此代码找到函数名的真正起始位置
	{  
		pbyStrTemp++;
		byRes++;
		if(*pbyStrTemp==' '||*pbyStrTemp=='*')
		{
			byNChar = usmart_search_nextc(pbyStrTemp);		//获取下一个字符
			if(byNChar!='(' && byNChar!='*')
			{
                byOffset = byRes; //跳过空格和*号
            }
		}
	}	 
	pbyStrTemp=pbyStr;
	if(byOffset)
	{
        pbyStrTemp = pbyStrTemp + (byOffset + 1);//跳到函数名开始的地方      
    }
	byRes   = 0;
	byNChar = 0;//是否正在字符串里面的标志,0，不在字符串;1，在字符串;
	while(1)
	{
		if(*pbyStrTemp==0)
		{
			byRes = USMART_FUNCERR;//函数错误
			break;
		}
        else if(*pbyStrTemp=='(' && byNChar==0)
        {
            byFover++;//括号深度增加一级     
        }
		else if(*pbyStrTemp==')' && byNChar==0)
		{
			if(byFover)
			{
                byFover--;
            }
			else
			{
                byRes=USMART_FUNCERR;//错误结束,没收到'('
            }
			if(byFover==0)
			{
                break;//到末尾了,退出       
            }
		}
        else if(*pbyStrTemp=='"')
        {
            byNChar = !byNChar;
        }

		if(byFover==0)//函数名还没接收完
		{
			if(*pbyStrTemp!=' ')//空格不属于函数名
			{
			   *pbyFName = *pbyStrTemp;//得到函数名
				pbyFName++;
			}
		}
        else //已经接受完了函数名了.
		{
			if(*pbyStrTemp==',')
			{
				byTemp=1;		//使能增加一个参数
				byPCnt++;	
			}
            else if(*pbyStrTemp!=' '&&*pbyStrTemp!='(')
			{
				if(byPCnt==0&&byFplCnt<5)		//当第一个参数来时,为了避免统计void类型的参数,必须做判断.
				{
					byFpName[byFplCnt]=*pbyStrTemp;//记录参数特征.
					byFplCnt++;
				}
				byTemp++;	//得到有效参数(非空格)
			}
			if(byFover==1&&byTemp==2)
			{
				byTemp++;		//防止重复增加
				byParmNum++; 	//参数增加一个
			}
		}
		pbyStrTemp++; 			
	}   
	if(byParmNum==1)//只有1个参数.
	{
		byFpName[byFplCnt]='\0';//加入结束符
		if(usmart_strcmp(byFpName,"void")==0)
		{
            byParmNum=0;//参数为void,表示没有参数.
        }
	}
	*pbyNum   = byParmNum;	//记录参数个数
	*pbyFName = '\0';	    //加入结束符
	return byRes;		    //返回执行结果
}
// ============================================================================
// 函数功能:从str中得到一个函数的参数
// 输入参数:*pbyStr:源字符串指针*pbyFparm:参数字符串指针*pbyType:参数类型 0，数字;1，字符串;0XFF，参数错误
// 返 回 值:0,已经无参数了;其他,下一个参数的偏移量.
// ============================================================================
UINT8 usmart_get_aparm(UINT8 *pbyStr,UINT8 *pbyFparm,UINT8 *pbyType)
{
	UINT8 byRet;
	UINT8 byEnOut;
	UINT8 byType;   //默认是数字
	UINT8 byString; //标记str是否正在读

    //初始化
    byRet    = 0;
	byEnOut  = 0;
	byType   = 0;   
	byString = 0;   
	
	while(1)
	{		    
		if(*pbyStr==',' && byString==0)
		{
            byEnOut=1;          //暂缓立即退出,目的是寻找下一个参数的起始地址
        }
		if((*pbyStr==')'||*pbyStr=='\0')&&byString==0)
		{
            break;//立即退出标识符
        }
		if(byType==0)//默认是数字的
		{
			if((*pbyStr>='0' && *pbyStr<='9')||*pbyStr=='-'||*pbyStr=='+'||(*pbyStr>='a' && *pbyStr<='f')||(*pbyStr>='A' && *pbyStr<='F')||*pbyStr=='X'||*pbyStr=='x')//数字串检测
			{
				if(byEnOut)
				{
                    break;                  //找到了下一个参数,直接退出.
                }
				if(*pbyStr>='a')
				{
                    *pbyFparm = *pbyStr-0X20; //小写转换为大写
                }
				else
				{
                    *pbyFparm = *pbyStr;              //小写或者数字保持不变
                }
				pbyFparm++;
			}
            else if(*pbyStr=='"')//找到字符串的开始标志
			{
				if(byEnOut)
				{
                    break;//找到,后才找到",认为结束了.
                }
				byType   = 1;
				byString = 1;//登记STRING 正在读了
			}
            else if(*pbyStr!=' ' && *pbyStr!=',')//发现非法字符,参数错误
			{
				byType=0XFF;
				break;
			}
		}
        else//string类
		{ 
			if(*pbyStr=='"')
			{
                byString=0;
            }
			if(byEnOut)
			{
                break;          //找到了下一个参数,直接退出.
            }
			if(byString)				//字符串正在读
			{	
				if(*pbyStr=='\\')		//遇到转义符(不复制转义符)
				{ 
					pbyStr++;			//偏移到转义符后面的字符,不管什么字符,直接COPY
					byRet++;
				}					
				*pbyFparm=*pbyStr;		//小写或者数字保持不变
				pbyFparm++;
			}	
		}
		byRet++;        //偏移量增加
		pbyStr++;
	}
	*pbyFparm = '\0';	//加入结束符
	*pbyType  = byType;	//返回参数类型
	return byRet;		//返回参数长度
}
// ============================================================================
// 函数功能:得到指定参数的起始地址
// 输入参数:num:第num个参数,范围0~9.
// 返 回 值:该参数的起始地址
// ============================================================================
UINT8 usmart_get_parmpos(UINT8 byNum)
{
	UINT8 byTemp;
	UINT8 byLoop;

    byTemp=0;
    
	for(byLoop=0;byLoop<byNum;byLoop++)
	{
        byTemp = byTemp + usmart_dev.plentbl[byLoop];
    }
	return byTemp;
}
// ============================================================================
// 函数功能:从str中得到函数参数
// 输入参数:pbyStr:源字符串;pbyArn:参数的多少.0表示无参数 void类型
// 返 回 值:0,成功;其他,错误代码.
// ============================================================================
UINT8 usmart_get_fparam(UINT8 *pbyStr,UINT8 *pbyArn)
{	
	UINT8  byLoop,byType;  
	UINT8  byNum;
	UINT8  byLen;
	UINT8  byStr[PARM_LEN+1];//字节长度的缓存,最多可以存放PARM_LEN个字符的字符串
	UINT32 dwRes;

    byNum=0;

	for(byLoop=0;byLoop<MAX_PARM;byLoop++)
	{
        usmart_dev.plentbl[byLoop]=0;//清空参数长度表
    }
	while(*pbyStr!='(')//偏移到参数开始的地方
	{
		pbyStr++;											    
		if(*pbyStr=='\0')
		{
            return USMART_FUNCERR;//遇到结束符了
        }
	}
	pbyStr++;//偏移到"("之后的第一个字节
	while(1)
	{
		byLoop = usmart_get_aparm(pbyStr,byStr,&byType);	//得到第一个参数  
		pbyStr+= byLoop;								    //偏移
		switch(byType)
		{
			case 0:	//数字
				if(byStr[0]!='\0')				            //接收到的参数有效
				{					    
					byLoop=usmart_str2num(byStr,&dwRes);	//记录该参数	 
					if(byLoop)
					{
                        return USMART_PARMERR;              //参数错误.
                    }
					*(u32*)(usmart_dev.parm+usmart_get_parmpos(byNum))=dwRes;//记录转换成功的结果.
					usmart_dev.parmtype&=~(1<<byNum);	//标记数字
					usmart_dev.plentbl[byNum]=4;		//该参数的长度为4  
					byNum++;							//参数增加  
					if(byNum>MAX_PARM)
					{
                        return USMART_PARMOVER;         //参数太多
                    }
				}
				break;
			case 1://字符串	 	
				byLen=usmart_strlen(byStr)+1;	//包含了结束符'\0'
				usmart_strcopy(byStr,&usmart_dev.parm[usmart_get_parmpos(byNum)]);//拷贝tstr数据到usmart_dev.parm[n]
				usmart_dev.parmtype|=1<<byNum;	//标记字符串 
				usmart_dev.plentbl[byNum]=byLen;	//该参数的长度为len  
				byNum++;
				if(byNum>MAX_PARM)
				{
                    return USMART_PARMOVER;//参数太多
                }
				break;
			case 0XFF://错误
				return USMART_PARMERR;//参数错误	  
		}
		if(*pbyStr==')'||*pbyStr=='\0')
		{
            break;//查到结束标志了.
        }
	}
	*pbyArn = byNum;	//记录参数的个数
	return USMART_OK;   //正确得到了参数
}
// ============================================================================

