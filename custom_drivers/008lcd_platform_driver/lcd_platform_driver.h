
#ifndef LCD_PLAT_DRIVER_H
#define LCD_PLAT_DRIVER_H

#define LCD_LINES (4+1+1+1)

enum {LCD_RS,LCD_RW,LCD_EN,LCD_D4,LCD_D5,LCD_D6,LCD_D7};

/*Device private data structure */
struct lcd_private_data
{
        int lcd_scroll;
        char lcdxy[8]; /*(16,16) */
 	struct device *dev;
        struct gpio_desc *desc[LCD_LINES];
};


/*Driver private data structure */
struct lcddrv_private_data
{
       struct class *class_lcd;
};

#endif 
