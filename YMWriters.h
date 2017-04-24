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

struct PortDWriter{
	void init(){
		DDRD = 0xFF;
	}

	void write(uint8_t value){
		PORTD = value;
	}

	void beginWrite(){
		DDRD = 0xFF;
	}
	void endWrite(){}
};



