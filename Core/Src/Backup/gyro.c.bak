#include "gyro.h"
#include "spi.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>

#define CTRL_REG1 0x20         // Rejestr kontrolny 1 (aktywny tryb)
#define CTRL_REG4 0x23
#define OUT_X_L 0x28           // Rejestr danych X (niższy bajt)
#define OUT_X_H 0x29           // Rejestr danych X (wyższy bajt)
#define OUT_Y_L 0x2A           // Rejestr danych Y (niższy bajt)
#define OUT_Y_H 0x2B           // Rejestr danych Y (wyższy bajt)
#define OUT_Z_L 0x2C           // Rejestr danych Z (niższy bajt)
#define OUT_Z_H 0x2D           // Rejestr danych Z (wyższy bajt)
#define MOVING_AVERAGE_WINDOW 5  // Okno do filtrowania
#define SENSITIVITY 0x30        //250dps (0x00) 2000(ox03)

static int16_t x_data[MOVING_AVERAGE_WINDOW];
static int16_t y_data[MOVING_AVERAGE_WINDOW];
static int16_t z_data[MOVING_AVERAGE_WINDOW];
static uint8_t data_index = 0;

ssize_t _write(int file, const char *data, size_t len) {
	HAL_UART_Transmit(&huart1, (uint8_t*) data, len, HAL_MAX_DELAY); // Wysyłanie danych przez UART
	return len;
}

void gyro_init(void) {
	uint8_t configData[2] = { CTRL_REG1, 0x0F }; // PD=1, Zen=1, Yen=1, Xen=1;

	while (!spi5_acquire())
		return;

	GYRO_CS_LOW();
	HAL_SPI_Transmit(&hspi5, configData, 2, 100);
	GYRO_CS_HIGH();

	spi5_release();

	gyro_set_sensitivity();

	// Weryfikacja połączenia
	gyro_ReadWhoAmI();
}

void gyro_set_sensitivity() {
	uint8_t configData[2] = { CTRL_REG4, SENSITIVITY };

	while (!spi5_acquire())
		return;

	GYRO_CS_LOW();
	HAL_SPI_Transmit(&hspi5, configData, 2, 100);
	GYRO_CS_HIGH();

	spi5_release();
}

void gyro_get_filtered_data(Gyro_Int_Data *gyro_data) {

	int32_t x_sum = 0, y_sum = 0, z_sum = 0;
	Gyro_Int_Data raw_data;
	data_index = 0;

	gyro_get_data(&raw_data);

	// Aktualizacja bufora
	x_data[data_index] = raw_data.x;
	y_data[data_index] = raw_data.y;
	z_data[data_index] = raw_data.z;

	//Średnia
	for (uint8_t i = 0; i < MOVING_AVERAGE_WINDOW; i++) {
		x_sum += x_data[i];
		y_sum += y_data[i];
		z_sum += z_data[i];
	}

	gyro_data->x = x_sum / MOVING_AVERAGE_WINDOW;
	gyro_data->y = y_sum / MOVING_AVERAGE_WINDOW;
	gyro_data->z = z_sum / MOVING_AVERAGE_WINDOW;

	// Aktualizuj indeks
	data_index = (data_index + 1) % MOVING_AVERAGE_WINDOW;
}

void gyro_ReadWhoAmI(void) {
	uint8_t tx = WHO_AM_I | 0x80;
	uint8_t rx = 0;

	if (!spi5_acquire())
		return;

	GYRO_CS_LOW();
	HAL_SPI_Transmit(&hspi5, &tx, 1, 10);
	HAL_SPI_Receive(&hspi5, &rx, 1, 10);
	GYRO_CS_HIGH();

	spi5_release();

	if ((rx != 0xD3) && (rx != 0xD4)) {
		printf("WHO_AM_I error: 0x%02X (expected 0xD3 or 0xD4)\r\n", rx);
	}
	else {
		printf("WHO_AM_I OK: 0x%02X\r\n", rx);
	}
}

void gyro_get_data(Gyro_Int_Data *gyro_data) {

	uint8_t tx = OUT_X_L | 0x80 | 0x40;
	uint8_t rx[6];
	if (gyro_is_data_ready()==false) {
//		printf("Data not ready\r\n");
		return;
	}

	if (!spi5_acquire())
		return;

	GYRO_CS_LOW();
	HAL_SPI_Transmit(&hspi5, &tx, 1, 10);
	HAL_SPI_Receive(&hspi5, rx, 6, 10);
	GYRO_CS_HIGH();

	spi5_release();

	gyro_data->x = (int16_t) (rx[1] << 8 | rx[0]);
	gyro_data->y = (int16_t) (rx[3] << 8 | rx[2]);
	gyro_data->z = (int16_t) (rx[5] << 8 | rx[4]);

}

bool gyro_is_data_ready(void) {
	uint8_t tx = STATUS_REG | 0x80;
	uint8_t rx = 0;

	if (!spi5_acquire())
		return false;

	GYRO_CS_LOW();
	HAL_SPI_Transmit(&hspi5, &tx, 1, 10);
	HAL_SPI_Receive(&hspi5, &rx, 1, 10);
	GYRO_CS_HIGH();

	spi5_release();

	return (rx & 0x08);
}

void gyro_calculate_offset(Gyro_Int_Data *offset) {
	int64_t sum_x = 0, sum_y = 0, sum_z = 0;
	const uint16_t samples = 500;
	Gyro_Int_Data raw_data;

	printf("Starting calibration...\r\n");

	for (uint16_t i = 0; i < samples; i++) {
		gyro_get_data(&raw_data);
		sum_x += raw_data.x;
		sum_y += raw_data.y;
		sum_z += raw_data.z;
		HAL_Delay(10);

		if (i % 100 == 0)
			printf(".\r\n");
	}

	offset->x = sum_x / samples;
	offset->y = sum_y / samples;
	offset->z = sum_z / samples;

	printf("\nCalibration complete. Offsets: X=%d, Y=%d, Z=%d\r\n", offset->x,
			offset->y, offset->z);
}

void gyro_compensate_and_scale(Gyro_Int_Data *gyro_data, Gyro_Int_Data *offset, Gyro_Int_Data *gyro_calibrated_dat) {

	// uwzględenienie kalibracji
	gyro_data->x -= offset->x;
	gyro_data->y -= offset->y;
	gyro_data->z -= offset->z;

	float scale = 1.0f;

	if (SENSITIVITY == 0x30)
		scale = 2000.0f / 32768.0f;

	if (SENSITIVITY == 0x00)
		scale = 250.0f / 32768.0f;

	// Konwersja do dps (dla skali 2000dps)
	gyro_calibrated_dat->x = gyro_data->x * scale;
	gyro_calibrated_dat->y = gyro_data->y * scale;
	gyro_calibrated_dat->z = gyro_data->z * scale;

	//printf("X: %d dps, Y: %d dps, Z: %d dps\r\n", gyro_calibrated_dat->x, gyro_calibrated_dat->y, gyro_calibrated_dat->z);
}

