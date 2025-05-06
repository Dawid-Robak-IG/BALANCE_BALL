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

#endif /* INC_BALL_PHYSICS_H_ */
