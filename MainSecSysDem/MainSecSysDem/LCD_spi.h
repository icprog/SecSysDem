/*
 * LCD_spi.h
 *
 * Created: 10/5/2015 3:32:11 PM
 *  Author: Luke
 */ 


//********************************************************
//****  Header file for 3310_routines.c  *****
//********************************************************
//Controller:	ATmega32 (Clock: 1 Mhz-internal)
//Compiler:		ImageCraft ICCAVR
//Author:		CC Dharmani, Chennai (India)
//Date:			Sep 2008
//
//Modifed by Michael Spiceland (http://tinkerish.com) to
//pixel level functions with lcd_buffer[][].
//Jan 2009
//
//********************************************************
#ifndef _LCD_spi_H_

#define _LCD_spi_H_

#include <avr/pgmspace.h>

// Data/Command Pin = PD2
#define SET_DC_PIN                 PORTD |= 0b00000100
#define CLEAR_DC_PIN               PORTD &= ~0b00000100
// Chip Enable Pin = PD4
#define SET_SCE_PIN                PORTD |= 0b00010000
#define CLEAR_SCE_PIN              PORTD &= ~0b00010000
// Reset Pin = PD3
#define SET_RST_PIN                PORTD |= 0b00001000
#define CLEAR_RST_PIN              PORTD &= ~0b00001000

 
/*--------------------------------------------------------------------------------------------------
                                 Public function prototypes
--------------------------------------------------------------------------------------------------*/
void LCD_init ( void );
void LCD_clear ( void );
void LCD_clear_row (uint8_t row);
void LCD_update ( void );
void LCD_gotoXY ( unsigned char x, unsigned char y );
void LCD_writeChar ( unsigned char character );
void LCD_writeChar_megaFont ( unsigned char character );
void LCD_writeData ( unsigned char data );
void LCD_writeCommand ( unsigned char command );
void LCD_writeString_megaFont ( char *string );
void LCD_writeString_F ( const char *string);
void LCD_setPixel ( unsigned char x, unsigned char y);
void LCD_clearPixel ( unsigned char x, unsigned char y);
void LCD_setPixelNoUpdate ( unsigned char x, unsigned char y);
void LCD_clearPixelNoUpdate ( unsigned char x, unsigned char y);
void LCD_drawLine ( unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2 );
void LCD_drawBorder (void );
void LCD_drawSplash (void );
void LCD_drawSplashNoUpdate (void );
void delay_ms ( int millisec );
void spi_init(void);
void LCD_splashScreen();
void LCD_lockScreen();



#endif  //  _LCD_spi_H_