#include <stc15.h>
#include "ds1302.h"
#include "oled.h"
#include "uart.h"
#include "user.h"

#define FOSC 11059200L

#define uchar unsigned char 
#define uint unsigned int

#define const_voice_short  40  
#define const_led_long   120
#define const_key_time  40   

sbit memu = P0^3;
sbit ret  = P0^6;
sbit left   = P0^4;
sbit right = P0^5;

sbit touch = P0^7;
sbit beep = P3^2;
sbit red = P4^4;
sbit green = P4^1;

uchar ucKeySec = 0;   //一些需要在不同函数之间使用的核心变量，只能用全局变量
uchar uiWrongCnt = 0;
uchar uiPassCnt = 0;
uchar WindowSec = 1;	//窗口变量
uchar wd_update=1;  //窗口整屏更新显示标志，本节最核心的变量 

bit part_1_2_update = 1;
bit part_1_3_update = 1;
bit part_2_2_update = 1;
bit part_3_2_update = 1;
bit part_4_2_update = 1;
bit add_user = 0;
bit add_admin = 0;
bit identity_admin = 1;
bit identity_user  = 0;
bit delte_model    = 0;
bit delte_allmodel = 0;
uchar submemu_num  = 1;

uint t = 0;
int user_num = 1;
uint  uiVoiceCnt = 0;  //一些需要在不同函数之间使用的核心变量，只能用全局变量
fp xdata user_Ifo[301];
//初始化时间日期暂存数组，秒、分、时、日、月、周、年 初值为= 17年6月19日 周1 12:58:50
uchar TimeData[7] ={20, 27, 16, 4, 7, 2, 17};

void init_mcu();
void delay(uint xms);
void init_peripheral();

/*void MCU_Recevice_Date(unsigned char ucLength);
void MCU_Synchronize_Time();
void MCU_Communicate_PC();*/

void T0_time();
void key_scan();
void key_service();

void time_update();
void OLED_Display();  	//OLED显示函数
void main_screen();	  	//主界面
void memu_screen();		//菜单界面
void ifo_screen();		//信息界面
void sub_screen();		//子菜单界面

void main()
{
	init_mcu();      					//初始化单片机
	delay(200);		  					//延时
	init_peripheral();					//初始化外设
	//MCU_Communicate_PC();				//单片机与计算机通信
	//add_admin = 1;
	while(1)
	{
		key_service();					//按键服务函数
		OLED_Display();					//OLED界面显示函数
		time_update();					//时间更新函数
	}
}

void init_mcu()
{
  	beep=1; 
  	TMOD=0x01;  

  	TH0=0xf8;   
  	TL0=0x2f;
//	UART1_init_57600();
  	UART2_init_57600();
}

void delay(uint xms)
{
	uint i,j;
	for(i=0;i<xms+3;i++)
	{
		for(j=0;j<102;j++);
	}
}

void init_peripheral()
{
	EA=1;     
 	ET0=1;    
	TR0=1; 
	OLED_Init();					//初始化OLED  
	OLED_Clear(); 					//清屏
	//Set_DS1302_Time(DS1302_W_ADDR);	//先写入时钟日历寄存器起始地址再设置时钟日历初值	
	Read_DS1302_Time(DS1302_R_ADDR); 	//先写入时钟日历寄存器起始地址再读出时钟日历写入到TimeData数组中
	//init_User_Ifo();
	//Set_User_Ifo();
	//add_admin = !(user_Ifo[0].isVaild);
}
void init_User_Ifo(void)
{
	unsigned int i;
	for(i=0;i<301;i++)
	{
		user_Ifo[i].ID = 0;
		user_Ifo[i].isVaild = 0;
		user_Ifo[i].sec = 0;
		user_Ifo[i].min = 0;
		user_Ifo[i].hour = 0;
	}
}
void Set_User_Ifo(void)
{
	unsigned int i;
	unsigned char* pTable;
	UART2_init_57600();
	pTable = FP_Get_ReadIndexTable(0);
	for(i = 0;i<256;i++)
	{
		user_Ifo[i].ID = i;
		user_Ifo[i].isVaild = (*(pTable + (i/8)) & (bit_num [(i%8)]));
	}
	UART2_init_57600();
	pTable = FP_Get_ReadIndexTable(1);
	for(i=256;i<301;i++)
	{
		user_Ifo[i].ID = i;
		user_Ifo[i].isVaild = (*(pTable + ((i/8)-32)) & (bit_num [(i%8)]));
	}
}
/*//单片机接收计算机的数据
void MCU_Recevice_Date(unsigned char ucLength)
{
	unsigned char i;
	for(i=0;i<ucLength;i++)
		UART1_MCU_RECEVICE_BUFFER[i] = UART1_Receive_Byte();
}

//单片机同步计算机时间
void MCU_Synchronize_Time()
{
	unsigned char i;
	for(i=0;i<7;i++)
	{
		TimeData[i] = UART1_MCU_RECEVICE_BUFFER[i+2];
	}
	UART1_Send_Byte(0x01);
	UART1_Send_Byte(0x02);
	for(i=2;i<9;i++)
	{
		UART1_Send_Byte(TimeData[i-2]);
	}
	UART1_Send_Byte(0xFF);
}
//单片机和计算机通信
void MCU_Communicate_PC()
{
	MCU_Recevice_Date(10);
	if(UART1_MCU_RECEVICE_BUFFER[1] == 0x01)
	{
		IAP_CONTR = 0x60;
	}
	else if(UART1_MCU_RECEVICE_BUFFER[1] == 0x02)
	{
		MCU_Synchronize_Time();
		Set_DS1302_Time(DS1302_W_ADDR);	//先写入时钟日历寄存器起始地址再设置时钟日历初值
		add_admin = 1;
	}
	else
		Read_DS1302_Time(DS1302_R_ADDR); 	//先写入时钟日历寄存器起始地址再读出时钟日历写入到TimeData数组中
}*/

//键盘扫描函数
void key_scan()
{
	//带static的局部变量
	static uint  uiKeyTimeCnt1=0;  
	static uchar ucKeyLock1=0;   
	static uint  uiKeyTimeCnt2=0; 
	static uchar ucKeyLock2=0;
	static uchar uiKeyTimeCnt3=0;
	static uchar ucKeyLock3=0;
	static uchar uiKeyTimeCnt4=0;
	static uchar ucKeyLock4=0;


  	if(memu == 1)//IO是高电平，说明按键没有被按下，这时要及时清零一些标志位
  	{
    	ucKeyLock1 = 0; //按键自锁标志清零
        uiKeyTimeCnt1 = 0;//按键去抖动延时计数器清零，此行非常巧妙，是我实战中摸索出来的。      
  	}
  	else if(ucKeyLock1 == 0)//有按键按下，且是第一次被按下
  	{
   		uiKeyTimeCnt1 ++; //累加定时中断次数
     	if(uiKeyTimeCnt1 > const_key_time)
     	{
	        uiKeyTimeCnt1 = 0; 
	        ucKeyLock1 = 1;  //自锁按键置位,避免一直触发
	        ucKeySec = 1;    //触发1号键
	    }
  	}

  	if(ret == 1)
  	{
     	ucKeyLock2 = 0; 
        uiKeyTimeCnt2 = 0;
  	}
  	else if(ucKeyLock2 == 0)
  	{
     	uiKeyTimeCnt2 ++; 
     	if(uiKeyTimeCnt2 > const_key_time)
     	{
	        uiKeyTimeCnt2 = 0;
	        ucKeyLock2 = 1; 
	        ucKeySec = 2;   
     	}
  	}

  	if(left == 1)//IO是高电平，说明按键没有被按下，这时要及时清零一些标志位
  	{
    	ucKeyLock3 = 0; //按键自锁标志清零
        uiKeyTimeCnt3 = 0;//按键去抖动延时计数器清零，此行非常巧妙，是我实战中摸索出来的。      
  	}
  	else if(ucKeyLock3 == 0)//有按键按下，且是第一次被按下
  	{
   		uiKeyTimeCnt3 ++; //累加定时中断次数
     	if(uiKeyTimeCnt3 > const_key_time)
     	{
	        uiKeyTimeCnt3 = 0; 
	        ucKeyLock3 = 1;  //自锁按键置位,避免一直触发
	        ucKeySec = 3;    //触发3号键
	    }
  	} 	

   	if(right == 1)//IO是高电平，说明按键没有被按下，这时要及时清零一些标志位
  	{
    	ucKeyLock4 = 0; //按键自锁标志清零
        uiKeyTimeCnt4 = 0;//按键去抖动延时计数器清零，此行非常巧妙，是我实战中摸索出来的。      
  	}
  	else if(ucKeyLock4 == 0)//有按键按下，且是第一次被按下
  	{
   		uiKeyTimeCnt4 ++; //累加定时中断次数
     	if(uiKeyTimeCnt4 > const_key_time)
     	{
	        uiKeyTimeCnt4 = 0; 
	        ucKeyLock4 = 1;  //自锁按键置位,避免一直触发
	        ucKeySec = 4;    //触发4号键
	    }
  	}
}

void key_service()
{
	switch(ucKeySec) 
  	{
  		//当memu键被按下
	    case 1:
	    	  switch(WindowSec)
	    	  {
	    	  		//在主菜单窗口下
	    	  		case 1:
	    	  			   WindowSec = 2;  		//进入菜单窗口
	    	  			   wd_update = 1;  		//切换窗口需要整屏更新
	    	  			   part_2_2_update = 1; //菜单窗口第二行更新
	    	  			   break;
	    	  		//在菜单窗口下
	    	  		case 2:
						   if(identity_admin && submemu_num < 4)
						   {
								WindowSec = 5;  		//弹出对话框
								wd_update = 1;
						   }
						   else
						   {
								WindowSec = 3;
								wd_update = 1;
								part_3_2_update = 1;
						   }
						   if(submemu_num == 4)
						   {
								WindowSec = 5;
  			   				 	wd_update = 1;
  			   				 	identity_user = 1;
						   }
						   break;
	    	  		//在子菜单窗口下
	    	  		case 3:
	    	  			   switch(submemu_num)
	    	  			   {
	    	  			   		case 1:
	    	  			   			   WindowSec = 5;  //进入提示窗口
  			   			  			   wd_update = 1;
  			   			   			   add_user = 1;
  			   			   			   break;
  			   			   	 	case 2:
  			   			   	 		   WindowSec = 5;
  			   			   	 		   wd_update = 1;
  			   			   	 		   delte_model = 1;
  			   			   	 		   break;
  			   					case 3:
  			   						   WindowSec = 5;
  			   						   wd_update = 1;
  			   						   delte_allmodel = 1;
  			   						   break;
  			   				 	case 4:
  			   				 		   WindowSec = 5;
  			   				 		   wd_update = 1;
  			   				 		   identity_user = 1;
  			   				 		   break;
	    	  			   }
  		       			   break;
  		       		//在信息提示窗口下
	    	  	  	case 4:
	    	  	    	   WindowSec = 1;  //返回主界面窗口
	    	  	    	   wd_update = 1;  
	    	  	    	   break;
					case 5:
						   WindowSec = 2;
						   wd_update = 1;
						   part_2_2_update = 1;
						   break;
	    	  }	
	          uiVoiceCnt=const_voice_short; 
	          ucKeySec=0; 
	          break;
	    //当ret键被按下        
	    case 2:
	    	  switch(WindowSec)
	    	  {
    	  		//在主窗口下
    	  		case 1:
    	  			   WindowSec = 4;  //进入信息窗口
    	  			   wd_update = 1;
    	  			   part_4_2_update = 1;
    	  			   break;
    	  		//在菜单窗口下
    	  		case 2:
    	  		       WindowSec = 1;  //返回主菜单窗口
    	  		       wd_update = 1;
    	  		       submemu_num = 1;
    	  		       break;
    	  		//在子菜单窗口下
    	  		case 3:
    	  			   WindowSec = 2;  //返回菜单窗口
    	  			   wd_update = 1;
    	  			   user_num = 1;
    	  			   identity_admin = 1;
    	  			   identity_user = 0;
    	  			   part_2_2_update = 1;
    	  			   break;
					case 5:
					   	WindowSec = 2;
							wd_update = 1;
							user_num = 1;
				     	identity_user = 0;
				     	part_2_2_update = 1;
						break;
	    	  }		    
	          uiVoiceCnt=const_voice_short;
	          ucKeySec=0;
	          break;
	    //当left键被按下时
	    case 3:
	    	  switch(WindowSec)
	    	  {
	    	  		case 1:add_admin = 1;
	    	  			   break;
	    	  		//在菜单窗口下
	    	  		case 2:
    	  			   	  submemu_num --;       //菜单编号自减
	  			   		  if(submemu_num == 0)  //菜单编号减到0时自动跳回最后一个菜单
	  			   		  {
	  			   		  		submemu_num = 5;
	  			   		  }
	  			   		  part_2_2_update = 1;  //菜单窗口第二行更新
	    	  			  break;
	    	  		//在子菜单窗口下
	    	  		case 3:
	    	  			   switch(submemu_num)
	    	  			   {
		    	  			   case 1:
		    	  			   case 2:
									  user_num --;
		    	  			   		  if(user_num == 0)
		    	  			   		  {
		    	  			   		  		user_num = 300;
		    	  			   		  }
		    	  			   		  part_3_2_update = 1;
		    	  			   		  break;
		    	  			   case 5:
		    	  			   		  user_num -= 2;
		    	  			   		  if(user_num == -1)
		    	  			   		  {
		    	  			   		  		user_num = 299;
		    	  			   		  }
		    	  			   		  part_3_2_update = 1;
		    	  			   		  break;	    	  			   	
	    	  			   }
	    	  			   break;
	    	  }
	          uiVoiceCnt=const_voice_short;
	          ucKeySec=0;
	    	  break;
	   	//当right键被按下时
	   	case 4:
	    	  switch(WindowSec)
	    	  {
	    	  		//在菜单窗口下
	    	  		case 2:
							submemu_num ++;
							if(submemu_num == 6)
							{
								submemu_num = 1;
							}
							part_2_2_update = 1;
							break;
	    	  	    case 3:
	    	  	    	    switch(submemu_num)
	    	  			   {
		    	  			   case 1:
		    	  			   case 2:
									  user_num ++;
				  			   		  if (user_num == 301)
				  			   		  {
				  			   		  		user_num = 1;
				  			   		  }
				  			   		  part_3_2_update = 1;
				  			   		  break;  
		    	  			   case 5:
				  			   		  user_num += 2;
				  			   		  if (user_num == 301)
				  			   		  {
				  			   		  		user_num = 1;
				  			   		  }
				  			   		  part_3_2_update = 1;
				  			   		  break;    	  			   	
	    	  			   }
	    	  			   break;
            	} 
		        uiVoiceCnt=const_voice_short;
				ucKeySec=0;
				break;
		}
}

void time_update()
{
	t ++;
	if(t==120*500)
	{
		Read_DS1302_Time(DS1302_R_ADDR); //先写入时钟日历寄存器起始地址再读出时钟日历写入到TimeData数组中
		part_1_2_update = 1;
		if(TimeData[0]==00 && TimeData[2] == 0)
		{
			part_1_3_update = 1;
		}
	}
	
}
void T0_time() interrupt 1
{
  TF0 = 0; 
  TR0 = 0;

  key_scan(); 

  if(uiVoiceCnt!=0)
  {
     uiVoiceCnt--; 
     beep = 0;  
  }
  else
  {
     beep = 1; 
  }
	if(uiWrongCnt != 0)
	{
		uiWrongCnt --;
		red = 0;
	}
	else
	{
		red = 1;
	}
	if(uiPassCnt != 0)
	{
		uiPassCnt --;
		green = 0;
	}
	else
	{
		green = 1;
	}
  TH0 = 0xf8;   
  TL0 = 0x2f;
  TR0 = 1; 
}


void main_screen()
{
	if(wd_update)
	{
		wd_update = 0;
		OLED_Clear(); 					//清屏
		//第一行的显示内容
		OLED_ShowChinese(18,0,8);//指
		OLED_ShowChinese(36,0,9);//纹
		OLED_ShowChinese(54,0,10);//考
		OLED_ShowChinese(72,0,11);//勤
		OLED_ShowChinese(90,0,12);//器

		//第二行的显示内容:显示时间
		OLED_ShowNum(25,2,TimeData[2]/10,1,16);
		OLED_ShowNum(33,2,TimeData[2]%10,1,16);
		OLED_ShowChar(40,2,':');
		OLED_ShowNum(49,2,TimeData[1]/10,1,16);
		OLED_ShowNum(57,2,TimeData[1]%10,1,16);
		OLED_ShowChar(64,2,':');
		OLED_ShowNum(73,2,TimeData[0]/10,1,16);
		OLED_ShowNum(81,2,TimeData[0]%10,1,16);

		//第三行的显示内容：显示日期和星期
		OLED_ShowNum(10,4,TimeData[6]/10,1,8);
		OLED_ShowNum(19,4,TimeData[6]%10,1,8);
		OLED_ShowChar(28,4,'-');
		OLED_ShowNum(38,4,TimeData[4]/10,1,12);
		OLED_ShowNum(47,4,TimeData[4]%10,1,12);
		OLED_ShowChar(54,4,'-');
		OLED_ShowNum(64,4,TimeData[3]/10,1,12);
		OLED_ShowNum(73,4,TimeData[3]%10,1,12);
		OLED_ShowChinese(90,4,0);			//周
		OLED_ShowChinese(108,4,TimeData[5]);//星期
		
		//第四行的显示内容：显示选项
		OLED_ShowChinese(0,6,13);	//菜
		OLED_ShowChinese(18,6,14);	//单
		OLED_ShowChinese(90,6,15);	//信
		OLED_ShowChinese(108,6,16);	//息	
	}
	if(part_1_2_update)
	{
		part_1_2_update = 0;
		//第二行的显示内容:显示时间
		OLED_ShowNum(25,2,TimeData[2]/10,1,16);
		OLED_ShowNum(33,2,TimeData[2]%10,1,16);
		OLED_ShowChar(40,2,':');
		OLED_ShowNum(49,2,TimeData[1]/10,1,16);
		OLED_ShowNum(57,2,TimeData[1]%10,1,16);
		OLED_ShowChar(64,2,':');
		OLED_ShowNum(73,2,TimeData[0]/10,1,16);
		OLED_ShowNum(81,2,TimeData[0]%10,1,16);
	}
	if(part_1_3_update)
	{
		part_1_3_update = 0;
		//第三行的显示内容：显示日期和星期
		OLED_ShowNum(10,4,TimeData[6]/10,1,8);
		OLED_ShowNum(19,4,TimeData[6]%10,1,8);
		OLED_ShowChar(28,4,'-');
		OLED_ShowNum(38,4,TimeData[4]/10,1,12);
		OLED_ShowNum(47,4,TimeData[4]%10,1,12);
		OLED_ShowChar(54,4,'-');
		OLED_ShowNum(64,4,TimeData[3]/10,1,12);
		OLED_ShowNum(73,4,TimeData[3]%10,1,12);
		OLED_ShowChinese(90,4,0);			//周
		OLED_ShowChinese(108,4,TimeData[5]);//星期	
	}
}

void memu_screen()
{
	if(wd_update)
	{
		wd_update = 0;
		OLED_Clear();
		//添(17) 加(18) 清(19) 空(20) 开(21) 始(22) 打(23) 卡(24) 查(25) 看(26) 确(27) 定(28) 返(29) 回(30) 管理员 （31，32，33）
		//line one
		OLED_ShowChinese(36,0,13);	//菜
		OLED_ShowChinese(72,0,14);	//单


		//line three
		OLED_ShowChinese(0,6,27);	//确
		OLED_ShowChinese(18,6,28);	//定
		OLED_ShowChar(50,6,'<');
		OLED_ShowChar(68,6,'>');
		OLED_ShowChinese(90,6,29);	//返
		OLED_ShowChinese(108,6,30);	//回

	}
	if(part_2_2_update)
	{
		part_2_2_update = 0;

		OLED_ShowNum(8,3,submemu_num,1,8);   //显示菜单数
		OLED_ShowChar(16,3,'.');

		switch(submemu_num)
		{
			case 1:	OLED_ShowChinese(25,3,17);  //添
					OLED_ShowChinese(43,3,18);  //加
					OLED_ShowChinese(61,3,8);   //指
					OLED_ShowChinese(79,3,9);   //纹
					break;
			case 2:	OLED_ShowChinese(25,3,47);  //删
					OLED_ShowChinese(43,3,48);  //除
					OLED_ShowChinese(61,3,8);   //指
					OLED_ShowChinese(79,3,9);   //纹
					break;
			case 3:
					OLED_ShowChinese(25,3,19);  //清
					OLED_ShowChinese(43,3,20);  //空
					OLED_ShowChinese(61,3,8);   //指
					OLED_ShowChinese(79,3,9);   //纹
					break;
			case 4:
					OLED_ShowChinese(25,3,21);  //开
					OLED_ShowChinese(43,3,22);  //始
					OLED_ShowChinese(61,3,23);  //打
					OLED_ShowChinese(79,3,24);  //卡
					break;
		    case 5:
					OLED_ShowChinese(25,3,25);  //查
					OLED_ShowChinese(43,3,26);  //看
					OLED_ShowChinese(61,3,10);  //考
					OLED_ShowChinese(79,3,11);  //勤
					break;
		}
	}	
}

void ifo_screen()
{
	if(wd_update)
	{
		wd_update = 0;
		OLED_Clear();
		//广(34) 东(35) 工(36) 业(37) 大(38) 学(39)
		//line one
		OLED_ShowChinese(9,0,34);//广
		OLED_ShowChinese(27,0,35);//东
		OLED_ShowChinese(45,0,36);//工
		OLED_ShowChinese(63,0,37);//业
		OLED_ShowChinese(81,0,38);//大
		OLED_ShowChinese(99,0,39);//学

		//吴(40) 荣(41) 杨(42) 森(43) 林(44) 区(45) 晓(46)
		//line two
		OLED_ShowChinese(18,3,40);//吴
		OLED_ShowChinese(54,3,41);//荣
		OLED_ShowChinese(90,3,35);//东

		delay(10000);
		OLED_ShowChinese(18,3,42);//杨
		OLED_ShowChinese(54,3,43);//森
		OLED_ShowChinese(90,3,44);//林

		delay(10000);
		OLED_ShowChinese(18,3,45);//区
		OLED_ShowChinese(54,3,46);//晓
		OLED_ShowChinese(90,3,35);//东
		delay(10000);
		OLED_ShowChinese(18,3,76);//制
		OLED_ShowChinese(54,3,79);//space
		OLED_ShowChinese(90,3,77);//作
		delay(10000);
		OLED_ShowChinese(18,3,8);//指
		OLED_ShowChinese(36,3,9);//纹
		OLED_ShowChinese(54,3,74);//数
		OLED_ShowChinese(72,3,75);//量
		OLED_ShowChinese(90,3,79);//space
		UART2_init_57600();
		//OLED_ShowNum(90,3,FP_Get_Templete_Num(),3,16);
		//line three
		OLED_ShowChinese(0,6,27);	//确
		OLED_ShowChinese(18,6,28);	//定		
	}

}

//子菜单界面
void sub_screen()
{
	if(wd_update)
	{
		wd_update = 0;
		OLED_Clear();
		switch(submemu_num)
		{
			case 1:
					//line one 菜单名
					OLED_ShowChinese(27,0,17);//添
					OLED_ShowChinese(45,0,18);//加
					OLED_ShowChinese(63,0,8);//指
					OLED_ShowChinese(81,0,9);//纹
					break;
			case 2:
					OLED_ShowChinese(27,0,47);//删
					OLED_ShowChinese(45,0,48);//除
					OLED_ShowChinese(63,0,8);//指
					OLED_ShowChinese(81,0,9);//纹
					break;
			case 3:
					OLED_ShowChinese(25,0,19);  //清
					OLED_ShowChinese(43,0,20);  //空
					OLED_ShowChinese(63,0,8);   //指
					OLED_ShowChinese(79,0,9);   //纹
					break;					
			case 4:
					OLED_ShowChinese(27,0,21);//开
					OLED_ShowChinese(45,0,22);//始
					OLED_ShowChinese(63,0,23);//打
					OLED_ShowChinese(81,0,24);//卡
					break;
			case 5:
					OLED_ShowChinese(27,0,25);//查
					OLED_ShowChinese(45,0,26);//看
					OLED_ShowChinese(63,0,10);//考
					OLED_ShowChinese(81,0,11);//勤	
					break;				
		}
	}

	//line two 信息查看
	if(part_3_2_update)
	{
		part_3_2_update = 0;

		switch(submemu_num)
		{
			//在添加指纹子菜单界面下
			case 1:
					OLED_ShowNum(27,3,user_num/100,1,8);
					OLED_ShowNum(36,3,(user_num/10)%10,1,8);
					OLED_ShowNum(45,3,user_num%100,1,8);
					if(user_Ifo[user_num].isVaild)
						OLED_ShowChinese(63,3,73);   //已
					else
						OLED_ShowChinese(63,3,78);   //未
					OLED_ShowChinese(81,3,60);   //录
					OLED_ShowChinese(99,3,61);   //入

					OLED_ShowChinese(0,6,27);	//确
					OLED_ShowChinese(18,6,28);	//定
					OLED_ShowChar(50,6,'<');
					OLED_ShowChar(68,6,'>');
					OLED_ShowChinese(90,6,29);	//取
					OLED_ShowChinese(108,6,30);	//消
					break;
			//在删除指纹子菜单界面下
			case 2:
					OLED_ShowNum(27,3,user_num/100,1,8);
					OLED_ShowNum(36,3,(user_num/10)%10,1,8);
					OLED_ShowNum(45,3,user_num%100,1,8);
					//if(user_Ifo[user_num].isVaild)
						//OLED_ShowChinese(63,3,73);   //已
					//else
						OLED_ShowChinese(63,3,78);   //未
					OLED_ShowChinese(81,3,60);   //录
					OLED_ShowChinese(99,3,61);   //入

					OLED_ShowChinese(0,6,27);	//确
					OLED_ShowChinese(18,6,28);	//定
					OLED_ShowChar(50,6,'<');
					OLED_ShowChar(68,6,'>');
					OLED_ShowChinese(90,6,29);	//返
					OLED_ShowChinese(108,6,30);	//回
					break;
			//在清空指纹界面下
			case 3:
					OLED_ShowChinese(25,3,27);  //确
					OLED_ShowChinese(43,3,50);  //认
					OLED_ShowChinese(61,3,19);  //清
					OLED_ShowChinese(79,3,20);  //空

					OLED_ShowChinese(0,6,27);	//确
					OLED_ShowChinese(18,6,28);	//定
					OLED_ShowChinese(90,6,66);	//取
					OLED_ShowChinese(108,6,67);	//消
					break;

			//在开始打卡子菜单界面下
			case 4:
					OLED_ShowChinese(90,6,68);	//退
					OLED_ShowChinese(108,6,69);	//出

					//显示用户编号
					OLED_ShowNum(45,2,user_num/100,1,8);
					OLED_ShowNum(54,2,(user_num/10)%10,1,8);
					OLED_ShowNum(63,2,user_num%100,1,8);
					//显示打卡时间
					OLED_ShowNum(25,4,TimeData[2]/10,1,16);
					OLED_ShowNum(33,4,TimeData[2]%10,1,16);
					OLED_ShowChar(40,4,':');
					OLED_ShowNum(49,4,TimeData[1]/10,1,16);
					OLED_ShowNum(57,4,TimeData[1]%10,1,16);
					OLED_ShowChar(64,4,':');
					OLED_ShowNum(73,4,TimeData[0]/10,1,16);
					OLED_ShowNum(81,4,TimeData[0]%10,1,16);
					delay(15000);
					WindowSec = 5;
					wd_update = 1;
					break;
			//在查看考勤子菜单界面下
			case 5:
					OLED_ShowNum(0,2,(user_num)/100,1,8);
					OLED_ShowNum(9,2,((user_num)/10)%10,1,8);
					OLED_ShowNum(18,2,(user_num)%100,1,8);
					
					OLED_ShowNum(36,2,TimeData[2]/10,1,16);
					OLED_ShowNum(45,2,TimeData[2]%10,1,16);
					OLED_ShowChar(54,2,':');
					OLED_ShowNum(63,2,TimeData[1]/10,1,16);
					OLED_ShowNum(72,2,TimeData[1]%10,1,16);
					OLED_ShowChar(81,2,':');
					OLED_ShowNum(90,2,TimeData[0]/10,1,16);
					OLED_ShowNum(99,2,TimeData[0]%10,1,16);
			
					//显示打卡时间
					//OLED_ShowNum(36,2,user_Ifo[user_num].hour/10,1,16);
					//OLED_ShowNum(45,2,user_Ifo[user_num].hour%10,1,16);
					//OLED_ShowChar(54,2,':');
					//OLED_ShowNum(63,2,user_Ifo[user_num].min/10,1,16);
					//OLED_ShowNum(72,2,user_Ifo[user_num].min%10,1,16);
					//OLED_ShowChar(81,2,':');
					//OLED_ShowNum(90,2,user_Ifo[user_num].sec/10,1,16);
					//LED_ShowNum(99,2,user_Ifo[user_num].sec%10,1,16);

					OLED_ShowNum(0,4,(user_num+1)/100,1,8);
					OLED_ShowNum(9,4,((user_num+1)/10)%10,1,8);
					OLED_ShowNum(18,4,(user_num+1)%100,1,8);

					//显示打卡时间
					OLED_ShowNum(36,4,TimeData[2]/10,1,16);
					OLED_ShowNum(45,4,TimeData[2]%10,1,16);
					OLED_ShowChar(54,4,':');
					OLED_ShowNum(63,4,TimeData[1]/10,1,16);
					OLED_ShowNum(72,4,TimeData[1]%10,1,16);
					OLED_ShowChar(81,4,':');
					OLED_ShowNum(90,4,TimeData[0]/10,1,16);
					OLED_ShowNum(99,4,TimeData[0]%10,1,16);

					//显示打卡时间
					//OLED_ShowNum(36,4,user_Ifo[user_num+1].hour/10,1,16);
					//OLED_ShowNum(45,4,user_Ifo[user_num+1].hour%10,1,16);
					//OLED_ShowChar(54,4,':');
					//OLED_ShowNum(63,4,user_Ifo[user_num+1].min/10,1,16);
					//OLED_ShowNum(72,4,user_Ifo[user_num+1].min%10,1,16);
					//OLED_ShowChar(81,4,':');
					////OLED_ShowNum(90,4,user_Ifo[user_num+1].sec/10,1,16);
					//OLED_ShowNum(99,4,user_Ifo[user_num+1].sec%10,1,16);
					
					OLED_ShowChinese(0,6,64);	//上
					OLED_ShowChinese(18,6,65);	//传
					OLED_ShowChar(50,6,'<');
					OLED_ShowChar(68,6,'>');
					OLED_ShowChinese(90,6,68);	//退
					OLED_ShowChinese(108,6,69);	//出

					break;
		}
	}

}
void dialog_screen()
{
	if(wd_update)
	{
		wd_update = 0;
		OLED_Clear();
		switch(submemu_num)
		{
			case 1:
			case 2:
			case 3:
					if(add_admin)
					{
						OLED_ShowChinese(9,0,49); //请
						OLED_ShowChinese(27,0,17);//添
						OLED_ShowChinese(45,0,18);//加
						OLED_ShowChinese(63,0,31);//管
						OLED_ShowChinese(81,0,32);//理
						OLED_ShowChinese(99,0,33);//员

						UART2_init_57600();
						//FP_add_new_user(0);
						delay(10000);
						//Set_User_Ifo();
						OLED_ShowChinese(25,3,60);  //录
						OLED_ShowChinese(43,3,61);  //入
						OLED_ShowChinese(61,3,63);  //成
						OLED_ShowChinese(79,3,72);  //功
						uiPassCnt = const_led_long;
						
						OLED_ShowChinese(0,6,27);	//确
						OLED_ShowChinese(18,6,28);	//定
						OLED_ShowChinese(90,6,29);	//返
						OLED_ShowChinese(108,6,30);	//回
						add_admin = 0;
						break;
					}
					if(identity_admin)
					{
						OLED_ShowChinese(9,0,49); //请
						OLED_ShowChinese(27,0,31);//管
						OLED_ShowChinese(45,0,32);//理
						OLED_ShowChinese(63,0,33);//员
						OLED_ShowChinese(81,0,27);//确
						OLED_ShowChinese(99,0,50);//认

						OLED_ShowChar(43,3,'.');  
						OLED_ShowChar(52,3,'.');   
						OLED_ShowChar(61,3,'.');
						UART2_init_57600();
						delay(10000);
						if(1)//FP_Admin_Identity())
						{
							OLED_ShowChinese(43,3,63);  //成
							OLED_ShowChinese(61,3,72);  //功
							uiPassCnt = const_led_long;
							WindowSec = 3;
							wd_update = 1;
							part_3_2_update = 1;
							identity_admin = 0;
						}
						else
						{
							OLED_ShowChinese(43,3,70);  //失
							OLED_ShowChinese(61,3,71);  //败
							uiWrongCnt = const_led_long;
							uiVoiceCnt = const_voice_short;
							while(uiVoiceCnt == 0 && uiWrongCnt == 0);
							WindowSec = 2;
							wd_update = 2;
							part_2_2_update = 1;
						}

						break;
					}
					if(add_user)
					{
						OLED_ShowChinese(9,0,49); //请
						OLED_ShowChinese(27,0,51);//触
						OLED_ShowChinese(45,0,52);//摸
						OLED_ShowChinese(63,0,53);//传
						OLED_ShowChinese(81,0,54);//感
						OLED_ShowChinese(99,0,55);//器

						UART2_init_57600();
						delay(10000);
						//FP_add_new_user(user_num);

						//Set_User_Ifo();
						OLED_ShowChinese(25,3,60);  //录
						OLED_ShowChinese(43,3,61);  //入
						OLED_ShowChinese(61,3,63);  //成
						OLED_ShowChinese(79,3,72);  //功
						
						OLED_ShowChinese(0,6,27);	//确
						OLED_ShowChinese(18,6,28);	//定
						OLED_ShowChinese(90,6,29);	//返
						OLED_ShowChinese(108,6,30);	//回
						WindowSec = 3;
						wd_update = 1;
						part_3_2_update = 1;
						add_user = 0;
						break;
					}
					if(delte_model)
					{
						UART2_init_57600();
						FP_Cmd_Delete_Model(user_num);
						Set_User_Ifo();
						OLED_ShowChinese(45,0,73);//已
						OLED_ShowChinese(63,0,47);//删
						OLED_ShowChinese(81,0,48);//除
						OLED_ShowNum(50,3,user_num/100,1,8);
						OLED_ShowNum(59,3,(user_num/10)%10,1,8);
						OLED_ShowNum(68,3,user_num%100,1,8);
						delay(10000);
						WindowSec = 3;
						wd_update = 1;
						part_3_2_update = 1;
						delte_model = 0;
						break;
					}
					if(delte_allmodel)
					{
						OLED_ShowChinese(25,0,19);  //清
						OLED_ShowChinese(43,0,20);  //空
						OLED_ShowChinese(61,0,8);   //指
						OLED_ShowChinese(79,0,9);   //纹

						UART2_init_57600();
						//FP_Cmd_Delete_All_Model();
						//Set_User_Ifo();
						delay(10000);
						OLED_ShowChinese(43,3,73);  //已
						OLED_ShowChinese(61,3,19);  //清
						OLED_ShowChinese(79,3,20);  //空
						
						OLED_ShowChinese(0,6,27);	//确
						OLED_ShowChinese(18,6,28);	//定
						OLED_ShowChinese(90,6,29);	//返
						OLED_ShowChinese(108,6,30);	//回
						add_admin = 1;
						delte_allmodel = 0;
						break;
					}
			case 4:		
					if(identity_user)
					{
						int cnt = 0;
						unsigned int num = 0;
						OLED_ShowChinese(9,3,49); //请
						OLED_ShowChinese(27,3,51);//触
						OLED_ShowChinese(45,3,52);//摸
						OLED_ShowChinese(63,3,53);//传
						OLED_ShowChinese(81,3,54);//感
						OLED_ShowChinese(99,3,55);//器
						OLED_ShowChinese(90,6,68);	//退
						OLED_ShowChinese(108,6,69);	//出
						do{
							time_update();
							if(touch == 0)
							{
								UART2_init_57600();
								num = FP_User_Identity();
							}
							key_scan();
							if(ucKeySec == 2)
							{
								ucKeySec = 0;
								num = 0;
								WindowSec = 1;
								wd_update = 1;
								break;
							}
						}while(num == 0 || num ==-1);
						if(num != 0)
						{
							user_num = num;
							user_Ifo[user_num].sec = TimeData[0];
							user_Ifo[user_num].min = TimeData[1];
							user_Ifo[user_num].hour = TimeData[2];
							uiPassCnt = const_led_long;
							WindowSec = 3;
							wd_update = 1;
							part_3_2_update = 1;
						}
					}
		}
	}
}
void OLED_Display()
{
	switch(WindowSec)
	{
		case 1:main_screen();break;
		case 2:memu_screen();break;
		case 3:sub_screen();break;
		case 4:ifo_screen();break;
		case 5:dialog_screen();break;
	}
}