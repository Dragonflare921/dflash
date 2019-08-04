#ifndef _MCP23S17_H_
#define _MCP23S17_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

// Header for MCP23S17 SPI GPIO expander
// Just some useful definitions to make things more readable

// READ AND WRITE PREFIXES
#define E_WRITE_PREFIX	0x40
#define E_READ_PREFIX		0x41

// EXPANDER BANK 0 ADDRESSES
#define E_B0_IODIRA     0x00
#define E_B0_IODIRB     0x01
#define E_B0_IPOLA      0x02
#define E_B0_IPOLB      0x03
#define E_B0_GPINTENA   0x04
#define E_B0_GPINTENB   0x05
#define E_B0_DEFVALA    0x06
#define E_B0_DEFVALB    0x07
#define E_B0_INTCONA    0x08
#define E_B0_INTCONB    0x09
#define E_B0_IOCON      0x0A
#define E_B0_IOCON_ALT  0x0B
#define E_B0_GPPUA      0x0C
#define E_B0_GPPUB      0x0D
#define E_B0_INTFA      0x0E
#define E_B0_INTFB      0x0F
#define E_B0_INTCAPA    0x10
#define E_B0_INTCAPB    0x11
#define E_B0_GPIOA      0x12
#define E_B0_GPIOB      0x13
#define E_B0_OLATA      0x14
#define E_B0_OLATB      0x15

// EXPANDER BANK 1 ADDRESSES
#define E_B1_IODIRA     0x00
#define E_B1_IPOLA      0x01
#define E_B1_GPINTENA   0x02
#define E_B1_DEFVALA    0x03
#define E_B1_INTCONA    0x04
#define E_B1_IOCON      0x05
#define E_B1_GPPUA      0x06
#define E_B1_INTFA      0x07
#define E_B1_INTCAPA    0x08
#define E_B1_GPIOA      0x09
#define E_B1_OLATA      0x0A
#define E_B1_IODIRB     0x10
#define E_B1_IPOLB      0x11
#define E_B1_GPINTENB   0x12
#define E_B1_DEFVALB    0x13
#define E_B1_INTCONB    0x14
#define E_B1_IOCON_ALT  0x15
#define E_B1_GPPUB      0x16
#define E_B1_INTFB      0x17
#define E_B1_INTCAPB    0x18
#define E_B1_GPIOB      0x19
#define E_B1_OLATB      0x1A

// EXPANDER BIT POSITION DEFINITIONS
// IOCON
#define E_IOCON_INTPOL  0x02
#define E_IOCON_ODR     0x04
#define E_IOCON_HAEN    0x08
#define E_IOCON_DISSLW  0x10
#define E_IOCON_SEQOP   0x20
#define E_IOCON_MIRROR  0x40
#define E_IOCON_BANK    0x80

// PORTS
#define E_GPIO_ALL			0xFF

#define E_GPIO_0        0x01
#define E_GPIO_1        0x02
#define E_GPIO_2        0x04
#define E_GPIO_3        0x08
#define E_GPIO_4        0x10
#define E_GPIO_5        0x20
#define E_GPIO_6        0x40
#define E_GPIO_7        0x80

// MISC GPIO LINES FOR PI
#define RESET		0x02

// from the linux spidev_test.c https://raw.githubusercontent.com/torvalds/linux/master/tools/spi/spidev_test.c
void transfer(uint8_t const* tx, uint8_t const* rx, size_t len);

// readers and writers
uint8_t expanderRead(uint8_t dev, uint8_t addr);
void expanderWrite(uint8_t dev, uint8_t addr, uint8_t val);
void clearExpanderMask(uint8_t dev, uint8_t addr, uint8_t mask);
void setExpanderMask(uint8_t dev, uint8_t addr, uint8_t mask);
void clearExpanderShift(uint8_t dev, uint8_t addr, uint8_t shift);
void setExpanderShift(uint8_t dev, uint8_t addr, uint8_t shift);

// helpers
void reset();

#endif //_MCP23S17_H_
