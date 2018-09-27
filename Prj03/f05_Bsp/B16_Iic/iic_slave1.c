#define START_STATE     0 //开始
#define CONTROL_STATE   1//控制命令
#define ACK_STATE       2 //ACK应答
#define NOACK_STATE     3 //非ACK应答
#define WRITE_STATE     4 //写从机
#define READ_STATE      5//读从机
#define STOP_STATE      6//停止

BYTE STATE=0;
BYTE address=0;//接收到的从机地址
BYTE count=0;//接收到一位计数，产生一个字节的计数
BYTE receive_BYTE;//从机接收主机单个字节
BYTE receive_buf[20]={0x00};//从机接收主机数据的buf
BYTE write_buf[20]={0x47,0x55,0x11};//从机发送数据给主机的buf
BYTE receive_len=0;//从机接收主机多字节时的长度
BYTE send_len=0;//从机要发送给主机字节的长度
BYTE write_end=0;//主机是否对从机写完
BYTE read_end=0;//主机是否接收完从机发送的数据
BYTE length_ACK=0;//从机是否收到了要发送数据给主机的长度
BYTE read_num=0;//
BYTE Temp=0;//

void PORT1_InterruptInit(void)
{
    EA=1;
    P1DIR&=0XFB;//p1.2输入，SCL
    IEN2|=0X10;//p1中断使能，scl
    P1IEN|=0X04;//p1.2中断使能，scl
    PICTL&=~0X02;//p1上升沿中断0:rising edge
    P1IFG&=~0X04;    
}

#pragma vector = P1INT_VECTOR
__interrupt void P1_ISR(void)
{
    if(P1IFG>0)//按键中断
    {
        P1IFG=0;
        LED1=~LED1;
    }        
    P1IF=0;//清中断标志

    if(P1IFG>0)
    {
        P1IFG=0;
        switch(STATE)
        {
            case START_STATE:
                SDA_(IN);
                if(SDA)
                {
                    while(SDA);
                    while(SCL);
                    STATE=CONTROL_STATE;
                }
                break;
             case CONTROL_STATE:
                address <<=1;
                if(1==SDA)
                {
                    address |= 0x01;
                }
                else
                {
                    address |= 0x00;
                }
                count++;
                if((address&0xfc)==0xa4)
                {
                    STATE = ACK_STATE;
                }
                else
                {
                    STATE = NOACK_STATE;
                }
                break;
            case ACK_STATE:
                SDA_(OUT);//SDA设置为输出
                SDA=0;
                if((write_end==1)||(read_end==1))//已经读完或者写完
                {
                    STATE = STOP_STATE;
                    write_end = 0;
                    read_end  = 0;
                }
                else
                {
                    if((address & 0x01)==0x00)//主机写slave
                    {
                        STATE = WRITE_STATE;
                    }
                    else
                    {
                        STATE = READ_STATE;
                    }
                }
                break;
            case NOACK_STATE:
                SDA_(OUT);//SDA设置为输出
                SDA=1;
                address = 0;
                STATE = START_STATE;
                break;
            case WRITE_STATE://主机写从机
                SDA_(IN);//SDA设置为输入,因为发送ACK时候置为输出了
                if((address & 0x02)==0x00)//写单字节
                {
                    receive_BYTE <<=1;
                    if(1==SDA)
                    {
                        receive_BYTE |= 0X01;
                    }
                    else
                    {
                        receive_BYTE |= 0X00;
                    }
                    count++;
                    if(8==count)
                    {
                        STATE = ACK_STATE;
                        count = 0;
                        write_end = 1;
                    }
                }
                else
                {
                    receive_buf[receive_len]<<=1;
                    if(1==SDA)
                    {
                        receive_buf[receive_len] |= 0X01;
                    }
                    else
                    {
                        receive_buf[receive_len] |= 0X00;
                    }
                    count++;
                    if(8==count)
                    {
                        count = 0;
                        receive_len++;
                        UART1_Send_BYTE(receive_buf[receive_len-1]);
                        if(receive_len>=receive_buf[0]+1)
                        {
                            write_end=1;
                            receive_len=0;
                        }
                        STATE = ACK_STATE;
                    }
                }
                break;
            case READ_STATE://主机读从机
                if(!length_ACK)//主机发送过从机长度
                {
                    SDA_(IN);
                    send_len<<=1;
                    if(1==SDA)
                        send_len |= 0x01;
                    else
                        send_len |= 0x00;
                    count++;
                    if(8==count)
                    {
                        length_ACK = 1;//主机发送长度给从机
                        LED0=1;
                        count=0;
                        STATE = ACK_STATE;
                    }
                }
                else
                {
                    SDA_(OUT);
                    Temp = write_buf[read_num];
                    Temp<<=count;
                    UART1_Send_BYTE(Temp);
                    if((Temp&0x80)==0x80)
                        SDA=1;
                    else
                        SDA=0;
                    count++;

                    if(8==count)//移了7位，正好读一个字节
                    {
                        count = 0;
                        read_num++;
                        if(read_num>=send_len)//读完了所有数据
                        {
                            read_num=0;
                            read_end=1;
                            length_ACK = 0;
                        }
                        STATE = ACK_STATE;
                    }
                }
                break;
            case STOP_STATE:
                SDA_(IN);
                while(!SDA);
                address=0;
//                receive_BYTE=0;
                receive_len=0;
                send_len =0;
                LED1=~LED1;
                STATE = START_STATE;
                break;
            default:
                break;
        }
    }
    P1IF = 0;
}

