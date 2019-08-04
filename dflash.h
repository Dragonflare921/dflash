#ifndef _DFLASH_H_
#define _DFLASH_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include <wiringPi.h>		// used for GPIO for resetting and interrupt lines

#include "MCP23S17.h"		// interacting with SPI GPIO expanders
#include "spidev.h"			// custom barebones Linux spidev

// EXPANDER DEVICE ADDRESSES
#define A_DEV		0x00
#define D_DEV		0x01

// EXPANDER INIT MODES
#define READ_MODE		0x00
#define WRITE_MODE	0x01

// initialization
int init();
void expanderInit(uint8_t write_mode);

// AT49F040A features
uint8_t flashRead(uint32_t addr);
void flashChipErase();
void flashSectorErase(uint32_t addr);
void flashByteProgram(uint32_t addr, uint8_t val);
void flashBootBlockLockout();

// buffer writing
void writeBuffer(uint32_t addr, uint32_t len, uint8_t* buff);

// helpers
void doBusCycle(uint32_t addr, uint32_t val);		// clock a bus cycle for the AT49F040A using !WE, !CE, and !OE

// bus clearing
void clearABus();
void clearDBus();
void clearBusses();


#endif //_DFLASH_H_
