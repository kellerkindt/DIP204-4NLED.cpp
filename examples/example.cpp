/*
 * example.cpp
 *
 *  Created on: 29.05.2016
 *      Author: Michael Watzko
 */

/**
 * Do not include the example code by default in your sketch
 */
#ifdef __INCLUDE_DIP2044NLED_EXAMPLE

#include "dip2044nled_driver_mcp23017.h"
#include "dip2044nled.h"
#include "Arduino.h"


EaDip2044NledDriver *lcdDriver = new EaDip2044NledMcp23017Driver(0x20, true, 3, 0, 1, 2);
EaDip2044Nled       *lcd       = new EaDip2044Nled(lcdDriver);

void setup() {

	lcdDriver->init();
	lcd      ->init();

	lcd->clearDisplay();
	lcd->cursorAtHome();


	// show cursor only on debugging, no blinking
	lcd->setOnOffControl(true, false, false);
	lcd->println(F("Hello World"));
}

void loop() {
	lcd->cursorAt(1);
	lcd->print  (F("Time (ms):"));
	lcd->println(millis());
}

#endif
