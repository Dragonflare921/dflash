dflash
======

## Overview

Custom chip flasher built with parts that were laying around.  
For use with Raspberry Pi Zero (W) and MCP23S17 SPI GPIO expanders.  

This code was last tested on a breadboard and has not yet been placed on a soldered perfboard, and as such **IS NOT GUARANTEED TO WORK**. There will be more supported chips in the future and a rough schematic will be added to the repository after testing on a prototyped board.

## Supported chips

```
AT49F040A       4Mb flash memory
```

## Dependencies

- WiringPi

## Notes

I am well aware that this is a very slow way to flash a chip, and that an FPGA is a much better solution, however as noted above, this was built with parts that I already had, and I had some free time to mess around.