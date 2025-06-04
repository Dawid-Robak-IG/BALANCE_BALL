#include "flash.h"

HighScores readHighScores(void) {
    HighScores scores;
    memcpy(&scores, (void*)FLASH_USER_START_ADDR, sizeof(HighScores));
    return scores;
}

void saveHighScores(HighScores *scores) {
    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t SectorError;

    EraseInitStruct.TypeErase    = FLASH_TYPEERASE_SECTORS;
    EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    EraseInitStruct.Sector       = FLASH_SECTOR_11;
    EraseInitStruct.NbSectors    = 1;

    if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK) {
    	printf("Couldn't erase flash memory for scores\r\n");
    }

    uint32_t *data = (uint32_t*)scores;
    for (uint32_t i = 0; i < sizeof(HighScores) / 4; i++) {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,
                          FLASH_USER_START_ADDR + i * 4,
                          data[i]);
    }

    HAL_FLASH_Lock();
}

void updateHighScores(uint32_t newTime) {
    HighScores scores = readHighScores();

    for (int i = 0; i < 3; i++) {
        if (newTime < scores.best_times[i] || scores.best_times[i] == 0xFFFFFFFF) {
            for (int j = 2; j > i; j--) {
                scores.best_times[j] = scores.best_times[j - 1];
            }
            scores.best_times[i] = newTime;
            saveHighScores(&scores);
            break;
        }
    }
}
void resetFlash(void) {
    HighScores scores = {
		.magic = 0xDEADBEEF,
		.best_times = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}
	};
    saveHighScores(&scores);
}
bool isFlashUninitialized(void) {
    HighScores *scores = (HighScores*)FLASH_USER_START_ADDR;
    return scores->magic != FLASH_MAGIC;
}

void initializeFlashIfNeeded(void) {
    if (isFlashUninitialized()) {
        resetFlash();
    }
}
