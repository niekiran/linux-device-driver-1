#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <unistd.h>

#include "gpio.h"
#include "lcd.h"



static void write_4_bits(uint8_t data);



void lcd_deinit(void)
{
	lcd_display_clear();      	  /* Clear display */
	lcd_display_return_home();        /* Cursor at home position */
}

/* 
 * LCD initalization sequence . 
 * This is written as per the HITACHI HD44780U datasheet 
 * https://www.sparkfun.com/datasheets/LCD/HD44780.pdf
 * Ref. page number 46 , Figure 24 for intialization sequence for 4-bit interface 
 */
void lcd_init(void)
{
	usleep(40 * 1000);
	
	/* RS=0 for LCD command */
	gpio_write_value(GPIO_LCD_RS,LOW_VALUE);
	
	/*R/nW = 0, for write */
	gpio_write_value(GPIO_LCD_RW,LOW_VALUE);
	
	write_4_bits(0x03);
	usleep( 5 * 1000);
	
	write_4_bits(0x03);
	usleep(100);
	
	write_4_bits(0x03);
	write_4_bits(0x02);

    /*4 bit data mode, 2 lines selection , font size 5x8 */
	lcd_send_command(LCD_CMD_4DL_2N_5X8F);
	
	/* Display ON, Cursor ON */
	lcd_send_command(LCD_CMD_DON_CURON);
	
	lcd_display_clear();
	
	/*Address auto increment*/
	lcd_send_command(LCD_CMD_INCADD);	
}

/*Clear the display */
void lcd_display_clear(void)
{
	lcd_send_command(LCD_CMD_DIS_CLEAR);
	/*
	 * check page number 24 of datasheet.
	 * display clear command execution wait time is around 2ms
	 */
	usleep(2000); 
}

/*Cursor returns to home position */
void lcd_display_return_home(void)
{

	lcd_send_command(LCD_CMD_DIS_RETURN_HOME);
	/*
	 * check page number 24 of datasheet.
	 * return home command execution wait time is around 2ms
	 */
	usleep(2000);
}


/**
  * @brief  Set Lcd to a specified location given by row and column information
  * @param  Row Number (1 to 2)
  * @param  Column Number (1 to 16) Assuming a 2 X 16 characters display
  */
void lcd_set_cursor(uint8_t row, uint8_t column)
{
	column--;
	switch (row)
	{
		case 1:
			/* Set cursor to 1st row address and add index*/
			lcd_send_command(column |= DDRAM_FIRST_LINE_BASE_ADDR);
		break;
		case 2:
			/* Set cursor to 2nd row address and add index*/
			lcd_send_command(column |= DDRAM_SECOND_LINE_BASE_ADDR);
		break;
		default:
		break;
	}
}

/* writes 4 bits of data/command on to D4,D5,D6,D7 lines */
static void write_4_bits(uint8_t data)
{
	/* 4 bits parallel data write */
	gpio_write_value(GPIO_LCD_D4, (data >> 0 ) & 0x1);
	gpio_write_value(GPIO_LCD_D5, (data >> 1 ) & 0x1);
	gpio_write_value(GPIO_LCD_D6, (data >> 2 ) & 0x1);
	gpio_write_value(GPIO_LCD_D7, (data >> 3 ) & 0x1);
	
	lcd_enable();
	
}
/*
 * @brief call this function to make LCD latch the data lines in to its internal registers.
 */
void lcd_enable(void)
{ 
	gpio_write_value(GPIO_LCD_EN,LOW_VALUE);
	usleep(1);
	gpio_write_value(GPIO_LCD_EN,HIGH_VALUE);
	usleep(1);
	gpio_write_value(GPIO_LCD_EN,LOW_VALUE);
	usleep(100); /* execution time > 37 micro seconds */
}

/*
 *This function sends a character to the LCD 
 *Here we used 4 bit parallel data transmission. 
 *First higher nibble of the data will be sent on to the data lines D4,D5,D6,D7
 *Then lower niblle of the data will be set on to the data lines D4,D5,D6,D7
 */
void lcd_print_char(uint8_t data)
{

	//RS=1, for user data
	gpio_write_value(GPIO_LCD_RS,HIGH_VALUE);
	
	/*R/nW = 0, for write */
	gpio_write_value(GPIO_LCD_RW,LOW_VALUE);
	
	write_4_bits(data >> 4); /* higher nibble */
	write_4_bits(data);      /* lower nibble */
}

void lcd_print_string(char *message)
{

      do
      {
          lcd_print_char((uint8_t)*message++);
      }
      while (*message != '\0');

}


/*
 *This function sends a command to the LCD 
 */
void lcd_send_command(uint8_t command)
{
	/* RS=0 for LCD command */
	gpio_write_value(GPIO_LCD_RS,LOW_VALUE);
	
	/*R/nW = 0, for write */
	gpio_write_value(GPIO_LCD_RW,LOW_VALUE);
	
	write_4_bits(command >> 4); /* higher nibble */
	write_4_bits(command);     /* lower nibble */

}

void lcd_printf(const char *fmt, ...)
{
      int i;
      uint32_t text_size, letter;
      static char text_buffer[32];
      va_list args;

      va_start(args, fmt);
      text_size = vsprintf(text_buffer, fmt, args);

      // Process the string
      for (i = 0; i < text_size; i++)
      {
        letter = text_buffer[i];

        if (letter == 10)
          break;
        else
        {
          if ((letter > 0x1F) && (letter < 0x80))
              lcd_print_char(letter);
        }
      }
  }
