#pragma once

#include <stddef.h>
#include <Arduino.h>

template<typename S> struct Midi {
	Midi(S& serial) : serial(serial) {}

	void begin() {
		serial.begin(31250);
	}

	void eventFunc() {
		while (serial.available()) {
			static uint8_t currentPos = 0;
			static uint8_t currentCommand = 0;
			static uint8_t bytes[3] { };
			uint8_t data = serial.read();
			if (data & 0x80) {
				currentCommand = data;
			} else {
				switch (currentCommand >> 4) {
				case 0x8:
				case 0x9:
					bytes[currentPos++] = data;
					if (currentPos == 2) {
						currentPos = 0;
						if ((currentCommand >> 4) == 0x8 || bytes[1] == 0) {
							if (onKeyReleased)
								onKeyReleased(bytes[0]);
						} else {
							if (onKeyPressed)
								onKeyPressed(bytes[0], bytes[1]);
						}
					}
					break;
				case 0b1110:
					bytes[currentPos++] = data;
					if (currentPos == 2) {
						currentPos = 0;
						onPitchBend(bytes[0] | (bytes[1] << 7));
					}
					break;
				case 0b1100:
					currentPos = 0;
					onProgramChange(data);
					break;
				case 0b1011:
					bytes[currentPos++] = data;
					if (currentPos == 2) {
						currentPos = 0;
						onControlChange(bytes[0],bytes[1]);
					}
					break;
				}
			}
		}
	}

	S& serial;
	void (*onKeyPressed)(uint8_t key, uint8_t gain) = [](uint8_t,uint8_t){};
	void (*onKeyReleased)(uint8_t key) = [](uint8_t){};
	void (*onPitchBend)(uint16_t value) = [](uint16_t){};
	void (*onProgramChange)(uint8_t instrument) = [](uint8_t){};
	void (*onControlChange)(uint8_t controller,uint8_t value) = [](uint8_t,uint8_t){};
};



