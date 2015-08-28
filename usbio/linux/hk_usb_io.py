#!/usr/bin/python
import usb.core
import usb.util
from array import array
import sys
import time
# Markham Thomas 2015
# This version is under heavy development 
# You can add your test code to the bottom
# below the === end of module statement, or 
# externally call the module
# 
# ---- This version adds a python class for constants

class Bunch(dict):
	def __init__(self, d = {}):
		dict.__init__(self, d)
		self.__dict__.update(d)
	def __setattr__(self, name, value):
		dict.__setitem__(self, name, value)
		object.__setattr__(self, name, value)
	def __setitem__(self, name, value):
		dict.__setitem__(self, name, value)
		object.__setattr__(self, name, value)
	def copy(self):
		return Bunch(dict.copy(self))
_mod_ver  = '0.52'	# python HKUSBIO module version
_mod_date = '8/28/2015'	# module date
u_ad0 = 0x37	# read ADC value from RA0
u_ad1 = 0x38	# read ADC value from RA1
u_i2c_init = 0x40	# i2c_init(void)
u_i2c_idle = 0x41	# i2c_idle(void)
u_i2c_strt = 0x42	# i2c_start(uchar)
u_i2c_stop = 0x43	# i2c_stop(void)
u_i2c_slak = 0x44	# uchar i2c_slave_ack(void)
u_i2c_writ = 0x45	# void i2c_write(uchar)
u_i2c_mack = 0x46	# void i2c_master_ack(uchar)
u_i2c_read = 0x47	# uchar i2c_read(void)
u_i2c_dtrd = 0x48	# uchar i2c_isdatardy(void)
u_spi_init = 0x50	# void spi_init(mode, baud, sample)
u_spi_tran = 0x51	# uchar spi_transfer(regAddr)
u_spi_cs   = 0x52	# void spi_cs(enable|disable)
u_rom = 0x85	# get PIC rom version
u_led = 0x80	# toggle LED 
u_swc = 0x81	# get switch pressed or not
u_gpd = 0x84	# configure GPIO direction on a pin
u_gpi = 0x82	# read value on GPIO pin
u_gpo = 0x83	# write value to GPIO pin
u_uss = 0x86	# send a string to the UART
u_tst = 0x87	# test if UART has a char available
u_urc = 0x88	# read a single char from UART
u_usc = 0x89	# send a single char to the UART
h_getr = 0x98	# SFR register to read
h_setr = 0x99	# SFR register to set
h_getb = 0x9a	# SFR read register bit
h_setb = 0x9b	# SFR set register bit
rd4 = 1		# GPIO pin rd4	def=input
rd5 = 2		# GPIO pin rd5	def=input
rd6 = 3		# GPIO pin rd6	def=output
rd7 = 4		# GPIO pin rd7	def=output
dir_output = 0	# control GPIO pin direction
dir_input  = 1
SPI_LOW_BAUD = 0# 750khz
SPI_MED_BAUD = 1# 3mhz
SPI_HI_BAUD  = 2# 12mhz
SPI_SAMP_MID = 0# sample input in middle data input time
SPI_SAMP_END = 1# sample input at end of data input
SPI_MODE0    = 0
SPI_MODE1    = 1
SPI_MODE2    = 2
SPI_MODE3    = 3
SPI_CS_ENABLE  = 0
SPI_CS_DISABLE = 1
I2C_DATA_ACK 		= 0	# i2c constants
I2C_DATA_NOACK 		= 1
I2C_WRITE_CMD 		= 0
I2C_READ_CMD 		= 1
I2C_START_CMD 		= 0
I2C_REP_START_CMD 	= 1
I2C_REQ_ACK 		= 0
I2C_REQ_NOACK 		= 0
def init():				# setup USB device structure
	# find our device
	dev = usb.core.find(idVendor=0x04d8, idProduct=0x003f)
	# was it found
	if dev is None:
		raise ValueError('Device not found')
	# handle if device is busy
	if dev.is_kernel_driver_active(0) is True:
		dev.detach_kernel_driver(0)
	# set the active configuration.  No args the first config
	# will become the active one
	dev.set_configuration()
	return dev
def module_version():
	a = 'Version: ' + _mod_ver + ', Date: ' + _mod_date
	return a
def rom_version(dev):			# get PIC ROM version
	# read ROM version 
	dev.write(1, [u_rom], 100)
	ret = dev.read(0x81, 64, 100)
	rom_version = ''
	rom_version += chr(ret[1])
	rom_version += '.'
	rom_version += chr(ret[2])
	rom_version += chr(ret[3])
	return rom_version
def toggle_led(dev):			# toggle LED
	dev.write(1, [u_led], 100)
def read_switch(dev):			# read switch press
	dev.write(1, [u_swc], 100)
	sw = dev.read(0x81, 64, 100)
	if (sw[1] == 0):
		 return True
	else:
		 return False
def gpio_init(dev,pin,pdir):		# set GPIO direction on pin
	dev.write(1,[u_gpd, pin, pdir], 100)
def gpio_out(dev,pin):			# otuput a value on GPIO pin
	dev.write(1, [u_gpo, pin, 1], 100)
def gpio_in(dev,pin):			# read value on GPIO pin
	dev.write(1,[u_gpi, pin], 100)
	ret = dev.read(0x81, 64, 100)
	return ret[1]
def adc_ra0(dev):			# do ADC conversion on RA0
	dev.write(1,[u_ad0], 100)
	ret = dev.read(0x81, 64, 100)
	value = ret[2] << 8
	value = value | ret[1]
	return value
def adc_ra1(dev):			# do ADC conversion on RA1
	dev.write(1,[u_ad1], 100)
	ret = dev.read(0x81, 64, 100)
	value = ret[2] << 8
	value = value | ret[1]
	return value
def ser_test(dev):			# check if a char available on serial port
	dev.write(1, [u_tst], 100)
	ret = dev.read(0x81, 64, 100)
	return ret[1]
def ser_putc(dev,schar):		# send a char to the serial port
	a = map( ord, schar)
	a.insert(0, u_usc)
	dev.write(1, a, 100)
def ser_puts(dev, strval):		# send a string to the serial port
	a = map( ord, strval)
	a.insert(0, u_uss)
	a.append(0)
	dev.write(1, a, 100)
def ser_getc(dev):			# get a single char from the serial port
	dev.write(1, [u_urc], 100)
	ret = dev.read(0x81, 64, 100)
	return ret[1]
def sfr_get_reg(dev, reg):		# get a SFR register
	a = array('B',[0,0,0,0,0,0,0,0,0,0,0,0,0,0])
	a[10] = reg
	a[0] = h_getr
	dev.write(1, a, 100)
	ret = dev.read(0x81, 64, 100)
	return ret[1]
def sfr_set_reg(dev, reg, rval):	# set a SFR register
	a = array('B',[0,0,0,0,0,0,0,0,0,0,0,0,0,0])
	a[10] = reg			# register to select
	a[11] = rval			# value to set
	a[0] = h_setr
	dev.write(1, a, 100)
	ret = dev.read(0x81, 64, 100)
	return ret[1]
def sfr_get_regbit(dev, reg, bval):	# get a SFR register bit
	a = array('B',[0,0,0,0,0,0,0,0,0,0,0,0,0,0])
	a[10] = reg			# register to select
	a[11] = bval			# bit value to get
	a[0] = h_getb
	dev.write(1, a, 100)
	ret = dev.read(0x81, 64, 100)
	return ret[1]
def sfr_set_regbit(dev, reg, rbit, bval):	# set a SFR register bit
	a = array('B',[0,0,0,0,0,0,0,0,0,0,0,0,0,0])
	a[10] = reg			# register to select
	a[11] = rbit			# bit to set
	a[12] = bval			# bit value to set
	a[0]  = h_setb
	dev.write(1, a, 100)
	ret = dev.read(0x81, 64, 100)
	return ret[1]
def i2c_init(dev):			# init i2c
	dev.write(1,[u_i2c_init], 100)
def i2c_idle(dev):			# i2c idle
	dev.write(1,[u_i2c_idle], 100)
def i2c_start(dev, cval):		# i2c start
	a = array('B',[0,0,0,0,0,0,0,0,0,0,0,0,0,0])
	a[0] = u_i2c_strt
	a[1] = cval
	dev.write(1, a, 100)
def i2c_stop(dev):			# i2c stop
	dev.write(1,[u_i2c_stop], 100)
def i2c_slave_ack(dev):			# i2c slave ack
	dev.write(1,[u_i2c_slak], 100)
	ret = dev.read(0x81, 64, 100)
	return ret[1]			# 1=no ack, 0=ack
def i2c_write(dev, cval):		# i2c write
	a = array('B',[0,0,0,0,0,0,0,0,0,0,0,0,0,0])
	a[0] = u_i2c_writ
	a[1] = cval
	dev.write(1, a, 100)
def i2c_master_ack(dev, cval):		# 1=nack, 0=ack
	a = array('B',[0,0,0,0,0,0,0,0,0,0,0,0,0,0])
	a[0] = u_i2c_mack
	a[1] = cval
	dev.write(1, a, 100)
def i2c_read(dev):			# i2c read
	dev.write(1,[u_i2c_read], 100)
	ret = dev.read(0x81, 64, 100)
	return ret[1]			# i2c_read char
def i2c_isdatardy(dev):			# check if i2c char avail
	dev.write(1,[u_i2c_dtrd], 100)
	ret = dev.read(0x81, 64, 100)
	return ret[1]			# i2c_read char
def spi_init(dev, mode, baud, sample):	# SPI init
	a = array('B',[0,0,0,0,0])
	a[0] = u_spi_init
	a[1] = mode
	a[2] = baud
	a[3] = sample
	dev.write(1, a, 100)
def spi_transfer(dev, value): 		# SPI transfer 
	a = array('B',[0,0,0,0,0])
	a[0] = u_spi_tran
	a[1] = value
	dev.write(1, a, 100)
	ret = dev.read(0x81, 64, 100)
	return ret[1]			# ret SPI char read
def spi_cs(dev, select): 		# enable or disable SPI CS
	a = array('B',[0,0,0,0,0])
	a[0] = u_spi_cs
	a[1] = select
	dev.write(1, a, 100)
def close(dev):				# reset USB device
	dev.reset()
#===================== end of module =========
