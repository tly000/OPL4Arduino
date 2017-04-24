#pragma once
/*
 * OPLLMidi.h
 *
 *  Created on: 24.04.2017
 *      Author: tly
 */

#include <Arduino.h>
#include "../YM2413.h"
#include "../writers/PortDWriter.h"
#include "../MidiToOPL.h"
#include <SoftwareSerial.h>

YM2413<PortDWriter> ym(/*csPin*/ A2,/*a0Pin*/ A3,/*icPin*/ A1);

SoftwareSerial softSerial(8,10);
MidiToOpl<SoftwareSerial,decltype(ym)> midiToOpl{softSerial,ym};

template<typename T> T pgmReadData(const T* data){
	T value;
	memcpy_P(&value,data,sizeof(T));
	return value;
}

void setup(){
	//set all pins to input_pullup as a default
	for (uint8_t p = 0; p < 20; p++) {
		pinMode(p, INPUT_PULLUP);
	}
	//create a 4Mhz PWM wave as a clock source for the synth.
	pinMode(9,OUTPUT);
	TCCR1A = (1 << COM1A0);
	TCCR1B = (1 << WGM12) | (1 << CS10);

	OCR1A = 1;
	TIMSK1 = 0;

	ym.init();
	midiToOpl.midi.onProgramChange = [](uint8_t instrument){
		if(instrument < 15){
			for(int i = 0; i < 9; i++){
				ym.setInstrument(i, instrument+1);
			}
		}else{
			uint8_t instrumentID = instrument-15;
			Instrument inst = pgmReadData(instruments+instrumentID);
			ym.loadInstrument(inst);
			for(int i = 0; i < 9; i++){
				ym.setInstrument(i, 0);
			}
		}
	};
	midiToOpl.midi.onProgramChange(2);

	pinMode(13,OUTPUT);
	digitalWrite(13,1);
	delay(500);
	digitalWrite(13,0);

	midiToOpl.init();
}

void loop(){
	midiToOpl.tick();
}



