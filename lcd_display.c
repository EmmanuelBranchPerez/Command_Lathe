/*
 * lcd_display.c
 *
 *  Created on: Dec 14, 2025
 *      Author: emmanuel
 */

/*==============================================*/
			/* PRIVATE INCLUDES */
/*==============================================*/
#include "lcd_display.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"
/*==============================================*/

/*==============================================*/
			/* PRIVATE VARIABLES */
/*==============================================*/
extern TIM_HandleTypeDef htim1;
/*==============================================*/

/*==============================================*/
			/* FUNCTION HEADERS */
/*==============================================*/
static void lcd_pcf8574_write(LCD_HandleTypeDef *hlcd, uint8_t data);
static void lcd_write_nibble(LCD_HandleTypeDef *hlcd, uint8_t nibble, uint8_t rs);

/*==============================================*/
			/* FUNCTION ISR */
/*==============================================*/
//Function that writes a byte to the PCF8 with the corresponding data from lcd_write_nibble
static void lcd_pcf8574_write(LCD_HandleTypeDef *hlcd, uint8_t data)
{
    HAL_I2C_Master_Transmit(hlcd->hi2c, hlcd->I2C_ADDRESS, &data, 1, 10); //Transmit's a byte of data to the i2C_adress of the PCF8 (slave adress) with a 10 ms timeout
}

//data variable build -> here we're building what the data transmitted is going to be
static void lcd_write_nibble(LCD_HandleTypeDef *hlcd, uint8_t nibble, uint8_t rs)
{
    uint8_t data = 0; //Local variable to send

    //if the bit_0 of the byte chain is in high mode then we do a mask with the previously made mask on the lcd_display.h, that way we can diferentiate between data send or a command
    if (rs) {
        data |= LCD_RS;
    }

    //Display enable bit, if the display is on then we get the value with the mask
    if (hlcd->DISPLAY_EN) {

        data |= LCD_BL;
    }

    // Llevar nibble (bits 0..3) a D4..D7 (bits 4..7)
    data |= (nibble & 0x0F) << 4;

    //Prepares all the data the data to be read
    lcd_pcf8574_write(hlcd, data);

    //Maintains the EN = 1 enough time so then when it goes down it can read it
    lcd_pcf8574_write(hlcd, data | LCD_EN);
    delay(50);

    //Data capture
    lcd_pcf8574_write(hlcd, data & ~LCD_EN); //Captures higher o lower nibble (4-bit)
    delay(50);
}

void LCD_SendCommand(LCD_HandleTypeDef *hlcd, uint8_t cmd)
{
    uint8_t high_nibble = (cmd >> 4) & 0x0F; //Shifts 4 bits to the right and compares to amask 00001111 that gives the higher nibble
    uint8_t low_nibble  = cmd & 0x0F; //doesn't shifts so gets the lower nibble

    lcd_write_nibble(hlcd, high_nibble, 0); // RS = 0 -> command (Instructions, takes all the higher nibble info to  the 4 bits disposed for the 4 bit data transmission to LCD
    lcd_write_nibble(hlcd, low_nibble, 0);  // RS = 0 -> command (Instrutions), same but with the lower nibble

    // Algunos comandos requieren delays mayores (clear, home)
    if (cmd == 0x01 || cmd == 0x02) { //Delay of some commands
        HAL_Delay(2); // ~2ms
    } else {
    	delay(50);
    }
}

void LCD_SendData(LCD_HandleTypeDef *hlcd, uint8_t data)
{
    uint8_t high_nibble = (data >> 4) & 0x0F; //Makes the same segregation
    uint8_t low_nibble  = data & 0x0F;// same

    lcd_write_nibble(hlcd, high_nibble, 1); // RS = 1 -> data, does the same thing as the command but this time data is stored
    lcd_write_nibble(hlcd, low_nibble, 1);  // RS = 1 -> data, same

    delay(50);
}

void LCD_Clear(LCD_HandleTypeDef *hlcd)
{
    LCD_SendCommand(hlcd, 0x01); // Clear display
}

void LCD_GotoXY(LCD_HandleTypeDef *hlcd, uint8_t row, uint8_t col)
{
	 if (row >= hlcd->ROWS) {
	        row = hlcd->ROWS - 1; //Good practices in order to be sure that the number of ROWs and COLUMNS we send are the same as the display support
	    }
	    if (col >= hlcd->COLUMNS) {
	        col = hlcd->COLUMNS - 1;//Same
	    }

	    uint8_t addr = 0; //Local variable address

	    // Fila 0 → 0x00
	    // Fila 1 → 0x40
	    // Fila 2 → 0x14
	    // Fila 3 → 0x54
	    switch (row) //Define switch case for the information to get in different parts of the display
	    {
	        case 0:
	            addr = 0x00 + col; //We sum up the first row with its column number pre-established as 20
	            break;
	        case 1:
	            addr = 0x40 + col; //same here, its not directl 0x14 because for the LCD is already pre-established 0x40 to be direction of the second row
	            break;
	        case 2:
	            addr = 0x14 + col; //Same
	            break;
	        case 3:
	            addr = 0x54 + col; //Same
	            break;
	        default:
	            addr = 0x00 + col; // Again, goood practices
	            break;
	    }

	    //Set DDRAM address
	    LCD_SendCommand(hlcd, 0x80 | addr); //Here we put the 7 bit of addr in 1 so the PCF8 knows the following is a DDRAM adress
    }

void LCD_Print(LCD_HandleTypeDef *hlcd, const char *str)
{
    while (*str) {
        LCD_SendData(hlcd, (uint8_t)(*str));
        str++;
    }
}

void LCD_Init(LCD_HandleTypeDef *hlcd)
{
    // Backlight ON por defecto
    hlcd->DISPLAY_EN = LCD_BL;

    HAL_Delay(50); // Espera >40ms tras power-on

    //In the HD44780 DATASHEET says that fopr entering th e4-bit mode operatioon we have to make this cycle
    lcd_write_nibble(hlcd, 0x03, 0); //(DB5, DB4 in high)
    HAL_Delay(5);

    lcd_write_nibble(hlcd, 0x03, 0); //Same
    HAL_Delay(5);

    lcd_write_nibble(hlcd, 0x03, 0); //Same

    HAL_Delay(1);

    lcd_write_nibble(hlcd, 0x02, 0); // Ahora estamos en modo 4 bits

    //Function set: 4 bits, 2 rows, 5x8 Datasheet page 24
    LCD_SendCommand(hlcd, 0x28);//    RS  r/w db7 db6 db5 db4(Data Length) db3(Display Lines) db2(Font) db1 db0
    							// 0b  0   0   1   0   1         0                  0             0      -   -

    //Display OFF
    LCD_SendCommand(hlcd, 0x08); // display off, cursor off, blink off

    //Clear display
    LCD_SendCommand(hlcd, 0x01);
    HAL_Delay(2);

    //Entry mode: Increment, not displace
    LCD_SendCommand(hlcd, 0x06); // I/D=1, S=0

    //Display ON, cursor OFF, blink OFF
    LCD_SendCommand(hlcd, 0x0C); // 0b0000 1100

    //Go to origin
    LCD_GotoXY(hlcd, 0, 0);
}

void delay(uint16_t us){//Delay Function that allows us to use microseconds
	__HAL_TIM_SET_COUNTER(&htim1,0);
	while(__HAL_TIM_GET_COUNTER(&htim1) < us);
}
/*==============================================*/

