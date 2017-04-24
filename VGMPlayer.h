#pragma once
#include "YM2413.h"
#include "YM3812.h"
#include <SD.h>

/*
 * VGMPlayer.h
 *
 *  Created on: 21.08.2016
 *      Author: tly
 */

volatile uint32_t samples = 0;
ISR(TIMER2_COMPA_vect){
	if(OCR2A == 255){
		OCR2A = 108;
	}else{
		OCR2A = 255;
		samples++;
	}
}

template<typename T> struct VGMWriteOpCode{};

template<typename YM> struct VGMPlayer{
	YM& ym;

	VGMPlayer(YM& ym) : ym(ym){
		SD.begin();

		TCCR2A = (1 << WGM21);
		TCCR2B = (1 << CS20);
		OCR2A = 255;
		TCNT2 = 0;
		TIMSK2 = (1 << OCIE2A);
	}

	void playSong(const char* name){
		File f = SD.open(name);
		if(f.available() && f.readStringUntil(' ') == "Vgm"){
			f.seek(0x80);
			uint32_t version = -1;
			f.readBytes((char*)&version,4);
			if(version >= 0x150){
				f.seek(0x34);
				unsigned long offset = 0x34 + f.read();
				f.seek(offset);
			}else{
				f.seek(0x40);
			}

			bool playing = true;
			samples = 0;
			while(playing){
				switch(int v = f.read()){
				case 0x61:{
					uint16_t lo = f.read();
					uint16_t hi = f.read();
					wait(lo | (hi << 8));
				}
					break;
				case 0x62:
					wait(735);
					break;
				case 0x63:
					wait(882);
					break;
				case VGMWriteOpCode<YM>::chip1:{
					uint16_t a = f.read();
					uint16_t d = f.read();
					ym.write(a,d);
				}
					break;
				case VGMWriteOpCode<YM>::chip2:{
					uint16_t a = f.read();
					uint16_t d = f.read();
					//ym.write(a,d);
				}
					break;
				case 0x70:
				case 0x71:
				case 0x72:
				case 0x73:
				case 0x74:
				case 0x75:
				case 0x76:
				case 0x77:
				case 0x78:
				case 0x79:
				case 0x7A:
				case 0x7B:
				case 0x7C:
				case 0x7D:
				case 0x7E:
				case 0x7F:
					wait(v & 0xF);
					break;
				default:
					playing = false;
					delay(20);
					break;
				}
			}
			//remove all notes
			for(int i = 0; i < 9; i++){
				ym.keyOff(i);
			}
		}
	}
protected:
	void wait(uint32_t s){
		while(samples < s);
		samples = 0;
	}
};

template<typename W> struct VGMWriteOpCode<YM3812<W>>{
	constexpr static uint32_t chip1 = 0x5A, chip2 = 0xAA;
};

template<typename W> struct VGMWriteOpCode<YM2413<W>>{
	constexpr static uint32_t chip1 = 0x51, chip2 = 0xA1;
};
