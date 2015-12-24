/**********************************************************************
* FileName:        i2c_ina231.c
**********************************************************************/

#include "p18cxxx.h"
#include "HardwareProfile.h"
#include "GenericTypeDefs.h"
#include "stdio.h"

#include "i2c.h"
#include "i2c_bma150.h"

//------------------------------------------------------------------
//------------------------------------------------------------------
void I2C1_init(void)
{
	TRISBbits.TRISB2 = 0; // input ALERT line  
	LATBbits.LATB2 = 0;
	
	TRISBbits.TRISB0 = 1; // input SDA line  
	TRISBbits.TRISB1 = 1; // input SCK line  
	WPUBbits.WPUB0 = 1;   // Pull-Up
	WPUBbits.WPUB1 = 1;   // Pull-Up
	
	SSP1STATbits.SMP = 1;
	SSP1STATbits.CKE = 0;  
	SSPADD = 0xBF; // 200 Kz for slave
	SSP1CON1 = 0b00101000; // mode master
	SSP1CON2 = 0b00000000;  

	IdleI2C1();
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void I2C1_deinit(void)
{
	SSP1CON1 = 0b00001000; // SSP1EN  disable
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void BMA150_WriteByte(BYTE address, BYTE data)
{
	IdleI2C1();
	StartI2C1();
	WriteI2C1((unsigned char)BMA150_I2C_ADDR);
	WriteI2C1(address);
	WriteI2C1(data);
	StopI2C1();
}

//------------------------------------------------------------------
//------------------------------------------------------------------
BYTE BMA150_ReadByte(BYTE address)
{
	BYTE read_data=0;

	IdleI2C1();                      // ensure module is idle
	StartI2C1();                     // initiate START condition
	WriteI2C1(BMA150_I2C_ADDR);
	WriteI2C1(address);
	RestartI2C1();
	WriteI2C1((0x01)|BMA150_I2C_ADDR);
	read_data = ReadI2C1();
	NotAckI2C1();
	StopI2C1();
	
	return read_data;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void BMA150_ReadXYZ(hidraw_packet *pdata)
{
	BYTE temp;
	
	temp = (BMA150_ReadByte (0x02)>>6);
	pdata->x = (signed char)BMA150_ReadByte (0x03);
	pdata->x <<= 2;
	pdata->x |= temp;

	temp = (BMA150_ReadByte (0x04)>>6);
	pdata->y = (signed char)(BMA150_ReadByte (0x05));
	pdata->y <<= 2;
	pdata->y |= temp;

	temp = (BMA150_ReadByte (0x06)>>6);
	pdata->z = (signed char)(BMA150_ReadByte (0x07));
	pdata->z <<= 2;
	pdata->z |= temp;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void InitBma150 (void)
{
	BYTE reg_data;

	reg_data = BMA150_ReadByte(BMA150_CHIP_ID);
	if(reg_data != 0x2) 
		while(1);

	reg_data = (BMA150_RANGE_2G << 3) | BMA150_BW_100;
	BMA150_WriteByte(BMA150_ADDR14, reg_data);

	BMA150_WriteByte(BMA150_ADDR11, 0x00);
	BMA150_WriteByte(BMA150_ADDR0B, 0x00);

	return;
}
