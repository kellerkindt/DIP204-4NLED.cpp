/*
 * dip2044nled_driver_local.h
 *
 *  Created on: 27.12.2014
 *      Author: Michael Watzko
 */

#ifndef DIP2044NLED_DRIVER_MCP23017_H_
#define DIP2044NLED_DRIVER_MCP23017_H_

#define DIP2044NLED_DRIVER_MCP23017_ADDRESS_IOLATA 0x14
#define DIP2044NLED_DRIVER_MCP23017_ADDRESS_IODIRA 0x00

#include "dip2044nled.h"
#include "Wire.h"

class EaDip2044NledMcp23017Driver: public EaDip2044NledDriver {

	private:
		uint8_t address;

		uint8_t GPA;
		uint8_t GPB;

		bool gpaIsData;

		int maskRS;
		int maskRW;

		int maskReset;
		int maskEnable;

		uint8_t getDataCached() const;
		uint8_t getBitsCached() const;

		void setDataCached(uint8_t data);
		void setBitsCached(uint8_t data);


	public:
		EaDip2044NledMcp23017Driver(uint8_t address, bool gpaIsData, int pinReset, int pinRS, int pinRW, int pinEnable);
		virtual ~EaDip2044NledMcp23017Driver();

		virtual void init();

		virtual void setRS(bool active);
		virtual void setRW(bool active);

		virtual void setBits(bool enabled, bool reset, bool rsActive, bool rwActive);

		virtual void setReset (bool active);
		virtual void setEnable(bool active);
		virtual void setData  (uint8_t data);

		/**
		 * Actual write to mcp23017
		 */
		void flush() const;



};

#endif /* DIP2044NLED_DRIVER_MCP23017_H_ */
