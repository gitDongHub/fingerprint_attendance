#include <stc15.h>
#include"uart.h"
/*void UART1_init_57600()	  //串口1初始化
{
	ACC = P_SW1;
    ACC &= ~(S1_S0 | S1_S1);    //S1_S0=0 S1_S1=0
    P_SW1 = ACC;                //(P3.0/RxD, P3.1/TxD)

	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x01;		//串口1选择定时器2为波特率发生器
	AUXR |= 0x04;		//定时器2时钟为Fosc,即1T
	T2L = 0xD0;			//设定定时初值
	T2H = 0xFF;			//设定定时初值
	AUXR |= 0x10;		//启动定时器2
}*/

void UART2_init_57600()   //串口2初始化
{
	P_SW2 &= ~S2_S0;    //S2_S0=0 (P1.0/RxD2, P1.1/TxD2) 
	S2CON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x04;		//定时器2时钟为Fosc,即1T
	T2L = 0xD0;			//设定定时初值
	T2H = 0xFF;			//设定定时初值
	AUXR |= 0x10;		//启动定时器2
}

/*void UART1_Send_Byte(unsigned char dat)//串口1发送一个字节的数据
{  
	SBUF = dat;     //把数据放在串口1发送缓冲器中
	while(!TI);  	//等待数据发送完毕
	TI = 0;         //清除TI位
}*/

void UART2_Send_Byte(unsigned char dat)//串口2发送一个字节的数据
{  
	S2BUF = dat;              //把数据放在串口2发送缓冲器中
	while(!(S2CON & S2TI));   //等待数据发送完毕
	S2CON &= ~S2TI;           //清除S2TI位
}

/*unsigned char UART1_Receive_Byte()//串口2接受一个字节的数据
{	
	unsigned char dat;     
	while(!RI);    		//等待数据接收完毕
	RI = 0;            	//清除RI位
	dat = SBUF;         //把数据从接收缓冲器中读出来放到dat中
	return (dat);		//返回接收到的数据
}*/

unsigned char UART2_Receive_Byte()//串口2接受一个字节的数据
{	
	unsigned char dat;     
	while(!(S2CON & S2RI));    //等待数据接收完毕
	S2CON &= ~S2RI;            //清除S2RI位
	dat = S2BUF;               //把数据从接收缓冲器中读出来放到dat中
	return (dat);
}