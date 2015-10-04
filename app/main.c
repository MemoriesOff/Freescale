/******************** (C) COPYRIGHT 2011 蓝宙电子工作室 ********************
* 文件名       ：main.c
* 描述         ：工程模版实验
*
* 实验平台     ：landzo电子开发版
* 库版本       ：
* 嵌入系统     ：
*
* 作者         ：landzo 蓝电子
* 淘宝店       ：http://landzo.taobao.com/

**********************************************************************************/

#include "include.h"
#include "calculation.h"

/*************************
设置系统的中断函数变量
*************************/
extern  uint8  LPT_INT_count;                    //LPT 脉冲计数器

extern uint8 TIME0flag_5ms;                    //PTI0 5ms标志位
extern uint8 TIME0flag_10ms;                    //PTI0 10ms标志位
extern uint8 TIME0flag_20ms;                    //PTI0 20ms标志位 
extern uint8 TIME0flag_80ms;                     //PTI0 80ms标志位   
extern uint8 TIME0flag_100ms;                     //PTI0 100ms标志位 
extern uint8 TIME0flag_200ms;                     //PTI0 100ms标志位 

extern uint16 Sample_Line_Count;                //DMA采集的行数
extern uint8  DMA_Over_Flag;                      //DMA采集完成标志位

/********
全局
********/                
/*********
速度变量
*********/
uint16 Set_Speed=0 ;                                  //目标速度
uint16 Speed_Count =0 ;                          //脉冲计数器
int SpeedPWM_Value_left=0 ;                          //PID计算的PWM 
int SpeedPWM_Value_right=0 ;                          //PID计算的PWM 
int SpeedPWM_Value_left_jiansu=0 ;                          //PID计算的PWM 
int SpeedPWM_Value_right_jiansu=0 ;                          //PID计算的PWM 
int test;
int num=0;
uint8 Speed_P;
uint8 Speed_I;
uint8 Speed_D;

/*********
转向变量
*********/
uint8  Turn_P=0;
uint8  Turn_D=0;
uint8  Turn_i=0;
uint16 TurnPWM_Value=4800 ;                        //转向的PWM值 
int16  Sum_Station=0;
char  Station=71 ;                         //CCD的黑线位置
uint8  MIDSTATION = 71 ;                 //CCD传感器表示的黑线的中间位置

/********
摄像头采集变量
********/
uint8  BUFF[500] ;
uint8  checkflag = 0 ;  
uint8  Sample_Over=0;
uint8  g_PointCount=DATACOUNT;
uint16  g_temp0=0; 
uint8  ADdata[DATAROW][DATACOUNT]={0};
uint8  *finger, *finger1; 
uint8  g_LineCalculation=0;
//uint8  Rightblackedge[DATAROW]={0};
//uint8  Leftblackedge[DATAROW]={0};
//uint8  CameraStation[DATAROW]={0};
uint8  Rightblackedge[5]={0};
uint8  Leftblackedge[5]={0};
uint8  CameraStation[5]={0};

uint8  single[2];
uint8  single_left;
uint8  singleflag=1;
uint8  lastleftedge;
uint8  lastrightedge;
uint8  lastsingleedge[3];
int shizi_flag=0;
int shizibiandan_flag=0;
int k1,k2;

int zhijiaoyanshi;
uint8 guaiwanyanshi;
int zhijiaoheixian;
uint8 zhijiaoflag;
int zhijiaozuoguai;
int zhijiaoyouguai;
uint8 left_num[5];
uint8 right_num[5];
uint8 diuxianflag;
uint8 diuxianyanshi;
int zuozhangaiflag;
int zhijiaoheixianhouyanshi;
int youzhangaiflag;

int danxianshu;
int danxianshuzuo;
int danxianshuyou;
int danxianshuzuo1;
int danxianshuyou1;
int danxianshuzuo2;
int danxianshuyou2;
int danxianstation;
int danxianshuzuo3;
int danxianshuyou3;


uint8 shizizhijiaoflag=0;

uint8 zhangaiyanshi;

uint8 zhijiaojiansu;

uint8 zhijiaoyouguaiyanshi;

int dengta=0;



void main()
{
  DisableInterrupts;                             //禁止总中断 
  /*********************************************************
  初始化全局变量
  *********************************************************/
  
  Set_Speed = 10 ;  //64
  
  Speed_I=0;
  Speed_D=10;
  
  

  
  /*********************************************************
  初始化程序
  *********************************************************/
  
  uart_init (UART0 , 9600);                      //初始化UART0，输出脚PTA15，输入脚PTA14，串口频率 115200
  gpio_init (PORTB , 17, GPO,HIGH);                //LED闪烁  
  gpio_init (PORTB, 3, GPI,HIGH);  
  
  
  gpio_init (PORTB , 11, GPO,LOW);                //系统板测试LED初始化
  gpio_init (PORTB , 13, GPO,HIGH);                //LED闪烁  
  gpio_init (PORTB, 7, GPI,HIGH);  
  gpio_init (PORTB, 9, GPI,HIGH);  
   OLED_Init();                                     //初始化OLED传感器 
  pit_init_ms(PIT0, 1);                            //初始化PIT0，定时时间为： 1ms
  Camera_init();                                    //初始化摄像头传感器    
  MOTORPWM_init();                                 //电机初始化
  TURNPWM_init() ;                                 //转向初始化  
    OLED_Print(5,0,"fuck");  //汉字字符串显示
   while(checkflag != 1 )  
  {
    checkflag = LandzoIICEEROM_INIT() ; //通过配置使摄像头输出数字信号
    BFdelay_1us(100);                   // 延时100us 
  }
  
  EnableInterrupts;			            //开总中断  
  
  /******************************************
  执行程序
  ******************************************/
  while(1)
  {
    
    
    if(gpio_get(PORTB, 7)==0)
    {
    /*********************
    5ms程序执行代码段
    *********************/
    if(TIME0flag_5ms == 1)
    { 
      TIME0flag_5ms = 0 ;
      
    }
    
    /*********************
    10ms程序执行代码段
    *********************/
    if(TIME0flag_10ms == 1)
    {
      TIME0flag_10ms = 0 ;
      //LPTMR0_CNR=1; 
/*      Speed_Count = LPTMR0_CNR;                                                     //保存脉冲计数器计算值
      lptmr_counter_clean();                                                  //清空脉冲计数器计算值（马上清空，这样才能保证计数值准确）
      Speed_Count = LPT_INT_count * LIN_COUT +  Speed_Count;                              //间隔10ms的脉冲次数
      LPT_INT_count = 0;     */        
      //清空LPT中断次数   
    }
    
    /*********************
    20ms程序执行代码段
    *********************/
    if(TIME0flag_20ms == 1)
    {
      uart_putchar(UART0,CameraStation[1]);
      TIME0flag_20ms = 0 ;
      //清除时间标志位   
      k2=gpio_get(PORTB, 3);
      if(gpio_get(PORTB, 9)==1)
      {
      if(dengta==0)
      {
         if( gpio_get(PORTB, 3)==1)
        {
           for(k1=0;k1<12;k1++)
           {
             BFdelay_1us(300);
             if(gpio_get(PORTB, 3)==0)
               break;
              if(k1%3==0)
                BFDly_ms(1);
               
          }
          if(k1==12)
          {
           
            dengta=1;
          }
        else
          dengta=0; 
        }
       }
      if(dengta==1)
      {
          if(gpio_get(PORTB, 3)==0)
          {
            dengta=0;
          }   
       }
    }
    else
    {
      if(dengta==0 )
      {
        k1++;
        if(k1>250)
          dengta=1;
      }
    }
      if(zhijiaoflag>0)
         gpio_init (PORTA , 17, GPO,LOW);
      else
         gpio_init (PORTA , 17, GPO,HIGH);   
      /******************单双判断程序**********************/
        singleflag=0;
    
        danxianshuzuo=0;
        
        for(int i=Leftblackedge[1]-5;i>Leftblackedge[1]-20;i--)
        {
          if(i<10)
            break;
          if(ADdata[15][i]>100)
            danxianshuzuo++;
        }
        danxianshuyou=0;
        for(int i=Rightblackedge[1]+5;i<Rightblackedge[1]+20;i++)
        {
          if(i>130)
            break;
          if(ADdata[15][i]>100)
            danxianshuyou++;
        }
        
         danxianshuzuo1=0;
        
        for(int i=Leftblackedge[1]-10;i>Leftblackedge[1]-40;i--)
        {
          if(i<20)
            break;
          if(ADdata[12][i]>100)
            danxianshuzuo1++;
        }
        danxianshuyou1=0;
        for(int i=Rightblackedge[1]+10;i<Rightblackedge[1]+40;i++)
        {
          if(i>120)
            break;
          if(ADdata[12][i]>100)
            danxianshuyou1++;
        }
        
            danxianshuzuo2=0;
        
        for(int i=Leftblackedge[3]-5;i>Leftblackedge[3]-20;i--)
        {
          if(i<10)
            break;
          if(ADdata[10][i]>100)
            danxianshuzuo2++;
        }
        danxianshuyou2=0;
        for(int i=Rightblackedge[3]+5;i<Rightblackedge[3]+20;i++)
        {
          if(i>130)
            break;
          if(ADdata[10][i]>100)
            danxianshuyou2++;
        }
         danxianshuzuo3=0;
        
        for(int i=Leftblackedge[3]-10;i>Leftblackedge[3]-40;i--)
        {
          if(i<20)
            break;
          if(ADdata[10][i]>100)
            danxianshuzuo3++;
        }
        danxianshuyou3=0;
        for(int i=Rightblackedge[3]+10;i<Rightblackedge[3]+40;i++)
        {
          if(i>120)
            break;
          if(ADdata[10][i]>100)
            danxianshuyou3++;
        }
        
        if(diuxianflag==0)
        {
        if((danxianshuzuo>10||danxianshuyou>10 || danxianshuzuo2>10||danxianshuyou2>10))
        {        
                  /*if(danxianshuzuo>10&&danxianshuyou<10 && Leftblackedge[1]>10 && Rightblackedge[1]<130)
                  {
                 //   Station=(Rightblackedge[1]+5)/2;
                    Station=Leftblackedge[1];
                    danxianstation=Station;
                    singleflag=2;
                  }
                  else if(danxianshuzuo<10&&danxianshuyou>10 && Leftblackedge[1]>10 && Rightblackedge[1]<130)
                  {
                //    Station=(Leftblackedge[1]+135)/2;
                     Station=Rightblackedge[1];
                    danxianstation=Station;
                    singleflag=2;
                  }*/
                  if(danxianshuzuo>10 && Leftblackedge[1]>10)
                  {
                    Station=Leftblackedge[1];
                    danxianstation=Station;
                    singleflag=1;
                  }
                  else if(danxianshuyou>10  && Rightblackedge[1]<130)
                  {
                    Station=Rightblackedge[1];
                    danxianstation=Station;
                    singleflag=1;
                  }
                 /* else if(danxianshuzuo2>10&&danxianshuyou2<10 && Leftblackedge[3]>10 && Rightblackedge[3]<130)
                  {
                 //   Station=(Rightblackedge[1]+5)/2;
                    Station=Leftblackedge[3];
                    danxianstation=Station;
                    singleflag=2;
                  }
                  else if(danxianshuzuo2<10&&danxianshuyou2>10 && Leftblackedge[3]>10 && Rightblackedge[3]<130)
                  {
                //    Station=(Leftblackedge[1]+135)/2;
                     Station=Rightblackedge[3];
                    danxianstation=Station;
                    singleflag=2;
                  }*/
                  else if(danxianshuzuo2>10 && Leftblackedge[3]>10)
                  {
                    Station=Leftblackedge[3];
                    danxianstation=Station;
                    singleflag=1;
                  }
                  else if(danxianshuyou2>10  && Rightblackedge[3]<130)
                  {
                    Station=Rightblackedge[3];
                    danxianstation=Station;
                    singleflag=1;
                  }
        }
        /***********正常取平均***********/
        else
        {
              Station=CameraStation[1];  
              singleflag=0;
         }
         if((Leftblackedge[1]<10 && Rightblackedge[1]>130))
        {
   
            Station=danxianstation;
            diuxianflag=1;
        }    

     
         /***********直角**********/

         if((zhangaiyanshi==0)&&(((left_num[0]>20)&&(right_num[0]>20))||((left_num[1]>20)&&(right_num[1]>20))||((left_num[2]>20)&&(right_num[2]>20))) &&(zhijiaoheixian==0)&&((Rightblackedge[4]<135 &&Leftblackedge[4]>5)|| (Rightblackedge[3]<135 &&Leftblackedge[3]>5) ) &&(( left_num[4]<20&& right_num[4]<20)||( left_num[3]<20&& right_num[3]<20)))    
     //   if((((left_num[0]>20)&&(right_num[0]>20))||((left_num[1]>20)&&(right_num[1]>20))||((left_num[2]>20)&&(right_num[2]>20))) &&(zhijiaoheixian==0)&&(zhangaiyanshi==0))
        {
            guaiwanyanshi=0;
            if(zhijiaoflag==0)
              zhijiaoflag=1;
            else
              zhijiaoflag=0;
            zhijiaoheixian=100;
            shizibiandan_flag=100;
            shizizhijiaoflag=10;
            zhijiaoyouguai=0;
            zhijiaozuoguai=0;
            zhijiaoheixianhouyanshi=40;
            zhijiaoyanshi=130;  
        }
        // if((zhijiaoflag==1)&&(zhijiaozuoguai==0)&&(zhijiaoyouguai==0)&&(((Leftblackedge[0]>5)&&(Rightblackedge[0]>100))||((Leftblackedge[1]>5)&&(Rightblackedge[1]>100))||((Leftblackedge[2]>5)&&(Rightblackedge[2]>100)))&&(zhijiaoheixianhouyanshi==0))
        if((zhijiaoflag==1)&&(zhijiaozuoguai==0)&&(zhijiaoyouguai==0)&&(CameraStation[0]>70 ||CameraStation[1]>70 ||CameraStation[2]>70||Rightblackedge[0]==135||Rightblackedge[1]==135||Rightblackedge[2]==135)&&(zhijiaoheixianhouyanshi==0))
            {
                 zhijiaoyouguai=25;
            }  
        // if((zhijiaoflag==1)&&(zhijiaozuoguai==0)&&(zhijiaoyouguai==0)&&(((Leftblackedge[0]<20)&&(Rightblackedge[0]<135))||((Leftblackedge[1]<20)&&(Rightblackedge[1]<135))||((Leftblackedge[2]<20)&&(Rightblackedge[2]<135)))&&(zhijiaoheixianhouyanshi==0))
        if((zhijiaoflag==1)&&(zhijiaoyouguai==0)&&(zhijiaozuoguai==0)&&(CameraStation[0]<55 || CameraStation[1]<55 ||CameraStation[2]<55||Leftblackedge[0]==5||Leftblackedge[1]==5||Leftblackedge[2]==5)&&(zhijiaoheixianhouyanshi==0))
            {            
                 zhijiaozuoguai=25;           
             }
            

        }else   //diuxianflag==1
        {               
          if( Leftblackedge[1]>20 &&Leftblackedge[1]<120)
          {
            diuxianflag=0;
            if(danxianshuzuo>10)
              Station=(Leftblackedge[1]+135)/2;
            else
              Station=Leftblackedge[1];     
          }
          if( Rightblackedge[1]>20 &&Rightblackedge[1]<120)
          {
            diuxianflag=0;
            if(danxianshuyou>10)
              Station=(Rightblackedge[1]+5)/2;
            else
              Station=Rightblackedge[1];    
          }
          if( CameraStation[1]>50 &&CameraStation[1]<90&&(Leftblackedge[1]>20 && Rightblackedge[1]<100))
          {
            diuxianflag=0;
            Station=CameraStation[1];    
          }
        }
           /***********障碍***********/

        if((shizi_flag==0)&&(singleflag<2)&&(diuxianflag==0)&&((Leftblackedge[4]>10)&&(Rightblackedge[4]<130)||(Leftblackedge[3]>10)&&(Rightblackedge[3]<130))&&(((danxianshuzuo1>4)&&(danxianshuzuo1<10))||((danxianshuzuo3>4)&&(danxianshuzuo3<10)))&&(lastsingleedge[0]>50 && lastsingleedge[0]<70))
        {
            zuozhangaiflag=20;
         //   zhangaiyanshi=10;
        }
        if((shizi_flag==0)&&(singleflag<2)&&(diuxianflag==0)&&((Leftblackedge[4]>10)&&(Rightblackedge[4]<100)||(Leftblackedge[3]>10)&&(Rightblackedge[3]<100))&&(((danxianshuyou1>4)&&(danxianshuyou1<10))||((danxianshuyou3>4)&&(danxianshuyou3<10)))&&(lastsingleedge[0]>50 && lastsingleedge[0]<70))
        {
            youzhangaiflag=30;
         //   zhangaiyanshi=10;
        }
        /***********障碍**********/
/*         if(zhijiaoflag==1 && ( zuozhangaiflag>0 ||  youzhangaiflag>0))
        {
          zuozhangaiflag=0;
          youzhangaiflag=0;
          zhangaiyanshi=0;
        }*/
        if(zhijiaoflag==1 && singleflag==1)
        {
          singleflag=0;
        }
        if(zuozhangaiflag>0)
        {
          zuozhangaiflag--;
          if(zuozhangaiflag==1)
            shizibiandan_flag=20;
          Station=Rightblackedge[1];
        }
        if(zhangaiyanshi>0)
        {
          zhangaiyanshi--;
        }
        if(youzhangaiflag>0)
        {
          youzhangaiflag--;
          if(youzhangaiflag==1)
            shizibiandan_flag=20;
          Station=Leftblackedge[1];
        }
                /***********十字***********/
        if((singleflag==0) &&((Rightblackedge[3]>110)&&(Leftblackedge[3]<20)) && ((Rightblackedge[4]<130)&&(Leftblackedge[4]>10)) && (( CameraStation[4]>30)&&( CameraStation[4]<100)) )
        {
           shizi_flag=8; 
           diuxianflag=0;
        }
                 /***********十字延时**********/
        if(shizi_flag>0)
        {
          shizi_flag--;
          shizibiandan_flag=50;
          Station=CameraStation[4];
          diuxianflag=0;
        }
        if( shizibiandan_flag>0)
        {
          shizibiandan_flag--;
        }
        /*****************直角误判************/
        /*if(Station<60||Station>80)
        {
          zhijiaoyanshi=10;
        }
        if(zhijiaoyanshi>0)
        {
          zhijiaoyanshi--;
          zhijiaoflag=0;
        }*/
         /***********直角延时**********/
      if(zhijiaoheixian>0)
        {
         
          zhijiaoheixian--;
        }
        if(guaiwanyanshi>1)
        {
          Station=65;
          guaiwanyanshi--;
        }
        if((zhijiaozuoguai>1))
        {
          
          
          zhijiaozuoguai--;
          Station=5;
          
        }
        if((zhijiaoyouguai>1))
        {
          
          zhijiaoyouguai--;
          Station=135;
          
        }
        if(zhijiaoheixianhouyanshi>0)
        {
          zhijiaoheixianhouyanshi--;
        }
        if(zhijiaoyanshi>0)
        {
          if(zhijiaoyanshi==1)
          {
            zhijiaoflag=0;
            zhijiaoyouguai=0;
            zhijiaozuoguai=0;
          }
          zhijiaoyanshi--;
        }
        if(shizizhijiaoflag>0)
        {
          shizizhijiaoflag--;
          Station=65;
        }
        
        /*******丢线延时********/
         lastsingleedge[2]=lastsingleedge[1];
         lastsingleedge[1]=lastsingleedge[0];
         lastsingleedge[0]=Station; 
         
         lastleftedge=Leftblackedge[1];
         lastrightedge=Rightblackedge[1];
        /******************单双判断程序结束**********************/
         
//       k1=TurnPWM(50, MIDSTATION,50,0,0);
         if(zuozhangaiflag>0)
         {
            TurnPWM_Value=3120+TurnPWM(Rightblackedge[1],90,20,0,30);
         }
         else if(youzhangaiflag>0)
         {
            TurnPWM_Value=3120+TurnPWM(Leftblackedge[1],40,20,0,30);
         }
         else if(singleflag>0)
         {
            TurnPWM_Value=3120+TurnPWM(Station,70,20,0,35);
         }
         else if(shizi_flag>0)
         {
            TurnPWM_Value=3120+TurnPWM(Station,70,20,0,35);
         }
         
         /*else if(Leftblackedge[1]==5 && Rightblackedge[1]!=135)
         {
           TurnPWM_Value=3120+TurnPWM(Rightblackedge[1],120,20,0,35);     //根据CCD位置值计算转向PWM值函数
         }
         else if(Leftblackedge[1]!=5 && Rightblackedge[1]==135)
         {
           TurnPWM_Value=3120+TurnPWM(Leftblackedge[1],20,20,0,35);     //根据CCD位置值计算转向PWM值函数
         }*/
         /*else if(Station>67 && Station<73 )
         {
           TurnPWM_Value=3120+TurnPWM(Station,70,5,0,0);  
         }*/
        /* else if(Station<66 ||  Station>84 )
         {
           TurnPWM_Value=3120+TurnPWM(Station,70,23,0,35);  
         }*/
         //  TurnPWM_Value=3120+TurnPWM(Station,70,20,0,35);     //根据CCD位置值计算转向PWM值函数
         else
         TurnPWM_Value=3120+TurnPWM(Station,70,23,0,35);     //根据CCD位置值计算转向PWM值函数
       

            
       
      if(TurnPWM_Value>3750)
         TurnPWM_Value=3750;
      if(TurnPWM_Value<2750)
        TurnPWM_Value=2750;
      FTM_CnV_REG(FTMx[FTM1], CH1) = TurnPWM_Value ;                            //转向赋值   舵机2   
    //  FTM_CnV_REG(FTMx[FTM1], CH1) = 2750;  
       if(dengta==0)
       {
         SpeedPWM_Value_left=0;
         SpeedPWM_Value_right=0;
         SpeedPWM_Value_left_jiansu=0;
         SpeedPWM_Value_right_jiansu=0;
            
       }
       else if(zhijiaoyouguai>0 && zhijiaozuoguai!=1 && zhijiaoyouguai!=1)
       {
         SpeedPWM_Value_left=400;
        SpeedPWM_Value_right=0;
         SpeedPWM_Value_left_jiansu=0 ;                          //PID计算的PWM 
        SpeedPWM_Value_right_jiansu=0 ;                          //PID计算的PWM 
       }
       else if(zhijiaozuoguai>0 &&zhijiaozuoguai!=1 && zhijiaoyouguai!=1)
       {
         SpeedPWM_Value_left=0;
        SpeedPWM_Value_right=500;
         SpeedPWM_Value_left_jiansu=0 ;                          //PID计算的PWM 
        SpeedPWM_Value_right_jiansu=0 ;                          //PID计算的PWM 
       }
        else if(zuozhangaiflag>0||youzhangaiflag>0)
        {
               SpeedPWM_Value_left=350;
               SpeedPWM_Value_right=350;
               SpeedPWM_Value_left_jiansu=0;                          //PID计算的PWM 
               SpeedPWM_Value_right_jiansu=0;                          //PID计算的PWM 
        }
        else if(zhijiaoflag==1&&zhijiaozuoguai!=1 && zhijiaoyouguai!=1)
       {
        if( SpeedPWM_Value_left>390 ||  SpeedPWM_Value_right>390)
        {
            zhijiaojiansu=10;
        }
        if(zhijiaojiansu>0)
        {
            SpeedPWM_Value_left=0;
            SpeedPWM_Value_right=0;
            SpeedPWM_Value_left_jiansu=0 ;                          //PID计算的PWM 
            SpeedPWM_Value_right_jiansu=0 ;                          //PID计算的PWM */
            zhijiaojiansu--;
        }
        else
        {
               SpeedPWM_Value_left=350;
               SpeedPWM_Value_right=350;
               SpeedPWM_Value_left_jiansu=0;                          //PID计算的PWM 
               SpeedPWM_Value_right_jiansu=0;                          //PID计算的PWM 
        }
       }
       else if(shizi_flag>0)
       {
           SpeedPWM_Value_left=500;
           SpeedPWM_Value_right=500;
           SpeedPWM_Value_left_jiansu=0 ;                          //PID计算的PWM 
           SpeedPWM_Value_right_jiansu=0 ;                          //PID计算的PWM 
       }
        else if(diuxianflag==1)
       {
    
        
               SpeedPWM_Value_left=350+(Station-70)*10;
               SpeedPWM_Value_right=350-(Station-70)*10;
               SpeedPWM_Value_left_jiansu=0;                          //PID计算的PWM 
              SpeedPWM_Value_right_jiansu=0;                          //PID计算的PWM 
       }
       else if(singleflag>0)
       {
              //SpeedPWM_Value_left=350+(Station-70)*10;
              //SpeedPWM_Value_right=350-(Station-70)*10;
              SpeedPWM_Value_left=600;
              SpeedPWM_Value_right=600;
              SpeedPWM_Value_left_jiansu=0;                          //PID计算的PWM 
              SpeedPWM_Value_right_jiansu=0;                          //PID计算的PWM 
       }
         
       /*else if((Station<60)||(Station>80)&& singleflag==1)
       {
           /* if( SpeedPWM_Value_left>390 ||  SpeedPWM_Value_right>390)
            {
              diuxianflag=5;
            }*/
    /*        if(diuxianflag>0)
            {
                  SpeedPWM_Value_left=100;
                  SpeedPWM_Value_right=100;
                  SpeedPWM_Value_left_jiansu=100 ;                          //PID计算的PWM 
                  SpeedPWM_Value_right_jiansu=100 ;                          //PID计算的PWM 
            }
            else
            {
              SpeedPWM_Value_left=350+(Station-70)*10;
             SpeedPWM_Value_right=350-(Station-70)*10;
             SpeedPWM_Value_left_jiansu=0 ;                          //PID计算的PWM 
             SpeedPWM_Value_right_jiansu=0 ;  
            }
       }*/
       else if((Station<80)&&(Station>60)&&(CameraStation[4]>60 && CameraStation[4]<80)&&(singleflag==0))
      {
     //   SpeedPWM_Value_left =600+(Station-70)*6;
     //   SpeedPWM_Value_right=600-(Station-70)*6;
        SpeedPWM_Value_left =600;
        SpeedPWM_Value_right=600;
         SpeedPWM_Value_left_jiansu=0 ;                          //PID计算的PWM 
        SpeedPWM_Value_right_jiansu=0 ;                          //PID计算的PWM 
      }
      else
      {

        {
        SpeedPWM_Value_left=500+(Station-70)*5;
        SpeedPWM_Value_right=500-(Station-70)*5;
        SpeedPWM_Value_left_jiansu=0 ;                          //PID计算的PWM 
        SpeedPWM_Value_right_jiansu=0 ;                          //PID计算的PWM 
        }
      }
     
      /*if(diuxianflag>0||diuxianyanshi>0)
      {
        SpeedPWM_Value_left=300;
        SpeedPWM_Value_right=300;
      }*/
       if(SpeedPWM_Value_left<0)
         SpeedPWM_Value_left=0;
       if(SpeedPWM_Value_right<0)
         SpeedPWM_Value_right=0;
        FTM_PWM_Duty(FTM0 , CH0,SpeedPWM_Value_left_jiansu);                                           //FTM0 CH0 PWM输出 ；电机控制
        FTM_PWM_Duty(FTM0 , CH1,SpeedPWM_Value_left);                              //FTM0 CH1 PWM输出 ；电机控制  
        FTM_PWM_Duty(FTM0 , CH2,SpeedPWM_Value_right_jiansu);                                           //FTM0 CH0 PWM输出 ；电机控制
        FTM_PWM_Duty(FTM0 , CH3,SpeedPWM_Value_right);                             //FTM0 CH1 PWM输出 ；电机控制  
     /*   FTM_PWM_Duty(FTM0 , CH0,0);                                           //FTM0 CH0 PWM输出 ；电机控制
        FTM_PWM_Duty(FTM0 , CH1,0);                              //FTM0 CH1 PWM输出 ；电机控制  
        FTM_PWM_Duty(FTM0 , CH2,0);                                           //FTM0 CH0 PWM输出 ；电机控制
        FTM_PWM_Duty(FTM0 , CH3,0);*/
        
    }
    
    /*********************
    80ms程序执行代码段
    *********************/    
    if(TIME0flag_80ms == 1)
    {        
      TIME0flag_80ms = 0 ;                                                    //清除时间标志位
      PTA16_OUT = ~PTA16_OUT ;                                                //测试LED闪烁      

      
       // SpeedPWM_Value+=SpeedControl(Speed_Count,Set_Speed,Speed_P,Speed_I,Speed_D) ;    //PID函数计算PWM值    
      
    }
    
    /*********************
    100ms程序执行代码段
    *********************/
    if(TIME0flag_100ms == 1)
    { 
      TIME0flag_100ms = 0 ;
      
    }
    
    if(DMA_Over_Flag==1)
    {
      DMA_Over_Flag=0;      

 //     Camera_Black_single(&ADdata[21][0],&single[1],&single_left);
 //     Camera_Black_single(&ADdata[22][0],&single[2]);  
      /*  uart_putchar (UART0 , 0);  
        uart_putchar (UART0 , 255);
        uart_putchar (UART0 , 1); 
        uart_putchar (UART0 , 0); 
        for(int i=0;i<30;i++)
          for(int j=0;j<140;j++)
        {
             uart_putchar (UART0 ,ADdata[i][j] );  
        }
        */
        Camera_Black1(&ADdata[12][0],&Rightblackedge[0],&Leftblackedge[0],&CameraStation[0]);
         left_num[0]=0;right_num[0]=0;
        for(int i=70;i>40;i--)
        {
          if(i<5)
            break;
          if(ADdata[12][i]<100)
            left_num[0]++;
        }
        for(int i=70;i<100;i++)
        {
          if(i>135)
            break;
          if(ADdata[12][i]<100)
            right_num[0]++;
        }
        Camera_Black2(&ADdata[14][0],&Rightblackedge[1],&Leftblackedge[1],&CameraStation[1]);
        left_num[1]=0;right_num[1]=0;
        for(int i=70;i>40;i--)
        {
          if(i<5)
            break;
          if(ADdata[14][i]<100)
            left_num[1]++;
        }
        for(int i=70;i<100;i++)
        {
          if(i>135)
            break;
          if(ADdata[14][i]<100)
            right_num[1]++;
        }
        Camera_Black3(&ADdata[16][0],&Rightblackedge[2],&Leftblackedge[2],&CameraStation[2]);
           left_num[2]=0;right_num[2]=0;
         for(int i=70;i>40;i--)
        {
          if(i<5)
            break;
          if(ADdata[16][i]<100)
            left_num[2]++;
        }
        for(int i=70;i<100;i++)
        {
          if(i>135)
            break;
          if(ADdata[16][i]<100)
            right_num[2]++;
        }
        Camera_Black4(&ADdata[10][0],&Rightblackedge[3],&Leftblackedge[3],&CameraStation[3]);
        left_num[3]=0;right_num[3]=0;
        for(int i=70;i>40;i--)
        {
          if(i<5)
            break;
          if(ADdata[10][i]<100)
            left_num[3]++;
        }
        for(int i=70;i<100;i++)
        {
          if(i>135)
            break;
          if(ADdata[10][i]<100)
            right_num[3]++;
        }
        Camera_Black5(&ADdata[3][0],&Rightblackedge[4],&Leftblackedge[4],&CameraStation[4]);
        left_num[4]=0;right_num[4]=0;
        for(int i=70;i>40;i--)
        {
          if(i<5)
            break;
          if(ADdata[3][i]<100)
            left_num[4]++;
        }
        for(int i=70;i<100;i++)
        {
          if(i>135)
            break;
          if(ADdata[3][i]<100)
            right_num[4]++;
        }
    }  
     
    
    
  }
  /***************************************************************************************************************/
  /***************************************************************************************************************/
  /***************************************************************************************************************/
  /***************************************************************************************************************/
  /***************************************************************************************************************/
  /***************************************************************************************************************/
  //慢速程序  
  /***************************************************************************************************************/
  /***************************************************************************************************************/
  /***************************************************************************************************************/
  /***************************************************************************************************************/
  /***************************************************************************************************************/
  /***************************************************************************************************************/
  else
  {
       /*********************
    5ms程序执行代码段
    *********************/
    if(TIME0flag_5ms == 1)
    { 
      TIME0flag_5ms = 0 ;
      
    }
    
    /*********************
    10ms程序执行代码段
    *********************/
    if(TIME0flag_10ms == 1)
    {
      TIME0flag_10ms = 0 ;
      //LPTMR0_CNR=1; 
/*      Speed_Count = LPTMR0_CNR;                                                     //保存脉冲计数器计算值
      lptmr_counter_clean();                                                  //清空脉冲计数器计算值（马上清空，这样才能保证计数值准确）
      Speed_Count = LPT_INT_count * LIN_COUT +  Speed_Count;                              //间隔10ms的脉冲次数
      LPT_INT_count = 0;     */        
      //清空LPT中断次数   
    }
    
    /*********************
    20ms程序执行代码段
    *********************/
    if(TIME0flag_20ms == 1)
    {

      TIME0flag_20ms = 0 ;
      //清除时间标志位   
           k2=gpio_get(PORTB, 3);
       k2=gpio_get(PORTB, 3);
      if(gpio_get(PORTB, 9)==1)
      {
      if(dengta==0)
      {
         if( gpio_get(PORTB, 3)==1)
        {
           for(k1=0;k1<12;k1++)
           {
             BFdelay_1us(300);
             if(gpio_get(PORTB, 3)==0)
               break;
              if(k1%3==0)
                BFDly_ms(1);
               
          }
          if(k1==12)
          {
           
            dengta=1;
          }
        else
          dengta=0; 
        }
       }
      if(dengta==1)
      {
          if(gpio_get(PORTB, 3)==0)
          {
            dengta=0;
          }   
       }
    }
    else
    {
      if(dengta==0 )
      {
        k1++;
        if(k1>250)
          dengta=1;
      }
    }
      if(zhijiaoflag>0)
         gpio_init (PORTA , 17, GPO,LOW);
      else
         gpio_init (PORTA , 17, GPO,HIGH);   
      /******************单双判断程序**********************/
        singleflag=0;
   
        danxianshuzuo=0;
        
        for(int i=Leftblackedge[1]-5;i>Leftblackedge[1]-20;i--)
        {
          if(i<10)
            break;
          if(ADdata[15][i]>100)
            danxianshuzuo++;
        }
        danxianshuyou=0;
        for(int i=Rightblackedge[1]+5;i<Rightblackedge[1]+20;i++)
        {
          if(i>120)
            break;
          if(ADdata[15][i]>100)
            danxianshuyou++;
        }
        
         danxianshuzuo1=0;
        
        for(int i=Leftblackedge[1]-10;i>Leftblackedge[1]-40;i--)
        {
          if(i<10)
            break;
          if(ADdata[13][i]>100)
            danxianshuzuo1++;
        }
        danxianshuyou1=0;
        for(int i=Rightblackedge[1]+10;i<Rightblackedge[1]+40;i++)
        {
          if(i>130)
            break;
          if(ADdata[13][i]>100)
            danxianshuyou1++;
        }
        
            danxianshuzuo2=0;
        
        for(int i=Leftblackedge[3]-5;i>Leftblackedge[3]-20;i--)
        {
          if(i<10)
            break;
          if(ADdata[10][i]>100)
            danxianshuzuo2++;
        }
        danxianshuyou2=0;
        for(int i=Rightblackedge[3]+5;i<Rightblackedge[3]+20;i++)
        {
          if(i>130)
            break;
          if(ADdata[10][i]>100)
            danxianshuyou2++;
        }
         danxianshuzuo3=0;
        
        for(int i=Leftblackedge[3]-10;i>Leftblackedge[3]-40;i--)
        {
          if(i<10)
            break;
          if(ADdata[10][i]>100)
            danxianshuzuo3++;
        }
        danxianshuyou3=0;
        for(int i=Rightblackedge[3]+10;i<Rightblackedge[3]+40;i++)
        {
          if(i>130)
            break;
          if(ADdata[10][i]>100)
            danxianshuyou3++;
        }
        
        if(diuxianflag==0)
        {
        if(danxianshuzuo>10||danxianshuyou>10 || danxianshuzuo2>10||danxianshuyou2>10)
        {        
                  if(danxianshuzuo>10&&danxianshuyou<10 && Leftblackedge[1]>20 && Rightblackedge[1]<130)
                  {
                 //   Station=(Rightblackedge[1]+5)/2;
                    Station=Leftblackedge[1];
                    danxianstation=Station;
                    singleflag=2;
                  }
                  else if(danxianshuzuo<10&&danxianshuyou>10 && Leftblackedge[1]>20 && Rightblackedge[1]<130)
                  {
                //    Station=(Leftblackedge[1]+135)/2;
                     Station=Rightblackedge[1];
                    danxianstation=Station;
                    singleflag=2;
                  }
                  else if(danxianshuzuo>10 && Leftblackedge[1]>20)
                  {
                    Station=Leftblackedge[1];
                    danxianstation=Station;
                    singleflag=1;
                  }
                  else if(danxianshuyou>10  && Rightblackedge[1]<130)
                  {
                    Station=Rightblackedge[1];
                    danxianstation=Station;
                    singleflag=1;
                  }
                  else if(danxianshuzuo2>10&&danxianshuyou2<10 && Leftblackedge[3]>20 && Rightblackedge[3]<130)
                  {
                 //   Station=(Rightblackedge[1]+5)/2;
                    Station=Leftblackedge[3];
                    danxianstation=Station;
                    singleflag=2;
                  }
                  else if(danxianshuzuo2<10&&danxianshuyou2>10 && Leftblackedge[3]>20 && Rightblackedge[3]<130)
                  {
                //    Station=(Leftblackedge[1]+135)/2;
                     Station=Rightblackedge[3];
                    danxianstation=Station;
                    singleflag=2;
                  }
                  else if(danxianshuzuo2>10 && Leftblackedge[3]>20)
                  {
                    Station=Leftblackedge[3];
                    danxianstation=Station;
                    singleflag=1;
                  }
                  else if(danxianshuyou2>10  && Rightblackedge[3]<130)
                  {
                    Station=Rightblackedge[3];
                    danxianstation=Station;
                    singleflag=1;
                  }
        }
        /***********正常取平均***********/
        else
        {
              Station=CameraStation[1];  
              singleflag=0;
         }
         if((Leftblackedge[1]<10 && Rightblackedge[1]>130))
        {
   
            Station=danxianstation;
            diuxianflag=1;
        }    

     
         /***********直角**********/

         if((zhangaiyanshi==0)&&(((left_num[0]>20)&&(right_num[0]>20))||((left_num[1]>20)&&(right_num[1]>20))||((left_num[2]>20)&&(right_num[2]>20))) &&(zhijiaoheixian==0)&&((Rightblackedge[4]<135 &&Leftblackedge[4]>5)|| (Rightblackedge[3]<135 &&Leftblackedge[3]>5) ) &&(( left_num[4]<20&& right_num[4]<20)||( left_num[3]<20&& right_num[3]<20)))    
     //   if((((left_num[0]>20)&&(right_num[0]>20))||((left_num[1]>20)&&(right_num[1]>20))||((left_num[2]>20)&&(right_num[2]>20))) &&(zhijiaoheixian==0)&&(zhangaiyanshi==0))
        {
            guaiwanyanshi=0;
            if(zhijiaoflag==0)
              zhijiaoflag=1;
            else
              zhijiaoflag=0;
            zhijiaoheixian=100;
            shizibiandan_flag=100;
            shizizhijiaoflag=20;
            zhijiaoyouguai=0;
            zhijiaozuoguai=0;
            zhijiaoheixianhouyanshi=40;
            zhijiaoyanshi=150;  
        }
         //if((zhijiaoflag==1)&&(((Leftblackedge[0]>5)&&(Rightblackedge[0]>100))||((Leftblackedge[1]>5)&&(Rightblackedge[1]>100))||((Leftblackedge[2]>5)&&(Rightblackedge[2]>100)))&&(zhijiaoheixianhouyanshi==0))
     //   if((zhijiaoflag==1)&&(zhijiaozuoguai==0)&&(zhijiaoyouguai==0)&&(CameraStation[0]>75 ||CameraStation[1]>75||CameraStation[3]>75||CameraStation[4]>75 ||CameraStation[2]>75)&&(zhijiaoheixianhouyanshi==0))
          if((zhijiaoflag==1)&&(zhijiaozuoguai==0)&&(zhijiaoyouguai==0)&&(CameraStation[1]>70 ||CameraStation[2]>70  || Rightblackedge[1]>130 || Rightblackedge[2]>130)&&(zhijiaoheixianhouyanshi==0))
            {
                 zhijiaoyouguai=30;
            }  
       //  else if((zhijiaoflag==1)&&(((Leftblackedge[0]<20)&&(Rightblackedge[0]<135))||((Leftblackedge[1]<20)&&(Rightblackedge[1]<135))||((Leftblackedge[2]<20)&&(Rightblackedge[2]<135)))&&(zhijiaoheixianhouyanshi==0))
        //if((zhijiaoflag==1)&&(zhijiaoyouguai==0)&&(zhijiaozuoguai==0)&&(CameraStation[0]<65 || CameraStation[1]<65 ||CameraStation[2]<65||CameraStation[3]<65||CameraStation[4]<65 )&&(zhijiaoheixianhouyanshi==0))
          if((zhijiaoflag==1)&&(zhijiaoyouguai==0)&&(zhijiaozuoguai==0)&&( CameraStation[1]<55 ||CameraStation[2]<55  ||Leftblackedge[1]<10 ||Leftblackedge[2]<10)&&(zhijiaoheixianhouyanshi==0))
            {            
                 zhijiaozuoguai=30;           
             }
            

        }else   //diuxianflag==1
        {               
          if( Leftblackedge[1]>20 &&Leftblackedge[1]<120)
          {
            diuxianflag=0;
            if(danxianshuzuo>10)
              Station=(Leftblackedge[1]+135)/2;
            else
              Station=Leftblackedge[1];     
          }
          if( Rightblackedge[1]>20 &&Rightblackedge[1]<120)
          {
            diuxianflag=0;
            if(danxianshuyou>10)
              Station=(Rightblackedge[1]+5)/2;
            else
              Station=Rightblackedge[1];    
          }
          if( CameraStation[1]>50 &&CameraStation[1]<90&&(Leftblackedge[1]>20 && Rightblackedge[1]<100))
          {
            diuxianflag=0;
            Station=CameraStation[1];    
          }
        }
           /***********障碍***********/

         if((shizi_flag==0)&&(singleflag<2)&&(diuxianflag==0)&&((Leftblackedge[4]>20)&&(Rightblackedge[4]<130)||(Leftblackedge[3]>20)&&(Rightblackedge[3]<130))&&(((danxianshuzuo1>4)&&(danxianshuzuo1<10))||((danxianshuzuo3>4)&&(danxianshuzuo3<10)))&&(lastsingleedge[0]>60 && lastsingleedge[0]<80))
        {
            zuozhangaiflag=20;
         //   zhangaiyanshi=10;
        }
        if((shizi_flag==0)&&(singleflag<2)&&(diuxianflag==0)&&((Leftblackedge[4]>10)&&(Rightblackedge[4]<100)||(Leftblackedge[3]>10)&&(Rightblackedge[3]<100))&&(((danxianshuyou1>4)&&(danxianshuyou1<10))||((danxianshuyou3>4)&&(danxianshuyou3<10)))&&(lastsingleedge[0]>60 && lastsingleedge[0]<80))
        {
            youzhangaiflag=30;
         //   zhangaiyanshi=10;
        }
        /***********障碍**********/
         if(zhijiaoflag==1 && ( zuozhangaiflag>0 ||  youzhangaiflag>0))
        {
          zuozhangaiflag=0;
          youzhangaiflag=0;
          zhangaiyanshi=0;
        }
        if(zhijiaoflag==1 && singleflag==1)
        {
          singleflag=0;
        }
        if(zuozhangaiflag>0)
        {
          zuozhangaiflag--;
          if(zuozhangaiflag==1)
            shizibiandan_flag=20;
          Station=Rightblackedge[1];
        }
        if(zhangaiyanshi>0)
        {
          zhangaiyanshi--;
        }
        if(youzhangaiflag>0)
        {
          youzhangaiflag--;
          if(youzhangaiflag==1)
            shizibiandan_flag=20;
          Station=Leftblackedge[1];
        }
                /***********十字***********/
        if((singleflag==0) &&((Rightblackedge[3]>110)&&(Leftblackedge[3]<20)) && ((Rightblackedge[4]<130)&&(Leftblackedge[4]>10)) && (( CameraStation[4]>30)&&( CameraStation[4]<100)) )
        {
           shizi_flag=10; 
           diuxianflag=0;
        }
                 /***********十字延时**********/
        if(shizi_flag>0)
        {
          shizi_flag--;
          shizibiandan_flag=50;
          Station=CameraStation[4];
          diuxianflag=0;
        }
        if( shizibiandan_flag>0)
        {
          shizibiandan_flag--;
        }
        /*****************直角误判************/
        /*if(Station<60||Station>80)
        {
          zhijiaoyanshi=10;
        }
        if(zhijiaoyanshi>0)
        {
          zhijiaoyanshi--;
          zhijiaoflag=0;
        }*/
         /***********直角延时**********/
      if(zhijiaoheixian>0)
        {
         
          zhijiaoheixian--;
        }
        if(guaiwanyanshi>1)
        {
          Station=70;
          Station=CameraStation[3];
          guaiwanyanshi--;
        }
        if((zhijiaozuoguai>1))
        {
          
          
          zhijiaozuoguai--;
          Station=5;
          
        }
        if((zhijiaoyouguai>1))
        {
          
          zhijiaoyouguai--;
          Station=135;
          
        }
        if(zhijiaoheixianhouyanshi>0)
        {
          zhijiaoheixianhouyanshi--;
        }
        if(zhijiaoyanshi>0)
        {
          if(zhijiaoyanshi==1)
          {
            zhijiaoflag=0;
            zhijiaoyouguai=0;
            zhijiaozuoguai=0;
          }
          zhijiaoyanshi--;
        }
        if(shizizhijiaoflag>0)
        {
          shizizhijiaoflag--;
          Station=65;
        }
        
        /*******丢线延时********/
         lastsingleedge[2]=lastsingleedge[1];
         lastsingleedge[1]=lastsingleedge[0];
         lastsingleedge[0]=Station; 
         
         lastleftedge=Leftblackedge[1];
         lastrightedge=Rightblackedge[1];
        /******************单双判断程序结束**********************/
         
//       k1=TurnPWM(50, MIDSTATION,50,0,0);
         if(zuozhangaiflag>0)
         {
            TurnPWM_Value=3120+TurnPWM(Rightblackedge[1],90,20,0,20);
         }
         else if(youzhangaiflag>0)
         {
            TurnPWM_Value=3120+TurnPWM(Leftblackedge[1],40,20,0,20);
         }
         else if(singleflag>0)
         {
            if(Station<66)
            {
              TurnPWM_Value=3120+TurnPWM(Station,70,30,0,35);
            }
            TurnPWM_Value=3120+TurnPWM(Station,70,30,0,35);
         }
         
         /*else if(Leftblackedge[1]==5 && Rightblackedge[1]!=135)
         {
           TurnPWM_Value=3120+TurnPWM(Rightblackedge[1],120,20,0,35);     //根据CCD位置值计算转向PWM值函数
         }
         else if(Leftblackedge[1]!=5 && Rightblackedge[1]==135)
         {
           TurnPWM_Value=3120+TurnPWM(Leftblackedge[1],20,20,0,35);     //根据CCD位置值计算转向PWM值函数
         }*/
         else 
           TurnPWM_Value=3120+TurnPWM(Station,70,20,0,35);     //根据CCD位置值计算转向PWM值函数
       

            
       
      if(TurnPWM_Value>3750)
         TurnPWM_Value=3750;
      if(TurnPWM_Value<2750)
        TurnPWM_Value=2750;
      FTM_CnV_REG(FTMx[FTM1], CH1) = TurnPWM_Value ;                            //转向赋值   舵机2   
    //   FTM_CnV_REG(FTMx[FTM1], CH1) = 3150;  
       if(dengta==0)
       {
           SpeedPWM_Value_left=0;
           SpeedPWM_Value_right=0;
           SpeedPWM_Value_left_jiansu=0 ;                          //PID计算的PWM 
           SpeedPWM_Value_right_jiansu=0 ;                          //PID计算的PWM 
       }
       else if(zhijiaoyouguai>0 && zhijiaozuoguai!=1 && zhijiaoyouguai!=1)
       {
         SpeedPWM_Value_left=400;
        SpeedPWM_Value_right=0;
         SpeedPWM_Value_left_jiansu=0 ;                          //PID计算的PWM 
        SpeedPWM_Value_right_jiansu=0 ;                          //PID计算的PWM 
       }
       else if(zhijiaozuoguai>0 &&zhijiaozuoguai!=1 && zhijiaoyouguai!=1)
       {
         SpeedPWM_Value_left=0;
        SpeedPWM_Value_right=500;
         SpeedPWM_Value_left_jiansu=0 ;                          //PID计算的PWM 
        SpeedPWM_Value_right_jiansu=0 ;                          //PID计算的PWM 
       }
        else if(zhijiaoflag==1&&zhijiaozuoguai!=1 && zhijiaoyouguai!=1)
       {
        if( SpeedPWM_Value_left>390 ||  SpeedPWM_Value_right>390)
        {
            zhijiaojiansu=10;
        }
        if(zhijiaojiansu>0)
        {
            SpeedPWM_Value_left=0;
            SpeedPWM_Value_right=0;
            SpeedPWM_Value_left_jiansu=0 ;                          //PID计算的PWM 
            SpeedPWM_Value_right_jiansu=0 ;                          //PID计算的PWM */
            zhijiaojiansu--;
        }
        else
        {
               SpeedPWM_Value_left=350;
               SpeedPWM_Value_right=350;
               SpeedPWM_Value_left_jiansu=0;                          //PID计算的PWM 
               SpeedPWM_Value_right_jiansu=0;                          //PID计算的PWM 
        }
       }
        else if(diuxianflag==1)
       {
    
        
               SpeedPWM_Value_left=350+(Station-70)*10;
               SpeedPWM_Value_right=350-(Station-70)*10;
               SpeedPWM_Value_left_jiansu=0;                          //PID计算的PWM 
              SpeedPWM_Value_right_jiansu=0;                          //PID计算的PWM 
       }
       else if(singleflag>0)
       {
              SpeedPWM_Value_left =500;
              SpeedPWM_Value_right=500;
              SpeedPWM_Value_left_jiansu=0 ;                          //PID计算的PWM 
              SpeedPWM_Value_right_jiansu=0 ;                          //PID计算的PWM                  
       }
       /*else if((Station<60)||(Station>80)&& singleflag==1)
       {
           /* if( SpeedPWM_Value_left>390 ||  SpeedPWM_Value_right>390)
            {
              diuxianflag=5;
            }*/
    /*        if(diuxianflag>0)
            {
                  SpeedPWM_Value_left=100;
                  SpeedPWM_Value_right=100;
                  SpeedPWM_Value_left_jiansu=100 ;                          //PID计算的PWM 
                  SpeedPWM_Value_right_jiansu=100 ;                          //PID计算的PWM 
            }
            else
            {
              SpeedPWM_Value_left=350+(Station-70)*10;
             SpeedPWM_Value_right=350-(Station-70)*10;
             SpeedPWM_Value_left_jiansu=0 ;                          //PID计算的PWM 
             SpeedPWM_Value_right_jiansu=0 ;  
            }
       }*/
       else if((Station<80)&&(Station>60)&&(CameraStation[3]>60 && CameraStation[3]<80)&&(singleflag==0))
      {
        SpeedPWM_Value_left =500;
        SpeedPWM_Value_right=500;
         SpeedPWM_Value_left_jiansu=0 ;                          //PID计算的PWM 
        SpeedPWM_Value_right_jiansu=0 ;                          //PID计算的PWM 
      }
      else
      {
        /*if(diuxianflag>0)
            {
                  SpeedPWM_Value_left=100;
                  SpeedPWM_Value_right=100;
                  SpeedPWM_Value_left_jiansu=100 ;                          //PID计算的PWM 
                  SpeedPWM_Value_right_jiansu=100 ;                          //PID计算的PWM 
            }*/
      //  else
        {
        SpeedPWM_Value_left=400+(Station-70)*10;
        SpeedPWM_Value_right=400-(Station-70)*10;
        SpeedPWM_Value_left_jiansu=0 ;                          //PID计算的PWM 
        SpeedPWM_Value_right_jiansu=0 ;                          //PID计算的PWM 
        }
      }
     
      /*if(diuxianflag>0||diuxianyanshi>0)
      {
        SpeedPWM_Value_left=300;
        SpeedPWM_Value_right=300;
      }*/
       if(SpeedPWM_Value_left<0)
         SpeedPWM_Value_left=0;
       if(SpeedPWM_Value_right<0)
         SpeedPWM_Value_right=0;
        FTM_PWM_Duty(FTM0 , CH0,SpeedPWM_Value_left_jiansu);                                           //FTM0 CH0 PWM输出 ；电机控制
        FTM_PWM_Duty(FTM0 , CH1,SpeedPWM_Value_left);                              //FTM0 CH1 PWM输出 ；电机控制  
        FTM_PWM_Duty(FTM0 , CH2,SpeedPWM_Value_right_jiansu);                                           //FTM0 CH0 PWM输出 ；电机控制
        FTM_PWM_Duty(FTM0 , CH3,SpeedPWM_Value_right);                             //FTM0 CH1 PWM输出 ；电机控制  
       /* FTM_PWM_Duty(FTM0 , CH0,0);                                           //FTM0 CH0 PWM输出 ；电机控制
        FTM_PWM_Duty(FTM0 , CH1,300);                              //FTM0 CH1 PWM输出 ；电机控制  
        FTM_PWM_Duty(FTM0 , CH2,0);                                           //FTM0 CH0 PWM输出 ；电机控制
        FTM_PWM_Duty(FTM0 , CH3,300);*/
        
    }
    
    /*********************
    80ms程序执行代码段
    *********************/    
    if(TIME0flag_80ms == 1)
    {        
      TIME0flag_80ms = 0 ;                                                    //清除时间标志位
      PTA16_OUT = ~PTA16_OUT ;                                                //测试LED闪烁      

      
       // SpeedPWM_Value+=SpeedControl(Speed_Count,Set_Speed,Speed_P,Speed_I,Speed_D) ;    //PID函数计算PWM值    
      
    }
    
    /*********************
    100ms程序执行代码段
    *********************/
    if(TIME0flag_100ms == 1)
    { 
      TIME0flag_100ms = 0 ;
      
    }
    
    if(DMA_Over_Flag==1)
    {
      DMA_Over_Flag=0;      

 //     Camera_Black_single(&ADdata[21][0],&single[1],&single_left);
 //     Camera_Black_single(&ADdata[22][0],&single[2]);  
        
        
        Camera_Black1(&ADdata[12][0],&Rightblackedge[0],&Leftblackedge[0],&CameraStation[0]);
         left_num[0]=0;right_num[0]=0;
        for(int i=70;i>40;i--)
        {
          if(i<5)
            break;
          if(ADdata[12][i]<100)
            left_num[0]++;
        }
        for(int i=70;i<100;i++)
        {
          if(i>135)
            break;
          if(ADdata[12][i]<100)
            right_num[0]++;
        }
        Camera_Black2(&ADdata[14][0],&Rightblackedge[1],&Leftblackedge[1],&CameraStation[1]);
        left_num[1]=0;right_num[1]=0;
        for(int i=70;i>40;i--)
        {
          if(i<5)
            break;
          if(ADdata[14][i]<100)
            left_num[1]++;
        }
        for(int i=70;i<100;i++)
        {
          if(i>135)
            break;
          if(ADdata[14][i]<100)
            right_num[1]++;
        }
        Camera_Black3(&ADdata[16][0],&Rightblackedge[2],&Leftblackedge[2],&CameraStation[2]);
           left_num[2]=0;right_num[2]=0;
         for(int i=70;i>40;i--)
        {
          if(i<5)
            break;
          if(ADdata[16][i]<100)
            left_num[2]++;
        }
        for(int i=70;i<100;i++)
        {
          if(i>135)
            break;
          if(ADdata[16][i]<100)
            right_num[2]++;
        }
        Camera_Black4(&ADdata[10][0],&Rightblackedge[3],&Leftblackedge[3],&CameraStation[3]);
        left_num[3]=0;right_num[3]=0;
        for(int i=70;i>40;i--)
        {
          if(i<5)
            break;
          if(ADdata[10][i]<100)
            left_num[3]++;
        }
        for(int i=70;i<100;i++)
        {
          if(i>135)
            break;
          if(ADdata[10][i]<100)
            right_num[3]++;
        }
        Camera_Black5(&ADdata[3][0],&Rightblackedge[4],&Leftblackedge[4],&CameraStation[4]);
        left_num[4]=0;right_num[4]=0;
        for(int i=70;i>40;i--)
        {
          if(i<5)
            break;
          if(ADdata[3][i]<100)
            left_num[4]++;
        }
        for(int i=70;i<100;i++)
        {
          if(i>135)
            break;
          if(ADdata[3][i]<100)
            right_num[4]++;
        }
    }  
     
     
  }
  
  
  
  }
}


/*
    风(かぜ)　新(あたら)しく　绿(みどり)を駆(か)ける
　　どこまでも远(とお)く　澄(す)み渡(わた)るよ
　　今(いま)　軽(かろ)やかに　光(ひかり)は回(まわ)る
　　全(すべ)てをやわらかく照(て)らすだろう
　　星(ほし)さえ见(み)えない
　　雨(あめ)の时(とき)でも
　　君(きみ)が梦(ゆめ)见(み)てる未来(みらい)は
　　侧(そば)にあるよ
　　いつの日(ひ)も歌(うた)おう
　　この心(こころ)のまま
　　响(ひび)くよ　空(そら)の向(む)こう
　　彼方(かなた)まで
　　そしてまたどこかで
　　君(きみ)に届(とど)いたら
　　思(おも)い出(だ)してほしい
　　耀(かがや)く笑颜(えがお)で过(す)ごした日々(ひび)を
　　风(かぜ)　透(す)き通(とお)り　大地(だいち)にそよぐ
　　优(やさ)しく时间(じかん)を包(つつ)んでるよ
　　今(いま)　穏(おだ)やかに　光(ひかり)は注(そそ)ぐ
　　全(すべ)てを懐(なつ)かしく照(て)らすだろう
　　道(みち)さえ见(み)えない
　　闇(やみ)の时(とき)でも
　　君(きみ)が信(しん)じてる未来(みらい)は
　　侧(そば)にあるよ
　　いつの日(ひ)も歌(うた)おう
　　この心(こころ)のまま
　　悲(かな)しみも超(こ)えて行(い)く
　　强(つよ)い声(こえ)
　　君(きみ)がその勇気(ゆうき)で
　　一歩(いっぽ)进(すす)む时(とき)
　　思(おも)い出(だ)してほしい
　　煌(きらめ)く瞳(ひとみ)で过(す)ごした日々(ひび)を
　　いつの日(ひ)も歌(うた)おう
　　この心(こころ)のまま
　　响(ひび)くよ　空(そら)の向(む)こう
　　彼方(かなた)まで
　　そしてまたどこかで
　　君(きみ)に届(とど)いたら
　　思(おも)い出(だ)してほしい
　　煌(きらめ)く瞳(ひとみ)で过(す)ごした日々(ひび)を
　　耀(かがや)く笑颜(えがお)で过(す)ごした日々(ひび)を
*/
