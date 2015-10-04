/******************** (C) COPYRIGHT 2011 蓝宙电子工作室 ********************
 * 文件名       ：gpio.h
 * 描述         ：gpio驱动头文件
 *
 * 实验平台     ：
 * 库版本       ：
 * 嵌入系统     ：
 *
 * 作者         ：蓝宙电子工作室
 * 淘宝店       ：
 * 技术支持论坛 ：
**********************************************************************************/


#ifndef __GPIO_H__
#define __GPIO_H__

#include "gpio_cfg.h"

//端口宏定义
typedef enum PORTx
{
    PORTA,
    PORTB,
    PORTC,
    PORTD,
    PORTE
} PORTx;

//定义管脚方向
typedef enum GPIO_CFG
{
    //这里的值不能改！！！
    GPI         = 0,                          //定义管脚输入方向      GPIOx_PDDRn里，0表示输入，1表示输出
    GPO         = 1,                          //定义管脚输出方向

    GPI_DOWN    = 0x02,                       //输入下拉              PORTx_PCRn需要PE=1，PS=0
    GPI_UP      = 0x03,                       //输入上拉              PORTx_PCRn需要PE=1，PS=1
    GPI_PF      = 0x10,                       //输入，带无源滤波器,滤波范围：10 MHz ~ 30 MHz 。不支持高速接口（>=2MHz）  0b10000           Passive Filter Enable
    GPI_DOWN_PF = GPI_DOWN | GPI_PF ,         //输入下拉，带无源滤波器
    GPI_UP_PF   = GPI_UP   | GPI_PF ,         //输入上拉，带无源滤波器

    GPO_HDS     = 0x41,                        //输出高驱动能力   0b100 0001    High drive strength
    GPO_SSR     = 0x05,                        //输出慢变化率          0b101    Slow slew rate
    GPO_HDS_SSR = GPO_HDS | GPO_SSR,           //输出高驱动能力、慢变化率
} GPIO_CFG;  //最低位为0，肯定是输入；GPI_UP 和 GPI_UP_PF的最低位为1，其他为输出

//定义管脚方式
typedef enum GPIO_INP{
  
   GPI_DISAB      = 0X0  ,                     //不允许中断
   GPI_EMA_RING   = 0X01 ,                     //EMA上升沿中断
   GPI_EMA_FALL   = 0X02 ,                     //EMA下降沿中断
   GPI_EMA_EITH   = 0X03 ,                     //EMA跳变沿中断
   GPI_INP_ZERO   = 0X08 ,                     //逻辑零中断
   GPI_INP_RING   = 0X09 ,                     //上升沿中断
   GPI_INP_FALL   = 0X0A ,                     //下降沿中断
   GPI_INP_EITH   = 0X0B ,                     //跳变沿中断
   GPI_INP_ONE    = 0X0C ,                     //逻辑一中断
 
    
} GPIO_INP ;  //引脚中断定义 

#define HIGH  1u
#define LOW   0u

#define DMARING       0x01
#define DMAFALLING    0x02
#define DMAEITHER     0x03
#define ZERO          0x08
#define RING          0x09
#define FALLING       0x0A
#define EITHER        0x0B
#define ONE           0x0C

extern volatile  struct GPIO_MemMap *GPIOx[5];
extern volatile struct  PORT_MemMap *PORTX[5];

/****************************外部使用****************************/

void    gpio_init  (PORTx, u8 n, GPIO_CFG, u8 data);  //初始化gpio
void gpio_Interrupt_init(PORTx portx, u8 n, GPIO_CFG cfg, u8 mode); //初始化中断

void    gpio_set   (PORTx,  u8 n,     u8 data);                                                 //设置引脚状态
void    gpio_turn  (PORTx,  u8 n);                                                              //翻转引脚状态
u8      gpio_get   (PORTx,  u8 n);                                                              //读取引脚状态

#define GPIO_SET(PORTx,n,x)           GPIO_SET_##x((PORTx),(n))                                 //设置输出电平x，x为0或1   例如 GPIO_SET(PORTA,1,1)  PA1输出高电平
#define GPIO_TURN(PORTx,n)           (GPIO_PDOR_REG(GPIOx[(PORTx)]) ^=  (1<<(n)))               //翻转输出电平
#define GPIO_Get(PORTx,n)            ((GPIO_PDIR_REG(GPIOx[(PORTx)])>>(n))&0x1)                 //读取引脚输入状态


//GPIO  1位操作
#define GPIO_SET_1bit(PORTx,n,data)   GPIO_PDOR_REG(GPIOx[(PORTx)])=(( GPIO_PDOR_REG(GPIOx[(PORTx)])& ~(0x1<<(n)) )|(((data)&0x01)<<(n)))   //写1位数据（n为最低位引脚号）
#define GPIO_DDR_1bit(PORTx,n,ddr)    GPIO_PDDR_REG(GPIOx[(PORTx)])=(( GPIO_PDDR_REG(GPIOx[(PORTx)])& ~(0x1<<(n)) )|(((ddr)&0x01)<<(n)))    //设置1位输入输出方向（n为最低位引脚号）
#define GPIO_GET_1bit(PORTx,n)        (( GPIO_PDIR_REG(GPIOx[(PORTx)])>>(n) ) & 0x1)                                                        //读1位数据（n为最低位引脚号）

//GPIO  2位操作
#define GPIO_SET_2bit(PORTx,n,data)   GPIO_PDOR_REG(GPIOx[(PORTx)])=(( GPIO_PDOR_REG(GPIOx[(PORTx)])& ~(0x3<<(n)) )|(((data)&0x03)<<(n)))   //写2位数据（n为最低位引脚号）
#define GPIO_DDR_2bit(PORTx,n,ddr)    GPIO_PDDR_REG(GPIOx[(PORTx)])=(( GPIO_PDDR_REG(GPIOx[(PORTx)])& ~(0x3<<(n)) )|(((ddr)&0x03)<<(n)))    //设置2位输入输出方向（n为最低位引脚号）
#define GPIO_GET_2bit(PORTx,n)        (( GPIO_PDIR_REG(GPIOx[(PORTx)])>>(n) ) & 0x3)                                                        //读2位数据（n为最低位引脚号）

//GPIO  4位操作
#define GPIO_SET_4bit(PORTx,n,data)   GPIO_PDOR_REG(GPIOx[(PORTx)])=(( GPIO_PDOR_REG(GPIOx[(PORTx)])& ~(0xf<<(n)) )|(((data)&0x0f)<<(n)))   //写4位数据（n为最低位引脚号）
#define GPIO_DDR_4bit(PORTx,n,ddr)    GPIO_PDDR_REG(GPIOx[(PORTx)])=(( GPIO_PDDR_REG(GPIOx[(PORTx)])& ~(0xf<<(n)) )|(((ddr)&0x0f)<<(n)))    //设置4位输入输出方向（n为最低位引脚号）
#define GPIO_GET_4bit(PORTx,n)        (( GPIO_PDIR_REG(GPIOx[(PORTx)])>>(n) ) & 0xf)                                                        //读4位数据（n为最低位引脚号）

//GPIO  8位操作
#define GPIO_SET_8bit(PORTx,n,data)   GPIO_PDOR_REG(GPIOx[(PORTx)])=(( GPIO_PDOR_REG(GPIOx[(PORTx)])& ~(0xff<<(n)) )|(((data)&0xff)<<(n)))  //写8位数据（n为最低位引脚号）  
#define GPIO_DDR_8bit(PORTx,n,ddr)    GPIO_PDDR_REG(GPIOx[(PORTx)])=(( GPIO_PDDR_REG(GPIOx[(PORTx)])& ~(0xff<<(n)) )|(((ddr)&0x0ff)<<(n)))  //设置8位输入输出方向（n为最低位引脚号）
#define GPIO_GET_8bit(PORTx,n)        (( GPIO_PDIR_REG(GPIOx[(PORTx)])>>(n) ) & 0xff)                                                       //读8位数据（n为最低位引脚号）


//GPIO  16位操作
#define GPIO_SET_16bit(PORTx,n,data)  GPIO_PDOR_REG(GPIOx[(PORTx)])=(( GPIO_PDOR_REG(GPIOx[(PORTx)])&~(0xffff<<(n)) )|(((data)&0xffff)<<(n)))   //写16位数据（n为最低位引脚号）
#define GPIO_DDR_16bit(PORTx,n,ddr)   GPIO_PDDR_REG(GPIOx[(PORTx)])=(( GPIO_PDDR_REG(GPIOx[(PORTx)])& ~(0xffff<<(n)) )|(((ddr)&0x0ffff)<<(n)))  //设置16位输入输出方向（n为最低位引脚号）
#define GPIO_GET_16bit(PORTx,n)       (( GPIO_PDIR_REG(GPIOx[(PORTx)])>>(n) ) & 0xffff)                                                         //读16位数据（n为最低位引脚号）

//GPIO  32位操作
#define GPIO_SET_32bit(PORTx,data)  GPIO_PDOR_REG(GPIOx[(PORTx)])=(data)                                                                    //写32位数据
#define GPIO_DDR_32bit(PORTx,ddr)   GPIO_PDDR_REG(GPIOx[(PORTx)])=(ddr)                                                                     //设置32位输入输出方向
#define GPIO_GET_32bit(PORTx)       GPIO_PDIR_REG(GPIOx[(PORTx)])                                                                           //读32位数据



/****************************内部使用，用户不需要关心****************************/
#define GPIO_SET_1(PORTx,n)          GPIO_PDOR_REG(GPIOx[(PORTx)]) |=  (1<<(n))      //设置输出为高电平        例如：GPIO_SET_H(PORTA,1)   PA1输出高电平
#define GPIO_SET_0(PORTx,n)          GPIO_PDOR_REG(GPIOx[(PORTx)]) &= ~(1<<(n))      //设置输出为低电平        例如：GPIO_SET_L(PORTA,1)   PA1输出低电平

#endif
