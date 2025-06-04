#ifndef INC_LEVELS_H_
#define INC_LEVELS_H_

#include "tim.h"
#include "lcd.h"
#include "usart.h"
#include "flash.h"

extern int screen_id;
extern volatile uint32_t second_pasted;



void set_up_calibration();
void set_up_menu() ;
void setup_first_lvl();
void setup_second_lvl();
void setup_third_lvl();
void setup_end();

void set_screen();
#endif /* INC_LEVELS_H_ */
