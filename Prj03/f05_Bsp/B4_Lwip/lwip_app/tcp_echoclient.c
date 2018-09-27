// ============================================================================
// Copyright (C) 2017-2018  All Rights Reserved
// 模块描述: TCP/通信Client组件
// 模块版本: V1.00
// 创建人员: GuXY
// 创建时间: 2017-04-18
// ============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// ============================================================================
#include "main.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "lwip/memp.h"
#include <stdio.h>
#include <string.h>

#if LWIP_TCP
// ============================================================================
#define CN_SEND_LEN         ( 100 )
#define CN_RECV_LEN         ( 100 )
// ============================================================================
UINT8   byClientSendBuf[CN_SEND_LEN];
UINT8   byClientRecvBuf[CN_RECV_LEN];
struct tcp_pcb *g_clientPcb;
// ============================================================================
/* ECHO protocol states */
enum echoclient_states
{
  ES_NOT_CONNECTED = 0,
  ES_CONNECTED,
  ES_RECEIVED,
  ES_CLOSING,
};
/* structure to be passed as argument to the tcp callbacks */
struct echoclient
{
  enum echoclient_states state; /* connection status */
  struct tcp_pcb *pcb;          /* pointer on the current tcp_pcb */
  struct pbuf *p_tx;            /* pointer on pbuf to be transmitted */
};
// ============================================================================
static err_t tcp_echoclient_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static void  tcp_echoclient_connection_close(struct tcp_pcb *tpcb, struct echoclient * es);
static err_t tcp_echoclient_poll(void *arg, struct tcp_pcb *tpcb);
static err_t tcp_echoclient_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
static void  tcp_echoclient_send(struct tcp_pcb *tpcb, struct echoclient * es);
static err_t tcp_echoclient_connected(void *arg, struct tcp_pcb *tpcb, err_t err);
// ============================================================================
/**
  * @brief  Connects to the TCP echo server
  * @param  None
  * @retval None
  */
err_t tcp_echoclient_connect(void)
{
    err_t ret_err;
    ip_addr_t DestIPaddr;
  
    /* create new tcp pcb */
    g_clientPcb = tcp_new();

    if (g_clientPcb != NULL)
    {
        IP4_ADDR( &DestIPaddr, DEST_IP_ADDR0, DEST_IP_ADDR1, DEST_IP_ADDR2, DEST_IP_ADDR3 );

        /* connect to destination address/port */
        ret_err = tcp_connect(g_clientPcb,&DestIPaddr,TCP_CLIENT_PORT,tcp_echoclient_connected);

        if(ret_err == ERR_OK)
        {
            return ERR_OK;
        }
        else
        {
            tcp_abort(g_clientPcb);
            return ERR_TIMEOUT;
        }
    }
    return ERR_OK;
}
//======================================================================================
//函数功能:用户应用程序调用此发送数据
//输入参数:无
//返 回 值:0-成功；其他-失败
//======================================================================================
err_t tcp_client_usersent(void)
{
    struct echoclient *es;
    
    /* allocate structure es to maintain tcp connection informations */
    es = g_clientPcb->callback_arg;

    if (es != NULL)
    {
        /* allocate pbuf */
        es->p_tx = pbuf_alloc(PBUF_TRANSPORT, strlen((char*)byClientRecvBuf) , PBUF_POOL);

        if(es->p_tx)
        {       
            /* copy data to pbuf */
            pbuf_take(es->p_tx, (char*)byClientRecvBuf, strlen((char*)byClientRecvBuf));

            /* send data */
            tcp_echoclient_send(g_clientPcb,es);

            if(es->p_tx)pbuf_free(es->p_tx);  //释放内存
        }
        memset(byClientRecvBuf,0,CN_RECV_LEN); 
        return ERR_OK;
    }
    else
    {
        //终止连接,删除pcb控制块
		tcp_abort(g_clientPcb);
		return ERR_ABRT;
    }
}
//======================================================================================
/**
  * @brief Function called when TCP connection established
  * @param tpcb: pointer on the connection contol block
  * @param err: when connection correctly established err should be ERR_OK 
  * @retval err_t: returned error 
  */
static err_t tcp_echoclient_connected(void *arg, struct tcp_pcb *tpcb, err_t err)
{
  struct echoclient *es = NULL;
  
  if (err == ERR_OK)   
  {
    /* allocate structure es to maintain tcp connection informations */
    es = (struct echoclient *)mem_malloc(sizeof(struct echoclient));
  
    if (es != NULL)
    {
      es->state = ES_CONNECTED;
      es->pcb = tpcb;
      
      sprintf((char*)byClientSendBuf, "tcp client connect to server successfully!");

      /* allocate pbuf */
      es->p_tx = pbuf_alloc(PBUF_TRANSPORT, strlen((char*)byClientSendBuf) , PBUF_POOL);
         
      if (es->p_tx)
      {       
        /* copy data to pbuf */
        pbuf_take(es->p_tx, (char*)byClientSendBuf, strlen((char*)byClientSendBuf));
        
        /* pass newly allocated es structure as argument to tpcb */
        tcp_arg(tpcb, es);
  
        /* initialize LwIP tcp_recv callback function */ 
        tcp_recv(tpcb, tcp_echoclient_recv);
  
        /* initialize LwIP tcp_sent callback function */
        tcp_sent(tpcb, tcp_echoclient_sent);
  
        /* initialize LwIP tcp_poll callback function */
        tcp_poll(tpcb, tcp_echoclient_poll, 1);
    
        /* send data */
        tcp_echoclient_send(tpcb,es);
        
        return ERR_OK;
      }
    }
    else
    {
      /* close connection */
      tcp_echoclient_connection_close(tpcb, es);
      
      /* return memory allocation error */
      return ERR_MEM;  
    }
  }
  else
  {
    /* close connection */
    tcp_echoclient_connection_close(tpcb, es);
  }
  return err;
}
    
/**
  * @brief tcp_receiv callback
  * @param arg: argument to be passed to receive callback 
  * @param tpcb: tcp connection control block 
  * @param err: receive error code 
  * @retval err_t: retuned error  
  */
static err_t tcp_echoclient_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{ 
  struct echoclient *es;
  err_t ret_err; 
  struct pbuf *q;
  UINT32 dwCurDataLen=0;

  LWIP_ASSERT("arg != NULL",arg != NULL);
  
  es = (struct echoclient *)arg;
  
  /* if we receive an empty tcp frame from server => close connection */
  if (p == NULL)
  {
    /* remote host closed connection */
    es->state = ES_CLOSING;
    if(es->p_tx == NULL)
    {
       /* we're done sending, close connection */
       tcp_echoclient_connection_close(tpcb, es);
    }
    else
    {    
      /* send remaining data*/
      tcp_echoclient_send(tpcb, es);
    }
    ret_err = ERR_OK;
  }   
  /* else : a non empty frame was received from echo server but for some reason err != ERR_OK */
  else if(err != ERR_OK)
  {
    /* free received pbuf*/
    if (p != NULL)
    {
      pbuf_free(p);
    }
    ret_err = err;
  }
  else if(es->state == ES_CONNECTED)
  {

    if(p != NULL)
    {
        //数据区清零
        memset(byClientRecvBuf,0,CN_RECV_LEN); 
        
        //遍历完整个pbuf链表
        for(q=p;q!=NULL;q=q->next)  
        {
            if(q->len > (CN_RECV_LEN-dwCurDataLen))
            {
                //拷贝数据
                memcpy(byClientRecvBuf+dwCurDataLen,q->payload,(CN_RECV_LEN-dwCurDataLen));
            }
            else
            {
                memcpy(byClientRecvBuf+dwCurDataLen,q->payload,q->len);
            }
            dwCurDataLen += q->len;   

            if(dwCurDataLen > CN_RECV_LEN)
            {
                break; //超出TCP客户端接收数组,跳出    
            }
        }
    }

    /* Acknowledge data reception */
    tcp_recved(tpcb, p->tot_len);  
    
    pbuf_free(p);
//    tcp_echoclient_connection_close(tpcb, es);
    ret_err = ERR_OK;
  }

  /* data received when connection already closed */
  else
  {
    /* Acknowledge data reception */
    tcp_recved(tpcb, p->tot_len);
    
    /* free pbuf and do nothing */
    pbuf_free(p);
    ret_err = ERR_OK;
  }

  tcp_client_usersent();
  
  return ret_err;
}

/**
  * @brief function used to send data
  * @param  tpcb: tcp control block
  * @param  es: pointer on structure of type echoclient containing info on data 
  *             to be sent
  * @retval None 
  */
static void tcp_echoclient_send(struct tcp_pcb *tpcb, struct echoclient * es)
{
  struct pbuf *ptr;
  err_t wr_err = ERR_OK;
 
  while ((wr_err == ERR_OK) &&
         (es->p_tx != NULL) && 
         (es->p_tx->len <= tcp_sndbuf(tpcb)))
  {
    
    /* get pointer on pbuf from es structure */
    ptr = es->p_tx;

    /* enqueue data for transmission */
    wr_err = tcp_write(tpcb, ptr->payload, ptr->len, 1);
    
    if (wr_err == ERR_OK)
    { 
      /* continue with next pbuf in chain (if any) */
      es->p_tx = ptr->next;
      
      if(es->p_tx != NULL)
      {
        /* increment reference count for es->p */
        pbuf_ref(es->p_tx);
      }
      
      /* free pbuf: will free pbufs up to es->p (because es->p has a reference count > 0) */
      pbuf_free(ptr);
   }
   else if(wr_err == ERR_MEM)
   {
      /* we are low on memory, try later, defer to poll */
     es->p_tx = ptr;
   }
   else
   {
     /* other problem ?? */
   }
  }
}

/**
  * @brief  This function implements the tcp_poll callback function
  * @param  arg: pointer on argument passed to callback
  * @param  tpcb: tcp connection control block
  * @retval err_t: error code
  */
static err_t tcp_echoclient_poll(void *arg, struct tcp_pcb *tpcb)
{
  err_t ret_err;
  struct echoclient *es;

  es = (struct echoclient*)arg;
  if (es != NULL)
  {
    if (es->p_tx != NULL)
    {
      /* there is a remaining pbuf (chain) , try to send data */
      tcp_echoclient_send(tpcb, es);
    }
    else
    {
      /* no remaining pbuf (chain)  */
      if(es->state == ES_CLOSING)
      {
        /* close tcp connection */
        tcp_echoclient_connection_close(tpcb, es);
      }
    }
    ret_err = ERR_OK;
  }
  else
  {
    /* nothing to be done */
    tcp_abort(tpcb);
    ret_err = ERR_ABRT;
  }
  return ret_err;
}

/**
  * @brief  This function implements the tcp_sent LwIP callback (called when ACK
  *         is received from remote host for sent data) 
  * @param  arg: pointer on argument passed to callback
  * @param  tcp_pcb: tcp connection control block
  * @param  len: length of data sent 
  * @retval err_t: returned error code
  */
static err_t tcp_echoclient_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
  struct echoclient *es;

  LWIP_UNUSED_ARG(len);

  es = (struct echoclient *)arg;
  
  if(es->p_tx != NULL)
  {
    /* still got pbufs to send */
    tcp_echoclient_send(tpcb, es);
  }

  return ERR_OK;
}

/**
  * @brief This function is used to close the tcp connection with server
  * @param tpcb: tcp connection control block
  * @param es: pointer on echoclient structure
  * @retval None
  */
static void tcp_echoclient_connection_close(struct tcp_pcb *tpcb, struct echoclient * es )
{
  /* remove callbacks */
  tcp_recv(tpcb, NULL);
  tcp_sent(tpcb, NULL);
  tcp_poll(tpcb, NULL,0);

  if (es != NULL)
  {
    mem_free(es);
  }

  /* close tcp connection */
  tcp_close(tpcb);  
}

#endif /* LWIP_TCP */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
