/******************** (C) COPYRIGHT 2011 蓝宙电子工作室 ********************
 * 文件名       ：sysinit.h
 * 描述         ：系统时钟配置相关函数的头文件
 * 备注         ：
 *
 * 实验平台     ：
 * 库版本       ：
 * 嵌入系统     ：
 *
 * 作者         ：
 * 淘宝店       ：http://landzo.taobao.com/
 * 技术支持论坛 ：
**********************************************************************************/

#ifndef    _SYSINIT_H_
#define    _SYSINIT_H_

/********************************************************************/
extern u32 core_clk_khz;
extern u32 core_clk_mhz;
extern u32 bus_clk_khz;



// function prototypes
void sysinit (void);
void trace_clk_init(void);
void fb_clk_init(void);
void enable_abort_button(void);



/********************************************************************/

#endif  //_SYSINIT_H_