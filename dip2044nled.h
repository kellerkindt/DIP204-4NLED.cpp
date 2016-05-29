/*
 * DIP2044NLED.h
 *
 *  Created on: 27.12.2014
 *      Author: Michael Watzko
 */

#ifndef DIP2044NLED_H_
#define DIP2044NLED_H_

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <Print.h>

class EaDip2044NledDriver {

	public:
		EaDip2044NledDriver() {};
		virtual ~EaDip2044NledDriver() {};

		/**
		 * Optional implementation
		 */
		virtual void init() = 0;

		virtual void setRS(bool active) = 0;
		virtual void setRW(bool active) = 0;

		virtual void setBits(bool enabled, bool reset, bool rsActive, bool rwActive) = 0;

		virtual void setReset (bool active) = 0;
		virtual void setEnable(bool active) = 0;
		virtual void setData  (uint8_t data) = 0;

		/**
		 * Depending on the implementation, a function call does not need to push
		 * its changes to hardware at the same time - though this method call
		 * forces any remaining changes to be flushed out
		 */
		virtual void flush() const = 0;

};


class EaDip2044Nled : public Print {

	private:
		EaDip2044NledDriver *driver;

		uint8_t cursorPosX = 0;

		/*
		 * Put all the state flags into a structure to a) reduce memory usage and b)
		 * be sure a flag is either 1 or 0 (since it is only 1bit wide)
		 *
		 * Names are mostly taken directly from the documentation
		 */
		struct state{
			bool ID:1; // current state of the cursor moving direction (0 -> decrement, 1-> increment)
			bool S :1; // current state of shift (0 -> disabled, 1 -> enabled)
			bool BD:1; // current state of the segment bidirectional function (!?) (0 -> Seg1->Seg60, 1 -> Seg60->Seg1)

			bool D :1; // current state of the display (on/off)
			bool C :1; // current state of the cursor (on/off)
			bool B :1; // current state of the cursor blinking (on/off)

			bool FW:1; // current state of the font width (0 -> 5-dot font width, 1 -> 6-dot font width)
			bool BW:1; // current state of the cursor     (0 -> normal cursor,    1 -> inverted cursor)
			bool NW:1; // current state of the line count (0 -> 1 or 2 lines,     1 -> 2 or 4 lines)

			bool SC:1; // current state whether to shift the cursor
			bool RL:1; // whether to shift to right

			// whether the specific line is selected for scrolling
			bool H4:1;
			bool H3:1;
			bool H2:1;
			bool H1:1;

			bool DL:1; // current state of the data-length flag (0 -> 4bit, 1 -> 8bit)
			bool N :1; // current number of display lines (0-> 1line, 1->2lines)
			bool RE:1; // current state of the extension register
			bool DH:1; // current state of scroll/shift (0 -> dot scroll, 1 -> display shift)
			bool RV:1; // current state of reverse (0 -> normal, 1 -> inverse display)
			bool BE:1; // current state of the ram blink (?) (0 -> disable, 1 -> enable)
			bool LP:1; // current state of power (0 -> normal, 1 -> low power mode)

			uint8_t SQ:6; // scroll quantity

		} state;



	public:
		EaDip2044Nled(EaDip2044NledDriver*);
		virtual ~EaDip2044Nled();


		// >>>>>>>>>>>>>>>>>> temporary public
		/**
		 * The internal extension register
		 */
		virtual void setRE(bool active);

		virtual void apply(bool rsActive, bool rwActive, uint8_t data);
		// <<<<<<<<<<<<<<<<<< temporary public

		virtual void init();

		virtual void clearDisplay();
		virtual void cursorAtHome();
		virtual void powerDownMode(bool active);
		virtual void setEntryMode(bool cursorIncrement, bool shift);
		virtual void setEntryMode(bool bidirection); // TODO whot?
		virtual void setOnOffControl(bool displayOn, bool cursorOn, bool blinkOn);
		virtual void setExtendedFunctionSet(bool wideFont, bool cusrorInverted, bool fourLines);
		virtual void setCursorDisplayShift(bool displayShift, bool shiftToRight);
		virtual void setScrollEnable(bool line1, bool line2, bool line3, bool line4);
		virtual void setFunctionSet0(bool eightBit, bool twoLine, bool RE, bool displayShift, bool inverseDisplay);
		virtual void setFunctionSet1(bool eightBit, bool twoLine, bool RE, bool ramBlink, bool lowPowerMode);
		virtual void setScrollQuantity(uint8_t scrollQuantity);

		virtual void cursorAt(int line);
		virtual void cursorAt(int line, int x);

		virtual int lineCount();
		virtual int lineWidht(int line);

		virtual bool isBusy();


	    virtual size_t write(uint8_t);
};

#endif /* DIP2044NLED_H_ */
