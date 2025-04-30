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
#define MOVING_AVERAGE_WINDOW 10  // Okno do filtrowania
#define SENSITIVITY 0x30        //250dps (0x00) 2000(0300)

static int16_t x_data[MOVING_AVERAGE_WINDOW];
static int16_t y_data[MOVING_AVERAGE_WINDOW];
static int16_t z_data[MOVING_AVERAGE_WINDOW];
static uint8_t data_index = 0;

ssize_t _write(int file, const char *data, size_t len) {
	HAL_UART_Transmit(&huart1, (uint8_t*) data, len, HAL_MAX_DELAY); // Wysyłanie danych przez UART
return len;
}

void gyro_set_sensitivity() {
	uint8_t configData[2] = { CTRL_REG4, SENSITIVITY };

	if (!spi5_acquire()) return;

	GYRO_CS_LOW();
	HAL_SPI_Transmit(&hspi5, configData, 2, 100);
	GYRO_CS_HIGH();

	spi5_release();
}

void gyro_init(void) {
	uint8_t configData[2] = { CTRL_REG1, 0x0F }; // PD=1, Zen=1, Yen=1, Xen=;

	if (!spi5_acquire()) return;

	GYRO_CS_LOW();
	HAL_SPI_Transmit(&hspi5, configData, 2, 100);
	GYRO_CS_HIGH();

	spi5_release();

	gyro_set_sensitivity();

	// Weryfikacja połączenia
	gyro_ReadWhoAmI();
}

void gyro_get_filtered_data(int16_t *x, int16_t *y, int16_t *z) {
	int16_t x_raw, y_raw, z_raw;
	int32_t x_sum = 0, y_sum = 0, z_sum = 0;

	// Bufory do filtracji
	memset(x_data, 0, sizeof(x_data));
	memset(y_data, 0, sizeof(y_data));
	memset(z_data, 0, sizeof(z_data));
	data_index = 0;


	gyro_get_data(&x_raw, &y_raw, &z_raw);

	// Aktualizacja bufora
	x_data[data_index] = x_raw;
	y_data[data_index] = y_raw;
	z_data[data_index] = z_raw;

	//Średnia
	for (uint8_t i = 0; i < MOVING_AVERAGE_WINDOW; i++) {
		x_sum += x_data[i];
		y_sum += y_data[i];
		z_sum += z_data[i];
	}

	*x = x_sum / MOVING_AVERAGE_WINDOW;
	*y = y_sum / MOVING_AVERAGE_WINDOW;
	*z = z_sum / MOVING_AVERAGE_WINDOW;

	// Aktualizuj indeks
	data_index = (data_index + 1) % MOVING_AVERAGE_WINDOW;
}

void gyro_ReadWhoAmI(void) {
	uint8_t tx = WHO_AM_I | 0x80;
	uint8_t rx = 0;

	if (!spi5_acquire())return;

	GYRO_CS_LOW();
	HAL_SPI_Transmit(&hspi5, &tx, 1, 10);
	HAL_SPI_Receive(&hspi5, &rx, 1, 10);
	GYRO_CS_HIGH();

	spi5_release();

	if (rx != 0xD3) {
		printf("WHO_AM_I error: 0x%02X (expected 0xD3)\r\n", rx);
	} else {
		printf("WHO_AM_I OK: 0x%02X\r\n", rx);
	}
}

void gyro_get_data(int16_t *x, int16_t *y, int16_t *z) {

	uint8_t tx = OUT_X_L | 0x80 | 0x40;
	uint8_t rx[6];
	if (!gyro_is_data_ready()) {
		printf("Data not ready\r\n");
		return;
	}

	if (!spi5_acquire()) return;

	GYRO_CS_LOW();
	HAL_SPI_Transmit(&hspi5, &tx, 1, 10);
	HAL_SPI_Receive(&hspi5, rx, 6, 10);
	GYRO_CS_HIGH();

	spi5_release();

	*x = (int16_t) (rx[1] << 8 | rx[0]);
	*y = (int16_t) (rx[3] << 8 | rx[2]);
	*z = (int16_t) (rx[5] << 8 | rx[4]);

}

uint8_t gyro_is_data_ready(void) {
	uint8_t tx = STATUS_REG | 0x80;
	uint8_t rx = 0;

	if (!spi5_acquire()) return 0;

	GYRO_CS_LOW();
	HAL_SPI_Transmit(&hspi5, &tx, 1, 10);
	HAL_SPI_Receive(&hspi5, &rx, 1, 10);
	GYRO_CS_HIGH();

	spi5_release();

	return (rx & 0x08) ? 1 : 0;
}

void gyro_calibration(int16_t *offset_x, int16_t *offset_y, int16_t *offset_z) {
	int32_t sum_x = 0, sum_y = 0, sum_z = 0;
	const uint16_t samples = 500;
	int16_t x, y, z;

	printf("Starting calibration...\r\n");

	for (uint16_t i = 0; i < samples; i++) {
		gyro_get_data(&x, &y, &z);
		sum_x += x;
		sum_y += y;
		sum_z += z;
		HAL_Delay(10);

		if (i % 100 == 0)
			printf(".\r\n");
	}

	*offset_x = sum_x / samples;
	*offset_y = sum_y / samples;
	*offset_z = sum_z / samples;

	printf("\nCalibration complete. Offsets: X=%d, Y=%d, Z=%d\r\n", *offset_x,
			*offset_y, *offset_z);
}



//void gyro_write(uint8_t reg, uint8_t* data, uint16_t size) {
//    // Zajmujemy się tylko wysyłaniem komend do żyroskopu przez SPI
//    uint8_t txData[256];  // Zmienna do przechowania danych
//
//    // Rejestr, do którego będziemy pisać
//    txData[0] = reg | 0x40;  // Ustawiamy 0x40, żeby mówić, że to zapis
//    memcpy(&txData[1], data, size);
//
//    // Wysyłamy dane przez SPI
//    if (spi5_acquire()) {
//        HAL_SPI_Transmit(&hspi5, txData, size + 1, HAL_MAX_DELAY);
//        spi5_release();
//    }
//
//}
//
//void gyro_read(uint8_t reg, uint8_t* data, uint16_t size) {
//    uint8_t txData[256];
//
//    // Rejestr, z którego będziemy odczytywać
//    txData[0] = reg | 0x80;  // Ustawiamy 0x80, żeby mówić, że to odczyt
//    memset(&txData[1], 0, size);  // Wypełniamy resztę bufora zerami (właśnie będziemy odczytywać)
//
//    // Wysyłamy zapytanie i odczytujemy dane przez SPI
//    if (spi5_acquire()) {
//        HAL_SPI_TransmitReceive(&hspi5, txData, data, size + 1, HAL_MAX_DELAY);
//        spi5_release();
//    }
//
//}
//

