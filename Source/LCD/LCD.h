/*********************************************************************************************************
*
* File                : LCD.h
* Hardware Environment:
* Build Environment   : RealView MDK-ARM  Version: 4.20
* Version             : V1.0
* By                  :
*
*                                  (c) Copyright 2005-2011, WaveShare
*                                       http://www.waveshare.net
*                                          All Rights Reserved
*
*********************************************************************************************************/

#ifndef __GLCD_H
#define __GLCD_H

/* Includes ------------------------------------------------------------------*/
#include <math.h>
#include "stm32f1xx.h"

/* Private define ------------------------------------------------------------*/
#define USE_16BIT_PMP

/*********************************************************************
* Overview: Horizontal and vertical display resolution
*                  (from the glass datasheet).
*********************************************************************/
//#define DISP_HOR_RESOLUTION				320
//#define DISP_VER_RESOLUTION				240

//#define DISP_ORIENTATION					0
//#define DISP_ORIENTATION					90
#define DISP_ORIENTATION					180
//#define DISP_ORIENTATION					270

/* Private define ------------------------------------------------------------*/

#if  ( DISP_ORIENTATION == 90 ) || ( DISP_ORIENTATION == 270 )

#define  MAX_X  320
#define  MAX_Y  240

#elif  ( DISP_ORIENTATION == 0 ) || ( DISP_ORIENTATION == 180 )

#define  MAX_X  240
#define  MAX_Y  320

#endif

/*********************************************************************
* Overview: Horizontal synchronization timing in pixels
*                  (from the glass datasheet).
*********************************************************************/
//#define DISP_HOR_PULSE_WIDTH		20    /* 20 */
//#define DISP_HOR_BACK_PORCH			51	  /* 48	*/
//#define DISP_HOR_FRONT_PORCH		20	  /* 20 */

/*********************************************************************
* Overview: Vertical synchronization timing in lines
*                  (from the glass datasheet).
*********************************************************************/
//#define DISP_VER_PULSE_WIDTH		2	  /* 2 */
//#define DISP_VER_BACK_PORCH			12	  /* 16 */
//#define DISP_VER_FRONT_PORCH		4	  /* 4 */

/*  PC0.8(RST) PC.1(WR), PC.2(RD) , PC.6(CS), PC.7(RS)*/
/* PA.3(BL_PWM),*/

#define GPIO_Pin_0  (1U << 0)
#define GPIO_Pin_1  (1U << 1)
#define GPIO_Pin_2  (1U << 2)
#define GPIO_Pin_3  (1U << 3)
#define GPIO_Pin_4  (1U << 4)
#define GPIO_Pin_5  (1U << 5)
#define GPIO_Pin_6  (1U << 6)
#define GPIO_Pin_7  (1U << 7)
#define GPIO_Pin_8  (1U << 8)
#define GPIO_Pin_9  (1U << 9)
#define GPIO_Pin_10 (1U << 10)
#define GPIO_Pin_11 (1U << 11)
#define GPIO_Pin_12 (1U << 12)
#define GPIO_Pin_13 (1U << 13)
#define GPIO_Pin_14 (1U << 14)
#define GPIO_Pin_15 (1U << 15)

#define Set_Cs        GPIOC->BSRR = GPIO_Pin_6
#define Clr_Cs        GPIOC->BRR = GPIO_Pin_6

#define Set_Rs        GPIOC->BSRR = GPIO_Pin_7
#define Clr_Rs        GPIOC->BRR = GPIO_Pin_7

#define Set_nWr       GPIOC->BSRR = GPIO_Pin_1
#define Clr_nWr       GPIOC->BRR = GPIO_Pin_1

#define Set_nRd       GPIOC->BSRR = GPIO_Pin_2
#define Clr_nRd       GPIOC->BRR = GPIO_Pin_2

/* LCD color */
#define White          0xFFFF
#define Black          0x0000
#define Grey           0xF7DE
#define Blue           0x001F
#define Blue2          0x051F
#define Red            0xF800
#define Magenta        0xF81F
#define Green          0x07E0
#define Cyan           0x7FFF
#define Yellow         0xFFE0

#define RGB565CONVERT(red, green, blue) (int) (((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3))

/* Private function prototypes -----------------------------------------------*/
void LCD_Initializtion(void);
void LCD_Clear(uint16_t Color);
//void LCD_SetBacklight(uint8_t intensity);
uint16_t LCD_GetPoint(uint16_t Xpos, uint16_t Ypos);
void LCD_SetPoint(uint16_t Xpos, uint16_t Ypos, uint16_t point);
void PutChar(uint16_t Xpos, uint16_t Ypos, uint8_t c, uint16_t charColor, uint16_t bkColor);
void LCD_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1 , uint16_t color);
void PutChinese(uint16_t Xpos, uint16_t Ypos, uint8_t *str, uint16_t Color, uint16_t bkColor);
void GUI_Text(uint16_t Xpos, uint16_t Ypos, uint8_t *str, uint16_t Color, uint16_t bkColor);
void GUI_Chinese(uint16_t Xpos, uint16_t Ypos, uint8_t *str, uint16_t Color, uint16_t bkColor);
void LCD_DrawPicture(uint16_t StartX, uint16_t StartY, uint16_t EndX, uint16_t EndY, uint16_t *pic);


void LCD_WriteIndex(uint16_t index);
void LCD_WriteData(uint16_t data);
uint16_t LCD_ReadData(void);
uint16_t LCD_ReadReg(uint16_t LCD_Reg);
void LCD_WriteReg(uint16_t LCD_Reg, uint16_t LCD_RegValue);


#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

