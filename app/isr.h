/******************** (C) COPYRIGHT 2011 蓝宙电子工作室 ********************
 * 文件名       ：isr.h
 * 描述         ：重新宏定义中断号，重映射中断向量表里的中断函数地址，
 *                使其指向我们所定义的中断服务函数。声明中断服务函数
 *                警告：只能在"vectors.c"包含，而且必须在"vectors.h"包含的后面！！！
 *
 * 实验平台     ：
 * 库版本       ：
 * 嵌入系统     ：
 *
 * 作者         ：蓝宙电子工作室
 * 淘宝店       ：http://landzo.taobao.com/
**********************************************************************************/



#ifndef __ISR_H
#define __ISR_H 1

#include  "include.h"

/*                          重新定义中断向量表
 *  先取消默认的中断向量元素宏定义       #undef  VECTOR_xxx
 *  在重新定义到自己编写的中断函数       #define VECTOR_xxx    xxx_IRQHandler
 *  例如：
 *       #undef  VECTOR_003
 *       #define VECTOR_003    HardFault_Handler    重新定义硬件上访中断服务函数
 */
#undef  VECTOR_014
#undef  VECTOR_015
#if     USOC_EN > 0u    //如果定义了用ucos系统  用ucos的滴答定时器中断服务函数
#define VECTOR_014    OS_CPU_PendSVHandler
#define VECTOR_015    OS_CPU_SysTickHandler
#else
#define VECTOR_014    PendSV_Handler
#define VECTOR_015    SysTick_Handler   //滴答定时器  OS用得到
#endif

#undef  VECTOR_003
#define VECTOR_003    HardFault_Handler   //发生硬件上访 （就是程序跑飞了）     推荐调试程序时，用个LED显示来指示程序跑飞了



#undef  VECTOR_016
#define VECTOR_016    DMA_CH0_Handler

#undef  VECTOR_020
#define VECTOR_020    DMA_CH4_Handler





#undef  VECTOR_063                        //要先取消了，因为在vectors.h里默认是定义为 default_isr  
#define VECTOR_063    USART1_IRQHandler   //重新定义63号中断的ISR：UART1：Single interrupt vector for UART status sources

#undef  VECTOR_078
#define VECTOR_078    FTM0_IRQHandler     //FTM0输入捕捉中断 

#undef  VECTOR_079
#define VECTOR_079    FTM1_IRQHandler     //FTM0输入捕捉中断

#undef  VECTOR_084
#define VECTOR_084    PIT0_IRQHandler     //重新定义84号中断为PIT0_IRQHandler中断

#undef  VECTOR_085
#define VECTOR_085    PIT1_IRQHandler     //重新定义85号中断为PIT1_IRQHandler中断

#undef  VECTOR_086
#define VECTOR_086    PIT2_IRQHandler     //重新定义86号中断为PIT2_IRQHandler中断

#undef  VECTOR_101
#define VECTOR_101    LPT_Handler        //LPT中断

#undef  VECTOR_104
#define VECTOR_104    PORTB_IRQHandler    //PORTB中断

#undef  VECTOR_106
#define VECTOR_106    PORTD_IRQHandler    //PORTD中断



extern void PendSV_Handler(void);         //可悬起的系统调用中断服务函数
extern void HardFault_Handler(void);      //发生硬件上访（程序跑飞）
extern void SysTick_Handler(void);        //滴答时钟(os 用到)
extern void USART1_IRQHandler();          //串口1 中断接收函数
extern void PIT0_IRQHandler();            //PIT0 定时中断服务函数
extern void PIT1_IRQHandler();            //PIT1 定时中断服务函数
extern void PIT2_IRQHandler();            //PIT2 定时中断服务函数
extern void PORTD_IRQHandler();           //PORTD中断服务函数
extern void PORTB_IRQHandler();           //PORTB中断服务函数
extern void FTM0_IRQHandler();            //FTM0输入捕捉中断
extern void FTM1_IRQHandler();            //FTM0输入捕捉中断
extern void LPT_Handler(void);           //LPT脉冲中断
extern void DMA_CH0_Handler(void);
extern void DMA_CH4_Handler(void);              //DMA通道4的中断服务函数

#endif  //__ISR_H

/* End of "isr.h" */
