#ifndef __DS1302_H
#define __DS1302_H

#define DS1302_W_ADDR 0x80  //写时钟日历寄存器起始地址
#define DS1302_R_ADDR 0x81	//读时钟日历寄存器起始地址
#define uchar unsigned char
#define INT16U unsigned int

//时钟日历暂存数组，秒、分、时、日、月、周、年
extern uchar TimeData[7];

sbit TSCLK = P0^0; //时钟
sbit TIO = P0^1;   //数据
sbit TRST = P0^2;  //使能

void DS1302_W_Byte(uchar dat);
uchar DS1302_R_Byte();
void DS1302_W_DAT(uchar cmd, uchar dat);
uchar DS1302_R_DAT(uchar cmd);
void DS1302_Clear_WP();
void DS1302_Set_WP();
void Set_DS1302_Time(uchar addr);
void Read_DS1302_Time(uchar addr);

#endif