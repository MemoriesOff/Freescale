#ifndef  CCD_H
#define  CCD_H  


#define SI_SetVal()   PTE4_OUT = 1 ;
#define SI_ClrVal()   PTE4_OUT = 0 ;
#define CLK_ClrVal()  PTE5_OUT = 0 ;
#define CLK_SetVal()  PTE5_OUT = 1 ;
#define PEXLCOUNT     (128) ;
#define TURNMIDPOSITION  (60) ;
#define  DATALINE   1         //采样行数
#define  DATACOUNT  400        //采样点数

void StartIntegration(void) ;   
void ImageCapture(uint8 * ImageData) ;
void SendHex(uint8 hex) ;
void SamplingDelay(void) ;
void CCD_init(void) ;
void CalculateIntegrationTime(void) ;
uint8 PixelAverage(uint8 len, uint8 *data) ;
void SendImageData(uint8 * ImageData) ;
int16  CCD_PointOUT (uint8 LinCout ,uint8 *LeftAryy , uint8 *RingAryy  ) ;
void CCD_Point(uint8 *point,uint8 LineCOUT ,uint8 * LineLeftAryy,uint8 * LineRightAryy,
                       uint8 * lastLeftAryy,uint8 *lastRightAryy) ;
void CCD_Black(uint8 *ImageData,uint8 *rightblackedge,uint8 *leftblackedge,int8 *centre_black);

#endif