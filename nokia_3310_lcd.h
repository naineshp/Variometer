/*
	nokia_3310_lcd.h
*/

#ifndef _NOKIA_3310_LCD_H
#define _NOKIA_3310_LCD_H

#include <inttypes.h>
#include "n3310_lcd.h"


class Nokia_3310_lcd
{
  public:
  
  	Nokia_3310_lcd();
  	
  	// lcd related routines
	void LCD_3310_init(void);
	void LCD_3310_write_byte(unsigned char dat, unsigned char dat_type);
	void LCD_3310_draw_bmp_pixel(unsigned char X,unsigned char Y,unsigned char *map,
                  unsigned char Pix_x,unsigned char Pix_y);
	void LCD_3310_write_string(unsigned char X,unsigned char Y,char *s, char mode);                  
	void LCD_3310_write_string_big ( unsigned char X,unsigned char Y, char *string, char mode );
	void LCD_3310_write_char_big (unsigned char X,unsigned char Y, unsigned char ch, char mode);
	void LCD_3310_write_char(unsigned char c, char mode);
	void LCD_3310_set_XY(unsigned char X, unsigned char Y);
	void LCD_3310_clear(void);
	
};	
		
#endif //_NOKIA_3310_LCD_H

