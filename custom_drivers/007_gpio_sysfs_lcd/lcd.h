/*
 * lcd_driver.h
 *
 *  Created on: 
 */

#ifndef LCD_DRIVER_H_
#define LCD_DRIVER_H_

/* BBB GPIO numbers used to connect with LCD pins */

#if 1
#define GPIO_LCD_RS   "gpio2.2"   /*  Register selection (Character vs. Command) */ 
#define GPIO_LCD_RW   "gpio2.7"   /*  Read/write     */
#define GPIO_LCD_EN   "gpio2.8"   /*  Enable */
#define GPIO_LCD_D4   "gpio2.9"   /*  Data line 4    */
#define GPIO_LCD_D5   "gpio2.10"  /*  Data line 5    */
#define GPIO_LCD_D6   "gpio2.11"  /*  Data line 6    */
#define GPIO_LCD_D7   "gpio2.12"  /*  Data line 7    */
#endif


#if 0
#define GPIO_LCD_RS   "gpio2.2"   /*  Register selection (Character vs. Command) */ 
//#define GPIO_LCD_RW   "gpio2.7"   /*  Read/write     */
#define GPIO_LCD_EN   "gpio2.3"   /*  Enable */
#define GPIO_LCD_D4   "gpio2.5"   /*  Data line 4    */
#define GPIO_LCD_D5   "gpio2.4"  /*  Data line 5    */
#define GPIO_LCD_D6   "gpio1.13"  /*  Data line 6    */
#define GPIO_LCD_D7   "gpio1.12"  /*  Data line 7    */
#endif 

/*LCD commands */
#define LCD_CMD_4DL_2N_5X8F  		0x28
#define LCD_CMD_DON_CURON    		0x0E
#define LCD_CMD_INCADD       		0x06
#define LCD_CMD_DIS_CLEAR    		0X01
#define LCD_CMD_DIS_RETURN_HOME  	0x02


/*Sets CGRAM address. CGRAM data is sent and received after this setting. */
#define LCD_CMD_SET_CGRAM_ADDRESS  			0x40

/* Sets DDRAM address. DDRAM data is sent and received after this setting. */
#define LCD_CMD_SET_DDRAM_ADDRESS  			0x80

#define DDRAM_SECOND_LINE_BASE_ADDR         	(LCD_CMD_SET_DDRAM_ADDRESS | 0x40 )
#define DDRAM_FIRST_LINE_BASE_ADDR          	LCD_CMD_SET_DDRAM_ADDRESS


#define LCD_ENABLE 1
#define LCD_DISABLE 0




//public function prototypes
void lcd_deinit(void);
void lcd_init(void);
void lcd_set_cursor(uint8_t row, uint8_t column);
void lcd_enable(void);
void lcd_print_char(uint8_t ascii_Value);
void lcd_print_string(char *message);
void lcd_send_command(uint8_t command);
void lcd_load_cgram(char tab[], uint8_t charnum);
void lcd_display_clear(void);
void lcd_printf(const char *fmt, ...);
void lcd_display_return_home(void);
int is_lcd_busy(void);

#endif /* LCD_DRIVER_H_ */
