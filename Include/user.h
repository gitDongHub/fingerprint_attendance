#ifndef __FINGERPRINT__
#define __FINGERPRINT__

typedef struct FP_Ifo
{
	unsigned int ID;
	unsigned char isVaild;
	unsigned char sec;
	unsigned char min;
	unsigned char hour;
}fp;

//volatile unsigned char idata UART1_MCU_RECEVICE_BUFFER[12];
unsigned char bit_num[8]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};

/*------------------ fp_diver函数声明 --------------------------*/
//初始化用户信息
void init_User_Ifo();
//获取用户信息
void Set_User_Ifo();
//删除指纹模块里的所有指纹模版
void FP_Cmd_Delete_All_Model(void);
//删除指纹模块里的指定指纹模版
void FP_Cmd_Delete_Model(unsigned int uiID_temp);
//获得指纹模板数量
unsigned int FP_Get_Templete_Num(void);
//获得索引表
unsigned char* FP_Get_ReadIndexTable(unsigned char PageNum);
//接收反馈数据缓冲
void FP_Recevice_Data(unsigned char ucLength);
//指纹添加新用户
unsigned char FP_add_new_user(unsigned int user_ID);
//用户指纹验证,验证成功，返回指纹对应用户的ID，验证失败，返回0
unsigned int FP_User_Identity();
//管理员指纹验证，验证成功返回1，失败返回0
bit FP_Admin_Identity();

#endif