/*
 ============================================================================
 Name        : gpio.c
 Version     : 1.0
 Copyright   : Your copyright notice
 Description : simple gpio file handling functions 
 
 ============================================================================
 */
 
#include<linux/gpio/consumer.h>
#include <linux/device.h>

#include "gpio.h"

#include "lcd_platform_driver.h"

extern struct lcd_private_data* get_lcd_private_data(void);

/*
 *  GPIO configure direction
 *  dir_value : 1 means 'out' , 0 means "in"
 */
int gpio_configure_dir(u8 desc_id , u8 dir_value, struct device *dev)
{
	int status;


	struct lcd_private_data *lcd_data = dev_get_drvdata(dev);

	if (dir_value)
		status = gpiod_direction_output(lcd_data->desc[desc_id],0);
	else
		status = gpiod_direction_input(lcd_data->desc[desc_id]);
	
	return status;
    

}

/*
 *  GPIO write value
 *  out_value : can be either 0 or 1
 */
int gpio_write_value(u8 desc_id, u8 out_value,struct device *dev)
{
	struct lcd_private_data *lcd_data = dev_get_drvdata(dev);

        gpiod_set_value(lcd_data->desc[desc_id],out_value);
	return 0;
}

