// interface for interacting with MCP23S17 SPI GPIO expander
// written for Raspberry Pi, uses custom barebones Linux spidev to communicate
// some auxiliary functionality supported by WiringPi
//
// dragonflare921

#include <wiringPi.h>

#include "MCP23S17.h"
#include "spidev.h"

extern int spi0;

// from the linux spidev_test.c https://raw.githubusercontent.com/torvalds/linux/master/tools/spi/spidev_test.c
void transfer(uint8_t const* tx, uint8_t const* rx, size_t len) {
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = len,
		.delay_usecs = 0,					// no delay
		.speed_hz = 10000000,			// 10 MHz
		.bits_per_word = 8,				// 8 bit word
	};

	ioctl(spi0, SPI_IOC_MESSAGE(1), &tr);
}


// send reset signal to expanders
void reset() {
	digitalWrite(RESET, LOW);
	delay(1);
	digitalWrite(RESET, HIGH);
	delay(1);
}

// returns a byte from expander with address dev at addr
uint8_t expanderRead(uint8_t dev, uint8_t addr) {
	uint8_t buff[3] = {E_WRITE_PREFIX | (dev << 1) | 0x01, addr, 0x00};
	uint8_t rbuff[3];
	transfer(buff, rbuff, 3);
	
	return rbuff[2];
}

// writes a byte val to expander with address dev at addr
void expanderWrite(uint8_t dev, uint8_t addr, uint8_t val) {
	uint8_t buff[3] = {E_WRITE_PREFIX | (dev << 1), addr, val};
	uint8_t rbuff[3];		// not really using this for writes, but passing it anyways
	transfer(buff, rbuff, 3);
}

// cleanly clears a bit on the expander with a mask
void clearExpanderMask(uint8_t dev, uint8_t addr, uint8_t mask) {
	expanderWrite(dev, addr, expanderRead(dev, addr) & ~(mask));
}

// cleanly sets a bit on the expander with a mask
void setExpanderMask(uint8_t dev, uint8_t addr, uint8_t mask) {
	expanderWrite(dev, addr, expanderRead(dev, addr) | mask);
}

// cleanly, safely, clears a single bit on the expander at position shift
void clearExpanderShift(uint8_t dev, uint8_t addr, uint8_t shift) {
	expanderWrite(dev, addr, expanderRead(dev, addr) & ~(1<<shift));
}

// cleanly, safely, sets a single bit on the expander at position shift
void setExpanderShift(uint8_t dev, uint8_t addr, uint8_t shift) {
	expanderWrite(dev, addr, expanderRead(dev, addr) | (1<<shift));
}
