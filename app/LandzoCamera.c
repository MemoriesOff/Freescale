#include "common.h"
#include "include.h"
#include  "Landzocamera.h"
#include "calculation.h"


/*********************************************************** 
* 函数名称：Camera_init
* 函数功能：对摄像头进行初始化
* 入口参数：
* 出口参数：无 
* 备 注： 
***********************************************************/
extern uint8  BUFF[500] ;
void Camera_init(void)
{
     gpio_Interrupt_init(PORTD,14, GPI_UP,FALLING) ;          //场中断
     gpio_Interrupt_init(PORTD,13, GPI_DOWN, RING) ;          //行中断 
     DMA_PORTx2BUFF_Init (DMA_CH4, (void *)&PTE_BYTE0_IN, BUFF, PTD12,
                          DMA_BYTE1, DATACOUNT, DMA_rising_down); //初始化DMA模块    
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

void Camera_Black1(uint8 *point1,uint8 *rightblackedge,uint8 *leftblackedge,uint8 *centre_black)
{
  
  uint8 point[150]={0};
  int16 temp0,temp1;  
  int16 temp_value1,temp_value2;
  uint16 leftedge,rightedge;
  uint16 leftavg,rightavg;
  uint8 leftflag,rightflag;
  uint8 leftcount,rightcount;
  uint8 LINEWITH, LEFTLINEFAV, RINGTLINFAV, FAV;
  static uint8 centre_lastblack =71; 
  static uint8 zuoflag=0;
  static uint8 youflag=0;
  
  LINEWITH=4;       //黑线宽度 
  LEFTLINEFAV=100;  //左下降沿两点电压变化值最小值
  RINGTLINFAV=100;  //右下降沿两点电压变化值最小值
  FAV=100;          //二值化阈值 
  leftcount=0;     //左黑线宽度计数值
  rightcount=0;    //右黑线宽度计数值 
  leftedge=0;      //左边界 
  rightedge=0;     //右边界
  leftavg=0;       //计算左黑线的平均值
  rightavg=0;      //计算右黑线的平均值
  leftflag=0;      //左丢线标志位
  rightflag=0;     //右丢线标志位
  
  for(int i=0;i<140;i++)
  {
    if( *point1++ > FAV)
    {
      point[i]=0xA0;  
    }
    else
    {
      point[i]=0;
    }   
   
      
  }
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
   //     testarry[temp0]=temp_value1;
      }
      else
      {
        leftcount=0;   
   //     testarry[temp0]=0;
      }           
    }    
    
    if(leftcount >= LINEWITH-1 )                        //判断是否满足边沿条件
    {
      leftedge = temp0 ;                                  //满足条件边沿找到
      leftflag=0;                                         //找到边沿，标志位置1 
      break;
    }
    else
    {
      leftcount=0;                                        //未找到清除计数  
      leftflag=1;                                         //标志位清除
      leftedge=5;                                         // 边沿赋值 
    }
     
    if(leftedge<5) leftedge = 5;   
  } 
  /*左边沿查找结束*/
  
  /*查找右边沿开始，找到边沿后跳出循环*/
  for(temp0=centre_lastblack;temp0< 135;temp0++)          //右边沿查找方式和查找左边沿一样
  {    
    for(temp1=temp0; temp1< ( temp0+LINEWITH );temp1++)
    {
      if(temp1>=135) break;
      
      temp_value2 = point[temp0]-point[temp1];
      
      if(temp_value2 >=RINGTLINFAV)
      {      
        rightcount++;
      }
      else
      {
        rightcount=0;
      }          
      
    }
    
    if( rightcount >= LINEWITH-1 )
    {
      rightedge = temp0;
      rightflag=0;
      break;
    }
    else
    {
      rightcount=0;  
      rightflag=1;
      rightedge=135;
    }
    
    if( rightedge > 135) rightedge=135;
    
  }    
  /*右边沿查找结束*/
  
  
  /*判断已找到边沿的可靠性*/
  
  /*左边沿的可靠性判断*/
  if(leftflag==0)
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
      if(temp0>=135)break;
      
      temp_value1 = point[temp0-1]-leftavg;                  //白点与黑线平均值作比较
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
  if(rightflag==0)                      // 右边界判断可靠性，和左边界判断方式一样                  
  {
    rightcount=0;
    for(temp0=rightedge;temp0<( rightedge+LINEWITH );temp0++)
    {
      if(temp0>=135)break;
      rightcount++;
      rightavg+=point[temp0-1];
    }
    
    rightavg=rightavg/rightcount;
    
    rightcount=0;
    for(temp0=rightedge;temp0>( rightedge-LINEWITH );temp0--)
    {
      if(temp0<=0)break;
      
      temp_value2 = point[temp0] - rightavg;
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
      *rightblackedge=135;
      rightcount=0;
      rightflag=0;
    }
    
  }
  else
  {
    *rightblackedge=135;
  }
  
  /*右边界可靠性判断结束*/
  

  if( ( leftflag==0 )&&( rightflag==0 ) )
  {
  *centre_black = (uint8)(( leftedge + rightedge )/2);
  centre_lastblack = *centre_black;
  }
  else if( ( leftflag==0 )&&( rightflag==1 ) )
  {
   *centre_black = (uint8)(( leftedge + rightedge )/2);
   centre_lastblack = *centre_black;

  }
  else if( ( leftflag==1 )&&( rightflag==0 ) )
  {
   *centre_black = (uint8)(( leftedge + rightedge )/2);
   centre_lastblack = *centre_black;
  }
  else if( ( leftflag==1 )&&( rightflag==1 ) )
  {
   *centre_black= centre_lastblack;
  }   
  
   *leftblackedge=leftedge;
  *rightblackedge=rightedge;
  
}

                          

void Camera_Black2(uint8 *point1,uint8 *rightblackedge,uint8 *leftblackedge,uint8 *centre_black)
{
  
  uint8 point[150]={0};
  int16 temp0,temp1;  
  int16 temp_value1,temp_value2;
  uint16 leftedge,rightedge;
  uint16 leftavg,rightavg;
  uint8 leftflag,rightflag;
  uint8 leftcount,rightcount;
  uint8 LINEWITH, LEFTLINEFAV, RINGTLINFAV, FAV;
  static uint8 centre_lastblack =71; 
  static uint8 zuoflag=0;
  static uint8 youflag=0;
  
  LINEWITH=4;       //黑线宽度 
  LEFTLINEFAV=100;  //左下降沿两点电压变化值最小值
  RINGTLINFAV=100;  //右下降沿两点电压变化值最小值
  FAV=100;          //二值化阈值 
  leftcount=0;     //左黑线宽度计数值
  rightcount=0;    //右黑线宽度计数值 
  leftedge=0;      //左边界 
  rightedge=0;     //右边界
  leftavg=0;       //计算左黑线的平均值
  rightavg=0;      //计算右黑线的平均值
  leftflag=0;      //左丢线标志位
  rightflag=0;     //右丢线标志位
  
  for(int i=0;i<140;i++)
  {
    if( *point1++ > FAV)
    {
      point[i]=0xA0;  
    }
    else
    {
      point[i]=0;
    }   
   
      
  }
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
   //     testarry[temp0]=temp_value1;
      }
      else
      {
        leftcount=0;   
   //     testarry[temp0]=0;
      }           
    }    
    
    if(leftcount >= LINEWITH-1 )                        //判断是否满足边沿条件
    {
      leftedge = temp0 ;                                  //满足条件边沿找到
      leftflag=0;                                         //找到边沿，标志位置1 
      break;
    }
    else
    {
      leftcount=0;                                        //未找到清除计数  
      leftflag=1;                                         //标志位清除
      leftedge=5;                                         // 边沿赋值 
    }
     
    if(leftedge<5) leftedge = 5;   
  } 
  /*左边沿查找结束*/
  
  /*查找右边沿开始，找到边沿后跳出循环*/
  for(temp0=centre_lastblack;temp0< 135;temp0++)          //右边沿查找方式和查找左边沿一样
  {    
    for(temp1=temp0; temp1< ( temp0+LINEWITH );temp1++)
    {
      if(temp1>=135) break;
      
      temp_value2 = point[temp0]-point[temp1];
      
      if(temp_value2 >=RINGTLINFAV)
      {      
        rightcount++;
      }
      else
      {
        rightcount=0;
      }          
      
    }
    
    if( rightcount >= LINEWITH-1 )
    {
      rightedge = temp0;
      rightflag=0;
      break;
    }
    else
    {
      rightcount=0;  
      rightflag=1;
      rightedge=135;
    }
    
    if( rightedge > 135) rightedge=135;
    
  }    
  /*右边沿查找结束*/
  
  
  /*判断已找到边沿的可靠性*/
  
  /*左边沿的可靠性判断*/
  if(leftflag==0)
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
      if(temp0>=135)break;
      
      temp_value1 = point[temp0-1]-leftavg;                  //白点与黑线平均值作比较
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
  if(rightflag==0)                      // 右边界判断可靠性，和左边界判断方式一样                  
  {
    rightcount=0;
    for(temp0=rightedge;temp0<( rightedge+LINEWITH );temp0++)
    {
      if(temp0>=135)break;
      rightcount++;
      rightavg+=point[temp0-1];
    }
    
    rightavg=rightavg/rightcount;
    
    rightcount=0;
    for(temp0=rightedge;temp0>( rightedge-LINEWITH );temp0--)
    {
      if(temp0<=0)break;
      
      temp_value2 = point[temp0] - rightavg;
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
      *rightblackedge=135;
      rightcount=0;
      rightflag=0;
    }
    
  }
  else
  {
    *rightblackedge=135;
  }
  
  /*右边界可靠性判断结束*/

  if( ( leftflag==0 )&&( rightflag==0 ) )
  {
  *centre_black = (uint8)(( leftedge + rightedge )/2);
  centre_lastblack = *centre_black;
  zuoflag=0;
  youflag=0;
  }
  else if( ( leftflag==0 )&&( rightflag==1 ) )
  {
   *centre_black = (uint8)(( leftedge + rightedge )/2);
   centre_lastblack = *centre_black;
   youflag=1;
  }
  else if( ( leftflag==1 )&&( rightflag==0 ) )
  {
   *centre_black = (uint8)(( leftedge + rightedge )/2);
   centre_lastblack = *centre_black;
   zuoflag=1;
  }
  else if( ( leftflag==1 )&&( rightflag==1 ) )
  {
   *centre_black= centre_lastblack;
  }   
  
   *leftblackedge=leftedge;
  *rightblackedge=rightedge;
  
}



void Camera_Black3(uint8 *point1,uint8 *rightblackedge,uint8 *leftblackedge,uint8 *centre_black)
{
  
  uint8 point[150]={0};
  int16 temp0,temp1;  
  int16 temp_value1,temp_value2;
  uint16 leftedge,rightedge;
  uint16 leftavg,rightavg;
  uint8 leftflag,rightflag;
  uint8 leftcount,rightcount;
  uint8 LINEWITH, LEFTLINEFAV, RINGTLINFAV, FAV;
  static uint8 centre_lastblack =71; 
  static uint8 zuoflag=0;
  static uint8 youflag=0;
  
  LINEWITH=4;       //黑线宽度 
  LEFTLINEFAV=100;  //左下降沿两点电压变化值最小值
  RINGTLINFAV=100;  //右下降沿两点电压变化值最小值
  FAV=100;          //二值化阈值 
  leftcount=0;     //左黑线宽度计数值
  rightcount=0;    //右黑线宽度计数值 
  leftedge=0;      //左边界 
  rightedge=0;     //右边界
  leftavg=0;       //计算左黑线的平均值
  rightavg=0;      //计算右黑线的平均值
  leftflag=0;      //左丢线标志位
  rightflag=0;     //右丢线标志位
  
  for(int i=0;i<140;i++)
  {
    if( *point1++ > FAV)
    {
      point[i]=0xA0;  
    }
    else
    {
      point[i]=0;
    }   
   
      
  }
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
   //     testarry[temp0]=temp_value1;
      }
      else
      {
        leftcount=0;   
   //     testarry[temp0]=0;
      }           
    }    
    
    if(leftcount >= LINEWITH-1 )                        //判断是否满足边沿条件
    {
      leftedge = temp0 ;                                  //满足条件边沿找到
      leftflag=0;                                         //找到边沿，标志位置1 
      break;
    }
    else
    {
      leftcount=0;                                        //未找到清除计数  
      leftflag=1;                                         //标志位清除
      leftedge=5;                                         // 边沿赋值 
    }
     
    if(leftedge<5) leftedge = 5;   
  } 
  /*左边沿查找结束*/
  
  /*查找右边沿开始，找到边沿后跳出循环*/
  for(temp0=centre_lastblack;temp0< 135;temp0++)          //右边沿查找方式和查找左边沿一样
  {    
    for(temp1=temp0; temp1< ( temp0+LINEWITH );temp1++)
    {
      if(temp1>=135) break;
      
      temp_value2 = point[temp0]-point[temp1];
      
      if(temp_value2 >=RINGTLINFAV)
      {      
        rightcount++;
      }
      else
      {
        rightcount=0;
      }          
      
    }
    
    if( rightcount >= LINEWITH-1 )
    {
      rightedge = temp0;
      rightflag=0;
      break;
    }
    else
    {
      rightcount=0;  
      rightflag=1;
      rightedge=135;
    }
    
    if( rightedge > 135) rightedge=135;
    
  }    
  /*右边沿查找结束*/
  
  
  /*判断已找到边沿的可靠性*/
  
  /*左边沿的可靠性判断*/
  if(leftflag==0)
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
      if(temp0>=135)break;
      
      temp_value1 = point[temp0-1]-leftavg;                  //白点与黑线平均值作比较
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
  if(rightflag==0)                      // 右边界判断可靠性，和左边界判断方式一样                  
  {
    rightcount=0;
    for(temp0=rightedge;temp0<( rightedge+LINEWITH );temp0++)
    {
      if(temp0>=135)break;
      rightcount++;
      rightavg+=point[temp0-1];
    }
    
    rightavg=rightavg/rightcount;
    
    rightcount=0;
    for(temp0=rightedge;temp0>( rightedge-LINEWITH );temp0--)
    {
      if(temp0<=0)break;
      
      temp_value2 = point[temp0] - rightavg;
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
      *rightblackedge=135;
      rightcount=0;
      rightflag=0;
    }
    
  }
  else
  {
    *rightblackedge=135;
  }
  
  /*右边界可靠性判断结束*/
  

  if( ( leftflag==0 )&&( rightflag==0 ) )
  {
  *centre_black = (uint8)(( leftedge + rightedge )/2);
  centre_lastblack = *centre_black;
  zuoflag=0;
  youflag=0;
  }
  else if( ( leftflag==0 )&&( rightflag==1 ) )
  {
   *centre_black = (uint8)(( leftedge + rightedge )/2);
   centre_lastblack = *centre_black;
   youflag=1;
  }
  else if( ( leftflag==1 )&&( rightflag==0 ) )
  {
   *centre_black = (uint8)(( leftedge + rightedge )/2);
   centre_lastblack = *centre_black;
   zuoflag=1;
  }
  else if( ( leftflag==1 )&&( rightflag==1 ) )
  {
   *centre_black= centre_lastblack;
  }   
  
   *leftblackedge=leftedge;
  *rightblackedge=rightedge;
  
}



void Camera_Black4(uint8 *point1,uint8 *rightblackedge,uint8 *leftblackedge,uint8 *centre_black)
{
  
  uint8 point[150]={0};
  int16 temp0,temp1;  
  int16 temp_value1,temp_value2;
  uint16 leftedge,rightedge;
  uint16 leftavg,rightavg;
  uint8 leftflag,rightflag;
  uint8 leftcount,rightcount;
  uint8 LINEWITH, LEFTLINEFAV, RINGTLINFAV, FAV;
  static uint8 centre_lastblack =71; 
  static uint8 zuoflag=0;
  static uint8 youflag=0;
  
  LINEWITH=4;       //黑线宽度 
  LEFTLINEFAV=100;  //左下降沿两点电压变化值最小值
  RINGTLINFAV=100;  //右下降沿两点电压变化值最小值
  FAV=100;          //二值化阈值 
  leftcount=0;     //左黑线宽度计数值
  rightcount=0;    //右黑线宽度计数值 
  leftedge=0;      //左边界 
  rightedge=0;     //右边界
  leftavg=0;       //计算左黑线的平均值
  rightavg=0;      //计算右黑线的平均值
  leftflag=0;      //左丢线标志位
  rightflag=0;     //右丢线标志位
  
  for(int i=0;i<140;i++)
  {
    if( *point1++ > FAV)
    {
      point[i]=0xA0;  
    }
    else
    {
      point[i]=0;
    }   
   
      
  }
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
   //     testarry[temp0]=temp_value1;
      }
      else
      {
        leftcount=0;   
   //     testarry[temp0]=0;
      }           
    }    
    
    if(leftcount >= LINEWITH-1 )                        //判断是否满足边沿条件
    {
      leftedge = temp0 ;                                  //满足条件边沿找到
      leftflag=0;                                         //找到边沿，标志位置1 
      break;
    }
    else
    {
      leftcount=0;                                        //未找到清除计数  
      leftflag=1;                                         //标志位清除
      leftedge=5;                                         // 边沿赋值 
    }
     
    if(leftedge<5) leftedge = 5;   
  } 
  /*左边沿查找结束*/
  
  /*查找右边沿开始，找到边沿后跳出循环*/
  for(temp0=centre_lastblack;temp0< 135;temp0++)          //右边沿查找方式和查找左边沿一样
  {    
    for(temp1=temp0; temp1< ( temp0+LINEWITH );temp1++)
    {
      if(temp1>=135) break;
      
      temp_value2 = point[temp0]-point[temp1];
      
      if(temp_value2 >=RINGTLINFAV)
      {      
        rightcount++;
      }
      else
      {
        rightcount=0;
      }          
      
    }
    
    if( rightcount >= LINEWITH-1 )
    {
      rightedge = temp0;
      rightflag=0;
      break;
    }
    else
    {
      rightcount=0;  
      rightflag=1;
      rightedge=135;
    }
    
    if( rightedge > 135) rightedge=135;
    
  }    
  /*右边沿查找结束*/
  
  
  /*判断已找到边沿的可靠性*/
  
  /*左边沿的可靠性判断*/
  if(leftflag==0)
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
      if(temp0>=135)break;
      
      temp_value1 = point[temp0-1]-leftavg;                  //白点与黑线平均值作比较
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
  if(rightflag==0)                      // 右边界判断可靠性，和左边界判断方式一样                  
  {
    rightcount=0;
    for(temp0=rightedge;temp0<( rightedge+LINEWITH );temp0++)
    {
      if(temp0>=135)break;
      rightcount++;
      rightavg+=point[temp0-1];
    }
    
    rightavg=rightavg/rightcount;
    
    rightcount=0;
    for(temp0=rightedge;temp0>( rightedge-LINEWITH );temp0--)
    {
      if(temp0<=0)break;
      
      temp_value2 = point[temp0] - rightavg;
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
      *rightblackedge=135;
      rightcount=0;
      rightflag=0;
    }
    
  }
  else
  {
    *rightblackedge=135;
  }
  
  /*右边界可靠性判断结束*/
  

  if( ( leftflag==0 )&&( rightflag==0 ) )
  {
  *centre_black = (uint8)(( leftedge + rightedge )/2);
  centre_lastblack = *centre_black;
  zuoflag=0;
  youflag=0;
  }
  else if( ( leftflag==0 )&&( rightflag==1 ) )
  {
   *centre_black = (uint8)(( leftedge + rightedge )/2);
   centre_lastblack = *centre_black;
   youflag=1;
  }
  else if( ( leftflag==1 )&&( rightflag==0 ) )
  {
   *centre_black = (uint8)(( leftedge + rightedge )/2);
   centre_lastblack = *centre_black;
   zuoflag=1;
  }
  else if( ( leftflag==1 )&&( rightflag==1 ) )
  {
   *centre_black= centre_lastblack;
  }   
  
   *leftblackedge=leftedge;
  *rightblackedge=rightedge;
  
}



void Camera_Black5(uint8 *point1,uint8 *rightblackedge,uint8 *leftblackedge,uint8 *centre_black)
{
  
  uint8 point[150]={0};
  int16 temp0,temp1;  
  int16 temp_value1,temp_value2;
  uint16 leftedge,rightedge;
  uint16 leftavg,rightavg;
  uint8 leftflag,rightflag;
  uint8 leftcount,rightcount;
  uint8 LINEWITH, LEFTLINEFAV, RINGTLINFAV, FAV;
  static uint8 centre_lastblack =71; 
  static uint8 zuoflag=0;
  static uint8 youflag=0;
  
  LINEWITH=4;       //黑线宽度 
  LEFTLINEFAV=100;  //左下降沿两点电压变化值最小值
  RINGTLINFAV=100;  //右下降沿两点电压变化值最小值
  FAV=100;          //二值化阈值 
  leftcount=0;     //左黑线宽度计数值
  rightcount=0;    //右黑线宽度计数值 
  leftedge=0;      //左边界 
  rightedge=0;     //右边界
  leftavg=0;       //计算左黑线的平均值
  rightavg=0;      //计算右黑线的平均值
  leftflag=0;      //左丢线标志位
  rightflag=0;     //右丢线标志位
  
  for(int i=0;i<140;i++)
  {
    if( *point1++ > FAV)
    {
      point[i]=0xA0;  
    }
    else
    {
      point[i]=0;
    }   
   
      
  }
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
   //     testarry[temp0]=temp_value1;
      }
      else
      {
        leftcount=0;   
   //     testarry[temp0]=0;
      }           
    }    
    
    if(leftcount >= LINEWITH-1 )                        //判断是否满足边沿条件
    {
      leftedge = temp0 ;                                  //满足条件边沿找到
      leftflag=0;                                         //找到边沿，标志位置1 
      break;
    }
    else
    {
      leftcount=0;                                        //未找到清除计数  
      leftflag=1;                                         //标志位清除
      leftedge=5;                                         // 边沿赋值 
    }
     
    if(leftedge<5) leftedge = 5;   
  } 
  /*左边沿查找结束*/
  
  /*查找右边沿开始，找到边沿后跳出循环*/
  for(temp0=centre_lastblack;temp0< 135;temp0++)          //右边沿查找方式和查找左边沿一样
  {    
    for(temp1=temp0; temp1< ( temp0+LINEWITH );temp1++)
    {
      if(temp1>=135) break;
      
      temp_value2 = point[temp0]-point[temp1];
      
      if(temp_value2 >=RINGTLINFAV)
      {      
        rightcount++;
      }
      else
      {
        rightcount=0;
      }          
      
    }
    
    if( rightcount >= LINEWITH-1 )
    {
      rightedge = temp0;
      rightflag=0;
      break;
    }
    else
    {
      rightcount=0;  
      rightflag=1;
      rightedge=135;
    }
    
    if( rightedge > 135) rightedge=135;
    
  }    
  /*右边沿查找结束*/
  
  
  /*判断已找到边沿的可靠性*/
  
  /*左边沿的可靠性判断*/
  if(leftflag==0)
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
      if(temp0>=135)break;
      
      temp_value1 = point[temp0-1]-leftavg;                  //白点与黑线平均值作比较
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
  if(rightflag==0)                      // 右边界判断可靠性，和左边界判断方式一样                  
  {
    rightcount=0;
    for(temp0=rightedge;temp0<( rightedge+LINEWITH );temp0++)
    {
      if(temp0>=135)break;
      rightcount++;
      rightavg+=point[temp0-1];
    }
    
    rightavg=rightavg/rightcount;
    
    rightcount=0;
    for(temp0=rightedge;temp0>( rightedge-LINEWITH );temp0--)
    {
      if(temp0<=0)break;
      
      temp_value2 = point[temp0] - rightavg;
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
      *rightblackedge=135;
      rightcount=0;
      rightflag=0;
    }
    
  }
  else
  {
    *rightblackedge=135;
  }
  
  /*右边界可靠性判断结束*/
  

  if( ( leftflag==0 )&&( rightflag==0 ) )
  {
  *centre_black = (uint8)(( leftedge + rightedge )/2);
  centre_lastblack = *centre_black;
  zuoflag=0;
  youflag=0;
  }
  else if( ( leftflag==0 )&&( rightflag==1 ) )
  {
   *centre_black = (uint8)(( leftedge + rightedge )/2);
   centre_lastblack = *centre_black;
   youflag=1;
  }
  else if( ( leftflag==1 )&&( rightflag==0 ) )
  {
   *centre_black = (uint8)(( leftedge + rightedge )/2);
   centre_lastblack = *centre_black;
   zuoflag=1;
  }
  else if( ( leftflag==1 )&&( rightflag==1 ) )
  {
   *centre_black= centre_lastblack;
  }   
  
   *leftblackedge=leftedge;
  *rightblackedge=rightedge;
  
}


void Camera_Black6(uint8 *point1,uint8 *rightblackedge,uint8 *leftblackedge,uint8 *centre_black)
{
  
  uint8 point[150]={0};
  int16 temp0,temp1;  
  int16 temp_value1,temp_value2;
  uint16 leftedge,rightedge;
  uint16 leftavg,rightavg;
  uint8 leftflag,rightflag;
  uint8 leftcount,rightcount;
  uint8 LINEWITH, LEFTLINEFAV, RINGTLINFAV, FAV;
  static uint8 centre_lastblack =71; 
  static uint8 zuoflag=0;
  static uint8 youflag=0;
  
  LINEWITH=4;       //黑线宽度 
  LEFTLINEFAV=100;  //左下降沿两点电压变化值最小值
  RINGTLINFAV=100;  //右下降沿两点电压变化值最小值
  FAV=100;          //二值化阈值 
  leftcount=0;     //左黑线宽度计数值
  rightcount=0;    //右黑线宽度计数值 
  leftedge=0;      //左边界 
  rightedge=0;     //右边界
  leftavg=0;       //计算左黑线的平均值
  rightavg=0;      //计算右黑线的平均值
  leftflag=0;      //左丢线标志位
  rightflag=0;     //右丢线标志位
  
  for(int i=0;i<140;i++)
  {
    if( *point1++ > FAV)
    {
      point[i]=0xA0;  
    }
    else
    {
      point[i]=0;
    }   
   
      
  }
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
   //     testarry[temp0]=temp_value1;
      }
      else
      {
        leftcount=0;   
   //     testarry[temp0]=0;
      }           
    }    
    
    if(leftcount >= LINEWITH-1 )                        //判断是否满足边沿条件
    {
      leftedge = temp0 ;                                  //满足条件边沿找到
      leftflag=0;                                         //找到边沿，标志位置1 
      break;
    }
    else
    {
      leftcount=0;                                        //未找到清除计数  
      leftflag=1;                                         //标志位清除
      leftedge=5;                                         // 边沿赋值 
    }
     
    if(leftedge<5) leftedge = 5;   
  } 
  /*左边沿查找结束*/
  
  /*查找右边沿开始，找到边沿后跳出循环*/
  for(temp0=centre_lastblack;temp0< 135;temp0++)          //右边沿查找方式和查找左边沿一样
  {    
    for(temp1=temp0; temp1< ( temp0+LINEWITH );temp1++)
    {
      if(temp1>=135) break;
      
      temp_value2 = point[temp0]-point[temp1];
      
      if(temp_value2 >=RINGTLINFAV)
      {      
        rightcount++;
      }
      else
      {
        rightcount=0;
      }          
      
    }
    
    if( rightcount >= LINEWITH-1 )
    {
      rightedge = temp0;
      rightflag=0;
      break;
    }
    else
    {
      rightcount=0;  
      rightflag=1;
      rightedge=135;
    }
    
    if( rightedge > 135) rightedge=135;
    
  }    
  /*右边沿查找结束*/
  
  
  /*判断已找到边沿的可靠性*/
  
  /*左边沿的可靠性判断*/
  if(leftflag==0)
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
      if(temp0>=135)break;
      
      temp_value1 = point[temp0-1]-leftavg;                  //白点与黑线平均值作比较
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
  if(rightflag==0)                      // 右边界判断可靠性，和左边界判断方式一样                  
  {
    rightcount=0;
    for(temp0=rightedge;temp0<( rightedge+LINEWITH );temp0++)
    {
      if(temp0>=135)break;
      rightcount++;
      rightavg+=point[temp0-1];
    }
    
    rightavg=rightavg/rightcount;
    
    rightcount=0;
    for(temp0=rightedge;temp0>( rightedge-LINEWITH );temp0--)
    {
      if(temp0<=0)break;
      
      temp_value2 = point[temp0] - rightavg;
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
      *rightblackedge=135;
      rightcount=0;
      rightflag=0;
    }
    
  }
  else
  {
    *rightblackedge=135;
  }
  
  /*右边界可靠性判断结束*/
  

  if( ( leftflag==0 )&&( rightflag==0 ) )
  {
  *centre_black = (uint8)(( leftedge + rightedge )/2);
  centre_lastblack = *centre_black;
  zuoflag=0;
  youflag=0;
  }
  else if( ( leftflag==0 )&&( rightflag==1 ) )
  {
   *centre_black = (uint8)(( leftedge + rightedge )/2);
   centre_lastblack = *centre_black;
   youflag=1;
  }
  else if( ( leftflag==1 )&&( rightflag==0 ) )
  {
   *centre_black = (uint8)(( leftedge + rightedge )/2);
   centre_lastblack = *centre_black;
   zuoflag=1;
  }
  else if( ( leftflag==1 )&&( rightflag==1 ) )
  {
   *centre_black= centre_lastblack;
  }   
  
   *leftblackedge=leftedge;
  *rightblackedge=rightedge;
  
}
                          