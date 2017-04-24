#pragma once
/*
 * OPL2Midi.h
 *
 *  Created on: 24.04.2017
 *      Author: tly
 */

#include <Arduino.h>
#include "../YM3812.h"
#include "../MidiToOPL.h"
#include "../writers/PortDWriter.h"

YM3812<SPIWriter> ym(/*csPin*/ A1,/*a0Pin*/ A4,/*icPin*/ A5, SPIWriter{/*latchPin*/6});

MidiToOpl<HardwareSerial,decltype(ym)> midiToOpl{Serial,ym};

void setup(){
	ym.init();
	ym.loadInstrument(instruments[2]);
	midiToOpl.init();
}

void loop(){
	midiToOpl.tick();
}
