#pragma once

#include <stdint.h>
#include <stdbool.h>
//#include <math.h>
#include "figures.h"
#include "font.h"


#define BLACK     0x0000
#define RED       0xf800
#define GREEN     0x07e0
#define BLUE      0x001f
#define YELLOW    0xffe0
#define MAGENTA   0xf81f
#define CYAN      0x07ff
#define WHITE     0xffff

#define BACKGROUND 0x001f

#define LCD_WIDTH 240
#define LCD_HEIGHT 320

#define RECTS_AMOUNT 10
#define CIRCLE_RADIUS 10
#define MAX_CHARS_ON_SCREEN 50


extern volatile Rectangle rects[RECTS_AMOUNT];
extern volatile Circle player;
extern volatile Buf_Char text[MAX_CHARS_ON_SCREEN];


extern volatile bool lcd_ready;
extern int16_t max_speed;


//==========================================
// LCD INTERFACE
//==========================================
void lcd_init(void); // to configure lcd
void lcd_put_pixel(int x, int y, uint16_t color); // to put indyvidual pixel on screen
void lcd_update(void); // to update state of lcd screen
void lcd_update_circle(int dx,int dy,int dradius);
void lcd_set_circle(int x,int y,int radius,uint16_t color);
//void lcd_delta_circle(int dx,int dy,int dradius);
void lcd_set_rectangle(uint16_t idx,int x,int y,int width,int height,uint16_t color);

void lcd_change_ball_color(uint16_t speed_x, uint16_t speed_y);
void lcd_set_char(uint16_t idx, int x, int y, char znak, uint16_t color);
//==========================================



bool lcd_is_busy(void);
void go_for_next_chunk(void);

