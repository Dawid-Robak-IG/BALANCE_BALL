#include "ball_physics.h"

int16_t max_speed = 20;


void ball_set_speed(int16_t *speed_x,int16_t *speed_y){
	int dx = (int)(gyro_scaled_data_s.y) ;
	int dy = (int)(gyro_scaled_data_s.x);

	if (abs(dx) < 2) dx=0;
	if (abs(dy) < 2) dy=0;

	if(dx==0){
		*speed_x*=0.99;
	}else{
		*speed_x -= 0.1*dx;
	}

	if(dy==0){
			*speed_y*=0.99;
		}
	else{
		*speed_y -=  0.1*dy;
	}


	if (*speed_x > max_speed) *speed_x = max_speed;
	if (*speed_x < -max_speed) *speed_x = -max_speed;

	if (*speed_y > max_speed) *speed_y = max_speed;
    if (*speed_y < -max_speed) *speed_y = -max_speed;
}



void ball_move(int16_t *speed_x,int16_t *speed_y){
	 int next_x = player.x + *speed_x;
	 int next_y = player.y + *speed_y;

	if(!check_inside_screen(next_x,next_y)){
			*speed_x*=-0.75;
			*speed_y*=-0.75;
			return;
	}


	for (int i = 0; i < RECTS_AMOUNT; i++) {
		if (check_collision(rects[i],next_x,next_y)) {
			*speed_x *= -0.75;
			*speed_y *= -0.75;
			return;
		}
	}

	lcd_update_circle(*speed_x, *speed_y, 0);
}



bool check_collision(Rectangle rec, int next_x, int next_y){
	 if ((next_x < rec.x + rec.width + player.r) &&
		              (next_x > rec.x - player.r) &&
		              (next_y > rec.y - player.r) &&
		              (next_y < rec.y + rec.height + player.r))return true;
	return false;
}


bool check_inside_screen(int next_x, int next_y){
	  if (next_x - player.r <= 0 || next_x + player.r >= LCD_WIDTH || next_y - player.r <= 0 || next_y + player.r >= LCD_HEIGHT) {
		   return false;
		}
	return true;
}

