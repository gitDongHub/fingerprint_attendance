#define S2RI  0x01              //S2CON.0
#define S2TI  0x02              //S2CON.1
#define S2RB8 0x04              //S2CON.2
#define S2TB8 0x08              //S2CON.3

// #define S1_S0 0x40              //P_SW1.6
// #define S1_S1 0x80              //P_SW1.7
#define S2_S0 0x01              //P_SW2.0

// void UART1_init_57600();	  				//����1��ʼ��
void UART2_init_57600();   					//����2��ʼ��
// void UART1_Send_Byte(unsigned char dat);	//����1����һ���ֽڵ�����
void UART2_Send_Byte(unsigned char dat);	//����2����һ���ֽڵ�����
// unsigned char UART1_Receive_Byte();			//����1����һ���ֽڵ�����
unsigned char UART2_Receive_Byte();			//����2����һ���ֽڵ�����