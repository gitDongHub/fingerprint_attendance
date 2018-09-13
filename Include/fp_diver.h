#ifndef __FINGERPRINT__
#define __FINGERPRINT__

volatile unsigned char idata Table[32];
volatile unsigned char idata UART2_FP_RECEVICE_BUFFER[24];
//FINGERPRINT通信协议定义
const unsigned char code FP_Pack_Head[6] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF};  //协议包头
const unsigned char code FP_Get_Img[6] = {0x01,0x00,0x03,0x01,0x00,0x05};    //获得指纹图像
const unsigned char code FP_Templete_Num[6] ={0x01,0x00,0x03,0x1D,0x00,0x21 }; //获得模版总数
const unsigned char code FP_Read_0_Table[7]={0x01,0x00,0x04,0x1F,0x00,0x00,0x24};    //获得模板第0页的有效模板信息
const unsigned char code FP_Read_1_Table[7]={0x01,0x00,0x04,0x1F,0x01,0x00,0x25};	//获得模板第1页的有效模板信息
const unsigned char code FP_Search[11]={0x01,0x00,0x08,0x04,0x01,0x00,0x00,0x01,0x2C,0x00,0x3B}; //搜索指纹搜索范围0 - 300
const unsigned char code FP_Load_Admin[9]={0x01,0x00,0x06,0x07,0x02,0x00,0x00,0x00,0x10}; //载入管理员指纹到charbuff2,只有一个
const unsigned char code FP_Match[6]={0x01,0x00,0x03,0x03,0x00,0x07};					   //精确对比两枚在缓冲区的指纹
const unsigned char code FP_Img_To_Buffer1[7]={0x01,0x00,0x04,0x02,0x01,0x00,0x08}; //将图像放入到BUFFER1
const unsigned char code FP_Img_To_Buffer2[7]={0x01,0x00,0x04,0x02,0x02,0x00,0x09}; //将图像放入到BUFFER2
const unsigned char code FP_Reg_Model[6]={0x01,0x00,0x03,0x05,0x00,0x09}; //将BUFFER1跟BUFFER2合成特征模版
const unsigned char code FP_Delet_All_Model[6]={0x01,0x0,0x03,0x0d,0x00,0x11};//删除指纹模块里所有的模版
volatile unsigned char idata FP_Save_Finger[9]={0x01,0x00,0x06,0x06,0x01,0x00,0x00,0x00,0x19};//将BUFFER1中的特征码存放到指定的位置
volatile unsigned char idata FP_Delete_Model[10]={0x01,0x00,0x07,0x0C,0x0,0x0,0x0,0x1,0x0,0x0}; //删除指定的模版

//录入图像
void FP_Cmd_Get_Img(void);
//生成特征存于chaarbuffter1
void FP_Cmd_Img_To_Buffer1(void);
//生成特征存于chaarbuffter2
void FP_Cmd_Img_To_Buffer2(void);
//生成模板
void FP_Cmd_Reg_Model(void);
//删除指纹模块里的所有指纹模版
void FP_Cmd_Delete_All_Model(void);
//删除指纹模块里的指定指纹模版
void FP_Cmd_Delete_Model(unsigned int uiID_temp);
//获得指纹模板数量
unsigned int FP_Get_Templete_Num(void);
//获得索引表
unsigned char* FP_Get_ReadIndexTable(unsigned char PageNum);
//搜索全部用户300枚
void FP_Cmd_Search_Finger(void);
//载入管理员指纹;
void FP_Cmd_Load_Finger_Admin(void);
//精确对比缓冲区的两枚的指纹
void FP_cmd_Match_Finger(void);
//保存指纹模板
void FP_Cmd_Save_Finger( unsigned char ucH_Char,unsigned char ucL_Char );
//接收反馈数据缓冲
void FP_Recevice_Data(unsigned char ucLength);
//指纹添加新用户
unsigned char FP_add_new_user(unsigned int user_ID);
//用户指纹验证,验证成功，返回指纹对应用户的ID，验证失败，返回0
unsigned int FP_User_Identity();
//管理员指纹验证，验证成功返回1，失败返回0
bit FP_Admin_Identity();
#endif