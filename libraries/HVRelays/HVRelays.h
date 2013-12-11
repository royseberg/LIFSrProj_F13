/*
 *	HVRelays.h
 *	Provides functions for safely turning on and off the relays
 *	Functions make sure only one relay at a time is on for each
 *		of the three ports. Delays are also given to allow the relay to open before the other closes.
 *
 *	EE490 - Microfluidics Analysis (Fall 2013)
 *	Daniel Andrews
 */

#ifndef HVRelays_h
#define HVRelays_h

#include "Arduino.h"

class HVRelays
{
	public:
		HVRelays(); //constructor
		void allOff();
		void flowBuffer();
		void loadSample();
		void flowSample();
		void setHV(int);
		
		
	private:
		void relay1Enable();
		void relay2Enable();
		void relay3Enable();
		void relay4Enable();
		void relay5Enable();
		void relay6Enable();
};

#endif