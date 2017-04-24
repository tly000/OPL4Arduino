#define OPLLMIDI

#ifdef OPL2MIDI
#include <Arduino.h>
#include "YM3812.h"
#include "MidiToOPL.h"
#include "YMWriters.h"

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
#endif

#ifdef OPL2VGM
#include <Arduino.h>
#include "VGMPlayer.h"
YM3812<SPIWriter> ym(/*csPin*/ A1,/*a0Pin*/ A4,/*icPin*/ A5, SPIWriter{/*latchPin*/6});
void setup(){
	ym.init();

	VGMPlayer<decltype(ym)> player(ym);
	player.playSong("YOUGET.VGM");
}

void loop(){}
#endif

#ifdef BLINK
#include <Arduino.h>
void setup(){
	pinMode(13,OUTPUT);
}

void loop(){
	static bool on = false;
	digitalWrite(13,on = !on);
	delay(1000);
}
#endif

#ifdef OPL2DRUMMER
#include <Arduino.h>
#include "YM3812.h"

YM3812<false> ym;

void setup(){
	ym.init();
	ym.loadInstrument(instruments[0]);

	ym.setRhythmEnabled(true);
	//drum settings ripped from Furries vgm
	ym.write(0x30 , 0x0);
	ym.write(0x32 , 0x0);
	ym.write(0x33 , 0x0);
	ym.write(0x50 , 0x0);
	ym.write(0x52 , 0x0);
	ym.write(0x53 , 0x0);
	ym.write(0x72 , 0xff);
	ym.write(0x92 , 0xff);
	ym.write(0x70 , 0xf7);
	ym.write(0x73 , 0xf3);
	ym.write(0x90 , 0xa8);
	ym.write(0x93 , 0x6a);
	ym.write(0x34 , 0x2d);
	ym.write(0x54 , 0xc0);
	ym.write(0x74 , 0xf7);
	ym.write(0x94 , 0x67);
	ym.write(0x31 , 0x5);
	ym.write(0x51 , 0x80);
	ym.write(0x71 , 0xfa);
	ym.write(0x91 , 0x86);

	ym.write(0xA6, 0xAB);
	ym.write(0xB6, 0x01);
	ym.write(0xA7, 0xAB);
	ym.write(0xb7, 0x01);
	ym.write(0xA8, 0xAB);
	ym.write(0xB8, 0x01);

	for(int i = 6; i < 9; i++){
		ym.setSustainMode(true,0,i);
		ym.setSustainMode(false,0,i);
	}
}

#define DRUM(a,b,c,d) 0b##a##b##c##d

const uint16_t drums[]{
	DRUM(1010,1010,1010,1010),
	DRUM(0000,0000,0000,0000),
	DRUM(0001,0001,0001,0001),
	DRUM(0000,1000,0000,1000),
	DRUM(1000,0010,0010,0010),
};

void loop(){
	static uint8_t drumPos = 0;
	for(int i = 0; i < 5; i++){
		if(drums[i] & (1 << (16 - drumPos % 16))){
			auto d = YM3812<>::Drum(i);
			if(ym.isDrumOn(d)){
				ym.drumOff(d);
			}
			ym.drumOn(d);
		}
	}
	drumPos++;
	delay(100);
}
#endif

#ifdef OPLLPLAYER

#include <Arduino.h>
#include "Midi.h"
#include "YM2413.h"
#include "YMWriters.h"
#include "VGMPlayer.h"

YM2413<PortDWriter> ym(/*csPin*/ A2,/*a0Pin*/ A3,/*icPin*/ A1);

void setup(){
	pinMode(9,OUTPUT);
	TCCR1A = (1 << COM1A0);
	TCCR1B = (1 << WGM12) | (1 << CS10);

	OCR1A = 1;
	TIMSK1 = 0;
	ym.init();

	VGMPlayer<decltype(ym)> player(ym);
	player.playSong("SONYC.VGM");
}

void loop(){}

#endif

#ifdef OPLLTEST

#include <Arduino.h>
#include "Midi.h"
#include "YM2413.h"
#include "YMWriters.h"

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
	ym.keyOn(0, MIDI_TO_FREQ(60));
	ym.keyOn(1, MIDI_TO_FREQ(64));
	delay(500);
	ym.keyOff(0);
	ym.keyOff(1);
	delay(5000);
}

#endif

#ifdef OPLLMIDI

#include <Arduino.h>
#include "Midi.h"
#include "YM2413.h"
#include "YMWriters.h"
#include "MidiToOPL.h"
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
	pinMode(9,OUTPUT);
	TCCR1A = (1 << COM1A0);
	TCCR1B = (1 << WGM12) | (1 << CS10);

	OCR1A = 1;
	TIMSK1 = 0;

	ym.init();
	pinMode(13,OUTPUT);
	midiToOpl.midi.onProgramChange = [](uint8_t instrument){
		if(instrument > 0 && instrument < 16){
			for(int i = 0; i < 9; i++){
				ym.setInstrument(i, instrument);
			}
		}else{
			uint8_t instrumentID = instrument-16;
			Instrument inst = pgmReadData(instruments+instrumentID);
			ym.loadInstrument(inst);
			for(int i = 0; i < 9; i++){
				ym.setInstrument(i, 0);
			}
		}
	};
	midiToOpl.midi.onProgramChange(1);
	digitalWrite(13,1);
	delay(500);
	digitalWrite(13,0);
	midiToOpl.init();
}

void loop(){
	midiToOpl.tick();
}
#endif
