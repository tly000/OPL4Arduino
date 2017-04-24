#pragma once
/*
 * OPL2Drummer.h
 *
 *  Created on: 24.04.2017
 *      Author: tly
 */

#include <Arduino.h>
#include "../YM3812.h"

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



