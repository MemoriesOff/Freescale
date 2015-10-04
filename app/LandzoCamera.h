#ifndef _LANDZOCAMERA_H
#define _LANDZOCAMERA_H

#include "gpio.h"

 /**********************
 *摄像头
 *********************/
#define  DATAROW    30//采样行数
#define  DATACOUNT  140        //采样点数
#define  ADLENGT    4000     
 /**********************/
/********************************************************************/

void Camera_init(void);
void Camera_Black1(uint8 *point1,uint8 *rightblackedge,uint8 *leftblackedge,uint8 *centre_black);
void Camera_Black2(uint8 *point1,uint8 *rightblackedge,uint8 *leftblackedge,uint8 *centre_black);
void Camera_Black3(uint8 *point1,uint8 *rightblackedge,uint8 *leftblackedge,uint8 *centre_black);
void Camera_Black4(uint8 *point1,uint8 *rightblackedge,uint8 *leftblackedge,uint8 *centre_black);
void Camera_Black5(uint8 *point1,uint8 *rightblackedge,uint8 *leftblackedge,uint8 *centre_black);
void Camera_Black6(uint8 *point1,uint8 *rightblackedge,uint8 *leftblackedge,uint8 *centre_black);
/********************************************************************/

/*********************
算法子程序 的define
***********************/

#define  FAVMIN      0X30        //AD二值化最小域值
#define  FAVMAX      0X80        //AD二值化最大域值
#define  FAVDEA      0X20        //AD二值化参考域值
#define  LEFTINIT    0X05        //左边初始化位置
#define  RINGTINIT   0X0A        //边右初始化位置
/*************************** 
函数名称：Line_calculate
***********************/

#define  L_WITHMIN     0X02      //左边黑线连续性的最小值
#define  L_WITHMAX     0X05      //左边黑线连续性的最大值
#define  R_WITHMIN     0X02      //右边黑线连续性的最小值
#define  R_WITHMAX     0X05      //右边黑线连续性的最大值

#endif
