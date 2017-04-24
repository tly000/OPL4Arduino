#pragma once

enum NoteMode {
	ATTACK = 0, SUSTAIN = 1, RELEASE = 2, TOBEREMOVED = 3
};

struct NoteData {
	uint8_t note;
	NoteMode mode;
};

template<uint8_t N> struct NoteManager {
	NoteManager() {
		//generate nodes
		for (uint8_t i = 0; i < N; i++) {
			noteNodes[i] = {
				/*.noteData =*/ {0,ATTACK},
				/*.previous =*/ noteNodes + (i-1),
				/*.next =*/ noteNodes + (i+1)
			};
		}
		head->previous = nullptr;
		back->next = nullptr;
	}

	int addNote(uint8_t note) {
		uint8_t& i = noteToNodeIndex[note];
		if (!i) {
			i = uint8_t(back - noteNodes + 1);
			//take back node and change noteData.
			auto& data = back->noteData;
			//set noteToNodeIndex to 0
			noteToNodeIndex[data.note] = 0;
			data.note = note;
			data.mode = ATTACK;
			if (N > 1) {
				//decouple from previous
				NoteNode* oldPrevious = back->previous;
				back->previous->next = nullptr;
				back->previous = nullptr;
				//attach back to front
				head->previous = back;
				back->next = head;
				head = back;
				//overwrite back
				back = oldPrevious;
			}
			return i - 1;
		} else {
			//rehit note.
			noteNodes[i - 1].noteData.mode = ATTACK;
			return i - 1;
		}
	}

	int removeNote(uint8_t note) {
		if (uint8_t& i = noteToNodeIndex[note]) {
			uint8_t oldNote = i;
			NoteNode* current = noteNodes + (i - 1);
			//set current note to 0.
			i = 0;
			//if note is still playing
			if (current->noteData.note == note) {
				current->noteData.note = 0;

				if (N > 1) {
					//attach node to back
					if (current != back) {
						//set next to nullptr;
						NoteNode* oldNext = current->next;
						current->next = nullptr;
						//link current to back:
						NoteNode* oldPrevious = current->previous;
						current->previous = back;
						back->next = current;
						back = current;
						//link oldNext to oldPrevious:
						if (oldPrevious) {
							oldPrevious->next = oldNext;
							oldNext->previous = oldPrevious;
						} else {
							head = oldNext;
							oldNext->previous = nullptr;
						}
					}
				}
			}
			return oldNote - 1;
		} else {
			return -1;
		}
	}

	struct NoteNode {
		NoteData noteData;
		NoteNode* previous;
		NoteNode* next;
	} noteNodes[N];
	private:
	NoteNode* head = noteNodes + 0;
	NoteNode* back = noteNodes + N - 1;

	uint8_t noteToNodeIndex[256] = { };
	public:
	template<typename F> void forEach(F&& func) {
		for (uint8_t i = 0; i < N; i++) {
			func(noteNodes[i].noteData);
		}
	}

	template<typename F> void forEachPlayingNote(F&& func) {
		for (uint8_t i = 0; i < N; i++) {
			auto& d = noteNodes[i].noteData;
			if (d.note) {
				func(d);
			}
		}
	}

	NoteNode* first() {
		return head;
	}

	NoteNode* last() {
		return back;
	}

	NoteData* getNote(uint8_t note) {
		if (uint8_t& i = noteToNodeIndex[note]) {
			NoteNode* current = noteNodes + (i - 1);
			//if note is still playing
			if (current->noteData.note == note) {
				return &current->noteData;
			}
		}
		return nullptr;
	}
};

