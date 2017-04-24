#pragma once
/*
 * OPLLPlayer.h
 *
 *  Created on: 24.04.2017
 *      Author: tly
 */

#include <Arduino.h>
#include "../YM2413.h"
#include "../writers/PortDWriter.h"
#include "../VGMPlayer.h"

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



