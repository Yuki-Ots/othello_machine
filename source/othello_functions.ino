#include "othello.h"
#include <stdbool.h>
#include <Adafruit_GFX.h>
#include <Adafruit_TFTLCD.h>
#include <TouchScreen.h>
#include <MCUFRIEND_kbv.h>

void draw_all(void)
{
	tft.fillScreen(CYAN);
	tft.fillRect(X1, Y1, WIDTH1, HEIGHT1, GREEN);
	tft.drawRect(X1, Y1, WIDTH1, HEIGHT1, BLACK);

	int x, y;
	for (y = 0; y < BOARD_WIDTH + 1; y++)
		tft.drawLine(
			X2, Y2 + HEIGHT2 * y,
			X2 + WIDTH2 * BOARD_WIDTH, Y2 + HEIGHT2 * y,
			BLACK
		);
	
	for (x = 0; x < BOARD_HEIGHT + 1; x++)
		tft.drawLine(
			X2 + x * WIDTH2, Y2, X2 + x * WIDTH2,
			Y2 + HEIGHT2 * BOARD_HEIGHT,
			BLACK
		);
	draw_board();

	/* ボタンの描画 */
	int i;
	for (i = 0; i < NUMBER_OF_BUTTONS; i++) {
		tft.drawRoundRect(
			BUTTONS[i].point.x,
			BUTTONS[i].point.y,
			BUTTONS[i].w,
			BUTTONS[i].h,
			BUTTON_RADIUS,
			BUTTONS[i].color
		);
		tft.fillRoundRect(
			BUTTONS[i].point.x + GAP_OF_BUTTON,
			BUTTONS[i].point.y + GAP_OF_BUTTON,
			BUTTONS[i].w - GAP_OF_BUTTON * 2,
			BUTTONS[i].h - GAP_OF_BUTTON * 2,
			BUTTON_RADIUS,
			BUTTONS[i].color
		);
	}
}

void draw_board(void)
{
	int x,y;

	for (y = 0; y < BOARD_HEIGHT; y++) {//オセロ石の描画
		for (x = 0; x < BOARD_WIDTH; x++) {
			tft.fillRect(
				X2 + WIDTH2 * x + GAP_OF_BOX_AND_CURSOR,
				Y2 + HEIGHT2 * y + GAP_OF_BOX_AND_CURSOR,
				WIDTH2 - GAP_OF_BOX_AND_CURSOR * 2,
				HEIGHT2 - GAP_OF_BOX_AND_CURSOR * 2,
				GREEN
			);
			if ((x == cursorX) && (y == cursorY)) {
				
				tft.fillCircle(
					X2 + WIDTH2 / 2 + WIDTH2 * x,
					Y2 + HEIGHT2 / 2 + HEIGHT2 * y,
					STONE_RADIUS,
					MAGENTA
				);
				
				tft.drawCircle(
					X2 + WIDTH2 / 2 + WIDTH2 * x,
					Y2 + HEIGHT2 / 2 + HEIGHT2 * y,
					STONE_RADIUS,
					BLACK
				);
				
				tft.drawCircle(
					X2 + WIDTH2 / 2 + WIDTH2 * x,
					Y2 + HEIGHT2 / 2 + HEIGHT2 * y,
					CURSOR_RADIUS,
					BLACK
				);
			}
			else {
				switch (cells[y][x]) {
					case COLOR_NONE:	break;
					case COLOR_BLACK:
						tft.fillCircle(
							X2 + WIDTH2 / 2 + WIDTH2 * x,
							Y2 + HEIGHT2 / 2 + HEIGHT2 * y,
							STONE_RADIUS,
							BLACK
						); break;
					case COLOR_WHITE:
						tft.fillCircle(
							X2 + WIDTH2 / 2 + WIDTH2 * x,
							Y2 + HEIGHT2 / 2 + HEIGHT2 * y,
							STONE_RADIUS,
							WHITE
						);
						tft.drawCircle(
							X2 + WIDTH2 / 2 + WIDTH2 * x,
							Y2 + HEIGHT2 / 2 + HEIGHT2 * y,
							STONE_RADIUS,
							BLACK
						); break;
					default:  break;
				}
			}
		}
	}

	for (y = 0; y < BOARD_HEIGHT; y++) {
		for (x = 0; x < BOARD_WIDTH; x++) {
			if (distinguish_can_put(x, y)) {
				tft.drawCircle(
					X2 + WIDTH2 / 2 + WIDTH2 * x,
					Y2 + HEIGHT2 / 2 + HEIGHT2 * y,
					STONE_RADIUS,
					MAGENTA
				);
#ifdef DEBUG
				Serial.print("(");
				Serial.print(x);
				Serial.print(",");
				Serial.print(y);
				Serial.print(")");
				Serial.println(" can put");
#endif /* DEBUG */
			}
		}
	}
}

bool check_change_cursor(BUTTON_TYPE button_type)
{
	if (
		button_type == BUTTON_UP && cursorY >= 1 ||
		button_type == BUTTON_DOWN && cursorY <= 6 ||
		button_type == BUTTON_LEFT && cursorX >= 1 ||
		button_type == BUTTON_RIGHT && cursorX <= 6
	)
		return true;
	else
		return false;
}

void fix_coordinate(int *touch_x, int *touch_y)
{

	/*x、y座標がディスプレイとタッチパネルで逆なので入れ替え*/
	*touch_x = *touch_x + *touch_y;
	*touch_y = *touch_x - *touch_y;
	*touch_x = *touch_y - *touch_x;

	/*タッチパネルの座標をディスプレイ座標に変換*/
	*touch_x =tft.width() - (
		map(*touch_x, TS_MINX, TS_MAXX, tft.width(), 0) - ERROR_SPACE
	);	//原因不明のスペース
	*touch_y = map(*touch_y, TS_MINY, TS_MAXY, 0, tft.height());

#ifdef DEBUG

	Serial.print("(");
	Serial.print(*touch_x);
	Serial.print(",");
	Serial.print(*touch_y);
	Serial.println(")");
	
#endif
}

bool distinguish_can_put(int _cursorX, int _cursorY)
{
	if (
		cells[_cursorY][_cursorX] == COLOR_WHITE ||
		cells[_cursorY][_cursorX] == COLOR_BLACK
	)return false;
	
	int i, k;
	for (i = 0; i < NUMBER_OF_VECTORS; i++) {
		if (
			_cursorY + VECTORS[i][0] < 0 ||
			_cursorY + VECTORS[i][0] >= BOARD_WIDTH ||
			_cursorX + VECTORS[i][1] < 0 ||
			_cursorX + VECTORS[i][1] >= BOARD_HEIGHT ||
			cells[_cursorY + VECTORS[i][0]][_cursorX + VECTORS[i][1]] == turn ||
			cells[_cursorY + VECTORS[i][0]][_cursorX + VECTORS[i][1]] == COLOR_NONE
		)continue;

		for (k = 2; k < BOARD_WIDTH; k++) {
			if (
				_cursorY + VECTORS[i][0] * k  < 0 ||
				_cursorY + VECTORS[i][0] * k  >= BOARD_WIDTH ||
				_cursorX + VECTORS[i][1] * k  < 0 ||
				_cursorX + VECTORS[i][1] * k  >= BOARD_HEIGHT ||
				cells[_cursorY + VECTORS[i][0] * k][_cursorX + VECTORS[i][1] * k] == COLOR_NONE
			)break;

			if (
				cells[_cursorY + VECTORS[i][0] * k][_cursorX + VECTORS[i][1] * k] == turn
			)return true;
		}
	}
	return false;
}

BUTTON_TYPE identify_pressed_button(int pressed_x, int pressed_y)
{
	int i;
	for (i = 0; i < NUMBER_OF_BUTTONS; i++) {
		if (
			pressed_x >= BUTTONS[i].point.x &&
			pressed_x <= BUTTONS[i].point.x + BUTTONS[i].w &&
			pressed_y >= BUTTONS[i].point.y &&
			pressed_y <= BUTTONS[i].point.y + BUTTONS[i].h
		)
		return BUTTONS[i].name;
	}
	return NOTHING_PRESSED_BUTTON;
}


void choose_computer_hand(void)
{
	POINT computer_hand;
	bool can_put_somewhere = false;
	int hand_evaluation = DEFALUT_VALUE;
	int x, y;
	for (y = 0; y < BOARD_HEIGHT; y++) {
		for (x = 0; x < BOARD_WIDTH; x++) {
			if (distinguish_can_put(x, y)) {
				can_put_somewhere = true;
				if (hand_evaluation < EVALUATION_BOARD[y][x]) {
					computer_hand.x = x;
					computer_hand.y = y;
					hand_evaluation = EVALUATION_BOARD[y][x];
				}
			}
		}
	}
	if (can_put_somewhere)	reverce_stones(computer_hand.x, computer_hand.y);
	
	/* 置くところがない場合*/
	pass_flags |= WHITE_PASS;
}

void reverce_stones(int _cursorX, int _cursorY)
{

	if (
		cells[_cursorY][_cursorX] == COLOR_WHITE ||
		cells[_cursorY][_cursorX] == COLOR_BLACK
	)return;
	
	int i, j, k;
	for (i = 0; i < NUMBER_OF_VECTORS; i++) {
		if (
			_cursorY + VECTORS[i][0] < 0||
			_cursorY + VECTORS[i][0] >= BOARD_WIDTH ||
			_cursorX + VECTORS[i][1] < 0 ||
			_cursorX + VECTORS[i][1] >= BOARD_HEIGHT ||
			cells[_cursorY + VECTORS[i][0]][_cursorX + VECTORS[i][1]] == turn ||
			cells[_cursorY + VECTORS[i][0]][_cursorX + VECTORS[i][1]] == COLOR_NONE
		)continue;

		for (k = 2; k < BOARD_WIDTH; k++) {
			if (
				_cursorY + VECTORS[i][0] * k  < 0 ||
				_cursorY + VECTORS[i][0] * k  >= BOARD_WIDTH ||
				_cursorX + VECTORS[i][1] * k  < 0 ||
				_cursorX + VECTORS[i][1] * k  >= BOARD_HEIGHT ||
				cells[_cursorY + VECTORS[i][0] * k][_cursorX + VECTORS[i][1] * k] == COLOR_NONE
			)break;

			if (
				cells[_cursorY + VECTORS[i][0] * k][_cursorX + VECTORS[i][1] * k] == turn
			) {
				for (j = k ; j >= 0; j--)
					cells[_cursorY + VECTORS[i][0] * j][_cursorX + VECTORS[i][1] * j] = turn;
			}
		}
	}
}

void draw_only_change_cursor(BUTTON_TYPE type)
{
	POINT old_cursor = {cursorX, cursorY};
	
	switch (type) {
		/*ボードの座標は画面のボタン側が”上”に当たるため
		VECTORSと上下反対になっている
		*/
		case BUTTON_UP:
			cursorX += VECTORS[VECTORS_DOWN][1];
			cursorY += VECTORS[VECTORS_DOWN][0];
			break;

		case BUTTON_DOWN:
			cursorX += VECTORS[VECTORS_UP][1];
			cursorY += VECTORS[VECTORS_UP][0];
			break;
		
		case BUTTON_LEFT:
			cursorX += VECTORS[VECTORS_LEFT][1];
			cursorY += VECTORS[VECTORS_LEFT][0];
			break;
		
		case BUTTON_RIGHT:
			cursorX += VECTORS[VECTORS_RIGHT][1];
			cursorY += VECTORS[VECTORS_RIGHT][0];
			break;
		
		default:
			break;
	}
	tft.fillCircle(
		X2 + WIDTH2 / 2 + WIDTH2 * cursorX,
		Y2 + HEIGHT2 / 2 + HEIGHT2 * cursorY,
		STONE_RADIUS,
		MAGENTA
	);

	tft.drawCircle(
		X2 + WIDTH2 / 2 + WIDTH2 * cursorX,
		Y2 + HEIGHT2 / 2 + HEIGHT2 * cursorY,
		STONE_RADIUS,
		BLACK
	);
				
	tft.drawCircle(
		X2 + WIDTH2 / 2 + WIDTH2 * cursorX,
		Y2 + HEIGHT2 / 2 + HEIGHT2 * cursorY,
		CURSOR_RADIUS,
		BLACK
	);

	tft.fillRect(
		X2 + WIDTH2 * old_cursor.x + GAP_OF_BOX_AND_CURSOR,
		Y2 + HEIGHT2 * old_cursor.y + GAP_OF_BOX_AND_CURSOR,
		WIDTH2 - GAP_OF_BOX_AND_CURSOR * 2,
		HEIGHT2 - GAP_OF_BOX_AND_CURSOR * 2,
		GREEN
	);

	if (cells[old_cursor.y][old_cursor.x] == COLOR_BLACK) {
		tft.fillCircle(
			X2 + WIDTH2 / 2 + WIDTH2 * old_cursor.x,
			Y2 + HEIGHT2 / 2 + HEIGHT2 * old_cursor.y,
			STONE_RADIUS,
			BLACK
		); 
	}

	if (cells[old_cursor.y][old_cursor.x] == COLOR_WHITE) {
		tft.fillCircle(
			X2 + WIDTH2 / 2 + WIDTH2 * old_cursor.x,
			Y2 + HEIGHT2 / 2 + HEIGHT2 * old_cursor.y,
			STONE_RADIUS,
			WHITE
		);
		tft.drawCircle(
			X2 + WIDTH2 / 2 + WIDTH2 * old_cursor.x,
			Y2 + HEIGHT2 / 2 + HEIGHT2 * old_cursor.y,
			STONE_RADIUS,
			BLACK
		); 
	}
	
	if (distinguish_can_put(old_cursor.x, old_cursor.y)) {
		tft.drawCircle(
			X2 + WIDTH2 / 2 + WIDTH2 * old_cursor.x,
			Y2 + HEIGHT2/ 2 + HEIGHT2 * old_cursor.y,
			STONE_RADIUS,
			MAGENTA
		);
	}
	delay(100);
		
}

void check_win(void)
{
	int black_stones = 0;
	int white_stones = 0;

	int x, y;
	for (y = 0;y < BOARD_HEIGHT; y++) {
		for (x = 0;x < BOARD_WIDTH; x++) {
			if (cells[y][x] == COLOR_WHITE)	white_stones++;
			if (cells[y][x] == COLOR_BLACK)	black_stones++;
		}
	}

	if (white_stones > black_stones){
		print_win(white_stones, black_stones);
		clearn_up_board();
	}

	if (white_stones < black_stones){
		print_lose(white_stones, black_stones);
		clearn_up_board();
	}

	if (white_stones == black_stones){
		print_draw();
		clearn_up_board();
	}
}

void clearn_up_board(void)
{
	turn = 0;
	int x, y;
	for (y = 0;y < BOARD_HEIGHT; y++) {
		for (x = 0;x < BOARD_WIDTH; x++) {
			cells[y][x] = COLOR_NONE;
		}
	}
	cells[3][3] = COLOR_WHITE;
	cells[4][4] = COLOR_WHITE;
	cells[3][4] = COLOR_BLACK;
	cells[4][3] = COLOR_BLACK;
	draw_all();
	
}

void print_win(int white_stones, int black_stones)
{
	tft.setTextColor(RED);
	tft.setTextSize(TEXT_SIZE);
	tft.println("  YOU WIN!");
	tft.print("white..");
	tft.println(white_stones);
	tft.print("black..");
	tft.println(black_stones);
	delay(10000);
	
}

void print_lose(int white_stones, int black_stones)
{
	tft.setTextColor(BLUE);
	tft.setTextSize(TEXT_SIZE);
	tft.println("  you lose..");
	tft.print("white..");
	tft.println(white_stones);
	tft.print("black..");
	tft.println(black_stones);
	delay(10000);
}

void print_draw(void)
{
	
	tft.setTextColor(BLACK);
	tft.setTextSize(TEXT_SIZE);
	tft.println("    DRAW");
	delay(10000);
}
