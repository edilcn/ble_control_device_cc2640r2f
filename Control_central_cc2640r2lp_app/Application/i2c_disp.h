/*
 * i2c_disp.h
 *
 *  Created on: 21 de jul de 2018
 *      Author: Edilberto Costa Neto
 */

#ifndef APPLICATION_I2C_DISP_H_
#define APPLICATION_I2C_DISP_H_


/* Std Includes */
//#include <unistd.h>
#include <stdint.h>
#include <stddef.h>

/* XDC module Headers */
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/Error.h>

/* TI-RTOS kernel */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>

/* Board Header file */
#include "Board.h"


/* Constants */
#define SSD1306_OLED_SLAVE_ADDRESS 0x3C  // 011110<0><R/W#>

/* Prototypes */

void i2c_ssd1306_init();
bool ssd1306_command(I2C_Handle i2c, UInt8 cmd);
bool ssd1306_writedata(I2C_Handle i2c, Void * data, size_t count);
void ssd1306_clearScreen(I2C_Handle i2c);

#endif /* APPLICATION_I2C_DISP_H_ */
