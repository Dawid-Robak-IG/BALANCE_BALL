#ifndef INC_GYRO_H_
#define INC_GYRO_H_

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

// Rejestry żyroskopu
#define CTRL_REG1 0x20
#define CTRL_REG4 0x23
#define STATUS_REG 0x27
#define OUT_X_L 0x28
#define WHO_AM_I 0x0F

// Makra do zarządzania CS
#define GYRO_CS_LOW()  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET)
#define GYRO_CS_HIGH() HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET)

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} Gyro_Int_Data;

//typedef struct {
//    float x;
//    float y;
//    float z;
//} Gyro_Float_Data;



void gyro_init(void);
void gyro_set_sensitivity();
void gyro_ReadWhoAmI(void);
bool gyro_is_data_ready(void);
void gyro_get_data(Gyro_Int_Data *gyro_data);
void gyro_get_filtered_data(Gyro_Int_Data *gyro_data);
void gyro_calculate_offset(Gyro_Int_Data *offset);
void gyro_compensate_and_scale(Gyro_Int_Data *gyro_data,Gyro_Int_Data *offset, Gyro_Int_Data *gyro_calibrated_dat);


//void gyro_write(uint8_t reg, uint8_t* data, uint16_t size);
//void gyro_read(uint8_t reg, uint8_t* data, uint16_t size);



#endif /* INC_GYRO_H_ */
