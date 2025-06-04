#include "levels.h"


const int znak_szer = 16;
const int znak_wys = 16;


void set_up_calibration() {
	lcd_clear_text();
	const char str1[12] = "BALANCE BALL";

	const char str_gyroscope[12] = "GYROSCOPE";
	const char str_calibration[14] = "CALIBRATION...";

	const char str_warn1[12] = "DO NOT MOVE";
	const char str_warn2[11] = "THE BOARD";

	const char str4[8] = "WAIT FOR";
	const char str5[9] = "GREEN LED";

	const char str12[5] = "PRESS";
	const char str2[11] = "BLUE BUTTON";
	const char str3[8] = "TO START";

	int x;
	int j = 1;
	int last_idx;

	for (int i = 0; i < 12; i++) {
		x = znak_szer * (i + 1);
		lcd_set_char(i, x, j * znak_szer, str1[i], GREEN);
	}
	j += 2;
	last_idx = 12;
	for (int i = 0; i < 12; i++) {
		x = znak_szer * (i + 1);
		lcd_set_char(i + last_idx, x, j * znak_szer, str_gyroscope[i], GREEN);
	}
	j++;
	last_idx += 12;
	for (int i = 0; i < 14; i++) {
		x = znak_szer * (i + 1);
		lcd_set_char(i + last_idx, x, j * znak_szer, str_calibration[i], GREEN);
	}
	j += 3;
	last_idx += 14;

	for (int i = 0; i < 12; i++) {
			x = znak_szer * (i + 1);
			lcd_set_char(i + last_idx, x, j * znak_szer, str_warn1[i], RED);
		}

	j ++;
		last_idx += 12;

		for (int i = 0; i < 11; i++) {
				x = znak_szer * (i + 1);
				lcd_set_char(i + last_idx, x, j * znak_szer, str_warn2[i], RED);
			}


	j += 3;
		last_idx += 11;
	for (int i = 0; i < 8; i++) {
		x = znak_szer * (i + 1);
		lcd_set_char(i + last_idx, x, j * znak_szer, str4[i], GREEN);
	}
	j++;
	last_idx += 8;

	for (int i = 0; i < 9; i++) {
		x = znak_szer * (i + 1);
		lcd_set_char(i + last_idx, x, j * znak_szer, str5[i], GREEN);
	}

	j += 3;
	last_idx += 9;
	for (int i = 0; i < 5; i++) {
		x = znak_szer * (i + 1);
		lcd_set_char(i + last_idx, x, j * znak_szer, str12[i], GREEN);
	}

	j++;
	last_idx += 5;
	for (int i = 0; i < 11; i++) {
		x = znak_szer * (i + 1);
		lcd_set_char(i + last_idx, x, j * znak_szer, str2[i], GREEN);
	}

	j++;
	last_idx += 11;
	for (int i = 0; i < 8; i++) {
		x = znak_szer * (i + 1);
		lcd_set_char(i + last_idx, x, j * znak_szer, str3[i], GREEN);
	}

}

void set_up_menu() {
	lcd_clear_text();

	lcd_set_rectangle(0, 0, 132, 150, 32, ORANGE);
	lcd_set_rectangle(1, 90, 215, 150, 32, PURPLE);
	lcd_set_rectangle(2, 0, 292, 150, 32, DARK_RED);

	lcd_set_rectangle(3, 15, 14, 190, 15, BLUE);
	lcd_set_rectangle(4, 15, 62, 90, 15, BLUE);
	lcd_set_rectangle(5, 15, 76, 155, 15, BLUE);

	lcd_set_rectangle(6, 15, 92, 80, 15, BLUE);
	lcd_set_rectangle(7, 0, 0, 0, 0, 0);
	lcd_set_rectangle(8, 0, 0, 0, 0, 0);
	lcd_set_rectangle(9, 0, 0, 0, 0, 0);

	//if (updateBall) {
		lcd_set_circle(220, 120, 8, GREEN);
//	}

	const char str1[12] = "BALANCE BALL";
	const char str12[6] = "SELECT";
	const char str2[10] = "DIFFICULTY";
	const char str3[5] = "LEVEL";
	const char str4[4] = "EASY";
	const char str5[6] = "MEDIUM";
	const char str6[4] = "HARD";

	int x;
	int j = 1;
	int last_idx;
	for (int i = 0; i < 12; i++) {
		x = znak_szer * (i + 1);
		lcd_set_char(i, x, j * znak_szer, str1[i], GREEN);
	}
	j += 3;
	last_idx = 12;
	for (int i = 0; i < 6; i++) {
		x = znak_szer * (i + 1);
		lcd_set_char(i + last_idx, x, j * znak_szer, str12[i], GREEN);
	}
	j++;
	last_idx += 6;
	for (int i = 0; i < 10; i++) {
		x = znak_szer * (i + 1);
		lcd_set_char(i + last_idx, x, j * znak_szer, str2[i], GREEN);
	}
	j++;
	last_idx += 10;
	for (int i = 0; i < 5; i++) {
		x = znak_szer * (i + 1);
		lcd_set_char(i + last_idx, x, j * znak_szer, str3[i], GREEN);
	}

	j += 3;
	last_idx += 5;
	for (int i = 0; i < 4; i++) {
		x = znak_szer * (i + 1);
		if (DifficultyLevel == 1)
			lcd_set_char(i + last_idx, x, j * znak_szer, str4[i], WHITE);
		else
			lcd_set_char(i + last_idx, x, j * znak_szer, str4[i], BLACK);
	}

	j += 5;
	last_idx += 4;
	for (int i = 0; i < 6; i++) {
		x = znak_szer * (i + 9);
		if (DifficultyLevel == 2)
			lcd_set_char(i + last_idx, x, j * znak_szer, str5[i], WHITE);
		else
			lcd_set_char(i + last_idx, x, j * znak_szer, str5[i], BLACK);
	}

	j += 5;
	last_idx += 6;
	for (int i = 0; i < 4; i++) {
		x = znak_szer * (i + 1);
		if (DifficultyLevel == 3)
			lcd_set_char(i + last_idx, x, j * znak_szer, str6[i], WHITE);
		else
			lcd_set_char(i + last_idx, x, j * znak_szer, str6[i], BLACK);
	}
}



void setup_first_lvl() {
	second_pasted = 0;

	lcd_clear_text();
	lcd_clear_screen();

	lcd_set_rectangle(0, 60, 0, 40, 100, RED);
	lcd_set_rectangle(1, 60, 100, 40, 80, YELLOW);
	lcd_set_rectangle(2, 40, 220, 40, 100, YELLOW);

	lcd_set_rectangle(3, 100, 140, 60, 40, YELLOW);
	lcd_set_rectangle(4, 140, 60, 100, 40, YELLOW);
	lcd_set_rectangle(5, 200, 0, 40, 60, GREEN);

	lcd_set_rectangle(6, 200, 100, 40, 220, YELLOW);

	if (DifficultyLevel >= 2) {
		lcd_set_rectangle(2, 40, 220, 40, 100, RED);
	}

	if (DifficultyLevel == 3) {
		lcd_set_rectangle(4, 140, 60, 100, 40, RED);

		lcd_set_rectangle(6, 200, 100, 40, 220, RED);
	}

	lcd_set_circle(10, 10, 8, GREEN);

	HAL_TIM_Base_Start_IT(&htim10);
}

void setup_second_lvl() {

	lcd_clear_text();
	lcd_clear_screen();

	lcd_set_rectangle(0, 0, 30, 180, 15, YELLOW);
	lcd_set_rectangle(1, 180, 30, 15, 210, YELLOW);
	lcd_set_rectangle(2, 30, 100, 15, 190, YELLOW);

	lcd_set_rectangle(3, 45, 100, 85, 15, YELLOW);
	lcd_set_rectangle(4, 45, 140, 50, 15, YELLOW);
	lcd_set_rectangle(5, 50, 120, 15, 15, GREEN);

	lcd_set_rectangle(6, 80, 180, 65, 15, YELLOW);
	lcd_set_rectangle(7, 130, 100, 15, 80, YELLOW);
	lcd_set_rectangle(8, 45, 230, 150, 15, YELLOW);
	lcd_set_rectangle(9, 100, 280, 140, 15, YELLOW);

	if (DifficultyLevel >= 2) {
		lcd_set_rectangle(2, 30, 100, 15, 190, RED);
		lcd_set_rectangle(7, 130, 100, 15, 80, RED);
	}

	if (DifficultyLevel == 3) {
		lcd_set_rectangle(4, 45, 140, 50, 15, RED);
		lcd_set_rectangle(6, 80, 180, 65, 15, RED);
	}

	lcd_set_circle(10, 10, 8, GREEN);

}

void setup_third_lvl() {

	lcd_clear_text();
	lcd_clear_screen();

	lcd_set_rectangle(0, 40, 0, 20, 70, YELLOW);
	lcd_set_rectangle(1, 0, 100, 170, 20, YELLOW);
	lcd_set_rectangle(2, 120, 40, 20, 90, YELLOW);
	lcd_set_rectangle(3, 200, 80, 30, 140, YELLOW);
	lcd_set_rectangle(4, 80, 170, 160, 20, YELLOW);
	lcd_set_rectangle(5, 90, 160, 30, 70, YELLOW);

	lcd_set_rectangle(6, 30, 200, 20, 100, YELLOW);
	lcd_set_rectangle(7, 0, 260, 200, 20, YELLOW);
	lcd_set_rectangle(8, 150, 220, 20, 80, YELLOW);
	lcd_set_rectangle(9, 10, 300, 10, 10, GREEN);

	if (DifficultyLevel >= 2) {
		lcd_set_rectangle(2, 120, 40, 20, 90, RED);
		lcd_set_rectangle(4, 80, 170, 160, 20, RED);
	}

	if (DifficultyLevel == 3) {
		lcd_set_rectangle(1, 0, 100, 170, 20, RED);
		lcd_set_rectangle(6, 30, 200, 20, 100, RED);
	}

	lcd_set_circle(10, 10, 8, GREEN);

}

void setup_end() {
	HAL_TIM_Base_Stop_IT(&htim10);
	//clear_rectangles();
	lcd_clear_text();
	lcd_clear_screen();

	lcd_set_rectangle(0, 14, 14, 120, 16, BLUE);
	lcd_set_rectangle(1, 14, 30, 180, 15, BLUE);
	lcd_set_rectangle(2, 14, 60, 60, 50, BLUE);
	lcd_set_rectangle(3, 0, 0, 0, 0, 0);
	lcd_set_rectangle(4, 0, 0, 0, 0, 0);
	lcd_set_rectangle(5, 0, 0, 0, 0, 0);

	lcd_set_rectangle(6, 0, 0, 0, 0, 0);
	lcd_set_rectangle(7, 0, 0, 0, 0, 0);
	lcd_set_rectangle(8, 0, 0, 0, 0, 0);
	lcd_set_rectangle(9, 0, 0, 0, 0, 0);

	lcd_set_circle(200, 100, 8, GREEN);

	const char str1[7] = "THE END";
	int x;
	int j = 1;
	int last_idx;
	for (int i = 0; i < 7; i++) {
		x = znak_szer * (i + 1);
		lcd_set_char(i, x, j * znak_szer, str1[i], GREEN);
	}
	j++;
	last_idx = 7;

	char time_str[32];
	snprintf(time_str, sizeof(time_str), "TIME %ld SEC", second_pasted);
	printf("%s\r\n", time_str);

	for (int i = 0; time_str[i] != '\0'; i++) {
		x = znak_szer * (i + 1);
		lcd_set_char(i + last_idx, x, j * znak_szer, time_str[i], GREEN);
	}
	j += 2;
	last_idx += 32;

	updateHighScores(second_pasted);
	HighScores scores = readHighScores();

	if (scores.best_times[0] != 0xFFFFFFFF) {
		char score_str[32];
		snprintf(score_str, sizeof(score_str), "1. %ld", scores.best_times[0]);
		printf("Best first time: %s\r\n", score_str);

		for (int i = 0; score_str[i] != '\0'; i++) {
			x = znak_szer * (i + 1);
			lcd_set_char(i + last_idx, x, j * znak_szer, score_str[i], RED);
		}
		j++;
		last_idx += 32;
	}

	if (scores.best_times[1] != 0xFFFFFFFF) {
		char score_str[32];
		snprintf(score_str, sizeof(score_str), "2. %ld", scores.best_times[1]);
		printf("Best second time: %s\r\n", score_str);

		for (int i = 0; score_str[i] != '\0'; i++) {
			x = znak_szer * (i + 1);
			lcd_set_char(i + last_idx, x, j * znak_szer, score_str[i], MAGENTA);
		}
		j++;
		last_idx += 32;
	}

	if (scores.best_times[2] != 0xFFFFFFFF) {
		char score_str[32];
		snprintf(score_str, sizeof(score_str), "3. %ld", scores.best_times[2]);
		printf("Best third time: %s\r\n", score_str);

		for (int i = 0; score_str[i] != '\0'; i++) {
			x = znak_szer * (i + 1);
			lcd_set_char(i + last_idx, x, j * znak_szer, score_str[i], CYAN);
		}
		j++;
		last_idx += 32;
	}

}
void set_screen() {
	HAL_TIM_Base_Stop_IT(&htim7);
	lcd_clear_screen();
	if (screen_id == 0) {
		set_up_calibration();
	} else if (screen_id == 1) {
		set_up_menu();
	} else if (screen_id == 2) {
		setup_first_lvl();
	} else if (screen_id == 3) {
		setup_second_lvl();
	} else if (screen_id == 4) {
		setup_third_lvl();
	} else if (screen_id == 5) {
		setup_end();
	}
	if (screen_id != 0 )
		lcd_update(0);
	else
		lcd_update(1);
	HAL_Delay(500);
	HAL_TIM_Base_Start_IT(&htim7);
}
