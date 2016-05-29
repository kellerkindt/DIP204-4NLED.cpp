/*
 * dip2044nled_driver_local.cpp
 *
 *  Created on: 27.12.2014
 *      Author: Michael Watzko
 */

#include "dip2044nled_driver_mcp23017.h"


EaDip2044NledMcp23017Driver::EaDip2044NledMcp23017Driver(uint8_t address, bool gpaIsData, int pinReset, int pinRS, int pinRW, int pinEnable) {

	this->GPA = 0x00;
	this->GPB = 0x00;

	this->address    = address;
	this->gpaIsData	 = gpaIsData;

	this->maskRS     = 0x01 << pinRS;
	this->maskRW     = 0x01 << pinRW;

	this->maskReset	 = 0x01 << pinReset;
	this->maskEnable = 0x01 << pinEnable;
}

EaDip2044NledMcp23017Driver::~EaDip2044NledMcp23017Driver() {

}



void EaDip2044NledMcp23017Driver::init() {

	// low -> output, let the rest be inputs
	uint8_t maskBits = ~(maskRS | maskRW | maskReset | maskEnable);

	Wire.beginTransmission(address);
	Wire.write(DIP2044NLED_DRIVER_MCP23017_ADDRESS_IODIRA);
	Wire.write(gpaIsData ? 0x00     : maskBits); // GPA
	Wire.write(gpaIsData ? maskBits : 0x00);	 // GPB
	Wire.endTransmission(true);

	// have some default data
	setBits(true, false, false, false);
	setData(0x00);
}


uint8_t EaDip2044NledMcp23017Driver::getBitsCached() const {
	return gpaIsData ? GPB : GPA;
}

uint8_t EaDip2044NledMcp23017Driver::getDataCached() const {
	return gpaIsData ? GPA : GPB;
}

void EaDip2044NledMcp23017Driver::setBitsCached(uint8_t bits) {
	(gpaIsData ? GPB : GPA) = bits;
}

void EaDip2044NledMcp23017Driver::setDataCached(uint8_t data) {
	(gpaIsData ? GPA : GPB) = data;
}



void EaDip2044NledMcp23017Driver::setRS(bool active) {
	setBitsCached( (getBitsCached() & ~maskRS) | (active ? maskRS : 0x00) );
}

void EaDip2044NledMcp23017Driver::setRW(bool active) {
	setBitsCached( (getBitsCached() & ~maskRW) | (active ? maskRW : 0x00) );
}

void EaDip2044NledMcp23017Driver::setReset(bool active) {
	// LOW-active, therefore on !active -> set, active -> no set
	setBitsCached( (getBitsCached() & ~maskReset) | (active ? 0x00 : 0x01) );
}

void EaDip2044NledMcp23017Driver::setEnable(bool active) {
	setBitsCached( (getBitsCached() & ~maskEnable) | (active ? maskEnable : 0x00) );
}

void EaDip2044NledMcp23017Driver::setBits(bool enable, bool reset, bool rsActive, bool rwActive) {
	setBitsCached( (getBitsCached() & ~(maskEnable | maskReset | maskRS | maskRW))
			  | (enable   ? maskEnable : 0x00)
			  | (reset    ? 0x00       : maskReset) // low active !
			  | (rsActive ? maskRS     : 0x00)
			  | (rwActive ? maskRW     : 0x00)
			  );
}

void EaDip2044NledMcp23017Driver::setData(uint8_t data) {
	setDataCached(data);
}


void EaDip2044NledMcp23017Driver::flush() const {
	Wire.beginTransmission(address);
	Wire.write(DIP2044NLED_DRIVER_MCP23017_ADDRESS_IOLATA);
	Wire.write(GPA);
	Wire.write(GPB);
	Wire.endTransmission(true);
}

