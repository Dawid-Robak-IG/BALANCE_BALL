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

static const uint16_t init_table[] = {
		CMD(FRAME_RATE_CONTROL),0x00, 0x1B, // by 70Hz
		CMD(FRAME_RATE_CONTROL2),0x00, 0x1B,
		CMD(FRAME_RATE_CONTROL3),0x00, 0x1B,
		CMD(DISP_INV_CON), 0x07, //moke inv coltrol in all modes,
		CMD(POWER_CON1), 0x21, //GVDD=4.5V
		CMD(POWER_CON2), 0x11,
		CMD(VCOM_CON1), 0x3E, 0x28, // VCOMH=3.450, VCOML = -1.5V
		CMD(COLMOD), 0x05, // chosen 16-bit, R5, G6, B5
		CMD(MAC), 0x00,
};
static void lcd_cmd(uint8_t cmd){
	HAL_GPIO_WritePin(WRX_DCX_GPIO_Port, WRX_DCX_Pin, GPIO_PIN_RESET); //teraz komendy
	HAL_GPIO_WritePin(CSX_GPIO_Port, CSX_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi5, &cmd, 1, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(CSX_GPIO_Port, CSX_Pin, GPIO_PIN_SET);
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
  int i;
  HAL_GPIO_WritePin(RDX_GPIO_Port, RDX_Pin, GPIO_PIN_RESET);
  HAL_Delay(100); // w nocie 10 mikro sec
  HAL_GPIO_WritePin(RDX_GPIO_Port, RDX_GPIO_Port, GPIO_PIN_SET);
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
	lcd_data16(x + width - 1);
	lcd_cmd(PAGE_ADR_SET);
	lcd_data16(y);
	lcd_data16(y + height- 1);
}
void lcd_put_rectangle(int x,int y,int width,int height,uint16_t color){
	lcd_set_window(x, y, width, height);
	for(int i=0;i<width*height;i++){
		lcd_data16(color); //start pos accoridng to MADCTL setting ( i named it MAC)
	}
}


