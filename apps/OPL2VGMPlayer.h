#pragma once
/*
 * OPL2VGMPlayer.h
 *
 *  Created on: 24.04.2017
 *      Author: tly
 */

#include <Arduino.h>
#include "../VGMPlayer.h"
YM3812<SPIWriter> ym(/*csPin*/ A1,/*a0Pin*/ A4,/*icPin*/ A5, SPIWriter{/*latchPin*/6});
void setup(){
	ym.init();

	VGMPlayer<decltype(ym)> player(ym);
	player.playSong("YOUGET.VGM");
}

void loop(){}



