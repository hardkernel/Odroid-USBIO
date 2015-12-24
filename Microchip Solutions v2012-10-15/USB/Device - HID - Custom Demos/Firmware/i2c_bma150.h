/**********************************************************************
* FileName:        i2c_bma150.h
**********************************************************************/
#ifndef I2C_BMA150_H
#define I2C_BMA150_H
#include <p18cxxx.h>

//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------
#define BMA150_I2C_ADDR 0x70

#define BMA150_ADDR15 0x15
#define BMA150_ADDR14 0x14
#define BMA150_ADDR13 0x13
#define BMA150_ADDR12 0x12
#define BMA150_ADDR11 0x11
#define BMA150_ADDR10 0x10
#define BMA150_ADDR0F 0x0F
#define BMA150_ADDR0E 0x0E
#define BMA150_ADDR0D 0x0D
#define BMA150_ADDR0C 0x0C
#define BMA150_ADDR0B 0x0B
#define BMA150_ADDR0A 0x0A
#define BMA150_ADDR09 0x09

#define BMA150_TEMP 0x08

#define BMA150_ACC_Z_MSB 0x07
#define BMA150_ACC_Z_LSB 0x06
#define BMA150_ACC_Y_MSB 0x05
#define BMA150_ACC_Y_LSB 0x04
#define BMA150_ACC_X_MSB 0x03
#define BMA150_ACC_X_LSB 0x02
#define BMA150_VERSION 0x01
#define BMA150_CHIP_ID 0x00

#define BMA150_READ 0x80
#define BMA150_WRITE 0x00

#define BMA150_RANGE_2G 0
#define BMA150_RANGE_4G 1
#define BMA150_RANGE_8G 2

#define BMA150_BW_25 0x00
#define BMA150_BW_50 0x01
#define BMA150_BW_100 0x02
#define BMA150_BW_190 0x03
#define BMA150_BW_375 0x04
#define BMA150_BW_750 0x05
#define BMA150_BW_1500 0x06

typedef struct {
	BYTE 	head;
	signed short	x;
	signed short	y;
	signed short	z;
	unsigned short	key;
	BYTE 	tail;
}hidraw_packet;

void I2C1_init(void);
void I2C1_deinit(void);

void InitBma150 (void);
void BMA150_ReadXYZ(hidraw_packet *pdata);

#endif