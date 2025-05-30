#include "lcd.h"
#include "spi.h"

#define CMD(x)			((x) | 0x100)


#define SLEEP_OUT 0x11
#define DISPLAY_ON 0x29
#define FRAME_RATE_CONTROL 0xb1 //normal mode
#define FRAME_RATE_CONTROL2 0xb2 //idle, 8 colors
#define FRAME_RATE_CONTROL3 0xb3 //full colors
#define DISP_INV_CON 0xb4
//AVD -> LCD, VGH -> turns on tft transistors, VGL -> turn off transistors, VCI -> for controller
#define POWER_CON1 0xC0 //set contrast
#define POWER_CON2 0xC1 // boosting voltage for controller
#define VCOM_CON1 0xC5 //stabilize common electrode
#define COLMOD 0x3A // rgb picture data
#define MAC 0x36 // memory access control, read/write direction, jak się odświeża ekran
#define COL_ADR_SET 0x2A // set columns which will be use
#define PAGE_ADR_SET 0x2B//sets a row which will be use
#define MEM_WRITE 0x2C // put data to operated area
#define DISP_FUN_CON 0xB6
#define POS_GAMMA_COR 0xE0
#define NEG_GAMMA_COR 0xE1


volatile static uint16_t screen_buffer[LCD_WIDTH * LCD_HEIGHT];
//volatile static uint16_t circle_buffer[4 * CIRCLE_RADIUS * CIRCLE_RADIUS];
static uint16_t current_chunk = 0;
static uint16_t how_many_chunks = 4;
static uint16_t chunk_size;
static uint16_t y_per_chunk;

volatile bool lcd_ready=true;

volatile Rectangle rects[RECTS_AMOUNT];
volatile Circle player;
volatile Buf_Char text[MAX_CHARS_ON_SCREEN];




static const uint16_t init_table[] = {
		CMD(FRAME_RATE_CONTROL),0x00, 0x1B, // by 70Hz
		CMD(FRAME_RATE_CONTROL2),0x00, 0x1B,
		CMD(FRAME_RATE_CONTROL3),0x00, 0x1B,
		CMD(DISP_INV_CON), 0x07, //make inv control in all modes,
		CMD(POWER_CON1), 0x21, //GVDD=4.5V
		CMD(POWER_CON2), 0x11,
		CMD(VCOM_CON1), 0x3E, 0x28, // VCOMH=3.450, VCOML = -1.5V
		CMD(COLMOD), 0x05, // chosen 16-bit, R5, G6, B5
		CMD(MAC), 0x48, //in documentation this means BGR but it turned out to be RGB, also it set (0,0) to up left corner
		CMD(POS_GAMMA_COR), 0x1F, 0x1A, 0x18, 0x0A, 0x0F, 0x06, 0x45, 0x87, 0x32, 0x0A, 0x07, 0x02, 0x07, 0x05, 0x00,
		CMD(NEG_GAMMA_COR), 0x00, 0x25, 0x27, 0x05, 0x10, 0x09, 0x3A, 0x56, 0x4C, 0x05, 0x0D, 0x0C, 0x2E, 0x2F, 0x0F,
};
static void lcd_cmd(uint8_t cmd){

	if (!spi5_acquire()) return;  // SPI jest zajęte, zwróć

	HAL_GPIO_WritePin(WRX_DCX_GPIO_Port, WRX_DCX_Pin, GPIO_PIN_RESET); //teraz komendy
	HAL_GPIO_WritePin(CSX_GPIO_Port, CSX_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi5, &cmd, 1, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(CSX_GPIO_Port, CSX_Pin, GPIO_PIN_SET);

	spi5_release();

}
static void lcd_data(uint8_t data){
	HAL_GPIO_WritePin(WRX_DCX_GPIO_Port, WRX_DCX_Pin, GPIO_PIN_SET); //teraz dane
	HAL_GPIO_WritePin(CSX_GPIO_Port, CSX_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi5, &data, 1, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(CSX_GPIO_Port, CSX_Pin, GPIO_PIN_SET);
}
static void lcd_data16(uint16_t value){
	lcd_data(value >> 8);
	lcd_data(value);
}
static void lcd_send(uint16_t value){
	if (value & 0x100) {
		lcd_cmd(value);
	} else {
		lcd_data(value);
	}
}
void lcd_init(void){
  chunk_size = (LCD_HEIGHT * LCD_WIDTH) / how_many_chunks;
  y_per_chunk = LCD_HEIGHT / how_many_chunks;

  int i;
  HAL_GPIO_WritePin(RDX_GPIO_Port, RDX_Pin, GPIO_PIN_RESET);
  HAL_Delay(100); // w nocie 10 mikro sec
  HAL_GPIO_WritePin(RDX_GPIO_Port, RDX_Pin, GPIO_PIN_SET);
  HAL_Delay(100);
  for (i = 0; i < sizeof(init_table) / sizeof(uint16_t); i++) {
    lcd_send(init_table[i]);
  }
  HAL_Delay(200);
  lcd_cmd(SLEEP_OUT);
  HAL_Delay(120);
  lcd_cmd(DISPLAY_ON);
}

static void lcd_set_window(int x, int y, int width, int height){
	lcd_cmd(COL_ADR_SET);
	lcd_data16(x);
	lcd_data16(x+width-1);
	lcd_cmd(PAGE_ADR_SET);
	lcd_data16(y);
	lcd_data16(y+height-1);
}
void lcd_set_rectangle(uint16_t idx,int x,int y,int width,int height,uint16_t color){
	rects[idx].x = x;
	rects[idx].y = y;
	rects[idx].width = width;
	rects[idx].height = height;
	rects[idx].color = color;
}
void lcd_set_char(uint16_t idx, int x, int y, char znak, uint16_t color){
	text[idx].x = x;
	text[idx].y = y;
	text[idx].c = znak;
	text[idx].color = color;
}

void lcd_set_circle(int x,int y,int radius,uint16_t color){
	player.x = x;
	player.y = y;
	player.r = radius;
	player.color = color;
}
static void send_circle(uint16_t circle_color){
	lcd_set_window(player.x - player.r, player.y-player.r, 2*player.r,2*player.r);
	lcd_cmd(MEM_WRITE);
	if(!spi5_acquire())return;
	for(int y=player.y-player.r;y<player.y+player.r;y++){
		for(int x=player.x-player.r;x<player.x+player.r;x++){
			if( ( (x-player.x)*(x-player.x)+(y-player.y)*(y-player.y)) <= (player.r*player.r)){
				lcd_data16(circle_color);
			} else{
				lcd_data16(BACKGROUND); // zle
			}
		}
	}
	spi5_release();
}
//static void send_circle_buffer(void){
//	current_chunk = how_many_chunks;
//	lcd_set_window(player.x - player.r, player.y-player.r, 2*player.r,2*player.r);
//	lcd_cmd(MEM_WRITE);
//	HAL_GPIO_WritePin(WRX_DCX_GPIO_Port, WRX_DCX_Pin, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(CSX_GPIO_Port, CSX_Pin, GPIO_PIN_RESET);
//
//	if (!spi5_acquire()) return;
//
//	HAL_SPI_Transmit_DMA(&hspi5, (uint8_t*)circle_buffer, 2*4*player.r*player.r);
//
//	spi5_release();
//}
static void clear_former_horizontal(Circle former){
	int min_y,max_y;

	if(former.y-former.r < player.y - player.r){
		min_y = former.y-former.r;
		max_y = player.y-player.r;
	} else{
		min_y = player.y+player.r;
		max_y = former.y+former.r;
	}

	int height = max_y-min_y;
	int d_wh = 2*former.r;
	if(height>d_wh)height=d_wh;

	lcd_set_window(former.x-former.r, min_y, d_wh, height);
	lcd_cmd(MEM_WRITE);
	if(!spi5_acquire()){return;}
	for(int i=0;i<d_wh*height;i++){
		lcd_data16(BACKGROUND);
	}
	spi5_release();
}
static void clear_former_vertical(Circle former){
	int min_x,max_x;

	if(former.x-former.r < player.x - player.r){
		min_x = former.x-former.r;
		max_x = player.x-player.r;
	} else{
		min_x = player.x+player.r;
		max_x = former.x+former.r;
	}

	int width = max_x-min_x;
	int d_wh = 2*former.r;
	if(width>d_wh)width=2*d_wh;

	lcd_set_window(min_x, former.y-former.r, width, d_wh);
	lcd_cmd(MEM_WRITE);
	if(!spi5_acquire()){return;}
	for(int i=0;i<d_wh*width;i++){
		lcd_data16(BACKGROUND);
	}
	spi5_release();
}
static void clear_former_circle(Circle former){
	clear_former_horizontal(former);
	clear_former_vertical(former);
}
void lcd_delta_circle(int dx,int dy,int dradius){
	player.x += dx;
	player.y += dy;
	player.r += dradius;



	if(player.r<0 || player.r > LCD_WIDTH/6 || player.r>LCD_HEIGHT/6)player.r = 5;

	if (player.x-player.r < 0) player.x = player.r;
	else if (player.x+player.r > LCD_WIDTH) player.x = LCD_WIDTH-player.r;
	if (player.y-player.r < 0) player.y = player.r;
	else if (player.y+player.r > LCD_HEIGHT) player.y = LCD_HEIGHT-player.r;
}
void lcd_update_circle(int dx,int dy,int dradius){
	Circle former = player;
	lcd_delta_circle(dx, dy, dradius);
	if(dx!=0 ||  dy!=0)clear_former_circle(former);
	send_circle(player.color);
	//	send_circle_buffer();
}
void lcd_put_pixel(int x, int y, uint16_t color){
	screen_buffer[ (LCD_WIDTH*y) + x] = __REV16(color); //to make send most significant bit first
}
static void lcd_put_rect_to_buffer(Rectangle rect){
	for(int y=rect.y;y<rect.y+rect.height;y++){
		for(int x=rect.x;x<rect.x+rect.width;x++){
			if(x>=0 && x<LCD_WIDTH && y>=0 && y<LCD_HEIGHT){
				lcd_put_pixel(x, y, rect.color);
			}
		}
	}
}
static void lcd_put_circ_to_buffer(Circle circle){
	for(int y=circle.y-circle.r;y<circle.y+circle.r;y++){
		for(int x=circle.x-circle.r;x<circle.x+circle.r;x++){
			if(x>=0 && x<LCD_WIDTH && y>=0 && y<LCD_HEIGHT){
				if( ( (x-circle.x)*(x-circle.x)+(y-circle.y)*(y-circle.y)) <= (circle.r*circle.r)){
					lcd_put_pixel(x, y, circle.color);
				}
			}
		}
	}
}
const uint16_t* get_char_bitmap(char c) {
    if (c >= '0' && c <= '9') {
        return font_10x14[c - '0'];  // indeksy 0-9
    }
    else if (c >= 'A' && c <= 'Z') {
        return font_10x14[10 + (c - 'A')];  // indeksy 10-35 (po cyfrach)
    }
    return NULL;  // znak nieobsługiwany
}
void lcd_put_char_to_buffer(Buf_Char ch) {
    const uint16_t* bitmap = get_char_bitmap(ch.c);
    if (!bitmap) return;

    for (int row = 0; row < 14; row++) {
        for (int col = 0; col < 10; col++) {
            if (bitmap[row] & (1 << (9 - col))) {
                int px = ch.x + col;
                int py = ch.y + row;
                if (px >= 0 && px < LCD_WIDTH && py >= 0 && py < LCD_HEIGHT) {
                    screen_buffer[py * LCD_WIDTH + px] = __REV16(ch.color);
                }
            }
        }
    }
}
static void put_figures_to_buffer(void){
	for(int i=0; i<LCD_WIDTH*LCD_HEIGHT;i++) {
		screen_buffer[i] = __REV16(BLUE);
	}

	for(int i=0;i<RECTS_AMOUNT;i++){
		lcd_put_rect_to_buffer(rects[i]);
	}
	for(int i=0;i<MAX_CHARS_ON_SCREEN;i++){
		lcd_put_char_to_buffer(text[i]);
	}
	lcd_put_circ_to_buffer(player);
}

void lcd_update(void){
	put_figures_to_buffer();

	current_chunk = 0;
	lcd_set_window(0, current_chunk*y_per_chunk, LCD_WIDTH, LCD_HEIGHT/how_many_chunks);
	lcd_cmd(MEM_WRITE);
	HAL_GPIO_WritePin(WRX_DCX_GPIO_Port, WRX_DCX_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(CSX_GPIO_Port, CSX_Pin, GPIO_PIN_RESET);

	if (!spi5_acquire()) return;

	HAL_SPI_Transmit_DMA(&hspi5, (uint8_t*)(screen_buffer + (current_chunk * chunk_size)), 2*chunk_size);

	spi5_release();
}
void lcd_transfer_done(void){
	HAL_GPIO_WritePin(CSX_GPIO_Port, CSX_Pin, GPIO_PIN_SET);
}
bool lcd_is_busy(void){
	if (HAL_SPI_GetState(&hspi5) == HAL_SPI_STATE_BUSY) return true;
	else return false;
}
static void send_next_chunk(void){
	lcd_set_window(0, current_chunk*y_per_chunk, LCD_WIDTH, LCD_HEIGHT/how_many_chunks);
	lcd_cmd(MEM_WRITE);
	HAL_GPIO_WritePin(WRX_DCX_GPIO_Port, WRX_DCX_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(CSX_GPIO_Port, CSX_Pin, GPIO_PIN_RESET);
	lcd_ready = false;
	if (!spi5_acquire()) return;
	HAL_SPI_Transmit_DMA(&hspi5, (uint8_t*)(screen_buffer + (current_chunk * chunk_size)), 2*chunk_size);

	spi5_release();
}
void go_for_next_chunk(void){
	current_chunk++;
	if (current_chunk < how_many_chunks) {
		send_next_chunk();
	}
	else {
		lcd_ready=true;
		lcd_transfer_done();
	}
}
void lcd_change_ball_color(uint16_t speed_x, uint16_t speed_y){
	uint32_t speed_sq = speed_x * speed_x + speed_y * speed_y;
	uint32_t sq_max_speed = max_speed*max_speed;
	if (speed_sq > sq_max_speed) {
		speed_sq = sq_max_speed;
	}
	float ratio = (float)speed_sq/sq_max_speed;

	// Kolor startowy: zielony (R=0, G=63, B=0)
	// Kolor docelowy: biały (R=31, G=63, B=31)

	uint8_t red   = (uint8_t)(31 * ratio);
	uint8_t green = 63;
	uint8_t blue  = (uint8_t)(31 * ratio);

	uint16_t color = (red << 11) | (green << 5) | blue;
	player.color = color;
}

