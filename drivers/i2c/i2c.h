/******************** (C) COPYRIGHT 2011 蓝宙电子工作室 ********************
 * 文件名       ：i2c.h
 * 描述         ：I2C头文件
 *
 * 实验平台     ：
 * 库版本       ：
 * 嵌入系统     ：
 *
 * 作者         ：
 * 淘宝店       ：
**********************************************************************************/

#include "i2c_cfg.h"

typedef enum I2Cn
{
    I2C0  = 0,
    I2C1  = 1
} I2Cn;

typedef enum MSmode
{
    MWSR =   0x00,  /* Master write  */
    MRSW =   0x01   /* Master read */
} MSmode;


void  I2C_init(I2Cn);                                         //初始化I2C
void  I2C_WriteAddr(I2Cn, u8 SlaveID, u8 Addr, u8 Data);      //读取地址里的内容
u8    I2C_ReadAddr(I2Cn, u8 SlaveID, u8 Addr);                //往地址里写入内容





void  I2C_StartTransmission (I2Cn, u8 SlaveID, MSmode);       //启动传输