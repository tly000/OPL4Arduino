#pragma once
#include <Arduino.h>
#include <math.h>
#include "Instruments.h"
#include "Utils.h"
#include "YMBase.h"

/*
 * YM2413.h
 *
 *  Created on: 17.07.2016
 *      Author: tly
 */

#define __unused(x,y) volatile uint8_t u##y[x]
#define _unused(x,y) __unused(x,y)
#define unused(x) _unused((x),__LINE__)

template<typename Writer> struct YM2413 : YMBase<YM2413<Writer>,Writer>{
	constexpr static double ADDRESS_WRITE_DELAY = 5;
	constexpr static double VALUE_WRITE_DELAY = 30;

	using YMBase<YM2413,Writer>::YMBase;

	static constexpr int CYCLES_PER_SAMPLE = 72;
	static constexpr float CLOCK_FREQ = 4000000; //3579545.0;
	static constexpr float SAMPLING_FREQ = CLOCK_FREQ / CYCLES_PER_SAMPLE;
	union{
		uint8_t registers[64];
		struct{
			struct {
				uint8_t frequencyMultiple : 4;
				bool keyboardScalingRate : 1;
				bool envGeneratorType : 1;
				bool enableVibrato : 1;
				bool enableAM : 1;
			} settings1[2];
			struct{
				uint8_t totalLevel: 6;
				uint8_t ksl1 : 2;
			} levelSettings1;
			struct{
				uint8_t feedback : 3;
				bool dm : 1;
				bool dc : 1;
				bool : 1;
				uint8_t ksl2 : 2;
			} levelSettings2;
			struct {
				uint8_t decay :4;
				uint8_t attack : 4;
			} attackDecaySettings[2];
			struct {
				uint8_t release :4;
				uint8_t sustain :4;
			} sustainReleaseSettings[2];
			unused(6);
			struct{
				bool hihatOn : 1;
				bool cymbalOn: 1;
				bool tomOn: 1;
				bool snareOn: 1;
				bool bassDrumOn: 1;
				bool rythmOn: 1;
				uint8_t : 2;
			} drums;
			uint8_t testData;
			uint8_t fNumberLow[9];
			unused(7);
			struct{
				uint8_t fNumberHigh :1;
				uint8_t octave :3;
				bool keyOn :1;
				bool susOn :1;
				uint8_t :2;
			} keyOn[9];
			unused(7);
			struct {
				uint8_t level :4;
				uint8_t instrument : 4;
			} instrumentSettings[9];
			unused(7);
		} __attribute__ ((packed)) registerData;
	};

	void calculateFNumberAndBlock(float frequency, uint8_t& block, uint16_t& fnum) {
		block = log2_fast(frequency) - 5;
		fnum = frequency * uint32_t(1L << (18L - block)) / SAMPLING_FREQ;
	}

	void setFrequency(int channel, float frequency) {
		uint16_t fnum;
		uint8_t block;
		calculateFNumberAndBlock(frequency, block, fnum);
		this->registerData.fNumberLow[channel] = fnum & 0xFF;
		writeRegister(this->registerData.fNumberLow[channel]);
		this->registerData.keyOn[channel].fNumberHigh = (fnum >> 8) & 1;
		this->registerData.keyOn[channel].octave = block;
		writeRegister(this->registerData.keyOn[channel]);
	}

	void keyOn(int channel, float frequency) {
		this->registerData.keyOn[channel].keyOn = true;
		this->setFrequency(channel, frequency);
	}

	void keyOff(int channel) {
		this->registerData.keyOn[channel].keyOn = false;
		writeRegister(this->registerData.keyOn[channel]);
	}

	bool isKeyOn(int channel) const {
		return this->registerData.keyOn[channel].keyOn;
	}

	void setInstrument(int channel, int instrument) {
		registerData.instrumentSettings[channel].level = 0;
		registerData.instrumentSettings[channel].instrument = instrument;
		writeRegister(registerData.instrumentSettings[channel]);
	}

	void loadInstrument(const Instrument& inst){
		writeRegister(reinterpret_cast<uint8_t&>(this->registerData.settings1[0]) = inst.modChar);
		writeRegister(reinterpret_cast<uint8_t&>(this->registerData.settings1[0+1]) = inst.carChar);

		struct LevelSettings{
			uint8_t level :6;
			uint8_t scalingLevel : 2;
		};
		this->registerData.levelSettings1.ksl1 = reinterpret_cast<const LevelSettings&>(inst.modScale).scalingLevel;
		this->registerData.levelSettings1.totalLevel = reinterpret_cast<const LevelSettings&>(inst.modScale).level;
		writeRegister(this->registerData.levelSettings1);

		writeRegister(reinterpret_cast<uint8_t&>(this->registerData.attackDecaySettings[0]) = inst.modAttack);
		writeRegister(reinterpret_cast<uint8_t&>(this->registerData.attackDecaySettings[0+1]) = inst.carAttack);

		writeRegister(reinterpret_cast<uint8_t&>(this->registerData.sustainReleaseSettings[0]) = inst.modSustain);
		writeRegister(reinterpret_cast<uint8_t&>(this->registerData.sustainReleaseSettings[0+1]) = inst.carSustain);

		this->registerData.levelSettings2.dm = inst.modWaveSelect & 1;
		this->registerData.levelSettings2.dc = inst.carWaveSelect & 1;
		this->registerData.levelSettings2.feedback = inst.feedback;
		this->registerData.levelSettings2.ksl2 = reinterpret_cast<const LevelSettings&>(inst.carScale).scalingLevel;

		writeRegister(this->registerData.levelSettings2);
	}
};

