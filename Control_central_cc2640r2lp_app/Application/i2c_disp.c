/*
 * i2c_disp.c
 *
 *  Created on: 21 de jul de 2018
 *      Author: edilc
 */
#include <i2c_disp.h>
#include <Board.h>

uint8_t ssd1306_init0_sequence[] = {
        0xAE,
        0xD5,
        0x80,
        0xA8,
        63,
        0xD3,
        0x00,
        0x40,
        0x8D,
        0x14,
        0x20,
        0x02, // PAGE ADDRESSING MODE
        0xA1,
        0xC8,
        0xDA,
        0x12,
        0x81,
        0xCF,
        0xD9,
        0xF1,
        0xDB,
        0x40,
        0xA4,
        0xA6
};


void i2c_ssd1306_init(I2C_Handle i2c, I2C_Params iP){
    GPIO_init();
    I2C_init();
    int i;
    I2C_Params_init(&iP);
    i2c = I2C_open(Board_I2C_TMP, &iP);
    if (i2c == NULL) {
        System_abort("I2C_open failed\n");
    }

    // Wait 10ms
    Task_sleep(10000 / Clock_tickPeriod);
    System_printf("Beginning OLED init-\n"); System_flush();

    for (i=0; i < sizeof(ssd1306_init0_sequence); i++) {
        ssd1306_command(i2c, ssd1306_init0_sequence[i]);
    }
    System_printf("done\n"); System_flush();
}

bool ssd1306_command(I2C_Handle i2c, uint8_t cmd){
    uint8_t buf[2];

       buf[0] = 0;
       buf[1] = cmd;
       return ssd1306_writedata(i2c, &buf[0], 2);
}

bool ssd1306_writedata(I2C_Handle i2c, void * data, size_t count){
    I2C_Transaction txn;
        txn.writeBuf = (void *)data;
        txn.writeCount = count;
        txn.readCount = 0;
        txn.readBuf = NULL;
        txn.slaveAddress = 0x3C;
    return I2C_transfer(i2c, &txn);
}

void ssd1306_clearScreen(I2C_Handle i2c)
{
    UInt8 buf[129];
    Int i;
    I2C_Transaction txn;
    txn.writeBuf = (void *)buf;
    txn.writeCount = 129;
    txn.readCount = 0;
    txn.readBuf = NULL;
    txn.slaveAddress = SSD1306_OLED_SLAVE_ADDRESS;
    buf[0] = 0x40;
    for (i=1; i < 129; i++) {
        buf[i] = 255;
    }
    for (i=0; i < 8; i++) {
        ssd1306_command(i2c, 0xB0 | i);
        ssd1306_command(i2c, 0x00);
        ssd1306_command(i2c, 0x10);
        I2C_transfer(i2c, &txn);
    }
}
