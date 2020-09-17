
#include <linux/delay.h>
#include "lcd_platform_driver.h"
#include "lcd.h"
#include "gpio.h"

static void write_4_bits(uint8_t data,struct device *dev);


void lcd_deinit(struct device *dev)
{
	lcd_display_clear(dev);      	  /* Clear display */
	lcd_display_return_home(dev);        /* Cursor at home position */
}



/* This function does basic initialization for your lcd  */
int lcd_init(struct device *dev)
{

	gpio_configure_dir(LCD_RS,GPIO_DIR_OUT,dev);
	gpio_configure_dir(LCD_RW,GPIO_DIR_OUT,dev);
	gpio_configure_dir(LCD_EN,GPIO_DIR_OUT,dev);
	gpio_configure_dir(LCD_D4,GPIO_DIR_OUT,dev);
	gpio_configure_dir(LCD_D5,GPIO_DIR_OUT,dev);
	gpio_configure_dir(LCD_D6,GPIO_DIR_OUT,dev);
	gpio_configure_dir(LCD_D7,GPIO_DIR_OUT,dev);

	gpio_write_value(LCD_RS,GPIO_LOW_VALUE,dev);
	gpio_write_value(LCD_EN,GPIO_LOW_VALUE,dev);

	gpio_write_value(LCD_D4,GPIO_LOW_VALUE,dev);
	gpio_write_value(LCD_D5,GPIO_LOW_VALUE,dev);
	gpio_write_value(LCD_D6,GPIO_LOW_VALUE,dev);
	gpio_write_value(LCD_D7,GPIO_LOW_VALUE,dev);

	mdelay(40);
	
	/* RS=0 for LCD command */
	gpio_write_value(LCD_RS,LOW_VALUE,dev);
	
	/*R/nW = 0, for write */
	gpio_write_value(LCD_RW,LOW_VALUE,dev);
	
	write_4_bits(0x03,dev);
	mdelay(5);
	
	write_4_bits(0x03,dev);
	udelay(150);
	
	write_4_bits(0x03,dev);
	write_4_bits(0x02,dev);

    /*4 bit data mode, 2 lines selection , font size 5x8 */
	lcd_send_command(LCD_CMD_4DL_2N_5X8F,dev);
	
	/* Display ON, Cursor ON */
	lcd_send_command(LCD_CMD_DON_CURON,dev);
	
	lcd_display_clear(dev);
	
	/*Address auto increment*/
	lcd_send_command(LCD_CMD_INCADD,dev);

	return 0;

}

/*Clear the display */
void lcd_display_clear(struct device *dev)
{
	lcd_send_command(LCD_CMD_DIS_CLEAR,dev);
	/*
	 * check page number 24 of datasheet.
	 * display clear command execution wait time is around 2ms
	 */
	mdelay(2);
}

/*Cursor returns to home position */
void lcd_display_return_home(struct device *dev)
{

	lcd_send_command(LCD_CMD_DIS_RETURN_HOME,dev);
	/*
	 * check page number 24 of datasheet.
	 * return home command execution wait time is around 2ms
	 */
	mdelay(2);
}


/**
  * @brief  Set Lcd to a specified location given by row and column information
  * @param  Row Number (1 to 2)
  * @param  Column Number (1 to 16) Assuming a 2 X 16 characters display
  */
void lcd_set_cursor(u8 row, u8 column,struct device *dev)
{
  column--;
  switch (row)
  {
    case 1:
      /* Set cursor to 1st row address and add index*/
      lcd_send_command((column |= DDRAM_FIRST_LINE_BASE_ADDR),dev);
      break;
    case 2:
      /* Set cursor to 2nd row address and add index*/
        lcd_send_command((column |= DDRAM_SECOND_LINE_BASE_ADDR),dev);
      break;
    default:
      break;
  }
}

/*
 * @brief call this function to make LCD latch the data lines in to its internal registers.
 */
void lcd_enable(struct device *dev)
{ 
	gpio_write_value(LCD_EN,LOW_VALUE,dev);
	udelay(10);
	gpio_write_value(LCD_EN,HIGH_VALUE,dev);
	udelay(10);
	gpio_write_value(LCD_EN,LOW_VALUE,dev);
	udelay(100); /* execution time > 37 micro seconds */
}



/*
 *This function sends a character to the LCD 
 *Here we used 4 bit parallel data transmission. 
 *First higher nibble of the data will be sent on to the data lines D4,D5,D6,D7
 *Then lower niblle of the data will be set on to the data lines D4,D5,D6,D7
 */
void lcd_print_char(uint8_t data, struct device *dev)
{

	//RS=1, for user data
	gpio_write_value(LCD_RS,HIGH_VALUE,dev);
	
	/*R/nW = 0, for write */
	gpio_write_value(LCD_RW,LOW_VALUE,dev);
	
	write_4_bits((data >> 4),dev); /* higher nibble */
	write_4_bits(data,dev);      /* lower nibble */
}

void lcd_print_string(char *message,struct device *dev)
{

      do
      {
          lcd_print_char((u8)*message++,dev);
      }
      while (*message != '\0');

}


/*
 *This function sends a command to the LCD 
 */
void lcd_send_command(uint8_t command,struct device *dev)
{
	/* RS=0 for LCD command */
	gpio_write_value(LCD_RS,LOW_VALUE,dev);
	
	/*R/nW = 0, for write */
	gpio_write_value(LCD_RW,LOW_VALUE,dev);
	
	write_4_bits((command >> 4),dev); /* higher nibble */
	write_4_bits(command,dev);     /* lower nibble */

}

/* writes 4 bits of data/command on to D4,D5,D6,D7 lines */
static void write_4_bits(uint8_t data,struct device *dev)
{
	/* 4 bits parallel data write */
	gpio_write_value(LCD_D4, ((data >> 0 ) & 0x1),dev);
	gpio_write_value(LCD_D5, ((data >> 1 ) & 0x1),dev);
	gpio_write_value(LCD_D6, ((data >> 2 ) & 0x1),dev);
	gpio_write_value(LCD_D7, ((data >> 3 ) & 0x1),dev);
	
	lcd_enable(dev);
	
}

