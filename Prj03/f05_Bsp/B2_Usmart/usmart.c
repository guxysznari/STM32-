// ============================================================================
// Copyright (C) 2017-2018  All Rights Reserved
// 模块描述: 负责与外部交互的模块
// 模块版本: V1.00
// 创建人员: GuXY
// 创建时间: 2017-04-18
// ============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// ============================================================================
#include "usmart.h"
#include "usart.h"
#include "mySys.h" 

// ============================================================================
TIM_HandleTypeDef TIM4_Handler;      //定时器句柄 

//系统命令
UINT8 *tabSysCmd[]=
{
	"?",
	"help",
	"list",
	"id",
	"hex",
	"dec",
	"runtime",	   
};	
// ============================================================================
// 功能描述:处理系统指令
// 输入参数:
// 返 回 值:0,成功处理;其他,错误代码;
// ============================================================================
UINT8 usmart_sys_cmd_exe(UINT8 *bypStr)
{
	UINT8  byLoop;
	UINT8  bySFName[MAX_FNAME_LEN];//存放本地函数名
	UINT8  byPNum;
	UINT8  byRVal;
	UINT32 dwRes;  
    
	dwRes = usmart_get_cmdname(bypStr,bySFName,&byLoop,MAX_FNAME_LEN);//得到指令及指令长度
	if(dwRes)
	{
        return USMART_FUNCERR;//错误的指令 
    }
    
	bypStr = bypStr + byLoop;
    
	for(byLoop=0;byLoop<sizeof(tabSysCmd)/4;byLoop++)//支持的系统指令
	{
        if(usmart_strcmp(bySFName,tabSysCmd[byLoop])==0)
        {
            break;
        }
	}
	switch(byLoop)
	{					   
		case 0:
		case 1://帮助指令
			printf("\r\n");
#if USMART_USE_HELP 
			printf("------------------------MyShell v1.0-------------------------\r\n");
//			printf("    MyShell是一个灵巧的串口调试组件,可以通过串口助手调用程序\r\n");
//			printf("里面的任何函数并执行,可以随意更改函数的输入参数,单个函数最多\r\n");
//			printf("支持10个输入参数,并支持函数返回值显示.支持参数显示进制设置功\r\n"),
//          printf("能,支持进制转换功能.(支持数字(10/16进制,支持负数)、字符串、 \r\n");
//			printf("函数入口地址等作为参数)\r\n"),  
//			printf("MyShell有7个系统命令(必须小写):\r\n\r\n");
			printf("?:      获取帮助信息\r\n");
			printf("help:   获取帮助信息\r\n");
			printf("list:   可用的函数列表\r\n\n");
			printf("id:     可用函数的ID列表\r\n\n");
			printf("hex:    参数16进制显示,后跟空格+数字即执行进制转换\r\n\n");
			printf("dec:    参数10进制显示,后跟空格+数字即执行进制转换\r\n\n");
			printf("runtime:1,开启函数运行计时;0,关闭函数运行计时;\r\n\n");
			printf("请按照程序编写格式输入函数名及参数并以回车键结束.\r\n");    
			printf("--------------------------MADE-BY-Sanyu---------------------- \r\n");
#else
			printf("指令失效\r\n");
#endif
			break;
		case 2://查询指令
			printf("\r\n");
			printf("-------------------------函数清单--------------------------- \r\n");
			for(byLoop=0;byLoop<usmart_dev.fnum;byLoop++)printf("%s\r\n",usmart_dev.ptFuns[byLoop].name);
			printf("\r\n");
			break;	 
		case 3://查询ID
			printf("\r\n");
			printf("-------------------------函数 ID --------------------------- \r\n");
			for(byLoop=0;byLoop<usmart_dev.fnum;byLoop++)
			{
				usmart_get_fname((UINT8*)usmart_dev.ptFuns[byLoop].name,bySFName,&byPNum,&byRVal);//得到本地函数名 
				printf("%s id is:\r\n0X%08X\r\n",bySFName,usmart_dev.ptFuns[byLoop].func); //显示ID
			}
			printf("\r\n");
			break;
		case 4://hex指令
			printf("\r\n");
			usmart_get_aparm(bypStr,bySFName,&byLoop);
			if(byLoop==0)//参数正常
			{
				byLoop = usmart_str2num(bySFName,&dwRes);	   	//记录该参数	
				if(byLoop==0)						  	//进制转换功能
				{
					printf("HEX:0X%X\r\n",dwRes);	   	//转为16进制
				}
                else if(byLoop!=4)
                {
                    return USMART_PARMERR;//参数错误.
                }
				else 				   				//参数显示设定功能
				{
					printf("16进制参数显示!\r\n");
					usmart_dev.sptype=SP_TYPE_HEX;  
				}

			}
            else 
            {
                return USMART_PARMERR;          //参数错误.
            }
			printf("\r\n"); 
			break;
		case 5://dec指令
			printf("\r\n");
			usmart_get_aparm(bypStr,bySFName,&byLoop);
			if(byLoop==0)//参数正常
			{
				byLoop=usmart_str2num(bySFName,&dwRes);	   	//记录该参数	
				if(byLoop==0)						   	//进制转换功能
				{
					printf("DEC:%lu\r\n",dwRes);	   	//转为10进制
				}
                else if(byLoop!=4)
                {
                    return USMART_PARMERR;//参数错误.
                }
				else 				   				//参数显示设定功能
				{
					printf("10进制参数显示!\r\n");
					usmart_dev.sptype=SP_TYPE_DEC;  
				}

			}
            else 
            {
                return USMART_PARMERR;          //参数错误. 
            }
			printf("\r\n"); 
			break;	 
		case 6://runtime指令,设置是否显示函数执行时间
			printf("\r\n");
			usmart_get_aparm(bypStr,bySFName,&byLoop);
			if(byLoop==0)//参数正常
			{
				byLoop=usmart_str2num(bySFName,&dwRes);	   		//记录该参数	
				if(byLoop==0)						   		//读取指定地址数据功能
				{
					if(USMART_ENTIMX_SCAN==0)
					{
                        printf("\r\nError! \r\nTo EN RunTime function,Please set USMART_ENTIMX_SCAN = 1 first!\r\n");//报错
                    }
					else
					{
						usmart_dev.runtimeflag=dwRes;
						if(usmart_dev.runtimeflag)
						{
                            printf("Run Time Calculation ON\r\n");
                        }
						else 
						{
                            printf("Run Time Calculation OFF\r\n"); 
                        }
					}
				}
                else 
                {
                    return USMART_PARMERR;              //未带参数,或者参数错误  
                }
 			}
            else 
            {
                return USMART_PARMERR;              //参数错误. 
            }
			printf("\r\n"); 
			break;	    
		default://非法指令
			return USMART_FUNCERR;
	}
	return 0;
}
// ============================================================================
// TIM4_IRQHandler和Timer4_Init,需要根据MCU特点自行修改.确保计数器计数频率为:10Khz即可.另外,定时器不要开启自动重装载功能!!
// ============================================================================
#if USMART_ENTIMX_SCAN==1
// ============================================================================
// 函数功能:复位runtime 
// 输入参数:无
// 返 回 值:无
// ============================================================================
void usmart_reset_runtime(void)
{
    __HAL_TIM_CLEAR_FLAG(    &TIM4_Handler,TIM_FLAG_UPDATE);//清除中断标志位 
    __HAL_TIM_SET_AUTORELOAD(&TIM4_Handler,0XFFFF);         //重装载值设置到最大
    __HAL_TIM_SET_COUNTER(   &TIM4_Handler,0);              //清空定时器的CNT
    
	usmart_dev.runtime=0;	
}
// ============================================================================
// 函数功能:获取函数运行时间,通过读取CNT值获取,由于usmart是通过中断调用的函数,
// 所以定时器中断不再有效,此时最大限度,只能统计2次CNT的值,也就是清零后+溢出一次,
// 当溢出超过2次,没法处理,所以最大延时,控制在:2*计数器CNT*0.1ms.对STM32来说,是:13.1s左右
// 输入参数:
// 返 回 值:执行时间,单位:0.1ms,最大延时时间为定时器CNT值的2倍*0.1ms
// ============================================================================
UINT32 usmart_get_runtime(void)
{
	if(__HAL_TIM_GET_FLAG(&TIM4_Handler,TIM_FLAG_UPDATE)==SET)//在运行期间,产生了定时器溢出
	{
		usmart_dev.runtime = usmart_dev.runtime + 0XFFFF;
	}
    
	usmart_dev.runtime = usmart_dev.runtime + __HAL_TIM_GET_COUNTER(&TIM4_Handler);

    //返回计数值
	return usmart_dev.runtime;		
}  
// ============================================================================
// 函数功能:定时器4中断服务程序
// 输入参数:无
// 返 回 值:无
// ============================================================================
void TIM4_IRQHandler(void)
{ 		    		  			       
    if(__HAL_TIM_GET_IT_SOURCE(&TIM4_Handler,TIM_IT_UPDATE)==SET)//溢出中断
    {
        usmart_dev.scan();	//执行usmart扫描
        __HAL_TIM_SET_COUNTER(   &TIM4_Handler,0);;     //清空定时器的CNT
        __HAL_TIM_SET_AUTORELOAD(&TIM4_Handler,100);    //恢复原来的设置
    }
    __HAL_TIM_CLEAR_IT(&TIM4_Handler, TIM_IT_UPDATE);   //清除中断标志位
}
// ============================================================================
// 函数功能:使能定时器4,使能中断.
// 输入参数:自动装载值,分频值
// 返 回 值:无
// ============================================================================
void Timer4_Init(UINT16 wArr,UINT16 wPsc)
{ 
    
    __HAL_RCC_TIM4_CLK_ENABLE();            //定时器4
    
    HAL_NVIC_SetPriority(TIM4_IRQn,3,3);    //设置中断优先级，抢占优先级3，子优先级3
    HAL_NVIC_EnableIRQ(  TIM4_IRQn);        //开启ITM4中断    
    
    TIM4_Handler.Instance           = TIM4;                   //通用定时器4
    TIM4_Handler.Init.Prescaler     = wPsc;                   //分频
    TIM4_Handler.Init.CounterMode   = TIM_COUNTERMODE_UP;     //向上计数器
    TIM4_Handler.Init.Period        = wArr;                   //自动装载值
    TIM4_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init(    &TIM4_Handler);
    HAL_TIM_Base_Start_IT(&TIM4_Handler); //使能定时器4和定时器4中断 					 
}
#endif

// ============================================================================
// 函数功能:初始化串口控制器
// 输入参数:sysclk:系统时钟（Mhz）
// 返 回 值:
// ============================================================================
void usmart_init(UINT8 bySysclk)
{
#if USMART_ENTIMX_SCAN==1
    //分频,时钟为10K ,100ms中断一次,注意,计数频率必须为10Khz,以和runtime单位(0.1ms)同步.
	Timer4_Init(1000,(UINT32)bySysclk*100-1);
#endif
	usmart_dev.sptype=1;	//十六进制显示参数
}
// ============================================================================
// 函数功能:从str中获取函数名,id,及参数信息
// 输入参数:字符串指针.
// 返 回 值:0,识别成功;其他,错误代码.
// ============================================================================
UINT8 usmart_cmd_rec(UINT8 *pbyStr) 
{
	UINT8 bySta,byLoop,byRval;      //状态	 
	UINT8 byRpnum,bySpnum;
	UINT8 byRfname[MAX_FNAME_LEN];  //暂存空间,用于存放接收到的函数名  
	UINT8 bySfname[MAX_FNAME_LEN];  //存放本地函数名
	
	bySta = usmart_get_fname(pbyStr,byRfname,&byRpnum,&byRval);//得到接收到的数据的函数名及参数个数	  

    if(bySta)
	{
        return bySta;//错误
    }
	for(byLoop=0;byLoop<usmart_dev.fnum;byLoop++)
	{
		bySta = usmart_get_fname((UINT8*)usmart_dev.ptFuns[byLoop].name,bySfname,&bySpnum,&byRval);//得到本地函数名及参数个数
		if(bySta)
		{
            return bySta;//本地解析有误     
        }
        
		if(usmart_strcmp(bySfname,byRfname)==0)     //相等
		{
			if(bySpnum>byRpnum)
			{
                return USMART_PARMERR;              //参数错误(输入参数比源函数参数少)
            }
			usmart_dev.id = byLoop;                 //记录函数ID.
			break;
		}	
	}
    
	if(byLoop==usmart_dev.fnum)
	{
        return USMART_NOFUNCFIND;                   //未找到匹配的函数
    }
    
 	bySta = usmart_get_fparam(pbyStr,&byLoop);		//得到函数参数个数	

    if(bySta)
    {
        return bySta;                               //返回错误
    }
	usmart_dev.pnum = byLoop;						//参数个数记录
    return USMART_OK;
}
// ============================================================================
// 函数功能:usamrt执行函数,最终执行从串口收到的有效函数.
// 输入参数:
// 返 回 值:
// ============================================================================
// 最多支持10个参数的函数,更多的参数支持也很容易实现.不过用的很少.一般5个左右的参数的函数已经很少见了.
// 该函数会在串口打印执行情况.以:"函数名(参数1，参数2...参数N)=返回值".的形式打印.
// 当所执行的函数没有返回值的时候,所打印的返回值是一个无意义的数据.
void usmart_exe(void)
{
	UINT8  byId,byLoop;
	UINT8  bySfname[MAX_FNAME_LEN];//存放本地函数名
	UINT8  byPnum,byRval;
	UINT32 dwRes;		   
	UINT32 dwTemp[MAX_PARM];//参数转换,使之支持了字符串 
    
	byId = usmart_dev.id;
    
	if(byId >= usmart_dev.fnum)
	{
        return;//不执行.
    }
    
	usmart_get_fname((UINT8*)usmart_dev.ptFuns[byId].name,bySfname,&byPnum,&byRval);//得到本地函数名,及参数个数 

    printf("\r\n%s(",bySfname);//输出正要执行的函数名

    for(byLoop=0;byLoop<byPnum;byLoop++)//输出参数
	{
		if(usmart_dev.parmtype&(1<<byLoop))//参数是字符串
		{
			printf("%c",'"');			 
			printf("%s",usmart_dev.parm+usmart_get_parmpos(byLoop));
			printf("%c",'"');
			dwTemp[byLoop]=(UINT32)&(usmart_dev.parm[usmart_get_parmpos(byLoop)]);
		}
        else						  //参数是数字
		{
			dwTemp[byLoop]=*(UINT32*)(usmart_dev.parm+usmart_get_parmpos(byLoop));
			if(usmart_dev.sptype==SP_TYPE_DEC)
			{
                printf("%ld",dwTemp[byLoop]);//10进制参数显示
            }
			else 
			{
                printf("0X%X",dwTemp[byLoop]);//16进制参数显示       
            }
		}
		if(byLoop!=byPnum-1)
		{
            printf(",");
        }
	}
    
	printf(")");
    
#if USMART_ENTIMX_SCAN==1
	usmart_reset_runtime();	//计时器清零,开始计时
#endif

	switch(usmart_dev.pnum)
	{
		case 0://无参数(void类型)											  
			dwRes=(*(UINT32(*)())usmart_dev.ptFuns[byId].func)();
			break;
	    case 1://有1个参数
			dwRes=(*(UINT32(*)())usmart_dev.ptFuns[byId].func)(dwTemp[0]);
			break;
	    case 2://有2个参数
			dwRes=(*(UINT32(*)())usmart_dev.ptFuns[byId].func)(dwTemp[0],dwTemp[1]);
			break;
	    case 3://有3个参数
			dwRes=(*(UINT32(*)())usmart_dev.ptFuns[byId].func)(dwTemp[0],dwTemp[1],dwTemp[2]);
			break;
	    case 4://有4个参数
			dwRes=(*(UINT32(*)())usmart_dev.ptFuns[byId].func)(dwTemp[0],dwTemp[1],dwTemp[2],dwTemp[3]);
			break;
	    case 5://有5个参数
			dwRes=(*(UINT32(*)())usmart_dev.ptFuns[byId].func)(dwTemp[0],dwTemp[1],dwTemp[2],dwTemp[3],dwTemp[4]);
			break;
	    case 6://有6个参数
			dwRes=(*(UINT32(*)())usmart_dev.ptFuns[byId].func)(dwTemp[0],dwTemp[1],dwTemp[2],dwTemp[3],dwTemp[4],\
			dwTemp[5]);
			break;
	    case 7://有7个参数
			dwRes=(*(UINT32(*)())usmart_dev.ptFuns[byId].func)(dwTemp[0],dwTemp[1],dwTemp[2],dwTemp[3],dwTemp[4],\
			dwTemp[5],dwTemp[6]);
			break;
	    case 8://有8个参数
			dwRes=(*(UINT32(*)())usmart_dev.ptFuns[byId].func)(dwTemp[0],dwTemp[1],dwTemp[2],dwTemp[3],dwTemp[4],\
			dwTemp[5],dwTemp[6],dwTemp[7]);
			break;
	    case 9://有9个参数
			dwRes=(*(UINT32(*)())usmart_dev.ptFuns[byId].func)(dwTemp[0],dwTemp[1],dwTemp[2],dwTemp[3],dwTemp[4],\
			dwTemp[5],dwTemp[6],dwTemp[7],dwTemp[8]);
			break;
	    case 10://有10个参数
			dwRes=(*(UINT32(*)())usmart_dev.ptFuns[byId].func)(dwTemp[0],dwTemp[1],dwTemp[2],dwTemp[3],dwTemp[4],\
			dwTemp[5],dwTemp[6],dwTemp[7],dwTemp[8],dwTemp[9]);
			break;
	}
    
#if USMART_ENTIMX_SCAN==1
	usmart_get_runtime();//获取函数执行时间
#endif

    //需要返回值.
	if(byRval==1)
	{
		if(usmart_dev.sptype==SP_TYPE_DEC)
		{
            printf("=%lu;\r\n",dwRes);//输出执行结果(10进制参数显示)
        }
		else 
		{
            printf("=0X%X;\r\n",dwRes);//输出执行结果(16进制参数显示)      
        }
	}
    else 
    {
        printf(";\r\n");//不需要返回值,直接输出结束        
    }

    //需要显示函数执行时间
	if(usmart_dev.runtimeflag)	
	{ 
		printf("Function Run Time:%d.%1dms\r\n",usmart_dev.runtime/10,usmart_dev.runtime%10);//打印函数执行时间 
	}	
}
// ============================================================================
// 函数功能:usmart扫描函数,实现usmart的各个控制.该函数需要每隔一定时间被调用一次
// 以及时执行从串口发过来的各个函数,可以在中断里面调用,从而实现自动管理.
// 输入参数:无
// 返 回 值:无
// ============================================================================
void usmart_scan(void)
{
	UINT8 bySta,byLen;  
    
    //串口接收完成？
	if(USART_RX_STA&0x8000)
	{					   
		byLen = USART_RX_STA&0x3fff;	//得到此次接收到的数据长度
		
		USART_RX_BUF[byLen] = '\0';	//在末尾加入结束符. 
		
		bySta = usmart_dev.cmd_rec(USART_RX_BUF);//得到函数各个信息
		
		if(bySta==0)
		{
            usmart_dev.exe();   //执行函数 
        }
		else 
		{  
			byLen=usmart_sys_cmd_exe(USART_RX_BUF);
            
			if(byLen!=USMART_FUNCERR)
			{
                bySta = byLen;
            }
            
			if(bySta)
			{   
				switch(bySta)
				{
					case USMART_FUNCERR:
						printf("函数错误!\r\n");
						break;	
					case USMART_PARMERR:
						printf("参数错误!\r\n");   			
						break;				
					case USMART_PARMOVER:
						printf("参数太多!\r\n");   			
						break;		
					case USMART_NOFUNCFIND:
						printf("未找到匹配的函数!\r\n");   			
						break;		
				}
			}
		}
		USART_RX_STA=0;//状态寄存器清空	    
	}
}
#if USMART_USE_WRFUNS==1 	//如果使能了读写操作
// ============================================================================
// 函数功能:读取指定地址的值
// 输入参数:无
// 返 回 值:无
// ============================================================================
UINT32 read_addr(UINT32 dwAddr)
{
	return *(UINT32*)dwAddr;
}
// ============================================================================
// 函数功能:在指定地址写入指定的值	
// 输入参数:无
// 返 回 值:无
// ============================================================================
void write_addr(UINT32 dwAddr,UINT32 dwVal)
{
	*(UINT32*)dwAddr = dwVal; 	
}
#endif
// ============================================================================



