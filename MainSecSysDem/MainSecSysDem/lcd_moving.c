/*****************************

lcd_moving.c

Author: Aaron crump

These functions will control lcd output and backlight output

*****************************/


#include <inttypes.h>
#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include "secSysDefines.h"
#include "LCD_spi.h"
#include "lcd_moving.h"
#include <util/delay.h>


/************************

	sets up timer for PWM with a frequency of 200Hz
	
*************************/

void LCD_light_init(void)
{
	TCNT1 = 0;
	TCCR1A |= (1<<COM1B1) | (1<<WGM11) | (1<<WGM10);
	TCCR1B |= (1<<CS12) | (1<<WGM13); //sets prescaler to 256
	OCR1A = 156/2;//sets period to 2*OCR1A 0.005 seconds, 200Hz
	OCR1B = 78; //sets duty cycle to 50% initially
}

/************************************

this function changes the PWM for the LCD backlight 

	brightness : a value between 1 and 10

************************************/

void Lcd_Light_bright(uint8_t brigtness)
{
				
}

/**********************************

	scrolling text
	This function scrolls an array or something
	
***********************************/



void Scrolling_Text(char input[])
{
	int i, j, k, length;
	char message[100], swap;
	sprintf(message, "%s              ",&input[0]);
	length = strlen(message)-1;
	for(i=0;i<19;i++)
	{
		swap = message[0];
		for(k=0;k < length;k++)
		{
			message[k] = message[k+1];
		}
		message[length] = swap;
		LCD_gotoXY(0,2);
		for(j=0;j<12;j++)
		{
			LCD_writeChar(message[j]);
		}
		_delay_ms(500);
		LCD_clear();
	}
}


/**********************************

	scrolling text single
	this fuction scrolls the array one character to the left
	scrolls one position to the left starting with the last shifted character
	
	arguments
	input: the array to be displayed
	position : if position = 1 the array will be shifted once
	
	!!!NOTE!!! to get it to scroll smoothly, the for loop for printing should be 23 iterations long i.e.
	
	for(i=0;i<23;i++)
	{
		Scrolling_Text_single(&message[0], i);
	}
	
	
***********************************/

void Scrolling_Text_single(char input[], uint8_t position)
{
	int i, j, k, l, length;
	char message[35], swap;
	sprintf(message, "%s ",&input[0]);
	if(strlen(input) < 23);
	{
		for(l=0;l<(23-strlen(input)-1);l++)
		{
			sprintf(message, "%s ",&message[0]);
		}
	}
	length = strlen(message)-1;
	for(i=0;i<position;i++)
	{
		swap = message[0];
		for(k=0;k < length;k++)
		{
			message[k] = message[k+1];
		}
		message[length] = swap;
	}
		LCD_gotoXY(0,0);
		for(j=0;j<12;j++)
		{
			LCD_writeChar(message[j]);
		}
		_delay_ms(500);
		LCD_clear_row(0);
}

/**********************************

	displays the temperature
	arguments
	int_temp: the temperature reading from the rtc in whole numbers
	dec_temp: the temperature reading form the rtc that is less than 1
	
***********************************/

void display_temp(uint8_t int_temp, uint8_t dec_temp)
{
	int j;
	char message[100];
	if(int_temp <= 9) sprintf(message, "Temp: %u.%u  ", int_temp, dec_temp);
	else if(int_temp <= 99) sprintf(message, "Temp: %u.%u ", int_temp, dec_temp);
	else if(int_temp >99) sprintf(message, "Temp: %u.%u", int_temp, dec_temp);
	LCD_clear_row(1);
	LCD_gotoXY(0,1);
	for(j=0;j<12;j++)
	{
		LCD_writeChar(message[j]);
	}
}

/**********************************

	displays whether or not the system is armed or unarmed
	argument: status: armed=1, unarmed=0, alarm=2
	argument: location: none=0, door=1, window=2, motion=3 
	
***********************************/

void display_status(uint8_t status, uint8_t location)
{
	int j, i;
	char message[100], state1[] = "ARMED  ", state2[] = "UNARMED", state3[] = "ALARM!!!";
	char place[15];
	if(location == 1) sprintf(place, "    %s      ", "Door");
	else if(location == 2) sprintf(place, "    %s    ", "Window");
	else if(location == 3) sprintf(place, "    %s    ", "Motion");
	else if(location == 4) sprintf(place, "    %s      ", "FIRE");
	if(status == 3)
	{
		sprintf(message, "%s       ", state1);
		LCD_clear_row(2);
		LCD_gotoXY(0,2);
		for(j=0;j<12;j++)
		{
			LCD_writeChar(message[j]);
		}
	}
	else if(status == 1)
	{
		sprintf(message, "%s       ", state2);
		LCD_clear_row(2);
		LCD_gotoXY(0,2);
		for(j=0;j<12;j++)
		{
			LCD_writeChar(message[j]);
		}
	}
	else if(status == 8 || status == 9 || status == 13 || status == 14)
	{
		sprintf(message, "%s       ", state3);
		LCD_clear_row(0);
		LCD_gotoXY(0,0);
		for(j=0;j<12;j++)
		{
			LCD_writeChar(message[j]);
		}
		LCD_clear_row(1);
		LCD_gotoXY(0,1);
		LCD_writeString_F("Location:");
		LCD_clear_row(2);
		LCD_gotoXY(0,2);
		for(i=0;i<12;i++)
		{
			LCD_writeChar(place[i]);
		}
	}
		
}
 
/*****************************************

	Menuing function display keypad needs to be read to switch
	
*****************************************/
 
void display_main_menu(void)
{
	LCD_clear();
	LCD_gotoXY(0,1);
	LCD_writeString_F("MENU");
	LCD_gotoXY(0,2);
	LCD_writeString_F("1. Dis/Arm");
	LCD_gotoXY(0,3);
	LCD_writeString_F("2. Last 5");
	LCD_gotoXY(0,4);
	LCD_writeString_F("3. Set Time");
}

void display_get_armcode(char code[])
{
	uint8_t i;
	char message[10] = "Enter Code";
	LCD_clear();
	LCD_gotoXY(0, 2);
	LCD_writeString_F(&message[0]);
	for(i=0; i<5;i++)
	{
		if(code[i] == 11) code[i] = 0;
	}
	LCD_clear_row(3);
	LCD_gotoXY(0, 3);
	LCD_writeString_F(&code[0]);
	
}

/****************************************

	displays last five alarm events, this function reads the onboard eeprom and gets the last five dates
	
****************************************/

void display_last_five_alarms(void)
{
	
	//read onboard eeprom
	LCD_clear();
	LCD_gotoXY(0,1);
	LCD_writeString_F("");// eeprom 
	LCD_gotoXY(0,2);
	LCD_writeString_F("");
	LCD_gotoXY(0,3);
	LCD_writeString_F("");
	LCD_gotoXY(0,4);
	LCD_writeString_F("");
}

/****************************************

	displays last five arm/disarm events, this function reads the onboard eeprom and gets the last five dates
	
****************************************/

void display_last_five_arm(void)
{
	
	//read onboard eeprom
	LCD_clear();
	LCD_gotoXY(0,1);
	LCD_writeString_F("");// eeprom 
	LCD_gotoXY(0,2);
	LCD_writeString_F("");
	LCD_gotoXY(0,3);
	LCD_writeString_F("");
	LCD_gotoXY(0,4);
	LCD_writeString_F("");
}

/*****************************************

	 function
	
*****************************************/





/*****************************************

	This function just shifts an array, it's unused but I didn't want to delete it.
	
*****************************************/
void array_shift(char message[])
{
	int j, k, length;
	char swap;
	length = strlen(message)-1;
	swap = message[0];
	for(k=0;k < length;k++)
	{
		message[k] = message[k+1];
	}
	message[length] = swap;	
}



