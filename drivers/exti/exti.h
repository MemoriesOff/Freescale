/******************** (C) COPYRIGHT 2011 蓝宙电子工作室 ********************
 * 文件名       ：exti.h
 * 描述         ：外部中断函数配置头文件
 *
 * 实验平台     ：
 * 库版本       ：
 * 嵌入系统     ：
 *
 * 作者         ：蓝宙电子工作室
 * 淘宝店       ：
 * 技术支持论坛 ：
**********************************************************************************/

#ifndef _EXTI_H_
#define _EXTI_H_

typedef enum exti_cfg
{
    zero_down     = 0x08u,     //低电平触发，内部下拉
    rising_down   = 0x09u,     //上升沿触发，内部下拉
    falling_down  = 0x0Au,     //下降沿触发，内部下拉
    either_down   = 0x0Bu,     //跳变沿触发，内部下拉
    one_down      = 0x0Cu,     //高电平触发，内部下拉

    //用最高位标志上拉和下拉
    zero_up       = 0x88u,     //低电平触发，内部上拉
    rising_up     = 0x89u,     //上升沿触发，内部上拉
    falling_up    = 0x8Au,     //下降沿触发，内部上拉
    either_up     = 0x8Bu,     //跳变沿触发，内部上拉
    one_up        = 0x8Cu      //高电平触发，内部上拉
} exti_cfg;

void  exti_init(PORTx, u8 n, exti_cfg);


#endif
