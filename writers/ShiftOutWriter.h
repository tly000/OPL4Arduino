/*
 * ShiftOutWriter.h
 *
 *  Created on: 24.04.2017
 *      Author: tly
 */

#pragma once

struct ShiftOutWriter{
	uint8_t latchPin, dataPin, clockPin;

	void init(){
		pinMode(latchPin,OUTPUT);
		pinMode(dataPin,OUTPUT);
		pinMode(clockPin,OUTPUT);
		digitalWrite(latchPin,HIGH);
	}

	void write(uint8_t value){
		digitalWrite(latchPin, LOW);
		shiftOut(dataPin,clockPin,MSBFIRST,value);
		digitalWrite(latchPin, HIGH);
	}

	void beginWrite(){}
	void endWrite(){}
};
