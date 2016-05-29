# DIP204-4NLED.cpp
C++ implementation to talk to a [DIP204-4NLED](http://www.lcd-module.de/pdf/doma/dip204-4.pdf)  as a Print subclass (for Arduino).

This has been developed in late 2014, and uploaded in 2016-05-29 after I discovered it again. Maybe someone can make use of it :)

#### [mcp23017](http://ww1.microchip.com/downloads/en/DeviceDoc/21952b.pdf) driver (dip2044nled_driver_mcp23017.cpp)
The EaDip2044NledMcp23017Driver is designed to drive the display behind the [mcp23017](http://ww1.microchip.com/downloads/en/DeviceDoc/21952b.pdf) i2c chip. One of its General Purpose Registers (GPA or GPB) is supposed to be connected to D0-D7 on [DIP204-4NLED](http://www.lcd-module.de/pdf/doma/dip204-4.pdf) while the other register is driving the control lanes. Whether the data lanes are on GPA or GPB and the pin numbers of the control lanes have to be specified in the constructor. Also the address of the [mcp23017](http://ww1.microchip.com/downloads/en/DeviceDoc/21952b.pdf) has to be specified in the constructor.

```cpp
/**
 * Instantiates a new EaDip2044NledMcp23017Driver. The address of the
 * mcp23017 chip is 0x20, D0-D7 of dip2044nled are connected to GPA.
 * GPB3 is Reset, GPB0 is RS GPB1 is RW, and GPB2 is Enable of dip2044nled.
 */
EaDip2044NledDriver *lcdDriver = new EaDip2044NledMcp23017Driver(0x20, true, 3, 0, 1, 2);

/**
 * Instantiate a new display handle for the driver.
 * For more details see the example.cpp file
 */ 
EaDip2044Nled *lcd = new EaDip2044Nled(lcdDriver);
```
