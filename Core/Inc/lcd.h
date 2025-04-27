#pragma once

#include <stdint.h>
#include <stdbool.h>


#define BLACK     0x0000
#define RED       0xf800
#define GREEN     0x07e0
#define BLUE      0x001f
#define YELLOW    0xffe0
#define MAGENTA   0xf81f
#define CYAN      0x07ff
#define WHITE     0xffff

#define LCD_WIDTH 240
#define LCD_HEIGHT 320

struct Rectangle{
	int x;
	int y;
	int width;
	int height;
	uint16_t color;
};
struct Circle{
	int x;
	int y;
	int radius;
	uint16_t color;
};

void lcd_init(void);
void lcd_put_pixel(int x, int y, uint16_t color);
void lcd_update(void);
bool lcd_is_busy(void);
void go_for_next_chunk(void);
