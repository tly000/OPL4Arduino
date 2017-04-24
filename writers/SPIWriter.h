#pragma once

/*
 * SPIWriter.h
 *
 *  Created on: 21.04.2017
 *      Author: tly
 */

struct SPIWriter{
	uint8_t latchPin;

	void init(){
		pinMode(latchPin,OUTPUT);
		digitalWrite(latchPin,HIGH);
	}

	void write(uint8_t value){
		digitalWrite(latchPin, LOW);
		SPI.transfer(value);
		digitalWrite(latchPin, HIGH);
	}

	void beginWrite(){
		SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
	}

	void endWrite(){
		SPI.endTransaction();
	}
};
