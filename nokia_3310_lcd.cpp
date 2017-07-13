// a wrapper class for Nokia 3310 LCD routines

extern "C" {
	#include "n3310_lcd.h"
	#include <avr/pgmspace.h>
 
}
#include "nokia_3310_lcd.h"

// constructor
Nokia_3310_lcd::Nokia_3310_lcd(){
}
  	
  	// lcd related routines
void Nokia_3310_lcd::LCD_3310_init(void){
	LCD_init();
}


void Nokia_3310_lcd::LCD_3310_write_byte(unsigned char dat, unsigned char dat_type){
	LCD_write_byte(dat, dat_type);
}

void Nokia_3310_lcd::LCD_3310_draw_bmp_pixel(unsigned char X,unsigned char Y,unsigned char *map,
                  unsigned char Pix_x,unsigned char Pix_y){
	LCD_draw_bmp_pixel(X,Y,map,Pix_x, Pix_y);
}

void Nokia_3310_lcd::LCD_3310_write_string(unsigned char X,unsigned char Y,char *s, char mode){
	LCD_write_string(X,Y,s,mode);
}

void Nokia_3310_lcd::LCD_3310_write_string_big ( unsigned char X,unsigned char Y, char *string, char mode ){
	LCD_write_string_big(X,Y,string,mode);
}


void Nokia_3310_lcd::LCD_3310_write_char_big (unsigned char X,unsigned char Y, unsigned char ch, char mode){
	LCD_write_char_big(X,Y,ch,mode);
}

void Nokia_3310_lcd::LCD_3310_write_char(unsigned char c, char mode){
	LCD_write_char(c,mode);
}

void Nokia_3310_lcd::LCD_3310_set_XY(unsigned char X, unsigned char Y){
	LCD_set_XY(X,Y);
}
	
void Nokia_3310_lcd::LCD_3310_clear(void){
	LCD_clear();
}

