// ============================================================================
// Copyright (C) 2017-2018  All Rights Reserved
// 模块描述: 主函数(标准化软件工程)
// 模块版本: V1.00
// 创建人员: GuXY
// 创建时间: 2017-04-18
// ============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// ============================================================================

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "mySys.h"
#include "Led.h"
#include "Delay.h"
#include "usart.h"
#include "usmart.h"
#include "sdcard_sdio.h"
#include "adc_temperature.h" 
#include "rtc.h" 
#include "sdram.h"
#include "malloc.h"
#include "lwip/opt.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/lwip_timers.h"
#include "netif/etharp.h"
#include "ethernetif.h"
#include "app_ethernet.h"

#ifdef MACRO_TCP_CLIENT
#include "tcp_echoclient.h"
#endif
#ifdef MACRO_TCP_SERVER
#include "tcp_echoserver.h"
#endif
#ifdef MACRO_UDP_CLIENT
#include "udp_echoclient.h"
#endif
#ifdef MACRO_UDP_SERVER
#include "udp_echoserver.h"
#endif

/* Private typedef -----------------------------------------------------------*/
struct netif g_netif;
/* UART handler declaration */
UART_HandleTypeDef UartHandle;
/* Private define ------------------------------------------------------------*/
RTC_TimeTypeDef RTC_TimeStruct;
RTC_DateTypeDef RTC_DateStruct;
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
// ============================================================================
void BSP_Config(void)
{
    /* Init IO Expander */
    BSP_IO_Init();
    
    /* Enable IO Expander interrupt for ETH MII pin */
    BSP_IO_ConfigPin(MII_INT_PIN, IO_MODE_IT_FALLING_EDGE);
}
// ============================================================================
static void Netif_Config(void)
{
  ip_addr_t ipAdr;
  ip_addr_t netMask;
  ip_addr_t gw;
  
  IP4_ADDR(&ipAdr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
  IP4_ADDR(&netMask, NETMASK_ADDR0, NETMASK_ADDR1 , NETMASK_ADDR2, NETMASK_ADDR3);
  IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
  
  /* Add the network interface */    
  netif_add(&g_netif, &ipAdr, &netMask, &gw, NULL, &ethernetif_init, &ethernet_input);
  
  /* Registers the default network interface */
  netif_set_default(&g_netif);
  
  if (netif_is_link_up(&g_netif))
  {
    /* When the netif is fully configured this function must be called */
    netif_set_up(&g_netif);
  }
  else
  {
    /* When the netif link is down this function must be called */
    netif_set_down(&g_netif);
  }
  
  /* Set the link callback function, this function is called on change of link status */
  netif_set_link_callback(&g_netif, ethernetif_update_config);
}
// ============================================================================
//sdram debug
void FMC_SDRAM_Capacity(void)
{  
    UINT8  bySVal;
	UINT8  byTempBuf;	   
	UINT32 dwLoop;  

	bySVal = 0;	
    byTempBuf = 0;
    
	for(dwLoop=0;dwLoop<32*1024*1024;dwLoop+=16*1024)
	{
        FMC_SDRAM_WrBuf(&byTempBuf,dwLoop,4);
		byTempBuf++;
	}
 	for(dwLoop=0;dwLoop<32*1024*1024;dwLoop+=16*1024) 
	{	
        FMC_SDRAM_RdBuf(&byTempBuf,dwLoop,4);
            
		if(dwLoop==0)
        {
            bySVal = byTempBuf;
        }
 		else if(byTempBuf<=bySVal)
        {
            break;   
        }
        printf("SDRAM Capacity:%dKB\r\n",(UINT16)(byTempBuf-bySVal+1)*16);
 	}	
}	
// ============================================================================
// 函数描述:主函数
// 输入参数:无
// 返 回 值:无
// ============================================================================
int main(void)
{
    FLOAT32 fTemperature;

    fTemperature = 0;
    
    HAL_Init();

    /* Configure the System clock */
    SystemClock_Config();

    /* Add your application code here*/
    Led_Init();

    //初始化延时函数
    delay_init(120);

    //串口初始化，设置波特率
    uart_init(9600);

    //usmart初始化设置
    usmart_dev.init(90);

    //内部温度传感器采集初始化
    Adc_Temperature_Init();

    //RTC初始化
    Rtc_Init();

    //初始化SDRAM
    SDRAM_Init();

    //初始化内存池
    my_mem_init(SRAMIN);		    
	my_mem_init(SRAMEX);		   
	my_mem_init(SRAMCCM);	

    printf("================================================\r\n");
    printf("=                                              =\r\n");
    printf("=      软件名称:标准化嵌入式软件               =\r\n");
    printf("=      软件版本:V1.1                           =\r\n");
    printf("=      开发人员:sanyu                          =\r\n");
    printf("=      软件功能:CAN通信、SD卡处理、shell等     =\r\n");
    printf("=                                              =\r\n");
    printf("=----------------------------------------------=\r\n");
    printf("=                                              =\r\n");
    printf("=        ^&^ -杜绝盗版,盗版必究- ^&^           =\r\n");
    printf("= Copyright (C) 2017-2027  All Rights Reserved =\r\n");
    printf("=                                              =\r\n");
    printf("================================================\r\n");
    
    /* Configure the BSP */
    BSP_Config();
      
    /* Initilaize the LwIP stack */
    lwip_init();
    
    /* Configure the Network interface */
    Netif_Config();

    /*  tcp/udp echo  Init */
#ifdef MACRO_TCP_CLIENT
        while(tcp_echoclient_connect()!=ERR_OK);  
#endif
#ifdef MACRO_TCP_SERVER
    tcp_echoserver_init();
#endif
#ifdef MACRO_UDP_CLIENT
    udp_echoclient_connect();
#endif
#ifdef MACRO_UDP_SERVER
    udp_echoserver_init();
#endif

    /* Notify user about the netwoek interface config */
    User_notification(&g_netif);

    //SDRAM大小获取
	FMC_SDRAM_Capacity();
    //SD卡检测
 	if(SD_Init())
	{
        printf("**********************************************\r\n");
        HAL_GPIO_WritePin(GPIOG,CN_LED1_PIN ,GPIO_PIN_SET); 
		delay_ms(2000);		
		printf("SD Card Error!Please Check!\r\n");
		delay_ms(2000);
        HAL_GPIO_WritePin(GPIOG,CN_LED1_PIN ,GPIO_PIN_RESET); 
        
        fTemperature = (FLOAT32)(getTemperature()/100.0);
        printf("温度:%f℃\r\n",fTemperature);
        
        HAL_RTC_GetTime(&RTC_Handler,&RTC_TimeStruct,RTC_FORMAT_BIN);
        printf("Time:%02d:%02d:%02d\r\n",RTC_TimeStruct.Hours,RTC_TimeStruct.Minutes,RTC_TimeStruct.Seconds); 
        HAL_RTC_GetDate(&RTC_Handler,&RTC_DateStruct,RTC_FORMAT_BIN);
        printf("Date:20%02d-%02d-%02d\r\n",RTC_DateStruct.Year,RTC_DateStruct.Month,RTC_DateStruct.Date); 
        printf("星期%d\r\n",RTC_DateStruct.WeekDay); 

	}
    
    //主任务
    while (1)
    {
        delay_ms(500);
        HAL_GPIO_WritePin(GPIOG,CN_LED3_PIN,GPIO_PIN_SET); 
        HAL_GPIO_WritePin(GPIOG,CN_LED4_PIN,GPIO_PIN_RESET); 

        delay_ms(500);
        HAL_GPIO_WritePin(GPIOG,CN_LED3_PIN,GPIO_PIN_RESET); 
        HAL_GPIO_WritePin(GPIOG,CN_LED4_PIN,GPIO_PIN_SET); 

        /* Read a received packet from the Ethernet buffers and send it 
        to the lwIP for handling */
        ethernetif_input(&g_netif);

        /* Handle timeouts */
        sys_check_timeouts();
    }
}
// ============================================================================

