#include "common.h"
#include "include.h"
#include "calculation.h"
#include "LandzoCCD.h"



/*************************************************************************
*                             蓝宙电子工作室
*
*  函数名称：CCD_init
*  功能说明：CCD初始化
*  参数说明：
*  函数返回：无
*  修改时间：2012-10-20
*  备    注：
*************************************************************************/
void CCD_init(void)
{
  gpio_init (PORTE , 4, GPO,HIGH);        //初始化SI
  gpio_init (PORTE , 5, GPO,HIGH);        //初始化CLK
  adc_init(ADC1, AD6b) ;                  //初始化AD
  
}

/*************************************************************************
*                           蓝宙电子工作室
*
*  函数名称：StartIntegration
*  功能说明：CCD启动程序
*  参数说明：
*  函数返回：无
*  修改时间：2012-10-20
*  备    注：
*************************************************************************/
void StartIntegration(void) {
  
  uint8_t i;
  
  SI_SetVal();            /* SI  = 1 */
  SamplingDelay();
  CLK_SetVal();           /* CLK = 1 */
  SamplingDelay();
  SI_ClrVal();            /* SI  = 0 */
  SamplingDelay();
  CLK_ClrVal();           /* CLK = 0 */
  
  for(i=0; i<127; i++) {
    SamplingDelay();
    SamplingDelay();
    CLK_SetVal();       /* CLK = 1 */
    SamplingDelay();
    SamplingDelay();
    CLK_ClrVal();       /* CLK = 0 */
  }
  SamplingDelay();
  SamplingDelay();
  CLK_SetVal();           /* CLK = 1 */
  SamplingDelay();
  SamplingDelay();
  CLK_ClrVal();           /* CLK = 0 */
}


/*************************************************************************
*                           蓝宙电子工作室
*
*  函数名称：ImageCapture
*  功能说明：CCD采样程序
*  参数说明：* ImageData   采样数组
*  函数返回：无
*  修改时间：2012-10-20
*  备    注：
*ImageData =  ad_once(ADC1, AD6a, ADC_8bit);
*************************************************************************/

void ImageCapture(uint8_t * ImageData) {
  
  uint8_t i;
  extern u8 AtemP ;
  
  SI_SetVal();            /* SI  = 1 */
  SamplingDelay();
  CLK_SetVal();           /* CLK = 1 */
  SamplingDelay();
  SI_ClrVal();            /* SI  = 0 */
  SamplingDelay();
  
  //Delay 10us for sample the first pixel
  /**/
  for(i = 0; i < 90; i++) 
  {
    SamplingDelay() ;  //200ns
  }
  
  //Sampling Pixel 1
  
  *ImageData =  ad_once(ADC1, AD6b, ADC_8bit);
  ImageData ++ ;
  CLK_ClrVal();           /* CLK = 0 */
  
  for(i=0; i<7; i++) 
  {
    SamplingDelay();
    SamplingDelay();
    CLK_SetVal();       /* CLK = 1 */
    SamplingDelay();
    SamplingDelay();
    SamplingDelay();
    SamplingDelay();
    //Sampling Pixel 2~128
    
    *ImageData =  ad_once(ADC1, AD6b, ADC_8bit);
    ImageData ++ ;
    CLK_ClrVal();       /* CLK = 0 */
  }
  
  for(i=7; i<120; i++) 
  {
    SamplingDelay();
    SamplingDelay();
    CLK_SetVal();       /* CLK = 1 */
    SamplingDelay();
    SamplingDelay();
    //Sampling Pixel 2~128
    
    *ImageData =  ad_once(ADC1, AD6b, ADC_8bit);
    ImageData ++ ;
    CLK_ClrVal();       /* CLK = 0 */
  }
  for(i=120; i<127; i++) 
  {
    SamplingDelay();
    SamplingDelay();
    CLK_SetVal();       /* CLK = 1 */
    SamplingDelay();
    SamplingDelay();
    SamplingDelay();
    SamplingDelay();
    //Sampling Pixel 2~128
    
    *ImageData =  ad_once(ADC1, AD6b, ADC_8bit);
    ImageData ++ ;
    CLK_ClrVal();       /* CLK = 0 */
  }
  
  SamplingDelay();
  SamplingDelay();
  CLK_SetVal();           /* CLK = 1 */
  SamplingDelay();
  SamplingDelay();
  CLK_ClrVal();           /* CLK = 0 */
}


/*************************************************************************
*                           蓝宙电子工作室
*
*  函数名称：CalculateIntegrationTime
*  功能说明：计算曝光时间
*  参数说明：
*  函数返回：无
*  修改时间：2012-10-20
*  备    注：
*************************************************************************/

/* 曝光时间，单位ms */
u8 IntegrationTime = 100;
void CalculateIntegrationTime(void) {
  extern u8 Pixel[128];
  /* 128个像素点的平均AD值 */
  u8 PixelAverageValue;
  /* 128个像素点的平均电压值的10倍 */
  u8 PixelAverageVoltage;
  /* 设定目标平均电压值，实际电压的10倍 */
  s16 TargetPixelAverageVoltage = 13;
  /* 设定目标平均电压值与实际值的偏差，实际电压的10倍 */
  s16 PixelAverageVoltageError = 0;
  /* 设定目标平均电压值允许的偏差，实际电压的10倍 */
  s16 TargetPixelAverageVoltageAllowError = 2;
  
  /* 计算128个像素点的平均AD值 */
  PixelAverageValue = PixelAverage(128,Pixel);
  /* 计算128个像素点的平均电压值,实际值的10倍 */
  PixelAverageVoltage = (uint8_t)((int)(PixelAverageValue * 33.0 / 256) );
  
  //   uart_putchar(UART0,0XBB); 
  //  uart_putchar(UART0,PixelAverageValue); 
  //  uart_putchar(UART0,PixelAverageVoltage); 
  
  PixelAverageVoltageError = TargetPixelAverageVoltage - PixelAverageVoltage;
  if(PixelAverageVoltageError < -TargetPixelAverageVoltageAllowError)
    IntegrationTime--;
  if(PixelAverageVoltageError > TargetPixelAverageVoltageAllowError)
    IntegrationTime++;
  if(IntegrationTime <= 1)
    IntegrationTime = 1;
  if(IntegrationTime >= 100)
    IntegrationTime = 100;
  //  uart_putchar(UART0,PixelAverageVoltageError); 
  // uart_putchar(UART0,IntegrationTime); 
}


/*************************************************************************
*                           蓝宙电子工作室
*
*  函数名称：PixelAverage
*  功能说明：求数组的均值程序
*  参数说明：
*  函数返回：无
*  修改时间：2012-10-20
*  备    注：
*************************************************************************/
u8 PixelAverage(u8 len, u8 *data) {
  uint8_t i;
  unsigned int sum = 0;
  for(i = 0; i<len; i++) {
    sum = sum + *data++;
  }
  return ((uint8_t)(sum/len));
}
/*************************************************************************
*                           蓝宙电子工作室
*
*  函数名称：SendHex
*  功能说明：采集发数程序
*  参数说明：
*  函数返回：无
*  修改时间：2012-10-20
*  备    注：
*************************************************************************/
void SendHex(uint8_t hex) {
  uint8_t temp;
  temp = hex >> 4;
  if(temp < 10) {
    uart_putchar(UART0,temp + '0');
  } else {
    uart_putchar(UART0,temp - 10 + 'A');
  }
  temp = hex & 0x0F;
  if(temp < 10) {
    uart_putchar(UART0,temp + '0');
  } else {
    uart_putchar(UART0,temp - 10 + 'A');
  }
}
/*************************************************************************
*                           蓝宙电子工作室
*
*  函数名称：SendImageData
*  功能说明：
*  参数说明：
*  函数返回：无
*  修改时间：2012-10-20
*  备    注：
*************************************************************************/
void SendImageData(uint8_t * ImageData) {
  
  uint8_t i;
  uint8_t crc = 0;
  
  /* Send Data */
  uart_putchar(UART0,'*');
  uart_putchar(UART0,'L');
  uart_putchar(UART0,'D');
  
  SendHex(0);
  SendHex(0);
  SendHex(0);
  SendHex(0);
  
  for(i=0; i<128; i++) {
    SendHex(*ImageData++);
  }
  
  SendHex(crc);
  uart_putchar(UART0,'#');
}
/*************************************************************************
*                           蓝宙电子工作室
*
*  函数名称：SamplingDelay
*  功能说明：CCD延时程序 200ns
*  参数说明：
*  函数返回：无
*  修改时间：2012-10-20
*  备    注：
*************************************************************************/
void SamplingDelay(void)
{
  volatile u8 i ;
  for(i=0;i<1;i++) 
  {
    asm("nop");
    asm("nop");
  }
  
}

/**************************************************************
*
*               线性CCD信号黑线识别函数
*
*     函数名称：CCD_Black
*    功能说明：黑线识别函数
*    参数说明：
*    函数返回:
*    修改时间：2013-03-15
*    备    注：
*************************************************************/  
int16 testarry[130]={0};
void CCD_Black(uint8 *point,uint8 *rightblackedge,uint8 *leftblackedge,int8 *centre_black)
{
  int16 temp0,temp1;
  uint8 leftedge,rightedge;
  uint16 leftavg,rightavg;
  uint8 leftflag,rightflag;
  int16 temp_value1,temp_value2;
  uint8 leftcount,rightcount;
  uint8 LINEWITH,LEFTLINEFAV, RINGTLINFAV;
  static uint8 centre_lastblack =64;
  
  LINEWITH=7;       //黑线宽度 
  LEFTLINEFAV=15;  //左下降沿两点电压变化值最小值
  RINGTLINFAV=15;  //右下降沿两点电压变化值最小值
  leftcount=0;     //左黑线宽度计数值
  rightcount=0;    //右黑线宽度计数值 
  leftedge=0;      //左边界 
  rightedge=0;     //右边界
  leftflag=0;      //左丢线标志位
  rightflag=0;     //右丢线标志位
  leftavg=0;       // 计算左黑线平均值
  rightavg=0;      // 计算右黑线平均值
 
   /*查找左边沿开始，找到边沿后跳出循环*/
  for(temp0=centre_lastblack;temp0>5;temp0--)               //检测左侧边沿
  {
    
    for(temp1=temp0; temp1>( temp0-LINEWITH );temp1--)      //寻找一点附近的几个点是否能构成下降沿
    {    
      if(temp1<=0) break;                                   //不满足实际条件跳出循环             
      
      temp_value1= point[temp0]-point[temp1];               //比较白点附近的几个值
      
      if(temp_value1>=LEFTLINEFAV)                          //判断是否满足要求
      {
        leftcount++;                                        //满足条件计数值加一
      //  testarry[temp0]=temp_value1;
      }
      else
      {
        leftcount=0;   
     //   testarry[temp0]=0;
      }           
    }
    
    
    if(leftcount >= (LINEWITH -2) )                        //判断是否满足边沿条件
    {
      leftedge = temp0 ;                                  //满足条件边沿找到
      leftflag=1;                                         //找到边沿，标志位置1 
      break;
    }
    else
    {
      leftcount=0;                                        //未找到清除计数  
      leftflag=0;                                         //标志位清除
      leftedge=5;                                         // 边沿赋值 
    }
     
    if(leftedge<5) leftedge = 5;   
  } 
  /*左边沿查找结束*/
  
  /*查找右边沿开始，找到边沿后跳出循环*/
  for(temp0=centre_lastblack;temp0< 121;temp0++)          //右边沿查找方式和查找左边沿一样
  {    
    for(temp1=temp0; temp1< ( temp0+LINEWITH );temp1++)
    {
      if(temp1>=128) break;
      
      temp_value2 = point[temp0]-point[temp1];
      
      if(temp_value2 >=RINGTLINFAV)
      {      
        rightcount++;
       // testarry[temp0]=temp_value2;
      }
      else
      {
        rightcount=0;
     //   testarry[temp0]=0;
      }          
      
    }
    
    if( rightcount >= (LINEWITH-2) )
    {
      rightedge = temp0;
      rightflag=1;
      break;
    }
    else
    {
      rightcount=0;  
      rightflag=0;
      rightedge=121;
    }
    
    if( rightedge > 121) rightedge=121;
    
  }    
  /*右边沿查找结束*/
  
  
  /*判断已找到边沿的可靠性*/
  
  /*左边沿的可靠性判断*/
  if(leftflag==1)
  {
    /*计算黑线的平均值*/
    leftcount=0;
    for(temp0=leftedge;temp0>( leftedge-LINEWITH );temp0--)    
    {
      if(temp0<=0)break;
      leftcount++;
      leftavg+=point[temp0+1];
    }
    
    leftavg=leftavg/leftcount;      
    
    
    /*比较白底和黑线之间的差值*/
    leftcount=0;
    for(temp0=leftedge;temp0<( leftedge+LINEWITH );temp0++)   //判断边沿附近的白点，是否满足条件
    {
      if(temp0>=127)break;
      
      temp_value1 = point[temp0-1]-leftavg;                  //白点与黑线平均值作比较
      testarry[temp0]=temp_value1; 
      if( temp_value1>= LEFTLINEFAV)                         //判断是否满足要求
      { 
        leftcount++;                                         //找到边沿，标志位置1 
      }
      else
      {
        leftcount=0;                                         // 标志位清除
      }
    }
    
    if(leftcount>(LINEWITH-2))                               //判断是否满足边沿条件
    {
      *leftblackedge=leftedge;                               //两个条件都满足赋值给边沿
      leftedge=1;
    }
    else
    {
      *leftblackedge=5;
      leftcount=0;                                          //未找到清除计数 
      leftflag=0;                                           //标志位清除
    }    
  }
  else
  {
    *leftblackedge=5;
  }
  /*左边界可靠性判断结束*/
  
  
  
  /*右边界可靠性判断开始*/
  if(rightflag==1)                      // 右边界判断可靠性，和左边界判断方式一样                  
  {
    rightcount=0;
    for(temp0=rightedge;temp0<( rightedge+LINEWITH );temp0++)
    {
      if(temp0>=127)break;
      rightcount++;
      rightavg+=point[temp0-1];
    }
    
    rightavg=rightavg/rightcount;
    
    rightcount=0;
    for(temp0=rightedge;temp0>( rightedge-LINEWITH );temp0--)
    {
      if(temp0<=0)break;
      
      temp_value2 = point[temp0] - rightavg;
      testarry[temp0]=temp_value2;
      if(temp_value2>= RINGTLINFAV)
      {
        rightcount++;
      }
      else
      {
        rightcount=0;
      }
      
    }
    
    if(rightcount>(LINEWITH-2))
    {
      *rightblackedge=rightedge;
      rightedge=1;
    }
    else
    {
      *rightblackedge=121;
      rightcount=0;
      rightflag=0;
    }
    
  }
  else
  {
    *rightblackedge=121;
  }
  
  /*右边界可靠性判断结束*/
  
//  *leftblackedge=leftedge;
//  *rightblackedge=rightedge;

  if( ( leftflag==1 )&&( rightflag==1 ) )
  {
  *centre_black = (uint8)(( *leftblackedge + *rightblackedge )/2);
  centre_lastblack = *centre_black;
  }
  else if( ( leftflag==1 )&&( rightflag==0 ) )
  {
   *centre_black = (uint8)(( *leftblackedge + *rightblackedge )/2);
   centre_lastblack = *centre_black;
  }
  else if( ( leftflag==0 )&&( rightflag==1 ) )
  {
   *centre_black = (uint8)(( *leftblackedge + *rightblackedge )/2);
   centre_lastblack = *centre_black;
  }
  else if( ( leftflag==0 )&&( rightflag==0 ) )
  {
   *centre_black= centre_lastblack;
  }   
  
//    while(!(UART_S1_REG(UARTx[UART0]) & UART_S1_TDRE_MASK));
//    UART_D_REG(UARTx[UART0]) = 0xff;
//    while(!(UART_S1_REG(UARTx[UART0]) & UART_S1_TDRE_MASK));
//    UART_D_REG(UARTx[UART0]) = 0xff;
//    uart_sendN (UART0, point, 128);
//    
//    while(!(UART_S1_REG(UARTx[UART0]) & UART_S1_TDRE_MASK));
//    UART_D_REG(UARTx[UART0]) = 0xee;
//    while(!(UART_S1_REG(UARTx[UART0]) & UART_S1_TDRE_MASK));
//    UART_D_REG(UARTx[UART0]) = 0xee;
//    
//    while(!(UART_S1_REG(UARTx[UART0]) & UART_S1_TDRE_MASK));
//    UART_D_REG(UARTx[UART0]) = *leftblackedge;
//    while(!(UART_S1_REG(UARTx[UART0]) & UART_S1_TDRE_MASK));
//    UART_D_REG(UARTx[UART0]) = *rightblackedge;    
//    while(!(UART_S1_REG(UARTx[UART0]) & UART_S1_TDRE_MASK));
//    UART_D_REG(UARTx[UART0]) = *centre_black;
//    
//    while(!(UART_S1_REG(UARTx[UART0]) & UART_S1_TDRE_MASK));
//    UART_D_REG(UARTx[UART0]) = 0xdd;
//    while(!(UART_S1_REG(UARTx[UART0]) & UART_S1_TDRE_MASK));
//    UART_D_REG(UARTx[UART0]) = 0xdd;   
//    
}

///**************************************************************
//*
//*               线性CCD信号黑线识别函数
//*
//*     函数名称：CCD_Black
//*    功能说明：黑线识别函数
//*    参数说明：
//*    函数返回:
//*    修改时间：2013-03-15
//*    备    注：
//*************************************************************/  
//void CCD_Black(uint8 *ImageData,uint8 *rightblackedge,uint8 *leftblackedge,int8 *centre_black)
//{
//  static int8 Centre_BlackOld;
//  uint8 Threshold_Value,Threshold_Value1=0,Threshold_Value2=255;
//  u8 Binaryzation[128];
//  
//  for(uint8 i=0;i<128;i++)
//  {
//   if(ImageData[i]>Threshold_Value1)
//     Threshold_Value1=ImageData[i];
//   if(ImageData[i]<Threshold_Value2)
//     Threshold_Value2=ImageData[i];  
//  }
//  
//  
//  Threshold_Value=(uint8)((Threshold_Value1+Threshold_Value2)/2.0);
//
//  for(int i=0;i<128;i++)  
//  {    
//    if(ImageData[i]>Threshold_Value)
//    {
//      Binaryzation[i]=0xff;
//    } 
//    else
//    {
//      Binaryzation[i]=0;
//    }    
//  }
//  
//  for(int i=Centre_BlackOld;i>3;i--)  //去掉前3个点
//  {
//    
//    if((!Binaryzation[i-1])&&(Binaryzation[i])&&(Binaryzation[i+1]))
//    {
//      *leftblackedge=i;break;
//    } 
//    else
//      *leftblackedge=3;
//  }
//  
//  for(int i=Centre_BlackOld;i<125;i++)  //去掉后3个点
//  {
//    if((Binaryzation[i-1])&&(Binaryzation[i])&&(!Binaryzation[i+1]))
//    {
//      *rightblackedge=i-1;break;
//    }
//    else
//      *rightblackedge=124;
//  }
//  
//  *centre_black=(uint8)((*leftblackedge+*rightblackedge)/2);
//  Centre_BlackOld = *centre_black;
//  
//  if(*centre_black<20)*centre_black=20;
//  if(*centre_black>107)*centre_black=107;
//  
//}