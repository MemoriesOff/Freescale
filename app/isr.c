/******************** (C) COPYRIGHT 2011 蓝宙电子工作室 ********************
* 文件名       ：isr.c
* 描述         ：中断处理例程
*
* 实验平台     ：
* 库版本       ：
* 嵌入系统     ：
*
* 作者         ：蓝宙电子工作室
* 淘宝店       ：http://landzo.taobao.com/
**********************************************************************************/



/******************** (C) COPYRIGHT 2011 蓝宙电子工作室 ********************
* 文件名       ：isr.c
* 描述         ：中断处理例程
*
* 实验平台     ：
* 库版本       ：
* 嵌入系统     ：
*
* 作者         ：蓝宙电子工作室
* 淘宝店       ：http://landzo.taobao.com/
**********************************************************************************/



#include "common.h"
#include "include.h"


uint8 TIME0flag_5ms  = 0 ;
uint8 TIME0flag_10ms = 0 ;
uint8 TIME0flag_20ms = 0 ;
uint8 TIME0flag_80ms = 0 ; 
uint8 TIME0flag_100ms = 0 ; 
uint8 TIME0flag_200ms = 0 ; 
uint16  TimeCount = 0 ;


/**************************
摄像头采用变量
***************************/
uint16 LineCount =0;                                    //摄像头实际行数  最大640
uint16 Sample_Line_Count =0;                            //DMA采集的行数
uint8  Sample_Line_flag =0 ;                           //采样标志位
uint16 *pSample_Line = 0;                              //采样指针，指向采样行数数组  
uint16 Sample_LineAryy[] = { 262, 270, 278, 0XFFFF, 0XFFFF};  //摄像头采样的行数
/*************************************************************************
*                             蓝宙电子工作室
*
*  函数名称：USART1_IRQHandler
*  功能说明：串口1 中断 接收 服务函数
*  参数说明：无
*  函数返回：无
*  修改时间：2012-2-14    已测试
*  备    注：
*************************************************************************/


void USART1_IRQHandler(void)
{
  uint8 ch;
  
  DisableInterrupts;		    //关总中断
  
  //接收一个字节数据并回发
  ch = uart_getchar (UART1);      //接收到一个数据
  uart_sendStr  (UART1, "\n你发送的数据为：");
  uart_putchar (UART1, ch);       //就发送出去
  
  EnableInterrupts;		    //开总中断
}
/*************************************************************************
*                             蓝宙嵌入式开发工作室
*
*  函数名称：PIT1_IRQHandler
*  功能说明：PIT1定时中断服务函数
*  参数说明：无
*  函数返回：无
*  修改时间：2012-9-18    已测试
*  备    注：
*************************************************************************/

void PIT1_IRQHandler(void)
{
  PIT_Flag_Clear(PIT1);       //清中断标志位
  
}

/*************************************************************************
*                             蓝宙电子工作室
*
*  函数名称：PIT0_IRQHandler
*  功能说明：PIT0 定时中断服务函数
*  参数说明：无
*  函数返回：无
*  修改时间：2012-2-18    已测试
*  备    注：
*************************************************************************/



void PIT0_IRQHandler(void)
{
  PIT_Flag_Clear(PIT0);       //清中断标志位
  
  TimeCount ++ ;
  
  if(TimeCount%5 == 0 )
  {
    TIME0flag_5ms = 1;
  } 
  if(TimeCount%10 == 0 )
  {
    TIME0flag_10ms = 1;
  } 
  if(TimeCount%20 == 0 )
  {
    TIME0flag_20ms = 1;
  }
  if(TimeCount%80 == 0 )
  {
    TIME0flag_80ms = 1;
  }
  if(TimeCount%100 == 0 )
  {
    TIME0flag_100ms = 1;
  } 
   if(TimeCount%200 == 0 )
  {
    TIME0flag_200ms = 1;
    TimeCount = 0 ;
  } 
  
}



/*************************************************************************
*                             蓝宙电子工作室
*
*  函数名称：SysTick_Handler
*  功能说明：系统滴答定时器中断服务函数
*  参数说明：无
*  函数返回：无
*  修改时间：2012-2-18    已测试
*  备    注：ucos里用得到
*************************************************************************/
void SysTick_Handler(void)
{
  //    OSIntEnter();
  //    OSTimeTick();
  //    OSIntExit();
}

/*************************************************************************
*                             蓝宙电子工作室
*
*  函数名称：PendSV_Handler
*  功能说明：PendSV（可悬起系统调用）中断服务函数
*  参数说明：无
*  函数返回：无
*  修改时间：2012-2-15    已测试
*  备    注：uC/OS用来切换任务
*************************************************************************/
void PendSV_Handler(void)
{
}
/*************************************************************************
*                             蓝宙电子工作室
*
*  函数名称：HardFault_Handler
*  功能说明：硬件上访中断服务函数
*  参数说明：无
*  函数返回：无
*  修改时间：2012-2-4    已测试
*  备    注：可以用LED闪烁来指示发生了硬件上访
*************************************************************************/
void HardFault_Handler(void)
{
  while (1)
  {
    printf("\n****硬件上访错误!!!*****\r\n\n");
  }
}

/*************************************************************************
*                             蓝宙电子工作室
*
*  函数名称：PORTA_IRQHandler
*  功能说明：PORTA端口中断服务函数
*  参数说明：无
*  函数返回：无
*  修改时间：2012-1-25    已测试
*  备    注：引脚号需要自己初始化来清除
*************************************************************************/
u8  DMA_Over_Flg = 0 ;     //场采集完成标志位
u16 LinCout =0;
u8 LinADCout =0;
u8 AcqFlg =0 ;
u16 *linarrycot = 0;
u16 AcqAryy[] = {0X110 ,0XFFFF ,0XFFFF};
u16 makcout = 0 ;
extern u8  BUFF[500] ;
extern u8  DMA_Over_Flg ;
extern u8  ADdata[DATAROW][DATACOUNT];


void PORTD_IRQHandler()
{
  
  /***************
  摄像头变量
  ***************/
/*  if(PORTD_ISFR & 0x2000)                                 //PTD13触发中断，采集的行中断
  {
    PORTD_ISFR  |= 0x2000;                              //写1清中断标志位  
    
    if(Sample_Line_flag )
    {       
      Sample_Line_flag = 0;  
          
      DMA_IRQ_CLEAN(DMA_CH4);                             //清除通道传输中断标志位    (这样才能再次进入中断)
      DMA_EN(DMA_CH4);                                    //使能通道CHn 硬件请求      (这样才能继续触发DMA传输) 
      DMA_IRQ_EN(DMA_CH4) ;                              //允许DMA通道传输
      
      Sample_Line_Count ++ ;                             //采样行数 
    }
    
    LineCount ++ ;
    if(*pSample_Line == LineCount )
    {         
      Sample_Line_flag = 1 ;
      if(*pSample_Line != 0xFFff )
      {
        pSample_Line ++ ;
      }
    }
  }
  
  if(PORTD_ISFR & 0x4000)                     //PTD14触发中断,采集的场中断。
  {
    PORTD_ISFR  |= 0x4000;                   //写1清中断标志位
    pSample_Line = &Sample_LineAryy[0] ;    //初始化要采集的行数
    LineCount = 0 ;                         //清除摄像头行数
  }*/
  
  
        if(PORTD_ISFR & 0x2000)            //PTD13触发中断，采集的行中断
    { 
        PORTD_ISFR  |= 0x2000;         //写1清中断标志位
        LinCout ++ ; 
        DMA_IRQ_CLEAN(DMA_CH4);                             //清除通道传输中断标志位    (这样才能再次进入中断)
        if((LinCout%(480/DATAROW)==0)&&(LinADCout<DATAROW))
        {
        DMA_DADDR(CH4) = (u32)(&ADdata[ LinADCout][0]);
          DMA_EN(DMA_CH4);                                    //使能通道CHn 硬件请求      (这样才能继续触发DMA传输) 
          DMA_IRQ_EN(DMA_CH4) ;                             //允许DMA通道传输
        //  PTA16_OUT = ~PTA16_OUT ;
         LinADCout ++ ;
        }
      
         if(LinADCout==DATAROW)
        DMA_Over_Flg = 1 ;
    }
    
    if(PORTD_ISFR & 0x4000)         //PTD14触发中断,采集的场中断。
    {
        PORTD_ISFR  |= 0x4000;        //写1清中断标志位
       
    //  uart_putchar(UART0,LinCout>>8);      //采样行数
    //    uart_putchar(UART0,LinCout);         //采样行数
        LinCout = 0 ;
        LinADCout=0;
        gpio_Interrupt_init(PORTD,13, GPI_DOWN, RING) ;          //开启行中断         
    }
  
}

/*************************************************************************
*                             蓝宙电子工作室
*
*  函数名称：PORTB_IRQHandler
*  功能说明：PORTB端口中断服务函数
*  参数说明：无
*  函数返回：无
*  修改时间：2012-1-25    已测试
*  备    注：引脚号需要根据自己初始化来修改
*************************************************************************/
void PORTB_IRQHandler()
{
  
  //  PORT_PCR_REG(PORTB , 10) |= PORT_PCR_ISF(1);
  uint8  n = 0;    //引脚号
  n = 0;
  if(PORTB_ISFR & (1 << n))         //PTB0触发中断
  {
    PORTB_ISFR  |= (1 << n);        //写1清中断标志位
    /*  用户任务  */
    
  }
  
  n = 10;
  if(PORTB_ISFR & (1 << n))         //PTB10触发中断
  {
    PORTB_ISFR  |= (1 << n);        //写1清中断标志位
    /*  用户任务  */
    
    
    
  }
}


/*************************************************************************
*                             蓝宙嵌入式开发工作室
*
*  函数名称：PIT2_IRQHandler
*  功能说明：PIT2定时中断服务函数
*  参数说明：无
*  函数返回：无
*  修改时间：2012-9-18    已测试
*  备    注：
*************************************************************************/

void PIT2_IRQHandler(void)
{
  PIT_Flag_Clear(PIT2);       //清中断标志位
  
  
}
/*************************************************************************
*                             蓝宙电子工作室
*
*  函数名称：FTM0_IRQHandler
*  功能说明：FTM0输入捕捉中断服务函数
*  参数说明：无
*  函数返回：无
*  修改时间：2012-2-25
*  备    注：引脚号需要根据自己初始化来修改，参考现有的代码添加自己的功能
*************************************************************************/
void FTM0_IRQHandler()
{
  
  
}

/*************************************************************************
*                             蓝宙电子工作室
*
*  函数名称：FTM1_IRQHandler
*  功能说明：FTM1输入捕捉中断服务函数
*  参数说明：无
*  函数返回：无
*  修改时间：2012-2-25
*  备    注：引脚号需要根据自己初始化来修改，参考现有的代码添加自己的功能
*************************************************************************/
void FTM1_IRQHandler()
{
  uint8 s = FTM1_STATUS;             //读取捕捉和比较状态  All CHnF bits can be checked using only one read of STATUS.
  uint8 n;
  FTM1_STATUS = 0x00;               //清中断标志位
  
  n = 0;
  if( s & (1 << n) )
  {
    FTM_CnSC_REG(FTM1_BASE_PTR, n) &= ~FTM_CnSC_CHIE_MASK; //禁止输入捕捉中断
    /*     用户任务       */
    
    printf("\nFTM1发送中断\n");
    
    /*********************/
    //不建议在这里开启输入捕捉中断，而是在main函数里根据需要来开启
    //通道 CH0、CH1、Ch2、Ch3 有滤波器
    //FTM_CnSC_REG(FTM1_BASE_PTR,n) |= FTM_CnSC_CHIE_MASK;  //开启输入捕捉中断
    //delayms(10);        //因为输入的信号跳变过程不稳定，容易触发多次输入捕捉，所以添加延时
    //但考虑到中断不应该过长延时，所以开输入捕捉中断就放在main函数里，根据需要来开启
  }
  
  n = 1;
  if( s & (1 << n) )
  {
    FTM_CnSC_REG(FTM1_BASE_PTR, n) &= ~FTM_CnSC_CHIE_MASK; //禁止输入捕捉中断
    /*     用户任务       */
    
    
    /*********************/
    //不建议在这里开启输入捕捉中断
    //FTM_CnSC_REG(FTM1_BASE_PTR,n) |= FTM_CnSC_CHIE_MASK;  //开启输入捕捉中断
  }
  
}



volatile uint8  pit_flag = 0;
volatile u32 dma_int_count = 0;


/*************************************************************************
*                             蓝宙电子工作室
*
*  函数名称：DMA_CH4_Handler
*  功能说明：DMA通道4的中断服务函数
*  参数说明：是采集摄像头数据，本数据位摄像头AD数据，可以采集到300个点。
设置标志位能够及时搬移。
*  函数返回：无
*  修改时间：2012-3-18    已测试
*  备    注：
*************************************************************************/
uint8  DMA_Over_Flag = 0 ;     //行采集完成标志位
void DMA_CH4_Handler(void)
{
  //DMA通道4
  DMA_IRQ_CLEAN(DMA_CH4) ;
  DMA_IRQ_DIS(DMA_CH4);
  DMA_DIS(DMA_CH4);
  DMA_Over_Flag = 1 ;
  
}

/*************************************************************************
*                             蓝宙电子工作室
*
*  函数名称：DMA_CH4_Handler
*  功能说明：DMA通道4的中断服务函数
*  参数说明：是采集摄像头数据，本数据位摄像头AD数据，可以采集到300个点。
设置标志位能够及时搬移。
*  函数返回：无
*  修改时间：2012-3-18    已测试
*  备    注：
*************************************************************************/
void DMA_CH0_Handler(void)
{
  
  //DMA通道0
  printf("DMA_CH0_Handler\n");
  DMA_IRQ_CLEAN(DMA_CH0);                             //清除通道传输中断标志位    (这样才能再次进入中断)
  DMA_EN(DMA_CH0);                                    //使能通道CHn 硬件请求      (这样才能继续触发DMA传输)
  //   uart_putchar(UART0,0XAA);
}


/*************************************************************************
*                             蓝宙电子工作室
*
*  函数名称：LPT_Handler
*  功能说明：LPT通道4的中断服务函数
*  参数说明：
*  函数返回：无
*  修改时间：2012-3-18    已测试
*  备    注：
*************************************************************************/

volatile uint8 LPT_INT_count = 0;
void  LPT_Handler(void)
{
  LPTMR0_CSR |= LPTMR_CSR_TCF_MASK;   //清除LPTMR比较标志
  LPT_INT_count++;                    //中断溢出加1
}