#ifndef _LANDZO_OELD_H
#define _LANDZO_OELD_H

#include "gpio.h"
/********************************************************************/
/*-----------------------------------------------------------------------
LCD_init          : OLED初始化

编写日期          ：2012-11-01
最后修改日期      ：2012-11-01
-----------------------------------------------------------------------*/
 extern uint8 lanzhou96x64[768];
 void OLED_Init(void);
 void OLED_CLS(void);
 void OLED_P6x8Str(uint8 x,uint8 y,uint8 ch[]);
 void OLED_P8x16Str(uint8 x,uint8 y,uint8 ch[]);
 void OLED_P14x16Str(uint8 x,uint8 y,uint8 ch[]);
 void OLED_HEXACSII(uint16 hex,uint8* Print);
 void OLED_Display_Num(uint8 x, uint8 y, uint16 num);
 void OLED_Print(uint8 x, uint8 y, uint8 ch[]);
 void OLED_PutPixel(uint8 x,uint8 y);
 void OLED_Rectangle(uint8 x1,uint8 y1,uint8 x2,uint8 y2,uint8 gif);
 void OLED_Set_Pos(uint8 x, uint8 y);
 void OLED_WrDat(uint8 data);
 void Draw_LibLogo(void);
 void Draw_Landzo(void);
 void Draw_BMP(uint8 x0,uint8 y0,uint8 x1,uint8 y1,uint8 bmp[]);
 void OLED_Fill(uint8 dat);
 void Dly_ms(uint16 ms);




/********************************************************************/

#endif
