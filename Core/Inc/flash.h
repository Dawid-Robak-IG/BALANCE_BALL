/*
 * flash.h
 *
 *  Created on: May 30, 2025
 *      Author: drworms
 */

#ifndef INC_FLASH_H_
#define INC_FLASH_H_

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"

typedef struct {
	uint32_t magic;
    uint32_t best_times[3];  // w milisekundach
} HighScores;

#define FLASH_USER_START_ADDR   ((uint32_t)0x080E0000)
#define FLASH_MAGIC 0xDEADBEEF

HighScores readHighScores(void);
void updateHighScores(uint32_t newTime);
void initializeFlashIfNeeded(void);


#endif /* INC_FLASH_H_ */
