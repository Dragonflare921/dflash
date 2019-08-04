#include "dflash.h"

static uint8_t mode;
int spi0;

// set address bus to 0
void clearABus() {
	clearExpanderMask(A_DEV, E_B0_GPIOA, E_GPIO_ALL);
	clearExpanderMask(A_DEV, E_B0_GPIOB, E_GPIO_ALL);
}

// set data bus to 0
void clearDBus() {
	clearExpanderMask(D_DEV, E_B0_GPIOA, E_GPIO_ALL);
}

// set address and data busses to 0
void clearBusses() {
	clearABus();
	clearDBus();
}

// clock a bus cycle for the AT49F040A using !WE, !CE, and !OE
void doBusCycle(uint32_t addr, uint32_t val) {
	// CLEAR ADDRESS AND DATA FOR NEXT CYCLE
	clearBusses();
	
	////
	// FIRST BUS CYCLE
	////
	// load address
	setExpanderMask(A_DEV, E_B0_GPIOA, addr & 0xFF);							// A0-A7
	setExpanderMask(A_DEV, E_B0_GPIOB, (addr & 0xFF00) >> 8);			// A8-A15
	setExpanderMask(D_DEV, E_B0_GPIOB, (addr & 0x70000) >> 13);		// A8-A15
	
	// load data
	setExpanderMask(D_DEV, E_B0_GPIOA, val);
	
	// bus clock (set controls)
	clearExpanderMask(D_DEV, E_B0_GPIOB, E_GPIO_0 | E_GPIO_2);	// set !WE and !CE low
	setExpanderMask(D_DEV, E_B0_GPIOB, E_GPIO_1);		// set !OE high	 (ONLY NEEDS DONE ON THE FIRST ONE, BUT EVERY TIME DOESNT HURT, REMEMBER TO SET LOW AGAIN LATER)
	setExpanderMask(D_DEV, E_B0_GPIOB, E_GPIO_0 | E_GPIO_2);	// set !WE and !CE high
}


// set up the expanders for writing to the flash
void expanderInit(uint8_t write_mode) {  
	
	// hit the reset pins
	reset();
	
	// enable the hardware address pins
	setExpanderMask(A_DEV, E_B0_IOCON, E_IOCON_HAEN);
	setExpanderMask(D_DEV, E_B0_IOCON, E_IOCON_HAEN);
	
	// AT49F040A PIN CONNECTIONS
	// ADDRESS MCP23S17												// DATA MCP23S17
	// PA0 -> A0		// PB0 -> A8							// PA0 -> I/O0		// PB0 -> !CE
	// PA1 -> A1		// PB1 -> A9							// PA1 -> I/O1		// PB1 -> !OE
	// PA2 -> A2		// PB2 -> A10							// PA2 -> I/O2		// PB2 -> !WE
	// PA3 -> A3		// PB3 -> A11							// PA3 -> I/O3		// PB3 -> A16
	// PA4 -> A4		// PB4 -> A12							// PA4 -> I/O4		// PB4 -> A17
	// PA5 -> A5		// PB5 -> A13							// PA5 -> I/O5		// PB5 -> A18
	// PA6 -> A6		// PB6 -> A14							// PA6 -> I/O6		// not using PB6
	// PA7 -> A7		// PB7 -> A15							// PA7 -> I/O7		// not using PB7
	
	if (write_mode) {
		// SET UP ADDRESS EXPANDER
		clearExpanderMask(A_DEV, E_B0_IODIRA, E_GPIO_ALL);		// set PORTA as output
		clearExpanderMask(A_DEV, E_B0_IODIRB, E_GPIO_ALL);		// set PORTB as output
		
		// SET UP DATA EXPANDER
		clearExpanderMask(D_DEV, E_B0_IODIRA, E_GPIO_ALL);		// set PORTA as output
		clearExpanderMask(D_DEV, E_B0_IODIRB, E_GPIO_ALL);		// set PORTB as output
		
		setExpanderMask(D_DEV, E_B0_GPIOB, E_GPIO_1);		// set !OE high since write ops dont have any output
	}
	else if (!write_mode) {
		// SET UP ADDRESS EXPANDER
		clearExpanderMask(A_DEV, E_B0_IODIRA, E_GPIO_ALL);		// set PORTA as output
		clearExpanderMask(A_DEV, E_B0_IODIRB, E_GPIO_ALL);		// set PORTB as output
		
		// SET UP DATA EXPANDER
		setExpanderMask(D_DEV, E_B0_IODIRA, E_GPIO_ALL);		// set PORTA as input
		clearExpanderMask(D_DEV, E_B0_IODIRB, E_GPIO_ALL);	// set PORTB as output
		
		setExpanderMask(D_DEV, E_B0_GPIOB, E_GPIO_2);		// set !WE high since read ops dont have any input
	}
	
	setExpanderMask(D_DEV, E_B0_GPIOB, E_GPIO_1);		// set !CE high since the chip shouldnt be enabled immediately
}

// start everything up
int init() {
	spi0 = open("/dev/spidev0.0", O_RDWR);
	
	ioctl(spi0, SPI_IOC_WR_MODE, &mode);
	ioctl(spi0, SPI_IOC_RD_MODE, &mode);
	
	// initialize wiringpi
	wiringPiSetup();
	
	// set up extra GPIO for reset and interrupts
	pinMode(RESET, OUTPUT);
	// TODO: set up interrupts
	
	// reset is active low, init high
	digitalWrite(RESET, HIGH);
	
	expanderInit(WRITE_MODE);		// set expanders for writing
	
	return 0;
}

// TODO: check this
// read a byte from the AT49F040A
uint8_t flashRead(uint32_t addr) {
#ifdef DEBUG0
	printf("reading byte from %04X\n", addr);
#endif

	// !!!!!MAKE SURE EXPANDERS ARE READ_MODE FOR SAFETY!!!!!!
	expanderInit(READ_MODE);

	uint8_t ret = 0;
	
	// CLEAR ADDRESS AND DATA FOR NEXT CYCLE
	clearBusses();
	
	/*
	// set initial control line states
	setExpanderMask(D_DEV, E_B0_GPIOB, E_GPIO_1 | E_GPIO_2);		// set !WE and !OE high
	clearExpanderMask(D_DEV, E_B0_GPIOB, E_GPIO_0);		// set !CE low
	*/
	
	// set address
	setExpanderMask(A_DEV, E_B0_GPIOA, addr & 0xFF);						// A0-A7
	setExpanderMask(A_DEV, E_B0_GPIOB, (addr & 0xFF00) >> 8);		// A8-A15
	setExpanderMask(D_DEV, E_B0_GPIOB, (addr & 0x70000) >> 13);	// A16-A18
	
	// set !CE and !OE low
	clearExpanderMask(D_DEV, E_B0_GPIOB, E_GPIO_0 | E_GPIO_1);
	
	// set !WE high
	setExpanderMask(D_DEV, E_B0_GPIOB, E_GPIO_2);
	
	// set !CE and !OE high
	setExpanderMask(D_DEV, E_B0_GPIOB, E_GPIO_0 | E_GPIO_1);
	
	// copy data from IO
	ret = expanderRead(D_DEV, E_B0_GPIOA);
	
#ifdef DEBUG0
	printf("read %X from %04X\n", ret, addr);
#endif

	// CLEAR ADDRESS AND DATA FOR NEXT CYCLE
	clearBusses();		// TODO: this is done for safety, might not be needed since its done twice in most cases
	
	return ret;
}

// TODO: maybe also do an "erase preamble" call since chip and sector erase differ only on the final cycle
// erase the entire AT49F040A
void flashChipErase() {
#ifdef DEBUG0
	printf("erasing chip\n");
#endif
	
	// !!!!!MAKE SURE EXPANDERS ARE WRITE_MODE FOR SAFETY!!!!!!
	//expanderInit(WRITE_MODE);
	
	// set initial control line states
	//clearExpanderMask(D_DEV, E_B0_GPIOB, E_GPIO_1);		// set !OE low
	setExpanderMask(D_DEV, E_B0_GPIOB, E_GPIO_0 | E_GPIO_2);		// set !WE and !CE high
	
	// FIRST CYCLE
	doBusCycle(0x555, 0xAA);
	
	// SECOND CYCLE
	doBusCycle(0xAAA, 0x55);
	
	// THIRD CYCLE
	doBusCycle(0x555, 0x80);
	
	// FOURTH CYCLE
	doBusCycle(0x555, 0xAA);
	
	// FIFTH CYCLE
	doBusCycle(0xAAA, 0x55);
	
	// SIXTH CYCLE
	doBusCycle(0x555, 0x10);
	
	// wait for erase to finish
#ifdef DEBUG0
	printf("finished chip erase. waiting 7 seconds\n");
#endif
	delay(7000);
	
	// finish
	//clearExpanderMask(D_DEV, E_B0_GPIOB, E_GPIO_0);	// set !CE low
	
	// CLEAR ADDRESS AND DATA FOR NEXT CYCLE
	clearBusses();		// TODO: this is done for safety, might not be needed since its done twice in most cases
}

// TODO: maybe also do an "erase preamble" call since chip and sector erase differ only on the final cycle
// erase a sector starting at addr on AT49F040A
void flashSectorErase(uint32_t addr) {
#ifdef DEBUG0
	printf("erasing sector at %04X\n", addr);
#endif

	// !!!!!MAKE SURE EXPANDERS ARE WRITE_MODE FOR SAFETY!!!!!!
	//expanderInit(WRITE_MODE);
	
	// set initial control line states
	//clearExpanderMask(D_DEV, E_B0_GPIOB, E_GPIO_1);		// set !OE low
	setExpanderMask(D_DEV, E_B0_GPIOB, E_GPIO_0 | E_GPIO_2);		// set !WE and !CE high
	
	// FIRST CYCLE
	doBusCycle(0x555, 0xAA);
	
	// SECOND CYCLE
	doBusCycle(0xAAA, 0x55);
	
	// THIRD CYCLE
	doBusCycle(0x555, 0x80);
	
	// FOURTH CYCLE
	doBusCycle(0x555, 0xAA);
	
	// FIFTH CYCLE
	doBusCycle(0xAAA, 0x55);
	
	// SIXTH CYCLE
	doBusCycle(addr, 0x30);
	
	// wait for erase to finish
#ifdef DEBUG0
	printf("finished sector erase. waiting 7 seconds\n");
#endif
	delay(7000);
	
	// finish
	//clearExpanderMask(D_DEV, E_B0_GPIOB, E_GPIO_0);	// set !CE low
	
	// CLEAR ADDRESS AND DATA FOR NEXT CYCLE
	clearBusses();		// TODO: this is done for safety, might not be needed since its done twice in most cases
}

// TODO: check this
// program a byte val at addr on AT49F040A
void flashByteProgram(uint32_t addr, uint8_t val) {
#ifdef DEBUG0
	printf("writing %X to %04X\n", val, addr);
#endif	

	// !!!!!MAKE SURE EXPANDERS ARE WRITE_MODE FOR SAFETY!!!!!!
	//expanderInit(WRITE_MODE);

	// set initial control line states
	//clearExpanderMask(D_DEV, E_B0_GPIOB, E_GPIO_1);		// set !OE low
	setExpanderMask(D_DEV, E_B0_GPIOB, E_GPIO_0 | E_GPIO_2);		// set !WE and !CE high
	
	// FIRST CYCLE
	doBusCycle(0x555, 0xAA);
	
	// SECOND CYCLE
	doBusCycle(0xAAA, 0x55);
	
	// THIRD CYCLE
	doBusCycle(0x555, 0xA0);
	
	// FOURTH CYCLE
	doBusCycle(addr, val);
	
	// wait for program to finish
	delayMicroseconds(40);
	
	// finish
	//clearExpanderMask(D_DEV, E_B0_GPIOB, E_GPIO_0 | E_GPIO_2);	// set !WE and !CE low
	//setExpanderMask(D_DEV, E_B0_GPIOB, E_GPIO_0 | E_GPIO_2);	// set !WE and !CE high
	
#ifdef DEBUG0
	printf("wrote %X to %04X\n", val, addr);
#endif

	// CLEAR ADDRESS AND DATA FOR NEXT CYCLE
	clearBusses();		// TODO: this is done for safety, might not be needed since its done twice in most cases
	
}

// writes a buffer buff of length len starting at addr using repeated flashByteProgram calls
void writeBuffer(uint32_t addr, uint32_t len, uint8_t* buff) {
	
	for (int i = 0; i < len; i++) {
		flashByteProgram(addr + i, buff[i]);	// TODO: maybe copy this to a local buffer of guaranteed length, idk
	}
}

int main() {	
	init();

	flashChipErase();
	//flashByteProgram(0x7FFFF, 0x55);
	uint8_t tbuf[16] = {0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA};
	writeBuffer(0, 0x10, tbuf);

  close(spi0);
  return 0;
}
