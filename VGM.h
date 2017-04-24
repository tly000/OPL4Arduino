#pragma once

/*
 * VGM.h
 *
 *  Created on: 19.08.2016
 *      Author: tly
 */

struct VGMHeader {
	char magic[4];

	/* VGM ver1.00 */
	uint32_t EOF_offset, version, SN76489_clock,
		YM2413_clock, GD3_offset, total_samples, loop_offset,
		loop_samples;

	/* VGM ver1.01 addtions */
	uint32_t sample_rate;

	/* VGM ver1.10 addtions */
	uint16_t SN76489_feedback;
	uint8_t SN76489_shift_register_width;

	/* VGM ver1.51 addtions */
	uint8_t SN76489_flags;

	/* VGM ver1.10 addtions */
	uint32_t YM2612_clock, YM2151_clock;

	/* VGM ver1.50 addtions */
	uint32_t VGM_data_offset;

	/* VGM ver1.51 addtions */
	uint32_t SEGA_PCM_clock, SEGA_PCM_interface_register,
		RF5C68_clock, YM2203_clock, YM2608_clock, YM2610B_clock,
		YM3812_clock, YM3526_clock, YM8950_clock, YMF262_clock,
		YM278B_clock, YMF271_clock, YMZ280B_clock, RF5C164_clock,
		PWM_clock, AY8910_clock;
	uint8_t AY8910_chip_type, AY8910_flags, YM2203_AY8910_flags, YM2608_AY8910_flags;

	/* VGM ver1.60 addtions */
	uint8_t volume_modifier, reserved0, loop_base;

	/* VGM ver1.51 addtions */
	uint8_t loop_modifier;

	/* VGM ver1.61 addtions */
	uint32_t GameBoy_DMG_clock, NES_APU_clock, Multi_PCM_clock, uPD7759_clock,
		OKIM6258_clock;
	uint8_t OKIM6258_flags, K054539_flags, C140_chip_type, reserved1;

	uint32_t OKIM6295_clock, K051649_clock,
		K054539_clock, HuC6280_clock, C140_clock, K053260_clock,
		POKEY_clock, QSound_clock, reserved2;

	/* VGM ver1.70 addtions */
	uint32_t extra_header_offset, reserved3[16];
};

