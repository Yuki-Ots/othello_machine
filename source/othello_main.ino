#include "othello.h"
#include <stdbool.h>
#include <Adafruit_GFX.h>
#include <Adafruit_TFTLCD.h>
#include <TouchScreen.h>
#include <MCUFRIEND_kbv.h>


TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

int cells[BOARD_HEIGHT][BOARD_WIDTH];

int turn = 0;

int cursorX = 0;
int cursorY = 0;

POINT pressed;

int pass_flags = 0;

void setup(void) {
	tft.reset();

	uint16_t identifier = tft.readID();
	tft.begin(identifier);

	
	Serial.begin(9600);
	Serial.println(F("TFT LCD test"));
	switch (identifier) {
		case 0x9325:
			Serial.println(F("Found ILI9325 LCD driver"));	break;
		case 0x9328:
			Serial.println(F("Found ILI9328 LCD driver"));	break;
		case 0x4535:
			Serial.println(F("Found LGDP4535 LCD driver"));	break;
		case 0x7575:
			Serial.println(F("Found HX8347G LCD driver"));	break;
		case 0x9341:
			Serial.println(F("Found ILI9341 LCD driver"));	break;
		case 0x7783:
			Serial.println(F("Found ST7781 LCD driver"));	break;
		case 0x8230:
			Serial.println(F("Found UC8230 LCD driver"));	break;
		case 0x8357:
			Serial.println(F("Found HX8357D LCD driver"));	break;
		case 0x0101:
			identifier = 0x9341;	Serial.println(F("Found 0x9341 LCD driver"));	break;
		case 0x9481:
			Serial.println(F("Found 0x9481 LCD driver"));	break;
		case 0x9486:
			Serial.println(F("Found 0x9486 LCD driver"));	break;
		default:
			Serial.print(F("Unknown LCD driver chip: "));
			Serial.println(identifier, HEX);
			Serial.println(F("If using the Adafruit 2.8\" TFT Arduino shield, the line:"));
			Serial.println(F("  #define USE_ADAFRUIT_SHIELD_PINOUT"));
			Serial.println(F("should appear in the library header (Adafruit_TFT.h)."));
			Serial.println(F("If using the breakout board, it should NOT be #defined!"));
			Serial.println(F("Also if using the breakout, double-check that all wiring"));
			Serial.println(F("matches the tutorial."));
			identifier = 0x9486;
			break;
	}
	Serial.print("TFT size is ");
	Serial.print(tft.width());
	Serial.print("x");
	Serial.println(tft.height());

	for (int y = 0; y < BOARD_HEIGHT; y++) {/*ƒIƒZƒ”Õ‚Ì‰Šú‰»*/
		for (int x = 0; x < BOARD_WIDTH; x++)
			cells[y][x] = COLOR_NONE;
	}
	cells[3][3] = COLOR_WHITE;
	cells[4][4] = COLOR_WHITE;
	cells[3][4] = COLOR_BLACK;
	cells[4][3] = COLOR_BLACK;
	
#ifdef DEBUG
	for (int y = 0; y < BOARD_HEIGHT; y++) {
		for (int x = 0; x < BOARD_WIDTH; x++) {
			cells[y][x] = TEST_BOARD[y][x];
		}
	}
#endif /* DEBUG */
	
	draw_all();
	pinMode(13, OUTPUT);
	
}
 
void loop(void) {
	pass_flags &= NOTHING_PASS;
	if (turn == COLOR_WHITE) {
		choose_computer_hand();
		turn ^= 1;
		draw_board();
	}

	int x, y;
	int number_of_stones = 0;
	pass_flags |= BLACK_PASS;
	for (y = 0;y < BOARD_HEIGHT; y++) {
		for (x = 0;x < BOARD_WIDTH; x++) {
			if (cells[y][x] != COLOR_NONE)
				number_of_stones++;
			if (distinguish_can_put(x, y)) {
				pass_flags &= NOTHING_PASS;
			}
		}
	}

	if (pass_flags & COLOR_WHITE) {
		/*tft.setcursor();
		tft.print("PASS!");
		*/
		turn ^= 1;
	}
	
	if (pass_flags & BLACK_PASS &&
		pass_flags & BLACK_PASS ||
		number_of_stones == BOARD_WIDTH * BOARD_HEIGHT
		
	)	check_win();

	
	PORTB |= B11001111; //digitalWrite(13, HIGH);
	TSPoint p = ts.getPoint();
	PORTB &= B11001111; //digitalWrite(13, LOW);
	
	pinMode(XM, OUTPUT);
	pinMode(YP, OUTPUT);

	if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
		pressed.x = p.x;
		pressed.y = p.y;
			
		fix_coordinate(&pressed.x, &pressed.y);

		switch (identify_pressed_button(pressed.x, pressed.y)) {
			case BUTTON_UP:
				if (check_change_cursor(BUTTON_UP)) {
					//cursorY--;
					draw_only_change_cursor(BUTTON_UP);
				}
				break;

			case BUTTON_DOWN:
				if (check_change_cursor(BUTTON_DOWN)) {
					//cursorY++;
					draw_only_change_cursor(BUTTON_DOWN);
				}
				break;

			case BUTTON_LEFT:
				if (check_change_cursor(BUTTON_LEFT)) {
					//cursorX--;
					draw_only_change_cursor(BUTTON_LEFT);
				}
				break;

			case BUTTON_RIGHT:
				if (check_change_cursor(BUTTON_RIGHT)) {
					//cursorX++;
					draw_only_change_cursor(BUTTON_RIGHT);
				}
				break;
					
			case BUTTON_PUT:
				if (distinguish_can_put(cursorX, cursorY)) {
					reverce_stones(cursorX, cursorY);
					turn ^= 1;
					draw_board();
				}
				break;

			case BUTTON_EXIT:
				clearn_up_board();
				break;

			default:
				break;
		}
		
	}
}

