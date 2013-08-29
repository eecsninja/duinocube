#include <stdint.h>
#include <avr/pgmspace.h>
const uint32_t spectrum_bmp_raw_data32[] PROGMEM = {
	0x1d1e1f00,0x191a1b1c,0x15161718,0x11121314,0x1e1f0001,0x1a1b1c1d,0x16171819,0x12131415,
	0x1f000102,0x1b1c1d1e,0x1718191a,0x13141516,0x10203,0x1c1d1e1f,0x18191a1b,0x14151617,
	0x1020304,0x1d1e1f00,0x191a1b1c,0x15161718,0x2030405,0x1e1f0001,0x1a1b1c1d,0x16171819,
	0x3040506,0x1f000102,0x1b1c1d1e,0x1718191a,0x4050607,0x10203,0x1c1d1e1f,0x18191a1b,
	0x5060708,0x1020304,0x1d1e1f00,0x191a1b1c,0x6070809,0x2030405,0x1e1f0001,0x1a1b1c1d,
	0x708090a,0x3040506,0x1f000102,0x1b1c1d1e,0x8090a0b,0x4050607,0x10203,0x1c1d1e1f,
	0x90a0b0c,0x5060708,0x1020304,0x1d1e1f00,0xa0b0c0d,0x6070809,0x2030405,0x1e1f0001,
	0xb0c0d0e,0x708090a,0x3040506,0x1f000102,0xc0d0e0f,0x8090a0b,0x4050607,0x10203,
	0xd0e0f10,0x90a0b0c,0x5060708,0x1020304,0xe0f1011,0xa0b0c0d,0x6070809,0x2030405,
	0xf101112,0xb0c0d0e,0x708090a,0x3040506,0x10111213,0xc0d0e0f,0x8090a0b,0x4050607,
	0x11121314,0xd0e0f10,0x90a0b0c,0x5060708,0x12131415,0xe0f1011,0xa0b0c0d,0x6070809,
	0x13141516,0xf101112,0xb0c0d0e,0x708090a,0x14151617,0x10111213,0xc0d0e0f,0x8090a0b,
	0x15161718,0x11121314,0xd0e0f10,0x90a0b0c,0x16171819,0x12131415,0xe0f1011,0xa0b0c0d,
	0x1718191a,0x13141516,0xf101112,0xb0c0d0e,0x18191a1b,0x14151617,0x10111213,0xc0d0e0f,
	0x191a1b1c,0x15161718,0x11121314,0xd0e0f10,0x1a1b1c1d,0x16171819,0x12131415,0xe0f1011,
	0x1b1c1d1e,0x1718191a,0x13141516,0xf101112,0x1c1d1e1f,0x18191a1b,0x14151617,0x10111213,
};
uint8_t* spectrum_bmp_raw_data8 = (uint8_t*) spectrum_bmp_raw_data32;
uint16_t* spectrum_bmp_raw_data16 = (uint16_t*) spectrum_bmp_raw_data32;
const int SPECTRUM_BMP_RAW_DATA_SIZE = 512;