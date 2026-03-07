/*
 * lcd_display.h
 *
 *  Created on: Dec 14, 2025
 *      Author: emmanuel
 */

#ifndef INC_LCD_DISPLAY_H_
#define INC_LCD_DISPLAY_H_

#include "main.h"

/*=============================================*/
	/* MAPPING DEFINES OF THE LCD DISPLAY */
/*=============================================*/
#define LCD_RS  (1 << 0)    // P0
#define LCD_RW  (1 << 1)    // P1
#define LCD_EN  (1 << 2)    // P2
#define LCD_BL  (1 << 3)    // P3
#define LCD_D4  (1 << 4)    // P4
#define LCD_D5  (1 << 5)    // P5
#define LCD_D6  (1 << 6)    // P6
#define LCD_D7  (1 << 7)    // P7
#define LCD_ADDRR 0x21 //Address
#define LCD_COLUMNS 20 //Column quantity
#define LCD_ROWS 4 //Row quantity
/*==============================================*/

/*==============================================*/
			  /*STRUCTURE DEFINES*/
/*==============================================*/
typedef struct {
	I2C_HandleTypeDef *hi2c;
	uint8_t I2C_ADDRESS;
	uint8_t DISPLAY_EN;
	uint8_t COLUMNS;
	uint8_t ROWS;
}LCD_HandleTypeDef;
/*==============================================*/

/*==============================================*/
			  /*FUNCTION HEADERS*/
/*==============================================*/
void lcd_display_on(LCD_HandleTypeDef *hlcd);
void LCD_Init(LCD_HandleTypeDef *hlcd);
void LCD_SendCommand(LCD_HandleTypeDef *hlcd, uint8_t cmd);
void LCD_SendData(LCD_HandleTypeDef *hlcd, uint8_t data);
void LCD_Clear(LCD_HandleTypeDef *hlcd);
void LCD_GotoXY(LCD_HandleTypeDef *hlcd, uint8_t row, uint8_t col);
void LCD_Print(LCD_HandleTypeDef *hlcd, const char *str);
void delay(uint16_t us);//Function header optimized for a faster communication
/*==============================================*/

#endif /* INC_LCD_DISPLAY_H_ */

