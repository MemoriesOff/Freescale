/******************** (C) COPYRIGHT 2011 蓝宙电子工作室 ********************
 * 文件名       ：assert.h
 * 描述         ：断言宏定义头文件
 *
 * 实验平台     ：
 * 库版本       ：
 * 嵌入系统     ：
 * 备    注     ：assert_failed() 在 assert.c 里定义
 *
 * 作者         ：
 * 淘宝店       ：
 * 技术支持论坛 ：
**********************************************************************************/


#ifndef _ASSERT_H_
#define _ASSERT_H_

/********************************************************************/

void assert_failed(char *, int);

#ifdef DEBUG
#define ASSERT(expr) \
    if (!(expr)) \
        assert_failed(__FILE__, __LINE__)
#else
#define ASSERT(expr)
#endif

/********************************************************************/
#endif /* _ASSERT_H_ */

