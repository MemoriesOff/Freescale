#include "common.h"
#include "include.h"
#include "calculation.h"


/*********************************************************** 
函数名称：MOTORPWM_init
函数功能：
入口参数：
出口参数：无 
备 注： 
***********************************************************/
void  MOTORPWM_init(void)
{
 //   lptmr_counter_init(LPT0_ALT1, LIN_COUT, 2, LPT_Rising)  ;   //初始化脉冲计数器 ，PTC5输入捕获脚，LIN_cout =100，预处理为2，上升沿捕获 
    FTM_PWM_init(FTM0 , CH0, 80000,0);                          //电机占空比设置初始化    占空比 = duty /(MOD +1 ) ;FTM_CnV_REG(FTMx[ftmn], ch) = cv;
    FTM_PWM_init(FTM0 , CH1, 80000,0);                          //电机占空比设置初始化    占空比 = duty /(MOD +1 ) ;FTM_CnV_REG(FTMx[ftmn], ch) = cv;
    FTM_PWM_init(FTM0 , CH2, 80000,0);                          //电机占空比设置初始化    占空比 = duty /(MOD +1 ) ;FTM_CnV_REG(FTMx[ftmn], ch) = cv;
    FTM_PWM_init(FTM0 , CH3, 80000,0);                          //电机占空比设置初始化    占空比 = duty /(MOD +1 ) ;FTM_CnV_REG(FTMx[ftmn], ch) = cv;
}


/*********************************************************** 
函数名称：TURNPWM_init
函数功能：
入口参数：
出口参数：无 
备 注： 
***********************************************************/
void  TURNPWM_init(void)
{
      
     // FTM_PWM_init(FTM1, CH0 , 180,30);                           //舵机占空比设置初始化   MOD =17361 ;舵机1
      FTM_PWM_init(FTM1, CH1 , 100,30);                           //舵机占空比设置初始化   MOD =17361 ;舵机2
      FTM_CnV_REG(FTMx[FTM1], CH1) = MIDSTRING ; 
}
/*********************************************************** 
函数名称：TurnPWM
函数功能：根据转向值和角度传感器计算转向值
入口参数：TurnPosition          计算黑线位置
          TurnMidPosition       黑线中点值

出口参数：无 
备 注： 
***********************************************************/
/*int16 TurnPWM(uint8 TurnPosition, uint8 TurnMidPosition)
{   
    s16 direction;
    s16 TurnPWM ;
    int TurnPWM1;
    static s16 TurnKP ,TurnKD ;
    static s16 direction0 = 60;
    direction = TurnPosition - TurnMidPosition ;
    
    if((direction > -10)&&(direction<0))      //舵机左转
    {
      TurnKP = 10 ;
      TurnKD = 0 ;
    }
    else if((direction<-10)&&(direction > -15))
    { 
      TurnKP = 15 ;
      TurnKD = 0 ;
    }
    if((direction>0)&&(direction < 10))      //舵机右转
    { 
      TurnKP = 40 ;
      TurnKD = 0 ;
    }
    else if((direction>10)&&(direction <15))
    { 
      TurnKP = 45 ;
      TurnKD = 0 ;
    }
    else
    {
     TurnKP = 40 ;
     TurnKD = 0 ;
    }
    
    
    TurnPWM =  TurnKP * direction + TurnKD * (direction - direction0);
    if(TurnPWM > TURNPWMMAX) {TurnPWM = TURNPWMMAX ;}
    else if (TurnPWM < TURNPWMMIN ) {TurnPWM = TURNPWMMIN ;}
    
    direction0 = direction ;
    TurnPWM1=-TurnPWM;
    TurnPWM1+= MIDSTRING ;
    
    return TurnPWM1;
} */
int TurnPWM(uint8 TurnPosition,uint8 TurnMidPosition,int turnKP,int turnKI,int turnKD){
    int Speed_P_Value,Speed_D_Value;
    static int Speed1_Err,SumErrSpeed;  //静态变量存储中间变量
    int Speed2_Err,Speed_EC;
    int  SpeedPWMOUT;
    Speed2_Err = Speed1_Err ;                //将上一次的偏差保存
  
    Speed1_Err = TurnMidPosition - TurnPosition;      //  计算新的偏差值
  
    Speed_EC = Speed1_Err - Speed2_Err;      //  计算新的偏差变化值 
   
    Speed_P_Value =  Speed1_Err * turnKP ;   //  增量式PID控制计算P调节量
  
    SumErrSpeed  +=  Speed1_Err * turnKI ;    //增量式PID控制计算I调节量

    Speed_D_Value =  Speed_EC   * turnKD;     //  增量式PID控制计算D调节量
  
    SpeedPWMOUT = Speed_P_Value + SumErrSpeed + Speed_D_Value;
    return  SpeedPWMOUT ;  
}

/*
** ===================================================================
** SpeedPID
   输入：speedCount采集车速，AmSpeed 目标车速  ；  
   输出 ：SpeedPWMOUT  计算车速 ；
** ===================================================================
*/

int16 SpeedControl(int16 speedCount,int16 AmSpeed,uint8 speedKP,uint8 speedKI,uint8 speedKD)
{
  
  static float Speed1_Err,SumErrSpeed;  //静态变量存储中间变量
  float Speed2_Err,Speed_EC;
  float Speed_P_Value,Speed_D_Value ;
  
  long  SpeedPWMOUT;
  
  Speed2_Err = Speed1_Err ;                //将上一次的偏差保存
  
  Speed1_Err = AmSpeed - speedCount  ;      //  计算新的偏差值
  
  Speed_EC = Speed1_Err - Speed2_Err ;      //  计算新的偏差变化值 
   
  Speed_P_Value =  Speed1_Err * speedKP/10.0 ;   //  增量式PID控制计算P调节量
  
  SumErrSpeed  +=  Speed1_Err * speedKI ;    //增量式PID控制计算I调节量

  Speed_D_Value =  Speed_EC   *  speedKD/100.0 ;     //  增量式PID控制计算D调节量
  
  SpeedPWMOUT = (int16)(Speed_P_Value + SumErrSpeed + Speed_D_Value);
  if(SpeedPWMOUT < SPEED_PWM_MIN )
  {
   SpeedPWMOUT = SPEED_PWM_MIN ;
  }
  else if(SpeedPWMOUT > SPEED_PWM_MAX)
  {
    SpeedPWMOUT = SPEED_PWM_MAX ;
         
  }
//  if(SpeedPWMOUT<=0)SpeedPWMOUT=0;
          
   return  SpeedPWMOUT ;  
}







 