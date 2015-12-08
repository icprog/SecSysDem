//********************************************************
//**** Functions for Interfacing NOKIA 5110 Display *****
//********************************************************
// ---- ORIGINAL ----
//Controller:	ATmega32 (Clock: 1 Mhz-internal)
//Compiler:		ImageCraft ICCAVR
//Author:		CC Dharmani, Chennai (India)
//Date:			Sep 2008
//
// ---- MODIFIED ----
//Modified by Michael Spiceland (http://tinkerish.com) to
//pixel level functions with lcd_buffer[][].
//Jan 2009
//
// ---- MODIFIED ----
//Modified by Luke Ganzevoort 
//Date: 9/5/2015
//Controller: ATmega328P
//Aquired from: https://github.com/mspiceland/avr-spiceduino-3310-thermistor
//
//********************************************************

#include <avr/io.h>

#include "LCD_spi.h"
#include "secSysDefines.h"
//#include "splash-temp.h"

//global variable for remembering where to start writing the next text string on 3310 LCD
unsigned char char_start;

/* current cursor */
static unsigned char cursor_row = 0; /* 0-6 */
static unsigned int cursor_col = 0; /* 0-83 */
static unsigned char lcd_buffer[6][84];

//***** Small fonts (5x7) **********
static const unsigned char smallFont[] PROGMEM =
{
     0x00, 0x00, 0x00, 0x00, 0x00,   // sp
     0x00, 0x00, 0x2f, 0x00, 0x00,    // !
     0x00, 0x07, 0x00, 0x07, 0x00,   // "
     0x14, 0x7f, 0x14, 0x7f, 0x14,   // #
     0x24, 0x2a, 0x7f, 0x2a, 0x12,   // $
     0xc4, 0xc8, 0x10, 0x26, 0x46,   // %
     0x36, 0x49, 0x55, 0x22, 0x50,   // &
     0x00, 0x05, 0x03, 0x00, 0x00,   // '
     0x00, 0x1c, 0x22, 0x41, 0x00,   // (
     0x00, 0x41, 0x22, 0x1c, 0x00,   // )
     0x14, 0x08, 0x3E, 0x08, 0x14,   // *
     0x08, 0x08, 0x3E, 0x08, 0x08,   // +
     0x00, 0x00, 0x50, 0x30, 0x00,   // ,
     0x10, 0x10, 0x10, 0x10, 0x10,   // -
     0x00, 0x60, 0x60, 0x00, 0x00,   // .
     0x20, 0x10, 0x08, 0x04, 0x02,   // /
     0x3E, 0x51, 0x49, 0x45, 0x3E,   // 0
     0x00, 0x42, 0x7F, 0x40, 0x00,   // 1
     0x42, 0x61, 0x51, 0x49, 0x46,   // 2
     0x21, 0x41, 0x45, 0x4B, 0x31,   // 3
     0x18, 0x14, 0x12, 0x7F, 0x10,   // 4
     0x27, 0x45, 0x45, 0x45, 0x39,   // 5
     0x3C, 0x4A, 0x49, 0x49, 0x30,   // 6
     0x01, 0x71, 0x09, 0x05, 0x03,   // 7
     0x36, 0x49, 0x49, 0x49, 0x36,   // 8
     0x06, 0x49, 0x49, 0x29, 0x1E,   // 9
     0x00, 0x36, 0x36, 0x00, 0x00,   // :
     0x00, 0x56, 0x36, 0x00, 0x00,   // ;
     0x08, 0x14, 0x22, 0x41, 0x00,   // <
     0x14, 0x14, 0x14, 0x14, 0x14,   // =
     0x00, 0x41, 0x22, 0x14, 0x08,   // >
     0x02, 0x01, 0x51, 0x09, 0x06,   // ?
     0x32, 0x49, 0x59, 0x51, 0x3E,   // @
     0x7E, 0x11, 0x11, 0x11, 0x7E,   // A
     0x7F, 0x49, 0x49, 0x49, 0x36,   // B
     0x3E, 0x41, 0x41, 0x41, 0x22,   // C
     0x7F, 0x41, 0x41, 0x22, 0x1C,   // D
     0x7F, 0x49, 0x49, 0x49, 0x41,   // E
     0x7F, 0x09, 0x09, 0x09, 0x01,   // F
     0x3E, 0x41, 0x49, 0x49, 0x7A,   // G
     0x7F, 0x08, 0x08, 0x08, 0x7F,   // H
     0x00, 0x41, 0x7F, 0x41, 0x00,   // I
     0x20, 0x40, 0x41, 0x3F, 0x01,   // J
     0x7F, 0x08, 0x14, 0x22, 0x41,   // K
     0x7F, 0x40, 0x40, 0x40, 0x40,   // L
     0x7F, 0x02, 0x0C, 0x02, 0x7F,   // M
     0x7F, 0x04, 0x08, 0x10, 0x7F,   // N
     0x3E, 0x41, 0x41, 0x41, 0x3E,   // O
     0x7F, 0x09, 0x09, 0x09, 0x06,   // P
     0x3E, 0x41, 0x51, 0x21, 0x5E,   // Q
     0x7F, 0x09, 0x19, 0x29, 0x46,   // R
     0x46, 0x49, 0x49, 0x49, 0x31,   // S
     0x01, 0x01, 0x7F, 0x01, 0x01,   // T
     0x3F, 0x40, 0x40, 0x40, 0x3F,   // U
     0x1F, 0x20, 0x40, 0x20, 0x1F,   // V
     0x3F, 0x40, 0x38, 0x40, 0x3F,   // W
     0x63, 0x14, 0x08, 0x14, 0x63,   // X
     0x07, 0x08, 0x70, 0x08, 0x07,   // Y
     0x61, 0x51, 0x49, 0x45, 0x43,   // Z
     0x00, 0x7F, 0x41, 0x41, 0x00,   // [
     0x55, 0x2A, 0x55, 0x2A, 0x55,   // 55
     0x00, 0x41, 0x41, 0x7F, 0x00,   // ]
     0x04, 0x02, 0x01, 0x02, 0x04,   // ^
     0x40, 0x40, 0x40, 0x40, 0x40,   // _
     0x00, 0x01, 0x02, 0x04, 0x00,   // '
     0x20, 0x54, 0x54, 0x54, 0x78,   // a
     0x7F, 0x48, 0x44, 0x44, 0x38,    // b
     0x38, 0x44, 0x44, 0x44, 0x20,   // c
     0x38, 0x44, 0x44, 0x48, 0x7F,   // d
     0x38, 0x54, 0x54, 0x54, 0x18,   // e
     0x08, 0x7E, 0x09, 0x01, 0x02,   // f
     0x0C, 0x52, 0x52, 0x52, 0x3E,   // g
     0x7F, 0x08, 0x04, 0x04, 0x78,   // h
     0x00, 0x44, 0x7D, 0x40, 0x00,   // i
     0x20, 0x40, 0x44, 0x3D, 0x00,   // j
     0x7F, 0x10, 0x28, 0x44, 0x00,   // k
     0x00, 0x41, 0x7F, 0x40, 0x00,   // l
     0x7C, 0x04, 0x18, 0x04, 0x78,   // m
     0x7C, 0x08, 0x04, 0x04, 0x78,   // n
     0x38, 0x44, 0x44, 0x44, 0x38,   // o
     0x7C, 0x14, 0x14, 0x14, 0x08,   // p
     0x08, 0x14, 0x14, 0x18, 0x7C,   // q
     0x7C, 0x08, 0x04, 0x04, 0x08,   // r
     0x48, 0x54, 0x54, 0x54, 0x20,   // s
     0x04, 0x3F, 0x44, 0x40, 0x20,   // t
     0x3C, 0x40, 0x40, 0x20, 0x7C,   // u
     0x1C, 0x20, 0x40, 0x20, 0x1C,   // v
     0x3C, 0x40, 0x30, 0x40, 0x3C,   // w
     0x44, 0x28, 0x10, 0x28, 0x44,   // x
     0x0C, 0x50, 0x50, 0x50, 0x3C,   // y
     0x44, 0x64, 0x54, 0x4C, 0x44,   // z
	 0x00, 0x06, 0x09, 0x09, 0x06    // �
};

static const uint8_t twoTurtleGangLogo[504] PROGMEM ={
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x80,
	0xc0,
	0x40,
	0x60,
	0x60,
	0x20,
	0x20,
	0x30,
	0x10,
	0x10,
	0x10,
	0x10,
	0x10,
	0x10,
	0x10,
	0x10,
	0x10,
	0x20,
	0x20,
	0x20,
	0x40,
	0xc0,
	0x80,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x80,
	0x80,
	0x40,
	0x40,
	0x40,
	0x20,
	0x20,
	0x20,
	0x30,
	0x10,
	0x10,
	0x10,
	0x10,
	0x30,
	0x20,
	0x20,
	0x60,
	0x40,
	0xc0,
	0x80,
	0x80,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0xf0,
	0x08,
	0x0c,
	0xc4,
	0x64,
	0x26,
	0xa5,
	0xa5,
	0xa8,
	0x48,
	0x90,
	0x10,
	0x30,
	0x30,
	0x18,
	0x98,
	0xc8,
	0x6c,
	0xa4,
	0xb6,
	0x92,
	0x13,
	0x13,
	0x33,
	0xe2,
	0x06,
	0x0c,
	0x1c,
	0x38,
	0x08,
	0x09,
	0x0e,
	0x0c,
	0x10,
	0x10,
	0xa0,
	0xc0,
	0x20,
	0x20,
	0x20,
	0x20,
	0x40,
	0x80,
	0x00,
	0x80,
	0x40,
	0x20,
	0x38,
	0x26,
	0x72,
	0x09,
	0xf4,
	0x0a,
	0xca,
	0x4a,
	0xda,
	0xe4,
	0x0c,
	0x14,
	0x04,
	0x04,
	0x14,
	0x18,
	0xe4,
	0xd2,
	0x4a,
	0xc9,
	0x09,
	0x1a,
	0xe2,
	0x04,
	0x79,
	0x33,
	0x26,
	0x5c,
	0xf0,
	0xc0,
	0x80,
	0x00,
	0x00,
	0x80,
	0x40,
	0x20,
	0x3c,
	0x93,
	0x71,
	0x30,
	0x19,
	0x1a,
	0x0c,
	0x0c,
	0x0d,
	0x05,
	0x05,
	0x06,
	0x07,
	0x02,
	0x03,
	0x03,
	0x07,
	0x06,
	0x05,
	0x05,
	0x05,
	0x0c,
	0x0c,
	0x08,
	0x1c,
	0x17,
	0x04,
	0x04,
	0x04,
	0x06,
	0x04,
	0x04,
	0x04,
	0x04,
	0x0c,
	0x0c,
	0x0f,
	0x0f,
	0x33,
	0x42,
	0xfc,
	0x08,
	0x00,
	0xff,
	0x00,
	0xbb,
	0x64,
	0x26,
	0x12,
	0x09,
	0x08,
	0x18,
	0x0b,
	0x04,
	0x07,
	0x03,
	0x03,
	0x03,
	0x02,
	0x02,
	0x02,
	0x02,
	0x02,
	0x02,
	0x03,
	0x03,
	0x07,
	0x07,
	0x0a,
	0x19,
	0x18,
	0x10,
	0x10,
	0x10,
	0x30,
	0x21,
	0x22,
	0xc4,
	0xfb,
	0x12,
	0x33,
	0x3f,
	0xc0,
	0x80,
	0x00,
	0x08,
	0xf0,
	0xf0,
	0x20,
	0x60,
	0x40,
	0x40,
	0x40,
	0x80,
	0x80,
	0x80,
	0x80,
	0x80,
	0x80,
	0x40,
	0x40,
	0x40,
	0x40,
	0x40,
	0x60,
	0x50,
	0xd0,
	0xc8,
	0x28,
	0x2c,
	0x24,
	0x26,
	0xf2,
	0xae,
	0xa2,
	0xf6,
	0x3c,
	0x10,
	0x00,
	0x00,
	0x00,
	0x80,
	0xc0,
	0x70,
	0xdf,
	0x80,
	0x7f,
	0x00,
	0x00,
	0x03,
	0x0c,
	0x30,
	0x46,
	0x82,
	0x87,
	0x05,
	0x08,
	0x18,
	0x30,
	0x70,
	0xe0,
	0xe0,
	0xe0,
	0x60,
	0x40,
	0x40,
	0x00,
	0x40,
	0x40,
	0x40,
	0x40,
	0xe0,
	0xc0,
	0x60,
	0x30,
	0x18,
	0x09,
	0x0e,
	0x04,
	0x08,
	0x80,
	0xc1,
	0x31,
	0x1e,
	0x00,
	0x00,
	0x00,
	0x01,
	0x03,
	0x82,
	0x84,
	0x85,
	0x89,
	0x0b,
	0x1a,
	0x12,
	0x33,
	0x23,
	0x66,
	0x44,
	0x44,
	0xc4,
	0xc4,
	0x84,
	0x84,
	0x84,
	0x84,
	0x84,
	0x82,
	0xc2,
	0x42,
	0x43,
	0x63,
	0x22,
	0x32,
	0x13,
	0x1b,
	0x19,
	0x08,
	0x0c,
	0x04,
	0x04,
	0x06,
	0x02,
	0x07,
	0x0d,
	0x3f,
	0x40,
	0x00,
	0x03,
	0x06,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x01,
	0x02,
	0x06,
	0x04,
	0x0c,
	0x0a,
	0x1c,
	0x18,
	0x31,
	0x23,
	0x62,
	0x42,
	0x42,
	0x42,
	0x42,
	0x22,
	0x31,
	0x19,
	0x0c,
	0x0a,
	0x08,
	0x0c,
	0x04,
	0x06,
	0x02,
	0x03,
	0x01,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x7f,
	0x7c,
	0x6c,
	0x6c,
	0x7c,
	0x7f,
	0x00,
	0x00,
	0x00,
	0x00,
	0x44,
	0x62,
	0x52,
	0x4c,
	0x00,
	0x00,
	0x00,
	0x00,
	0x02,
	0x02,
	0x7e,
	0x02,
	0x02,
	0x00,
	0x3e,
	0x40,
	0x40,
	0x40,
	0x3e,
	0x00,
	0x7e,
	0x1a,
	0x2a,
	0x4e,
	0x00,
	0x02,
	0x02,
	0x7e,
	0x02,
	0x02,
	0x00,
	0x7e,
	0x40,
	0x40,
	0x40,
	0x00,
	0x7e,
	0x4a,
	0x4a,
	0x42,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x3c,
	0x42,
	0x42,
	0x52,
	0x34,
	0x00,
	0x7c,
	0x12,
	0x12,
	0x12,
	0x7c,
	0x00,
	0x7e,
	0x04,
	0x18,
	0x20,
	0x7e,
	0x00,
	0x3c,
	0x42,
	0x42,
	0x52,
	0x34,
	0x00,
	0x00,
	0x00
};


//******* VERY LARGE FONTS ********** 
//used here for displaying temperature
// #if 0
// 
// static const unsigned char mega_number[13][3][16] PROGMEM = {
// {
// 	{0,128,192,224,224,96,224,224,192,128,0,0,0,0,0,0}, //'0'
// 	 {112,255,255,1,0,0,0,0,255,255,254,0,0,0,0,0},
// 	 {0,15,31,60,56,48,56,56,31,15,3,0,0,0,0,0}},
// 	{{0,0,0,0,128,224,224,0,0,0,0,0,0,0,0,0},//'1'
// 	 {0,0,3,3,3,255,255,0,0,0,0,0,0,0,0,0},
// 	 {0,0,56,56,56,63,63,56,56,56,0,0,0,0,0,0}},
// 	{{0,192,192,224,96,96,224,224,192,128,0,0,0,0,0,0},//'2'
// 	 {0,1,0,0,128,192,224,249,63,31,0,0,0,0,0,0},
// 	 {0,60,62,63,63,59,57,56,56,56,56,0,0,0,0,0}},
// 	{{0,192,224,224,96,96,224,224,192,192,0,0,0,0,0,0},   //'3'
// 	 {0,1,0,0,48,48,56,125,239,207,0,0,0,0,0,0},
// 	 {0,28,56,56,48,48,56,60,31,15,1,0,0,0,0,0}},
// 	{{0,0,0,0,0,128,192,224,224,0,0,0,0,0,0,0}, //'4'
// 	 {224,240,248,222,207,199,193,255,255,192,192,0,0,0,0,0},
// 	 {0,0,0,0,0,0,0,63,63,0,0,0,0,0,0,0}},
// 	{{0,224,224,224,224,224,224,224,224,224,224,0,0,0,0,0},//'5'
// 	 {0,63,63,63,56,56,48,112,240,224,0,0,0,0,0,0},
// 	 {0,28,56,56,48,48,56,60,31,15,1,0,0,0,0,0}},
// 	{{0,0,128,192,192,224,96,96,224,224,0,0,0,0,0,0},//'6'
// 	 {224,254,255,55,57,24,24,56,240,240,192,0,0,0,0,0},
// 	 {0,15,31,28,56,48,48,56,31,15,7,0,0,0,0,0}},
// 	{{0,224,224,224,224,224,224,224,224,224,224,0,0,0,0,0},	 //'7'
// 	 {0,0,0,0,128,224,248,126,31,7,1,0,0,0,0,0},
// 	 {0,0,56,62,31,7,1,0,0,0,0,0,0,0,0,0}},
// 	{{0,128,192,224,224,96,96,224,192,192,0,0,0,0,0,0},	 //'8'
// 	 {0,207,255,127,56,48,112,112,255,239,199,0,0,0,0,0},
// 	 {3,15,31,60,56,48,48,56,31,31,15,0,0,0,0,0}},
// 	{{0,128,192,224,224,96,224,224,192,128,0,0,0,0,0,0},//'9'
// 	 {12,63,127,241,224,192,192,225,255,255,254,0,0,0,0,0},
// 	 {0,0,56,48,48,56,56,30,15,7,0,0,0,0,0,0}},
// 	{{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},	 //'.'
// 	 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
// 	 {60,60,60,0,0,0,0,0,0,0,0,0,0,0,0,0} },
// 	{{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},  //'+'
// 	 {0,0,64,64,64,64,64,254,254,64,64,64,64,64,0,0},
// 	 {0,0,0,0,0,0,0,15,15,0,0,0,0,0,0,0}},
// 	{{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},  //'-'
// 	 {0,64,64,64,64,64,64,0,0,0,0,0,0,0,0,0},
// 	 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}}
// };

// static const uint8_t image_data_Image[504] = {
// 	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x0f,0x1f,0x1f,0x3e,0x7c,0x78,0x78,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0x78,0x78,0x7c,0x3e,0x1f,0x1f,0x0f,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
// 	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7f,0xff,0xff,0xe0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xf8,0xff,0xff,0x7f,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
// 	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x03,0x03,0xff,0xff,0xff,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x8b,0xff,0xff,0xff,0xab,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
// 	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfc,0xf8,0xf0,0xf0,0xf8,0xf8,0xfc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
// 	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x7f,0x18,0x00,0x00,0x00,0x00,0x3c,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
// 	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x7f,0x7f,0x7f,0x7f,0x7f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
// };
/*
static const uint8_t image_data_logo[504] = {
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0xf8,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0xc0,
	0x70,
	0x18,
	0x08,
	0x08,
	0x30,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x70,
	0xf8,
	0x88,
	0xcc,
	0x7c,
	0x38,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0xe0,
	0x38,
	0x0c,
	0x38,
	0xf0,
	0x00,
	0x00,
	0x00,
	0xf0,
	0x18,
	0x0c,
	0x04,
	0x04,
	0x0c,
	0x18,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x1f,
	0x10,
	0x10,
	0x10,
	0x10,
	0x10,
	0x00,
	0x00,
	0x0f,
	0x08,
	0x18,
	0x12,
	0x12,
	0x1e,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x0c,
	0x1e,
	0x13,
	0x11,
	0x1b,
	0x0e,
	0x0f,
	0x18,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x1f,
	0x01,
	0x01,
	0x01,
	0x01,
	0x01,
	0x1f,
	0x00,
	0x00,
	0x07,
	0x1c,
	0x18,
	0x10,
	0x10,
	0x18,
	0x1c,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x30,
	0x30,
	0x30,
	0xf0,
	0xf0,
	0x30,
	0x30,
	0x30,
	0x00,
	0x00,
	0xf0,
	0xf0,
	0xe0,
	0xc0,
	0x80,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0xf0,
	0xf0,
	0x00,
	0x00,
	0x00,
	0xc0,
	0xe0,
	0x70,
	0x30,
	0x30,
	0x30,
	0x30,
	0x70,
	0x60,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0xc0,
	0xc0,
	0xc0,
	0xff,
	0xff,
	0xc0,
	0xc0,
	0xc0,
	0x00,
	0x00,
	0xff,
	0xff,
	0x00,
	0x01,
	0x03,
	0x07,
	0x0e,
	0x1c,
	0x38,
	0x70,
	0xff,
	0xff,
	0x00,
	0x00,
	0x00,
	0x7f,
	0xff,
	0xe0,
	0xc0,
	0xc0,
	0xc0,
	0xe0,
	0xf0,
	0x30,
	0x00,
	0x00,
	0xc0,
	0xc0,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00
};
static const uint8_t image_data_Image[504] = {
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x80,
	0xe0,
	0xf0,
	0xf8,
	0xf8,
	0xfc,
	0x7c,
	0x3e,
	0x3e,
	0x1e,
	0x1f,
	0x0f,
	0x0f,
	0x0f,
	0x0f,
	0x0f,
	0x1f,
	0x1e,
	0x3e,
	0x3e,
	0x7c,
	0xfc,
	0xf8,
	0xf8,
	0xf0,
	0xe0,
	0x80,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0x03,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x03,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0xc0,
	0xc0,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xc0,
	0xc0,
	0xc0,
	0xc0,
	0xc0,
	0xc0,
	0xc0,
	0xc0,
	0xc0,
	0xc0,
	0xc0,
	0xc0,
	0xc0,
	0xc0,
	0xc0,
	0xc0,
	0xc0,
	0xc0,
	0xc0,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xc0,
	0xc0,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0x3f,
	0x1f,
	0x0f,
	0x0f,
	0x0f,
	0x0f,
	0x1f,
	0x3f,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xfc,
	0x18,
	0x00,
	0x00,
	0x00,
	0x00,
	0x18,
	0xfc,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xfe,
	0xfe,
	0xfe,
	0xfe,
	0xfe,
	0xfe,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00
};
#endif */

/*--------------------------------------------------------------------------------------------------
  Name         :  spi_init
  Description  :  Initialising atmega SPI for using with 3310 LCD
  Argument(s)  :  None.
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
//SPI initialize
//clock rate: 250000hz
void spi_init(void)
{
	//Enable spi
	//Set as master
	//Set prescaler of 16
	SPCR = 0x51; //setup SPI
}

/*--------------------------------------------------------------------------------------------------
  Name         :  LCD_init
  Description  :  LCD controller initialization.
  Argument(s)  :  None.
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
void LCD_init ( void )
{
	DDRD |= LCD_BACKLIGHT_PIN | LCD_SS_PIN | LCD_DC_PIN_IO | LCD_RST_PIN_IO;//sets LCD pins as outputs
	
	delay_ms(100);
			  
	CLEAR_SCE_PIN;    //Enable LCD
				
	CLEAR_RST_PIN;	//reset LCD
	delay_ms(100);
	SET_RST_PIN;
			  
	SET_SCE_PIN;	//disable LCD

	LCD_writeCommand( 0x21 );  // LCD Extended Commands.
	LCD_writeCommand( 0xC8 );  // Set LCD Vop (Contrast).
	LCD_writeCommand( 0x06 );  // Set Temp coefficent.
	LCD_writeCommand( 0x13 );  // LCD bias mode 1:48.
	LCD_writeCommand( 0x20 );  // LCD Standard Commands, Horizontal addressing mode.
	LCD_writeCommand( 0x0c );  // LCD in normal mode.

	LCD_clear();
}

/*--------------------------------------------------------------------------------------------------
  Name         :  LCD_writeCommand
  Description  :  Sends command to display controller.
  Argument(s)  :  command -> command to be sent
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
void LCD_writeCommand ( unsigned char command )
{
	CLEAR_SCE_PIN;	  //enable LCD

	CLEAR_DC_PIN;	  //set LCD in command mode

	//  Send data to display controller.
	SPDR = command;

	//  Wait until Tx register empty.
	while ( !(SPSR & 0x80) );

	SET_SCE_PIN;   	 //disable LCD
}

/*--------------------------------------------------------------------------------------------------
  Name         :  LCD_writeData
  Description  :  Sends Data to display controller.
  Argument(s)  :  Data -> Data to be sent
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
void LCD_writeData ( unsigned char Data )
{
	CLEAR_SCE_PIN;	  //enable LCD

	SET_DC_PIN;	  //set LCD in Data mode

	//  Send data to display controller.
	SPDR = Data;

	//  Wait until Tx register empty.
	while ( !(SPSR & 0x80) );

	SET_SCE_PIN;   	 //disable LCD
}

/*--------------------------------------------------------------------------------------------------
  Name         :  LCD_clear
  Description  :  Clears the display
  Argument(s)  :  None.
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
void LCD_clear ( void )
{
	int i,j;
	
	LCD_gotoXY (0,0);  	//start with (0,0) position

	for(i=0; i<8; i++)
	{
		for(j=0; j<90; j++)
		{
			LCD_writeData( 0x00 );
			if ((i < 6) && (j < 84))
				lcd_buffer[i][j] = 0x00;
		}
	}
   
    LCD_gotoXY (0,0);	//bring the XY position back to (0,0)
      
}

/*--------------------------------------------------------------------------------------------------
  Name         :  LCD_clear_row
  Description  :  Clears the display row
  Argument(s)  :  row, 1-6, the row to be cleared
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
void LCD_clear_row (uint8_t row)
{
	int j;
	
	LCD_gotoXY (0,row);  	//start with (0,row) position

		for(j=0; j<84; j++)
		{
			LCD_writeData( 0x00 );
			if ((row < 6) && (j < 84))
				lcd_buffer[row][j] = 0x00;
		}
   
    LCD_gotoXY (0,0);	//bring the XY position back to (0,0)
      
}

void LCD_update( void )
{
	int i,j;
	
	LCD_gotoXY (0,0);  	//start with (0,0) position

	for(i=0; i<7; i++)
	{
		LCD_gotoXY (0,i);
		for(j=0; j<84; j++)
		{
			LCD_writeData(lcd_buffer[i][j]);
		}
	}
   
	LCD_gotoXY (0,0);	//bring the XY position back to (0,0)
      
}

void LCD_setPixelNoUpdate( unsigned char x, unsigned char y )
{
	unsigned char value;
	unsigned char row;
	
	row = y / 8;

	value = lcd_buffer[row][x];
	value |= (1 << (y % 8));
	lcd_buffer[row][x] = value;
}

void LCD_setPixel( unsigned char x, unsigned char y )
{
	unsigned char value;
	unsigned char row;
	
	row = y / 8;

	value = lcd_buffer[row][x];
	value |= (1 << (y % 8));
	lcd_buffer[row][x] = value;

	//LCD_update();
	LCD_gotoXY (x,row);
	LCD_writeData(value);
}

void LCD_clearPixel( unsigned char x, unsigned char y )
{
	unsigned char value;
	unsigned char row;
	
	row = y / 8;

	value = lcd_buffer[row][x];
	value &= ~(1 << (y % 8));
	lcd_buffer[row][x] = value;

	//LCD_update();
	LCD_gotoXY (x,row);
	LCD_writeData(value);
}

void LCD_clearPixelNoUpdate( unsigned char x, unsigned char y )
{
	unsigned char value;
	unsigned char row;
	
	row = y / 8;

	value = lcd_buffer[row][x];
	value &= ~(1 << (y % 8));
	lcd_buffer[row][x] = value;
}

/*--------------------------------------------------------------------------------------------------
  Name         :  LCD_gotoXY
  Description  :  Sets cursor location to xy location corresponding to basic font size.
  Argument(s)  :  x - range: 0 to 84
  			   	  y -> range: 0 to 6
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
void LCD_gotoXY ( unsigned char x, unsigned char y )
{
	//LCD_writeCommand (0x20);
	
	LCD_writeCommand (0x80 | x);   //column
	LCD_writeCommand (0x40 | y);   //row

	cursor_row = y;
	cursor_col = x; //+ 84*y;
}

/*--------------------------------------------------------------------------------------------------
  Name         :  LCD_writeChar
  Description  :  Displays a character at current cursor location and increment cursor location.
  Argument(s)  :  ch   -> Character to write.
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
// void LCD_writeChar (unsigned char ch)
// {
// 	unsigned char j;
//   
// 	lcd_buffer[cursor_row][cursor_col] = 0x00;
// 	for(j=0; j<5; j++)
// 	{
// 		lcd_buffer[cursor_row][cursor_col + j] = 0x00;
// 		lcd_buffer[cursor_row][cursor_col + j] = pgm_read_byte(&(smallFont [(ch-32)*5 + j] ));
// 	}
// 
// 	lcd_buffer[cursor_row][cursor_col + 5] = 0x00;
// 	lcd_buffer[cursor_row][cursor_col + 6] = 0x00;
// 
// 	for(j=0; j<7; j++)
// 		LCD_writeData(lcd_buffer[cursor_row][cursor_col++]);
// } 
void LCD_writeChar (unsigned char ch)
{
	unsigned char j;
	
	lcd_buffer[cursor_row][cursor_col] = 0x00;
	for(j=0; j<5; j++)
		lcd_buffer[cursor_row][cursor_col + j] |=  pgm_read_byte(&(smallFont [(ch-32)*5 + j] ));

	lcd_buffer[cursor_row][cursor_col + 6] = 0x00;

	for(j=0; j<7; j++)
		LCD_writeData(lcd_buffer[cursor_row][cursor_col++]);
}

#if 1

/*--------------------------------------------------------------------------------------------------
  Name         :  LCD_writeChar_megaFont
  Description  :  Displays a character in large fonts, used here for displatying temperature 
		  (for displaying '.', '+', '-', and '0' to '9', stored 
		  in 3310_routines.h as three dimensional array, number[][][])
  Argument(s)  :  ch   -> Character to write.
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
// void LCD_writeChar_megaFont (unsigned char ch)
// {
// 	unsigned char i, j;
//    
// 	if(ch == '.')
// 		ch = 10;
// 	else if (ch == '+')
// 		ch = 11;
// 	else if (ch == '-')
// 		ch = 12;
// 	else
// 		ch = ch & 0x0f;
// 	
// 	for(i=0;i<3;i++)
// 	{	
// 		LCD_gotoXY (4 + char_start, i+1);
//  
// 		for(j=0; j<16; j++) {
// 			lcd_buffer[cursor_row][cursor_col + j] |=  pgm_read_byte(&(mega_number[ch][i][j]));
// 			LCD_writeData(lcd_buffer[cursor_row][cursor_col + j]);
// 		}
// 	} 
//    
// 	if(ch == '.') 
// 		char_start += 5;
// 	else
// 		char_start += 12;
// }

/*--------------------------------------------------------------------------------------------------
  Name         :  LCD_writeString_megaFont
  Description  :  Displays a string at current location, in large fonts
  Argument(s)  :  string -> Pointer to ASCII string (stored in RAM)
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
void LCD_writeString_megaFont ( char *string )
{
	char_start = 25;
	
	while ( *string )
		LCD_writeChar_megaFont( *string++ );
	
	LCD_gotoXY(char_start+6, 2);
	//LCD_writeChar('z'+1); 			  //symbol of Degree
	//LCD_writeChar('F');
}
#endif

/*--------------------------------------------------------------------------------------------------
  Name         :  LCD_writeString_F
  Description  :  Displays a string stored in FLASH, in small fonts (refer to 3310_routines.h)
  Argument(s)  :  string -> Pointer to ASCII string (stored in FLASH)
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
void LCD_writeString_F ( const char *string )
{
	while ( *string )
		LCD_writeChar( *string++ );
}

void LCD_splashScreen()
{
	int i;
	LCD_clear();
	
	for(i=0;i<504;i++)
		LCD_writeData(pgm_read_byte(&(twoTurtleGangLogo [i] )));
		//LCD_writeData(twoTurtleGangLogo[i]);
		//LCD_writeData(image_data_logo[i]);
}

// void LCD_lockScreen()
// {
// 	int i;
// 	LCD_clear();
// 	
// 	for(i=0;i<504;i++)
// 		LCD_writeData(image_data_Image[i]);
// }

/*--------------------------------------------------------------------------------------------------
  Name         :  delay_ms
  Description  :  1 millisec delay (appx.)
  Argument(s)  :  None.
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
void delay_ms(int miliSec)  //for 1Mhz clock
{
	int i,j;
  
	for(i=0;i<miliSec;i++)
		for(j=0;j<100;j++)
		{
			asm("nop");
			asm("nop");
		}
}

/*--------------------------------------------------------------------------------------------------
  Name         :  LCD_drawBorder
  Description  :  Draws rectangle border on the display
  Argument(s)  :  None
  Return value :  None
--------------------------------------------------------------------------------------------------*/
void LCD_drawBorder (void )
{
	unsigned char i, j;  
	    
	for(i=0; i<7; i++)
	{
		LCD_gotoXY (0,i);
			
		for(j=0; j<84; j++)
		{
			if(j == 0 || j == 83) {
				lcd_buffer[cursor_row][cursor_col + j] |= 0xff;
			} else if(i == 0) {
				lcd_buffer[cursor_row][cursor_col + j] |= 0x08;
				//LCD_writeData (0x08);		// row 0 is having only 5 bits (not 8)
			} else if(i == 6) {
				lcd_buffer[cursor_row][cursor_col + j] |= 0x04;
				//LCD_writeData (0x04);		// row 6 is having only 3 bits (not 8)
			} else {
				lcd_buffer[cursor_row][cursor_col + j] |= 0x00;
				//LCD_writeData (0x00);
			}
		}
	}

	LCD_update();
}	

/*--------------------------------------------------------------------------------------------------
                                         End of file.
--------------------------------------------------------------------------------------------------*/