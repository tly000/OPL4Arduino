#pragma once
#include <Arduino.h>

/*
 * YMBase.h
 *
 *  Created on: 22.09.2016
 *      Author: tly
 */

enum YMDrum{
	HIHAT = 0,
	CYMBAL= 1,
	TOM   = 2,
	SNARE = 3,
	BASS  = 4
};

template<typename YMDerived,typename Writer> struct YMBase{
	uint8_t csPin, a0Pin, icPin;
	Writer w;

	YMBase(uint8_t csPin, uint8_t a0Pin, uint8_t icPin, Writer w = {}):csPin(csPin), a0Pin(a0Pin), icPin(icPin), w(w){}

	void init(){
		w.init();

		pinMode(csPin,OUTPUT);
		pinMode(a0Pin,OUTPUT);
		pinMode(icPin,OUTPUT);

		digitalWrite(icPin,LOW);
		delay(50);
		digitalWrite(icPin,HIGH);
		delay(50);

		for(int i = 0; i < sizeof(static_cast<YMDerived*>(this)->registers); i++){
			write(i,0x00);
		}
	}

	void write(uint8_t address,uint8_t value){
		w.beginWrite();
		//set write mode to address
		digitalWrite(a0Pin,LOW);
		//write address
		w.write(address);

		//disable writing mode
		digitalWrite(csPin,LOW);
		_delay_us(YMDerived::ADDRESS_WRITE_DELAY);
		digitalWrite(csPin,HIGH);

		//set write mode to content
		digitalWrite(a0Pin,HIGH);
		//write value
		w.write(value);

		//disable writing mode
		digitalWrite(csPin,LOW);
		_delay_us(YMDerived::VALUE_WRITE_DELAY);
		digitalWrite(csPin,HIGH);
		static_cast<YMDerived*>(this)->registers[address] = value;
		w.endWrite();
	}

	template<typename T> void writeRegister(T& reg){
		write(
			(intptr_t)(&reg - (intptr_t)&static_cast<YMDerived*>(this)->registerData),
			*(uint8_t*)(&reg)
		);
	}
};


