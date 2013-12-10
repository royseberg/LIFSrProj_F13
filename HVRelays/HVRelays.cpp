/*
 *	HVRelays.cpp
 *	EE490 - Microfluidics Analysis (Fall 2013)
 *	Author: Daniel Andrews
 *
 *	Provides functions for safely turning on the relays for the
 *		electroosmotic pump
 *
 *	The relay board has four high voltage output wires, one for each port on the microfluidic chip.
 *	- Port 2 is always at ground
 *	- Ports 1, 3, and 4 can go between two different values (eg, Port 4 is High Voltage or Ground)
 *		or can be disconnected (both relays turned off).
 *
 *	Modes are described on nanomicro.byu.edu:3455/maingroup/10307
 *
 *	Functions make sure only one relay is on for each port. E.g. if both relays for Port 4 are
 *		closed the High Voltage supply will be shorted to ground, not good.
 *
 *	Delays are also given to allow the relay to open before the other closes.
 *
 */
 
#include "Arduino.h"
#include "HVRelays.h"
#include <Adafruit_MCP4725.h>
 
// delay between switching two relays on same port (in milliseconds)
// "Release Time - Typical: 3.0 ms" from Coto 5500 Relay datasheet
#define SWITCH_DELAY	10 // 10ms

// Arduino pins for each relay
#define RELAY1	13
#define RELAY2	12
#define RELAY3	11
#define RELAY4	10
#define RELAY5	9
#define RELAY6	8

Adafruit_MCP4725 dac;
int dacValue;

// Set digital output pins
HVRelays::HVRelays() {
	dacValue = 0;
	dac.begin(0x62);
	//dac.setVoltage(dacValue, false);
	
	pinMode(RELAY1, OUTPUT);
	pinMode(RELAY2, OUTPUT);
	pinMode(RELAY3, OUTPUT);
	pinMode(RELAY4, OUTPUT);
	pinMode(RELAY5, OUTPUT);
	pinMode(RELAY6, OUTPUT);
	allOff(); // Make sure everything is off to start
}

void enableRelay(int, int); // Should this be declared (as private) in the class declaration?

//-------------------------------------------
// Modes for each microfluidic operation:
//-------------------------------------------


/* This mode (two 'flow buffer' modes actually) is shown on first picture on nanomicro.byu.edu:3455/maingroup/10307,
* But on the bottom (third) picture there are only "load sample" and
* "flow sample" modes.
*/
void HVRelays::flowBuffer() { // Same as loadSample() right now...
	allOff();
	relay1Enable(); // port1: GND
	// port3 is disconnected since allOff() was called
	// relay4Enable(); // port3: HV
	relay6Enable(); // port4: GND
}

void HVRelays::loadSample() {
	relay1Enable(); // port1: GND
	relay4Enable(); // port3: HV
	relay5Enable(); // port4: GND
}

void HVRelays::flowSample() {
	relay2Enable(); // port1: 40%
	relay3Enable(); // port3: 40%
	relay6Enable(); // port4: HV
}

// Turn all relays off, all ports disconnected
void HVRelays::allOff() {
	digitalWrite(RELAY1, 0);
	digitalWrite(RELAY2, 0);
	digitalWrite(RELAY3, 0);
	digitalWrite(RELAY4, 0);
	digitalWrite(RELAY5, 0);
	digitalWrite(RELAY6, 0);
}

// Set value for DAC
void HVRelays::setHV(int newValue) {
	if((dacValue >= 0) && (dacValue < 4096))
		dac.setVoltage(newValue, false);
}

//---------------------------------------------------------------------
//	Private functions to manipulate individual relays
//---------------------------------------------------------------------

// Functions to enable either relay on each port:

//---------------------------------
// Port 1 (relays 1 and 2)
//---------------------------------
void HVRelays::relay1Enable() {
	enableRelay(RELAY1, RELAY2);
}

void HVRelays::relay2Enable() {
	enableRelay(RELAY2, RELAY1);
}


//---------------------------------
// Port 3 (relays 3 and 4)
//---------------------------------
void HVRelays::relay3Enable() {
	enableRelay(RELAY3, RELAY4);
}

void HVRelays::relay4Enable() {
	enableRelay(RELAY4, RELAY3);
}


//---------------------------------
// Port 4 (relays 5 and 6)
//---------------------------------
void HVRelays::relay5Enable() {
	enableRelay(RELAY5, RELAY6);
}

void HVRelays::relay6Enable() {
	enableRelay(RELAY6, RELAY5);
}


/*	Turns relays 1 - 6 on safely
 *
 *	Turns on primaryRelay. First checks if otherRelay is already on.
 *	If so, otherRelay is turned off first, the function waits for SWITCH_DELAY
 *	milliseconds, then enables primaryRelay.
 *
 *	enableRelay( primaryRelay, otherRelay )
 *		primaryRelay: relay to turn on
 *		otherRelay:	the other relay on the port
 *
 *	Example: if primaryRelay = 1, then otherRelay is 2 (both on Port 1)
 * 			and vice versa, if primaryRelay=2 then otherRelay=1
 *
 */
void enableRelay(int primaryRelay, int otherRelay) {
	bool status = digitalRead(otherRelay); // check if otherRelay is already on
	if (status) {
		digitalWrite(otherRelay, 0); // turn otherRelay off
		delay(SWITCH_DELAY);
		digitalWrite(primaryRelay, 1); // turn primaryRelay on	
	}
	else {
		digitalWrite(primaryRelay, 1); // turn primaryRelay on
	}
}
