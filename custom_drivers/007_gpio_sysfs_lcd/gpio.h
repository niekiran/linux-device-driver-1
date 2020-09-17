/*
 * lcd_driver.h
 *
 *  Created on: 
 *      Author: 
 */

#ifndef GPIO_DRIVER_H_
#define GPIO_DRIVER_H_


#define SOME_BYTES          100

/* This is the path corresponds to GPIOs in the 'sys' directory */
#define SYS_GPIO_PATH       "/sys/class/bone_gpios"

#define HIGH_VALUE          1
#define LOW_VALUE           0

#define GPIO_DIR_OUT        HIGH_VALUE
#define GPIO_DIR_IN         LOW_VALUE

#define GPIO_LOW_VALUE      LOW_VALUE
#define GPIO_HIGH_VALUE     HIGH_VALUE

//public function prototypes .
int gpio_configure_dir(char *label, uint8_t dir_value);
int gpio_write_value(char *label, uint8_t out_value);
int gpio_read_value(char *label);
int gpio_file_open(char *label);
int gpio_file_close(int fd);

#endif /* GPIO_DRIVER_H_ */
