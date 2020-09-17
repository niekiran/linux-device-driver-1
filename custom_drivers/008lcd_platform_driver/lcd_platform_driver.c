#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/gpio/driver.h>
#include<linux/gpio/consumer.h>

#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/delay.h>
#undef pr_fmt
#define pr_fmt(fmt) "%s : " fmt,__func__

#include "lcd_platform_driver.h"
#include "lcd.h"

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("platform driver for 16x2 LCD");



struct lcddrv_private_data drv_data;

struct lcd_private_data lcd_data = { .lcd_scroll = 0 , .lcdxy = "(1,1)" };

static ssize_t lcdcmd_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	long value;
	int status;
	status = kstrtol(buf, 0, &value);
	if(!status)
		lcd_send_command((u8)value,dev);
	return status ? : size;
}

static DEVICE_ATTR_WO(lcdcmd);

static ssize_t lcdtext_store(struct device *dev,struct device_attribute *attr,\
	       						const char *buf, size_t size)
{
	if(buf){
		dev_info(dev,"lcdtext: %s\n",buf);
		lcd_print_string((char*)buf,dev);
	}
	else
		return -EINVAL;

	return size;

}

static DEVICE_ATTR_WO(lcdtext);

static ssize_t lcdscroll_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	int ret;
	struct lcd_private_data *dev_data = dev_get_drvdata(dev);

	if(dev_data->lcd_scroll)
		ret = sprintf(buf, "%s\n", "on");
	else
		ret = sprintf(buf, "%s\n", "off");

	return ret;
}


static ssize_t lcdscroll_store(struct device *dev,struct device_attribute *attr,\
	       					const char *buf, size_t size)
{
	int status = 0;
	struct lcd_private_data *dev_data = dev_get_drvdata(dev);
	
	if (sysfs_streq(buf, "on")){
		dev_data->lcd_scroll = 1;
		/*Display shift left */
		lcd_send_command(0x18,dev);
	}
	else if (sysfs_streq(buf, "off")){
		dev_data->lcd_scroll = 0;
		/*return home */
		lcd_send_command(0x2,dev);
		/*Turn off display shift */
		lcd_send_command(0x10,dev);
	}
	else
		status = -EINVAL;
	
	return status ? : size;
}	

static DEVICE_ATTR_RW(lcdscroll);


static ssize_t lcdxy_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	int status;
	struct lcd_private_data *dev_data = dev_get_drvdata(dev);
	
	status = sprintf(buf,"%s\n",dev_data->lcdxy);
	return status;
}


static ssize_t lcdxy_store(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	long value;
	int status;
	int x, y;
	struct lcd_private_data *dev_data = dev_get_drvdata(dev);
	
	status = kstrtol(buf,10,&value);
	if(status)
		return status;
	x = value /10;
	y = value % 10;
	status = sprintf(dev_data->lcdxy ,"(%d,%d)",x,y);
	lcd_set_cursor(x,y,dev);
	return status;

}

static DEVICE_ATTR_RW(lcdxy);


static struct attribute *lcd_attrs[] = {
	&dev_attr_lcdcmd.attr,
	&dev_attr_lcdtext.attr,
	&dev_attr_lcdscroll.attr,
	&dev_attr_lcdxy.attr,
	NULL
};

struct attribute_group lcd_attr_group = {
	.attrs = lcd_attrs
};


static const struct attribute_group *lcd_attr_groups[] = {
	&lcd_attr_group,
	NULL
};



int device_create_files(struct device *dev, struct lcd_private_data *lcd_data)
{
	lcd_data->dev = device_create_with_groups(drv_data.class_lcd,dev,0,\
			lcd_data,lcd_attr_groups,"LCD16x2");
	if (IS_ERR(lcd_data->dev)) {
		dev_err(dev,"Error while creating class entry \n");
		return PTR_ERR(lcd_data->dev);
	}

	return 0;
}

int lcd_remove(struct platform_device *pdev)
{

	struct lcd_private_data *lcd_data = dev_get_drvdata(&pdev->dev);

	lcd_deinit(&pdev->dev);
	
	dev_info(&pdev->dev,"remove called\n");
	
	device_unregister(lcd_data->dev);
	
	return 0;
}



struct lcd_private_data* get_lcd_private_data(void)
{
	return &lcd_data;

}

int lcd_probe(struct platform_device *pdev)
{
	int  ret;
	struct device *dev = &pdev->dev;


	dev_set_drvdata(dev,&lcd_data);

	lcd_data.desc[LCD_RS] = gpiod_get(dev, "rs", GPIOD_OUT_LOW);
	lcd_data.desc[LCD_RW] = gpiod_get(dev, "rw", GPIOD_OUT_LOW);
	lcd_data.desc[LCD_EN] = gpiod_get(dev, "en", GPIOD_OUT_LOW);
	lcd_data.desc[LCD_D4] = gpiod_get(dev, "d4", GPIOD_OUT_LOW);
	lcd_data.desc[LCD_D5] = gpiod_get(dev, "d5", GPIOD_OUT_LOW);
	lcd_data.desc[LCD_D6] = gpiod_get(dev, "d6", GPIOD_OUT_LOW);
	lcd_data.desc[LCD_D7] = gpiod_get(dev, "d7", GPIOD_OUT_LOW);

	if(IS_ERR(lcd_data.desc[LCD_RS]) || \
			IS_ERR(lcd_data.desc[LCD_RW]) || \
			IS_ERR(lcd_data.desc[LCD_EN]) || \
			IS_ERR(lcd_data.desc[LCD_D4]) || \
			IS_ERR(lcd_data.desc[LCD_D5]) || \
			IS_ERR(lcd_data.desc[LCD_D6]) || \
			IS_ERR(lcd_data.desc[LCD_D7])  )
	{
		dev_err(dev,"Gpio error\n");
		return -EINVAL;
	}

	ret = lcd_init(dev);

	if(ret){
		dev_err(dev,"LCD, init failed \n");
		return ret;
	}

	dev_info(dev,"LCD init successful\n");

	ret = device_create_files(dev,&lcd_data);
	if(ret){
		dev_err(dev,"LCD sysfs dev create failed\n");
		return ret;
	}


	lcd_print_string("16x2 LCD Driver demo",dev);

	dev_info(dev,"probe successful\n");

	return 0;

}


struct of_device_id lcd_device_match[] = {
        { .compatible = "org,lcd16x2"},
        { }
};

struct platform_driver lcdsysfs_platform_driver = 
{
	.probe = lcd_probe,
	.remove = lcd_remove,
	.driver = {
		.name = "lcd-sysfs",
		.of_match_table = of_match_ptr(lcd_device_match)
	}

};

int __init lcd_sysfs_init(void)
{
	drv_data.class_lcd = class_create(THIS_MODULE,"lcd");
	platform_driver_register(&lcdsysfs_platform_driver);
	pr_info("module load success\n");
	return 0;

}


void __exit lcd_sysfs_exit(void)
{
	platform_driver_unregister(&lcdsysfs_platform_driver);
	class_destroy(drv_data.class_lcd);
}


module_init(lcd_sysfs_init);
module_exit(lcd_sysfs_exit);
