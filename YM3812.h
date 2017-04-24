#pragma once
#include <Arduino.h>
#include <math.h>
#include "Instruments.h"
#include "Utils.h"
#include "YMBase.h"

/*
 * YM3812.h
 *
 *  Created on: 17.07.2016
 *      Author: tly
 */

#define __unused(x,y) volatile uint8_t u##y[x]
#define _unused(x,y) __unused(x,y)
#define unused(x) _unused((x),__LINE__)

static constexpr uint8_t CH_OFFSET[9] = {
	0,1,2,8,9,10,16,17,18,
};

template<typename Writer> struct YM3812 : YMBase<YM3812<Writer>,Writer>{
	static constexpr double ADDRESS_WRITE_DELAY = 0.05;
	static constexpr double VALUE_WRITE_DELAY = 0.05;

	static constexpr uint8_t OP2_OFFSET = 3;
	static constexpr float SAMPLING_FREQ = 4000000.0 / 72.0;

	using YMBase<YM3812,Writer>::YMBase;
	union{
		uint8_t registers[256];
		struct{
			unused(1);
			struct{
				uint8_t :5,
					ws :1,
					:2;
			};
			uint8_t timer1, timer2;
			struct{
				bool timer1Ctrl :1, timer2Ctrl :1;
				uint8_t :3;
				bool timer1Mask :1, timer2Mask :1;
				bool irqReset:1;
			};
			unused(3);
			struct{
				uint8_t :6;
				bool keyboardSplit :1;
				bool csmSelect :1;
			};
			unused(16+7);
			struct {
				uint8_t frequencyMultiple : 4;
				bool keyboardScalingRate : 1;
				bool envGeneratorType : 1;
				bool enableVibrato : 1;
				bool enableAM : 1;
			} settings1[32];
			struct {
				uint8_t level :6;
				uint8_t scalingLevel : 2;
			} levelSettings[32];
			struct {
				uint8_t decay :4;
				uint8_t attack : 4;
			} attackDecaySettings[32];
			struct {
				uint8_t release :4;
				uint8_t sustain :4;
			} sustainReleaseSettings[32];
			uint8_t fNumberLow[9];
			unused(7);
			struct{
				uint8_t fNumberHigh :2;
				uint8_t octave :3;
				bool keyOn :1;
				uint8_t :2;
			} keyOn[9];
			unused(4);
			struct{
				bool hihatOn : 1;
				bool cymbalOn: 1;
				bool tomOn: 1;
				bool snareOn: 1;
				bool bassDrumOn: 1;
				bool rythmOn: 1;
				bool vibratoDepth: 1;
				bool amDepth: 1;
			} amVibDrums;
			unused(2);
			struct{
				bool algorithm :1;
				uint8_t feedbackStrength:3;
				uint8_t :4;
			} feedbackAlgorithm[9];
			unused(16+7);
			struct{
				uint8_t wave:2;
				uint8_t :6;
			} waveformSelect[32];
		} __attribute__ ((packed)) registerData;
	};

	void init(){
		YMBase<YM3812,Writer>::init();

		this->registerData.ws = true;
		this->writeRegister(*((uint8_t*)(&this->registerData) + 1));

		this->setMultiple(true,1);
		this->setLevel(true,0x2F);
		this->setAttack(true,15);
		this->setDecay(true,0);
		this->setSustain(true,3);
		this->setRelease(true,7);

		this->setMultiple(false,1);
		this->setLevel(false,0x2F);
		this->setAttack(false,15);
		this->setDecay(false,0);
		this->setSustain(false,7);
		this->setRelease(false,7);
	}

	#define _set(name,bitfield,position) \
	void set##name(bool modulator,uint8_t val,uint8_t channel = -1){ \
		if(channel == -1){ \
			for(int i = 0; i < 9; i++){ \
				int offset = CH_OFFSET[i] + (modulator ? 0 : OP2_OFFSET); \
				this->registerData.position[offset].bitfield = val; \
				writeRegister(this->registerData.position[offset]); \
			}  \
		}else{ \
			int offset = CH_OFFSET[channel] + (modulator ? 0 : OP2_OFFSET); \
			this->registerData.position[offset].bitfield = val; \
			writeRegister(this->registerData.position[offset]); \
		} \
	} \
	uint8_t get##name(bool modulator,uint8_t channel = 0){ \
		int offset = CH_OFFSET[channel] + (modulator ? 0 : OP2_OFFSET); \
		return this->registerData.position[offset].bitfield; \
	}

	_set(Multiple,frequencyMultiple,settings1);
	_set(Level,level,levelSettings);
	_set(Attack,attack,attackDecaySettings);
	_set(Decay,decay,attackDecaySettings);
	_set(Sustain,sustain,sustainReleaseSettings);
	_set(Release,release,sustainReleaseSettings);
	_set(Waveform,wave,waveformSelect);
	_set(SustainMode,envGeneratorType,settings1);

	#undef _set

	//there were some hints in multiple yamaha datasheets how to calculate the F-Nums manually,
	//this calculation was derived by some trial and error.
	void calculateFNumberAndBlock(float frequency,uint8_t& block,uint16_t& fnum){
		block = log2_fast(frequency) - 4;
		fnum = frequency * uint32_t(1L << (20L - block)) / SAMPLING_FREQ;
		block -= 1;
	}

	void keyOn(int channel, float frequency){
		this->registerData.keyOn[channel].keyOn = true;
		this->setFrequency(channel,frequency);
	}

	void setFrequency(int channel, float frequency){
		uint16_t fnum;
		uint8_t block;
		calculateFNumberAndBlock(frequency,block,fnum);
		this->registerData.fNumberLow[channel] = fnum & 0xFF;
		writeRegister(this->registerData.fNumberLow[channel]);
		this->registerData.keyOn[channel].fNumberHigh = (fnum >> 8) & 3;
		this->registerData.keyOn[channel].octave = block;
		writeRegister(this->registerData.keyOn[channel]);
	}

	void keyOff(int channel){
		this->registerData.keyOn[channel].keyOn = false;
		writeRegister(this->registerData.keyOn[channel]);
	}

	bool isKeyOn(int channel) const {
		return this->registerData.keyOn[channel].keyOn;
	}

	void setRhythmEnabled(bool b){
		if(b){
			for(int i = 6; i < 9; i++){
				this->keyOff(i);
			}
		}
		this->registerData.amVibDrums.rythmOn = b;
		this->writeRegister(this->registerData.amVibDrums);
	}

	void drumOn(YMDrum d){
		reinterpret_cast<uint8_t&>(this->registerData.amVibDrums) |= (1 << d);
		this->writeRegister(this->registerData.amVibDrums);
	}

	bool isDrumOn(YMDrum d){
		return reinterpret_cast<const uint8_t&>(this->registerData.amVibDrums) & (1 << d);
	}

	void drumOff(YMDrum d){
		reinterpret_cast<uint8_t&>(this->registerData.amVibDrums) &= ~(1 << d);
		this->writeRegister(this->registerData.amVibDrums);
	}

	void loadInstrument(const Instrument& inst){
		for(int i = 0; i < 9; i++){
			int offset = CH_OFFSET[i];
			writeRegister(reinterpret_cast<uint8_t&>(this->registerData.settings1[offset]) = inst.modChar);
			writeRegister(reinterpret_cast<uint8_t&>(this->registerData.settings1[offset+OP2_OFFSET]) = inst.carChar);

			writeRegister(reinterpret_cast<uint8_t&>(this->registerData.levelSettings[offset]) = inst.modScale);
			writeRegister(reinterpret_cast<uint8_t&>(this->registerData.levelSettings[offset+OP2_OFFSET]) = inst.carScale);

			writeRegister(reinterpret_cast<uint8_t&>(this->registerData.attackDecaySettings[offset]) = inst.modAttack);
			writeRegister(reinterpret_cast<uint8_t&>(this->registerData.attackDecaySettings[offset+OP2_OFFSET]) = inst.carAttack);

			writeRegister(reinterpret_cast<uint8_t&>(this->registerData.sustainReleaseSettings[offset]) = inst.modSustain);
			writeRegister(reinterpret_cast<uint8_t&>(this->registerData.sustainReleaseSettings[offset+OP2_OFFSET]) = inst.carSustain);

			writeRegister(reinterpret_cast<uint8_t&>(this->registerData.waveformSelect[offset]) = inst.modWaveSelect);
			writeRegister(reinterpret_cast<uint8_t&>(this->registerData.waveformSelect[offset+OP2_OFFSET]) = inst.carWaveSelect);

			writeRegister(reinterpret_cast<uint8_t&>(this->registerData.feedbackAlgorithm[offset]) = inst.feedback);
		}
	}
};

