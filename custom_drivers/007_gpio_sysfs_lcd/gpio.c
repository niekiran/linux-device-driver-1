/*
 ============================================================================
 Name        : gpio.c
 Version     : 1.0
 Copyright   : Your copyright notice
 Description : simple gpio file handling functions 
 
 ============================================================================
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include<string.h>
#include<stdlib.h>
#include "gpio.h"


/*
 *  GPIO configure direction
 *  dir_value : 1 means 'out' , 0 means "in"
 */
int gpio_configure_dir(char *gpio_label, uint8_t dir_value)
{
    int fd;
    char buf[SOME_BYTES];

    snprintf(buf, sizeof(buf), SYS_GPIO_PATH "/%s/direction", gpio_label);

    /* opens the sysfs attribute 'direcito' in the provided sysfs path */
    fd = open(buf, O_WRONLY | O_SYNC);
    if (fd < 0) {
        perror("gpio direction configure\n");
        return fd;
    }

    /* Writes to sysfs attribute 'direction' */
    if (dir_value)
        write(fd, "out", 4); //3+1  +1 for NULL character 
    else
        write(fd, "in", 3);

    close(fd);

    return 0;
}

/*
 *  GPIO write value
 *  out_value : can be either 0 or 1
 */
int gpio_write_value(char *gpio_label, uint8_t out_value)
{
    int fd;
    char buf[SOME_BYTES];

    snprintf(buf, sizeof(buf), SYS_GPIO_PATH "/%s/value", gpio_label);

    fd = open(buf, O_WRONLY | O_SYNC);
    if (fd < 0) {
        perror("gpio write value\n");
        return fd;
    }

    if (out_value)
        write(fd, "1", 2);
    else
        write(fd, "0", 2);

    close(fd);
    return 0;
}

/*
 *  GPIO read value
 */
int gpio_read_value(char *gpio_label)
{
    int fd;
    uint8_t read_value=0;
    char buf[SOME_BYTES];

    snprintf(buf, sizeof(buf), SYS_GPIO_PATH "/%s/value", gpio_label);

    fd = open(buf, O_RDONLY | O_SYNC);
    if (fd < 0) {
        perror("gpio read value\n");
        return fd;
    }

    read(fd, &read_value, 1);

    read_value = atoi(&read_value);

    close(fd);

    return read_value;
}


/*
 *  Opens the sysfs file corresponding to gpio label
 */
int gpio_file_open(char *gpio_label)
{
    int fd;
    char buf[SOME_BYTES];

    snprintf(buf, sizeof(buf), SYS_GPIO_PATH "/%s/value", gpio_label);

    fd = open(buf, O_RDONLY | O_NONBLOCK );

    if (fd < 0) {
        perror("gpio file open\n");
    }

    return fd;
}

/*
 *  close a file
 */
int gpio_file_close(int fd)
{
    return close(fd);
}
