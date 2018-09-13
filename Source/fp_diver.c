#include "uart.h"  			//单片机串口2头文件
#include "fp_diver.h"

/*------------------ fp_diver函数定义 --------------------------*/
/*************************一级函数******************************/
//FINGERPRINT_获得指纹图像命令
void FP_Cmd_Get_Img(void)
{
    unsigned char i;

    for(i=0;i<6;i++) //发送包头
       UART2_Send_Byte(FP_Pack_Head[i]);
    
    for(i=0;i<6;i++) //发送命令 0x1d
       UART2_Send_Byte(FP_Get_Img[i]);
}

//讲图像转换成特征码存放在Buffer1中
void FP_Cmd_Img_To_Buffer1(void)
{
 	unsigned char i;
 	
   	for(i=0;i<6;i++)    //发送包头
       	UART2_Send_Byte(FP_Pack_Head[i]);   
   
	for(i=0;i<7;i++)   //发送命令 将图像转换成 特征码 存放在 CHAR_buffer1
		UART2_Send_Byte(FP_Img_To_Buffer1[i]);
}

//将图像转换成特征码存放在Buffer2中
void FP_Cmd_Img_To_Buffer2(void)
{
    unsigned char i;
   	for(i=0;i<6;i++)    //发送包头
    {
       UART2_Send_Byte(FP_Pack_Head[i]);   
	}
	for(i=0;i<7;i++)   //发送命令 将图像转换成 特征码 存放在 CHAR_buffer1
    {
		UART2_Send_Byte(FP_Img_To_Buffer2[i]);
	}
}

//将BUFFER1 跟 BUFFER2 中的特征码合并成指纹模版
void FP_Cmd_Reg_Model(void)
{
    unsigned char i;    

    for(i=0;i<6;i++) //包头
    {
      UART2_Send_Byte(FP_Pack_Head[i]);   
    }

    for(i=0;i<6;i++) //命令合并指纹模版
    {
      UART2_Send_Byte(FP_Reg_Model[i]);   
    }

}

//删除指纹模块里的所有指纹模版
void FP_Cmd_Delete_All_Model(void)
{
    unsigned char i;    

    for(i=0;i<6;i++) //包头
      UART2_Send_Byte(FP_Pack_Head[i]);   

    for(i=0;i<6;i++) //命令合并指纹模版
      UART2_Send_Byte(FP_Delet_All_Model[i]);

    FP_Recevice_Data(12);   
}

/**************************二级函数*****************************/
//删除指纹模块里的指定指纹模版
void FP_Cmd_Delete_Model(unsigned int uiID_temp)
{
    volatile unsigned int uiSum_temp = 0;
	unsigned char i;    
	//把16位的int变量转化位2个8位的char并存入指令包相应的位置
	FP_Delete_Model[4]=(uiID_temp&0xFF00)>>8;
	FP_Delete_Model[5]=(uiID_temp&0x00FF);
	for(i=0;i<8;i++)					//计算校验和
	    uiSum_temp = uiSum_temp + FP_Delete_Model[i];
	//将校验和存入指令包相应的位置
	FP_Delete_Model[8]=(uiSum_temp&0xFF00)>>8;
	FP_Delete_Model[9]=uiSum_temp&0x00FF;
    for(i=0;i<6;i++) 					//发送包头
      UART2_Send_Byte(FP_Pack_Head[i]);   
    for(i=0;i<10;i++) 					//发送命令删除指定指纹模版
      UART2_Send_Byte(FP_Delete_Model[i]);   
  	FP_Recevice_Data(12);				//接收12个长度的反馈码
}
//获得指纹模板数量
unsigned int FP_Get_Templete_Num(void)
{  
	unsigned int i,num;
  	unsigned char numH;
  	unsigned char numL;

   	for(i=0;i<6;i++) 					//包头
      	UART2_Send_Byte(FP_Pack_Head[i]);		
   	for(i=0;i<6;i++)					//发送命令 0x1d
		UART2_Send_Byte(FP_Templete_Num[i]);
	FP_Recevice_Data(12);
	numH = UART2_FP_RECEVICE_BUFFER[10];
	numL = UART2_FP_RECEVICE_BUFFER[11];

	num = numH *100000000  + numL;
	return num;
}
//读索引表
unsigned char* FP_Get_ReadIndexTable(unsigned char PageNum)
{
	unsigned char i;
	unsigned char* pTable;
	pTable = Table;
	for(i=0;i<6;i++)
		UART2_Send_Byte(FP_Pack_Head[i]);
	if(PageNum == 0)
	{
		for(i=0;i<7;i++)
			UART2_Send_Byte(FP_Read_0_Table[i]);
	}
	if(PageNum == 1)
	{
		for(i=0;i<7;i++)
			UART2_Send_Byte(FP_Read_1_Table[i]);
	}
	for(i=0;i<10;i++)
		UART2_FP_RECEVICE_BUFFER[i] = UART2_Receive_Byte();
	for(i=0;i<32;i++)
		Table[i] = UART2_Receive_Byte();
	UART2_FP_RECEVICE_BUFFER[10] = UART2_Receive_Byte();
	UART2_FP_RECEVICE_BUFFER[11] = UART2_Receive_Byte();

	return pTable;
}

//搜索全部用户300枚
void FP_Cmd_Search_Finger(void)
{
	unsigned char i;	   
	for(i=0;i<6;i++)   				//发送命令搜索用户指纹库
   	{
      	UART2_Send_Byte(FP_Pack_Head[i]);   
	}

	for(i=0;i<11;i++)
	{
	    UART2_Send_Byte(FP_Search[i]);   
	}
}

//载入管理员指纹模板
void FP_Cmd_Load_Finger_Admin(void)
{
   unsigned char i;	   
   for(i=0;i<6;i++)   				//发送命令载入管理员指纹库到charBufftter2
   	{
      UART2_Send_Byte(FP_Pack_Head[i]);   
	}

   for(i=0;i<9;i++)
   	{
      UART2_Send_Byte(FP_Load_Admin[i]);   
	}
}

//精确对比缓冲区里的两枚指纹
void FP_cmd_Match_Finger(void)
{
	unsigned char i;
	for(i=0;i<6;i++)
		UART2_Send_Byte(FP_Pack_Head[i]);
	for(i=0;i<6;i++)
		UART2_Send_Byte(FP_Match[i]);
}

//保存指纹模板
void FP_Cmd_Save_Finger( unsigned char ucH_Char,unsigned char ucL_Char )
{
   unsigned long temp = 0;
   unsigned char i;

   FP_Save_Finger[5] = ucH_Char;
   FP_Save_Finger[6] = ucL_Char;
   
   for(i=0;i<7;i++)   //计算校验和
   	   temp = temp + FP_Save_Finger[i];
	    
   FP_Save_Finger[7]=(temp & 0x00FF00) >> 8; 	//存放校验数据
   FP_Save_Finger[8]= temp & 0x0000FF;
   
   for(i=0;i<6;i++)    
       UART2_Send_Byte(FP_Pack_Head[i]);        //发送包头

   for(i=0;i<9;i++)  
		UART2_Send_Byte(FP_Save_Finger[i]);     //发送命令 将图像转换成 特征码 存放在 CHAR_buffer1
}

//接收反馈数据缓冲
void FP_Recevice_Data(unsigned char ucLength)
{
  unsigned char i;
  for (i=0;i<ucLength;i++)
     UART2_FP_RECEVICE_BUFFER[i] = UART2_Receive_Byte();
}

//指纹添加新用户
unsigned char FP_add_new_user(unsigned int user_ID)
{
	unsigned char ucH_user;
	unsigned char ucL_user;
	ucH_user = (char)((user_ID >> 8) & 0xff);
	ucL_user = (char)(user_ID & 0xff);
   	do {	          		     	 
        FP_Cmd_Get_Img(); 						//获得指纹图像
        FP_Recevice_Data(12); 					//接收12个长度的反馈码
    }while ( UART2_FP_RECEVICE_BUFFER[9]!=0x00 ); 	//检测是否成功的按了指纹

  	 FP_Cmd_Img_To_Buffer1(); 					//将图像转换成特征码存放在Buffer1中
     FP_Recevice_Data(12);   					//接收12个长度的反馈码

    do{ 
	     FP_Cmd_Get_Img(); 						//获得指纹图像
	     FP_Recevice_Data(12); 					//接收12个长度的反馈码			 
	}while( UART2_FP_RECEVICE_BUFFER[9]!=0x00 );
    
	 FP_Cmd_Img_To_Buffer2(); 					//将图像转换成特征码存放在Buffer2中
     FP_Recevice_Data(12);   					//接收12个长度的反馈码

	 FP_Cmd_Reg_Model();						//转换成特征码
     FP_Recevice_Data(12); 
	 
	 FP_Cmd_Save_Finger(ucH_user,ucL_user);                		         
     FP_Recevice_Data(12);

     return 0;
}

//用户指纹验证,验证成功，返回指纹对应用户的ID，验证失败，返回0
unsigned int FP_User_Identity()
{
	unsigned int user_ID;
	unsigned char user_IDL;
	unsigned char user_IDH;
	do
	{
		FP_Cmd_Get_Img();						//获得指纹图像
		FP_Recevice_Data(12);					//接收12个长度的反馈码
	}while( UART2_FP_RECEVICE_BUFFER[9]!=0x00 );//检测是否成功按了指纹

	FP_Cmd_Img_To_Buffer1();					//将图像转换成特征码存放在Buffer1中
	FP_Recevice_Data(12);						//接收12个长度的反馈码

	FP_Cmd_Search_Finger();						//搜索整个用户指纹库
	FP_Recevice_Data(12);						//接收12个长度的反馈码

	if( UART2_FP_RECEVICE_BUFFER[9] == 0x00)	//如果找到匹配的指纹
	{
		user_IDH = UART2_FP_RECEVICE_BUFFER[10];//将该指纹所在的编号高8位存入user_IDH中
		user_IDL = UART2_FP_RECEVICE_BUFFER[11];//将该指纹所在的编号低8位存入user_IDL中

		user_ID = user_IDH *100000000 + user_IDL;		
		return user_ID;							//返回16位的user_ID
	}
	else
	{
		return -1;								//如果找不到匹配的指纹，返回-1
	}
}

//管理员指纹验证，验证成功返回1，失败返回0
bit FP_Admin_Identity()
{
	bit match = 0;								//匹配标志位
	do
	{
		FP_Cmd_Get_Img();						//获得指纹图像
		FP_Recevice_Data(12);					//接收12个长度的反馈码
	}while( UART2_FP_RECEVICE_BUFFER[9]!=0x00 );//检测是否成功按了指纹

	FP_Cmd_Img_To_Buffer1();					//将图像转换成特征码放在Buffer1中
	FP_Recevice_Data(12);						//接收12个长度的反馈码

	FP_Cmd_Load_Finger_Admin();					//载入管理员指纹到charbuffter2
	FP_Recevice_Data(12);						//接收12个长度的反馈码
	
	FP_cmd_Match_Finger();
	FP_Recevice_Data(14);

	if( UART2_FP_RECEVICE_BUFFER[9] == 0x00 )	//如果指纹匹配
	{
		match = 1;								//返回1
	}
	
	return match;
}