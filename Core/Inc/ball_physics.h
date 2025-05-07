#ifndef INC_BALL_PHYSICS_H_
#define INC_BALL_PHYSICS_H_


#include <stdint.h>
#include "gyro.h"
#include "spi.h"
#include "usart.h"

extern int16_t max_speed;
//extern volatile Circle player;
void ball_set_speed(int16_t *speed_x,int16_t *speed_y);
void ball_move(int16_t *speed_x,int16_t *speed_y);


bool check_collision(Rectangle rec,int next_x, int next_y);
bool check_inside_screen(int next_x, int next_y);

#endif /* INC_BALL_PHYSICS_H_ */
