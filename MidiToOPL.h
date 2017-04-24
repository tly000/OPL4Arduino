#pragma once
#include "Midi.h"
#include "Note.h"

/*
 * MidiToOPL.h
 *
 *  Created on: 21.08.2016
 *      Author: tly
 */

inline float pitchBendFreq(float freq,float t,float range) {
	return freq * pow(2,range * t / 12);
}

template<typename Serial,typename YM> struct MidiToOpl{
	MidiToOpl(Serial& s,YM& ym) : midi(s), ym(ym){}

	float pitchBendRange = 2;

	void tick(){
		midi.eventFunc();
	}

	void init(){
		static MidiToOpl<Serial,YM>* instance = this;
		midi.begin();
		midi.onKeyPressed = [](uint8_t note,uint8_t velocity){
			auto n = instance->noteManager.getNote(note);
			int noteIndex = instance->noteManager.addNote(note);
			if(n || instance->ym.isKeyOn(noteIndex)){
				//note already existed, needs rehit.
				instance->ym.keyOff(noteIndex);
			}
			instance->ym.keyOn(noteIndex,pitchBendFreq(MIDI_TO_FREQ(note),instance->pitchBend,instance->pitchBendRange));
		};
		midi.onKeyReleased = [](uint8_t note){
			if(!instance->sustain){
				int noteIndex = instance->noteManager.removeNote(note);
				if(noteIndex >= 0 && noteIndex < 9){
					instance->ym.keyOff(noteIndex);
				}
			}else{
				if(auto n = instance->noteManager.getNote(note)){
					n->mode = SUSTAIN;
				}
			}
		};
		midi.onPitchBend = [](uint16_t value){
			instance->pitchBend = (value - 8192.0)/8192;
			for(uint8_t i = 0; i < 9; i++){
				auto& node = instance->noteManager.noteNodes[i];
				if(node.noteData.note){
					instance->ym.setFrequency(i,pitchBendFreq(MIDI_TO_FREQ(node.noteData.note),instance->pitchBend,instance->pitchBendRange));
				}
			}
		};
		midi.onControlChange = [](uint8_t controller,uint8_t value){
			if(controller == 64){
				if(value > 63){
					instance->sustain = true;
				}else{
					instance->sustain = false;
					for(uint8_t i = 0; i < 9; i++){
						auto& node = instance->noteManager.noteNodes[i];
						if(node.noteData.note && node.noteData.mode == SUSTAIN){
							int noteIndex = instance->noteManager.removeNote(node.noteData.note);
							if(noteIndex >= 0 && noteIndex < 9){
								instance->ym.keyOff(noteIndex);
							}
						}
					}
				}
			}
		};
	}

	Midi<Serial> midi;
protected:
	float pitchBend = 0;
	bool sustain = false;
	YM& ym;
	NoteManager<9> noteManager;
};

