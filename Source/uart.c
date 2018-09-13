#include <stc15.h>
#include"uart.h"
/*void UART1_init_57600()	  //����1��ʼ��
{
	ACC = P_SW1;
    ACC &= ~(S1_S0 | S1_S1);    //S1_S0=0 S1_S1=0
    P_SW1 = ACC;                //(P3.0/RxD, P3.1/TxD)

	SCON = 0x50;		//8λ����,�ɱ䲨����
	AUXR |= 0x01;		//����1ѡ��ʱ��2Ϊ�����ʷ�����
	AUXR |= 0x04;		//��ʱ��2ʱ��ΪFosc,��1T
	T2L = 0xD0;			//�趨��ʱ��ֵ
	T2H = 0xFF;			//�趨��ʱ��ֵ
	AUXR |= 0x10;		//������ʱ��2
}*/

void UART2_init_57600()   //����2��ʼ��
{
	P_SW2 &= ~S2_S0;    //S2_S0=0 (P1.0/RxD2, P1.1/TxD2) 
	S2CON = 0x50;		//8λ����,�ɱ䲨����
	AUXR |= 0x04;		//��ʱ��2ʱ��ΪFosc,��1T
	T2L = 0xD0;			//�趨��ʱ��ֵ
	T2H = 0xFF;			//�趨��ʱ��ֵ
	AUXR |= 0x10;		//������ʱ��2
}

/*void UART1_Send_Byte(unsigned char dat)//����1����һ���ֽڵ�����
{  
	SBUF = dat;     //�����ݷ��ڴ���1���ͻ�������
	while(!TI);  	//�ȴ����ݷ������
	TI = 0;         //���TIλ
}*/

void UART2_Send_Byte(unsigned char dat)//����2����һ���ֽڵ�����
{  
	S2BUF = dat;              //�����ݷ��ڴ���2���ͻ�������
	while(!(S2CON & S2TI));   //�ȴ����ݷ������
	S2CON &= ~S2TI;           //���S2TIλ
}

/*unsigned char UART1_Receive_Byte()//����2����һ���ֽڵ�����
{	
	unsigned char dat;     
	while(!RI);    		//�ȴ����ݽ������
	RI = 0;            	//���RIλ
	dat = SBUF;         //�����ݴӽ��ջ������ж������ŵ�dat��
	return (dat);		//���ؽ��յ�������
}*/

unsigned char UART2_Receive_Byte()//����2����һ���ֽڵ�����
{	
	unsigned char dat;     
	while(!(S2CON & S2RI));    //�ȴ����ݽ������
	S2CON &= ~S2RI;            //���S2RIλ
	dat = S2BUF;               //�����ݴӽ��ջ������ж������ŵ�dat��
	return (dat);
}