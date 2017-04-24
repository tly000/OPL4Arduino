#pragma once
/*
 * OPLLTest.h
 *
 *  Created on: 24.04.2017
 *      Author: tly
 */

#include <Arduino.h>
#include "../YM2413.h"
#include "../writers/PortDWriter.h"

YM2413<PortDWriter> ym(/*csPin*/ A2,/*a0Pin*/ A3,/*icPin*/ A1);

void setup(){
	pinMode(9,OUTPUT);
	TCCR1A = (1 << COM1A0);
	TCCR1B = (1 << WGM12) | (1 << CS10);

	OCR1A = 1;
	TIMSK1 = 0;

	ym.init();
	ym.setInstrument(0,9);
  	ym.setInstrument(1,9);
}

void loop() {
	ym.keyOn(0, midiToFreq(60));
	ym.keyOn(1, midiToFreq(64));
	delay(500);
	ym.keyOff(0);
	ym.keyOff(1);
	delay(5000);
}


