#ifndef OTHELLO_H_INCLUDED
#define OTHELLO_H_INCLUDED

#include <stdbool.h>
#include <Adafruit_GFX.h>
#include <Adafruit_TFTLCD.h>
#include <TouchScreen.h>
#include <MCUFRIEND_kbv.h>

#define DEBUG

#define YP	(A2)  /* must be an analog pin, use "An" notation! */
#define XM	(A3)  /* must be an analog pin, use "An" notation! */
#define YM	(8)   /* can be a digital pin */
#define XP	(9)   /* can be a digital pin */

#define BLACK	(0x0000)
#define RED		(0xF800)
#define GREEN	(0x07E0)
#define BLUE	(0x001F)
#define CYAN	(0x07FF)
#define MAGENTA	(0xF81F)
#define YELLOW	(0xFFE0)
#define WHITE	(0xFFFF)
#define GRAY	(0x7BEF)

#define LCD_CS		(A3)
#define LCD_CD		(A2)
#define LCD_WR		(A1)
#define LCD_RD		(A0)
#define LCD_RESET	(A4)

#define BOARD_WIDTH		(8)
#define BOARD_HEIGHT	(8)

#define BUTTON_RADIUS	(10)
#define STONE_RADIUS	(15)
#define CURSOR_RADIUS	(10)

#define X1			(10)
#define Y1			(10)

#define X2			(20)
#define Y2			(20)

#define X3			(325)
#define Y3			(325)

#define X4			(90)
#define Y4			(330)

#define X5			(90)
#define Y5			(430)

#define X6			(40)
#define Y6			(330)

#define X7			(240)
#define Y7			(330)

#define X8			(90)
#define Y8			(375)

#define X9			(320)
#define Y9			(480)

#define WIDTH1		(300)
#define HEIGHT1		(300)

#define WIDTH2		(35)
#define HEIGHT2		(35)

#define WIDTH3		(100)
#define HEIGHT3		(5)

#define WIDTH4		(140)
#define HEIGHT4		(40)

#define WIDTH5		(140)
#define HEIGHT5		(40)

#define WIDTH6		(40)
#define HEIGHT6		(140)

#define WIDTH7		(40)
#define HEIGHT7		(140)

#define WIDTH8		(140)
#define HEIGHT8		(50)

#define WIDTH9		(20)
#define HEIGHT9		(140)


#define GAP_OF_BUTTON	(5)
#define GAP_OF_BOX_AND_CURSOR	(1)

#define TS_MINX		(130)
#define TS_MAXX		(905)

#define TS_MINY		(75)
#define TS_MAXY		(930)

#define ERROR_SPACE		(420)

#define NUMBER_OF_BUTTONS	(6)
#define NUMBER_OF_VECTORS	(8)

#define DEFALUT_VALUE	(-500) //ï]âøílÇÃç≈è¨íl
#define TEXT_SIZE		(5)

#define MINPRESSURE (10)
#define MAXPRESSURE (1000)

#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

typedef struct point {
	int x;
	int y;
}POINT;

typedef enum {
    COLOR_NONE = -1,
    COLOR_BLACK = 0,
    COLOR_WHITE = 1,
}COLOR_STATUS;


enum vectors_name {
	VECTORS_UP = 0,
	VECTORS_UP_RIGHT,
	VECTORS_RIGHT,
	VECTORS_DOWN_RIGHT,
	VECTORS_DOWN,
	VECTORS_DOWN_LEFT,
	VECTORS_LEFT,
	VECTORS_UP_LEFT,
};

typedef enum button_type{
	BUTTON_UP,
	BUTTON_DOWN,
	BUTTON_LEFT,
	BUTTON_RIGHT,
	BUTTON_PUT,
	BUTTON_EXIT,
	NOTHING_PRESSED_BUTTON,
}BUTTON_TYPE;

/*	int RADIUS;
	int color;
}CIRCLE;
*/
typedef struct button {
	BUTTON_TYPE name;
	POINT point;
	int w;
	int h;
	int color;
}BUTTON;

const int VECTORS[][2] = {
	/* {y ,x} */
	{1, 0},		//UP
	{1, 1},		//UP_RIGHT
	{0, 1},		//RIGHT
	{-1, 1},	//DOWN_RIGHT
	{-1, 0},	//DOWN
	{-1, -1},	//DOWN_LEFT
	{0, -1},	//LEFT
	{1, -1},	//UP_LEFT
};


const int EVALUATION_BOARD[BOARD_HEIGHT][BOARD_WIDTH] = {
	{100, -40, 20, 5, 5,20, -40,100},
	{-40, -80, -1,-1,-1,-1,-80,-40},
	{20, -1, 5, 1,1,5,-1, 20},
	{5, -1, 1, 0, 0, 1,-1, 5},
	{5, -1, 1, 0, 0, 1,-1, 5},
	{20, -1, 5, 1,1,5,-1, 20},
	{-40, -80, -1,-1,-1,-1,-80,-40},
	{100, -40, 20, 5, 5,20, -40,100},
};

#ifdef DEBUG
const int TEST_BOARD[BOARD_HEIGHT][BOARD_WIDTH] = {
	{-1, -1, -1, -1, -1, -1, 0, -1},
	{-1, -1, -1, -1, -1, -1, 1, -1},
	{-1, -1, -1, -1, -1, -1, 1, -1},
	{-1, -1, -1, -1, -1, -1, 1, -1},
	{-1, -1, -1, -1, -1, -1, 1, -1},
	{-1, -1, -1, -1, -1, -1, 1, -1},
	{-1, -1, -1, -1, -1, -1, 1, -1},
	{-1, -1, -1, -1, -1, -1, -1, -1},
};
#endif /* DEBUG */

enum pass {
	NOTHING_PASS = 0,
	BLACK_PASS = 1,
	WHITE_PASS = 2,
}PASS;

BUTTON BUTTONS[NUMBER_OF_BUTTONS] = {
	{
		BUTTON_UP,
		{X4, Y4},
		WIDTH4,
		HEIGHT4,
		BLUE
	},

	{
		BUTTON_DOWN,
		{X5, Y5},
		WIDTH5,
		HEIGHT5,
		BLUE
	},

	{
		BUTTON_LEFT,
		{X6, Y6},
		WIDTH6,
		HEIGHT6,
		BLUE
	},

	{
		BUTTON_RIGHT,
		{X7, Y7},
		WIDTH7,
		HEIGHT7,
		BLUE
	},

	{
		BUTTON_PUT,
		{X8, Y8},
		WIDTH8,
		HEIGHT8,
		RED
		
	},

	{
		BUTTON_EXIT,
		{X9, Y9},
		WIDTH9,
		HEIGHT9,
		GRAY
	},

};

/*CIRCLE circles[NUMBER_OF_CIRCLES] = {
	{},	
	{},
	{},
};
*/
MCUFRIEND_kbv tft;

extern int turn;
extern int cursorX;
extern int cursorY;
extern POINT pressed;

void draw_all(void);
void draw_board(void);
bool distinguish_can_put(int _cursorX, int _curxorY);
BUTTON_TYPE identify_pressed_button(int x, int y);
bool check_change_cursor(BUTTON_TYPE type);
void choose_computer_hand(void);
void reverce_stones(int _cursorX, int cursorY);
void draw_only_change_cursor(BUTTON_TYPE type);
void check_win(void);
void clearn_up_board(void);
void print_win(int black_stones, int white_stones);
void print_lose(int black_stones, int white_stones);
void print_draw(void);


#endif /*OTHELLO_H_INCLUDED*/