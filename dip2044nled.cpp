/*
 * DIP2044NLED.cpp
 *
 *  Created on: 27.12.2014
 *      Author: Michael Watzko
 */

#include "dip2044nled.h"
#include "Arduino.h"

#define DIP2044NLED_LINE_COUNT  4
#define DIP2044NLED_LINE_WIDTH 20


EaDip2044Nled::EaDip2044Nled(EaDip2044NledDriver *driver) {
	this->driver = driver;

}

EaDip2044Nled::~EaDip2044Nled() {

}

void EaDip2044Nled::init () {

	driver->setReset(true);
	driver->flush();
	driver->setReset(false);

	// only known state:
	state.RE   = false;

	// default configuration
	setFunctionSet0(true, true, true, false, false);
	setExtendedFunctionSet(false, false, true);
	setFunctionSet1(true, false, false, false, false);
	setOnOffControl(true, true, true);
	clearDisplay();
	setEntryMode(true, false);
}


size_t EaDip2044Nled::write(uint8_t data) {


	// TODO '\n' '\0'
	if (data != '\n' && data != '\r' && data != '\0') {

		/*
		Serial.print  ("printing: ");
		Serial.println(data);
		*/

		apply(true, false, data);
		cursorPosX = (cursorPosX+1)%20; // TODO
		return 1;
	}

	else if (data == '\n' && cursorPosX > 0) {
		// TODO
		size_t count;
		for (count = 0; cursorPosX < 20; ++cursorPosX, ++count) {
			apply(true, false, ' ');
		}
		cursorPosX = 0;
		return count;
	}

	return 0;
}

void EaDip2044Nled::apply(bool rsActive, bool rwActive, uint8_t data) {
	driver->setBits(true, false, rsActive, rwActive);
	driver->setData(data);
	driver->flush();

	// only for debugging
	// delay(100);

	driver->setEnable(false);
	driver->flush();

	// only for debugging
	// delay(100);
}

void EaDip2044Nled::setRE(bool active) {
	// only do something if needed (be sure there wont be an expression like if(2 != 1)..., therefore don't compare them directly, compare their boolean value instead)
	if ((active == true) != (state.RE == true)) {
		// the apply values depend on the current RE state
		if (state.RE) {
			// setting it to false (being true)
			apply(false, false, 0x20 | (state.DL<<4) | (state.N<<3) |/*(active<<2)|*/(state.BE<<1) | state.LP);

		} else {
			//setting it to true (being false)
			apply(false, false, 0x20 | (state.DL<<4) | (state.N<<3) |   (0x01<<2) |  (state.DH<<1) | state.RV);
		}

		// save state
		state.RE = active;
	}
}


void EaDip2044Nled::clearDisplay() {
	/*
	 * Documentation:
	 *   RE   = *
	 *   RS   = LOW
	 *   RW   = LOW
	 *   DATA = 0x01
	 */
	apply(false, false, 0x01);
}

void EaDip2044Nled::cursorAtHome() {
	/**
	 * Documentation:
	 *   RE   = LOW
	 *   RS   = LOW
	 *   RW   = LOW
	 *   DATA = 0b0000001*
	 */
	setRE(false);
	apply(false, false, 0x02);
}

void EaDip2044Nled::powerDownMode(bool active) {
	/**
	 * Documentation:
	 *   RE   = HIGH
	 *   RS   = LOW
	 *   RW   = LOW
	 *   DATA = 0b0000001(PD)
	 *
	 *  where (PD) is the flag indicating enabled or disabled
	 *   PD = HIGH -> PowerDown-Mode enabled
	 *   PD = LOW  -> PowerDown-Mode disabled
	 */
	setRE(true);
	apply(false, false, 0x02 | (active ? 0x01 : 0x00));
}


void EaDip2044Nled::setEntryMode(bool cursorIncrement, bool shift) {
	/**
	 * Documentation:
	 *   RE   = LOW
	 *   RS   = LOW
	 *   RW   = LOW
	 *   DATA = 0b0000 01(I/D)(S)
	 *
	 *  where (I/D) is the flag indicating on
	 *   HIGH -> increment cursor moving direction
	 *   LOW  -> decrement cursor moving direction
	 *
	 *  where (s) is the flag indicating
	 *   HIGH -> shift enabled
	 *   LOW  -> shift disabled
	 */
	state.ID = cursorIncrement;
	state.S  = shift;

	setRE(false);
	apply(false, false, 0x04 | (state.ID<<1) | state.S);
}

void EaDip2044Nled::setEntryMode(bool bidirection) {
	/**
	 * Documentation:
	 *   RE   = TRUE
	 *   RS   = LOW
	 *   RW   = LOW
	 *   DATA = 0b0000 011(BD)
	 *
	 *  where (BD) is the flag indicating on
	 *   HIGH -> the segment bidirectional function Seg60->Seg1
	 *   LOW  -> the segment bidirectional function Seg1->Seg60
	 */
	state.BD = bidirection;

	setRE(true);
	apply(false, false, 0x06 | state.BD);
}


void EaDip2044Nled::setOnOffControl(bool displayOn, bool cursorOn, bool blinkOn) {
	/**
	 * Documentation:
	 *   RE   = LOW
	 *   RS   = LOW
	 *   RW   = LOW
	 *   DATA = 0b0000 1(D)(C)(B)
	 *
	 *  where (D) is the flag whether the display is on
	 *  where (C) is the flag whether the cursor is on
	 *  where (B) is the flag whether the cursor blinks
	 */
	state.D = displayOn;
	state.C = cursorOn;
	state.B = blinkOn;

	setRE(false);
	apply(false, false, 0x08 | (state.D<<2) | (state.C<<1) | state.B);
}

void EaDip2044Nled::setExtendedFunctionSet(bool wideFont, bool cursorInverted, bool fourLines) {
	/**
	 * Documentation:
	 *   RE   = HIGH
	 *   RS   = LOW
	 *   RW   = LOW
	 *   DATA = 0b0000 1(FW)(BW)(NW)
	 *
	 *  where (FW) is the flag indicating on
	 *   HIGH -> 6-dot font width
	 *   LOW  -> 5-dot font width
	 *
	 *  where (BW) is the flag indicating on
	 *   HIGH -> inverted cursor
	 *   LOW  -> normal cursor
	 *
	 *  where (NW) is the flag indicating on
	 *   HIGH -> 2 or 4 lines
	 *   LOW  -> 1 or 2 lines
	 */
	state.FW = wideFont;
	state.BW = cursorInverted;
	state.NW = fourLines;

	setRE(true);
	apply(false, false, 0x08 | (state.FW<<2) | (state.BW<<1) | (state.NW));
}

void EaDip2044Nled::setCursorDisplayShift(bool displayShift, bool shiftToRight) {
	/**
	 * Documentation:
	 *   RE   = LOW
	 *   RS   = LOW
	 *   RW   = LOW
	 *   DATA = 0b0001 (S/C)(R/L)**
	 *
	 *  where (S/C) is the flag indicating on
	 *   HIGH -> display shift
	 *   LOW  -> cursor shift
	 *
	 *  where (R/L) is the flag indicating on
	 *   HIGH -> shift to right
	 *   LOW  -> shift to left
	 */
	state.SC = displayShift;
	state.RL = shiftToRight;

	setRE(false);
	apply(false, false, 0x10 | (state.SC<<3) | (state.RL<<2));
}

void EaDip2044Nled::setScrollEnable(bool line1, bool line2, bool line3, bool line4) {
	/**
	 * Documentation:
	 *   RE   = HIGH
	 *   RS   = LOW
	 *   RW   = LOW
	 *   DATA = 0b0001 (H4)(H3)(H2)(H1)
	 *
	 *  where (H*) is the flag indicating whether to scroll/shift that line
	 */
	state.H4 = line4;
	state.H3 = line3;
	state.H2 = line2;
	state.H1 = line1;

	setRE(true);
	apply(false, false, 0x10 | (state.H4<<3) | (state.H3<<2) | (state.H2<<1) | state.H1);
}



void EaDip2044Nled::setFunctionSet0(bool eightBit, bool twoLine, bool RE, bool displayShift, bool inverseDisplay) {
	/**
	 * Documentation:
	 *   RE   = LOW
	 *   RS   = LOW
	 *   RW   = LOW
	 *   DATA = 0b001 (DL)(N)(RE)(DH)(RV)
	 *
	 *  where (DL) indicates on
	 *   HIGH -> 8bit data length
	 *   LOW  -> 4bit data length
	 *
	 *  where (N) indicates on
	 *   HIGH -> 2 display lines
	 *   LOW  -> 1 display line
	 *
	 *  where (RE) sets the extension register
	 *
	 *  where (DH) indicates on
	 *   HIGH -> display shift
	 *   LOW  -> dot scroll
	 *
	 *  where (RV) indicates whether to inverse the display
	 */
	setRE(false);

	state.DL = eightBit;
	state.N  = twoLine;
	state.RE = RE;
	state.DH = displayShift;
	state.RV = inverseDisplay;

	apply(false, false, 0x20 | (state.DL<<4) | (state.N<<3) | (state.RE<<2) | (state.DH<<1) | state.RV);

}
void EaDip2044Nled::setFunctionSet1(bool eightBit, bool twoLine, bool RE, bool ramBlink, bool lowPowerMode) {
	/**
	 * Documentation:
	 *   RE   = HIGH
	 *   RS   = LOW
	 *   RW   = LOW
	 *   DATA = 0b001 (DL)(N)(RE)(BE)(LP)
	 *
	 *  where (DL) indicates on
	 *   HIGH -> 8bit data length
	 *   LOW  -> 4bit data length
	 *
	 *  where (N) indicates on
	 *   HIGH -> 2 display lines
	 *   LOW  -> 1 display line
	 *
	 *  where (RE) sets the extension register
	 *  where (DH) indicates whether to blink on CG-/SEG-RAM
	 *  where (LP) indicates whether low power mode is enabled
	 */
	setRE(true);

	state.DL = eightBit;
	state.N  = twoLine;
	state.RE = RE;
	state.BE = ramBlink;
	state.LP = lowPowerMode;

	apply(false, false, 0x20 | (state.DL<<4) | (state.N<<3) | (state.RE<<2) | (state.BE<<1) | state.LP);
}

void EaDip2044Nled::setScrollQuantity(uint8_t scrollQuantity) {
	/**
	 * Documentation:
	 *   RE   = HIGH
	 *   RS   = LOW
	 *   RW   = LOW
	 *   DATA = 0b1*1 (6bit long SQ)
	 *
	 *  where (SQ) tells the dot scroll quantity
	 */
	state.SQ = scrollQuantity;

	setRE(true);
	apply(false, false, 0x80 | state.SQ);
}

void EaDip2044Nled::cursorAt(int line) {
	cursorAt(line, 0);
}

void EaDip2044Nled::cursorAt(int line, int x) {

	// limit x
	x = (0x1F & x);

	// shift and limit the line to
	line = (0x70 & (line<<5));

	setRE(false);
	apply(false, false, 0x80 | line | x);

	this->cursorPosX = x;
}


int EaDip2044Nled::lineCount() {
	return DIP2044NLED_LINE_COUNT;
}

int EaDip2044Nled::lineWidht(int line) {
	if (line >= 0 && line < lineCount()) {
		return DIP2044NLED_LINE_WIDTH;
	} else {
		return 0;
	}
}


bool EaDip2044Nled::isBusy() {
	// TODO
	return false;
}
