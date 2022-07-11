#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 11
#define HEIGHT 25
#define EMPTY 0
#define WALL 1
#define STOP_BLOCK 2
#define ACTIVE_BLOCK -1

#define LEFT 75
#define RIGHT 77
#define UP 72
#define DOWN 80

int screen[HEIGHT][WIDTH];
int screenCopy[HEIGHT][WIDTH];

int bx, by;
int blockType = 0;
int spin = 0;

int blocks[7][4][4][4] =
{
	{{{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}},{{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0}},	// I
	{{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}},{{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0}}},
	{{{0,0,1,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},{{0,1,0,0},{0,1,0,0},{0,1,1,0},{0,0,0,0}},	// L
	{{1,1,1,0},{1,0,0,0},{0,0,0,0},{0,0,0,0}},{{0,1,1,0},{0,0,1,0},{0,0,1,0},{0,0,0,0}}},
	{{{1,0,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},{{0,1,1,0},{0,1,0,0},{0,1,0,0},{0,0,0,0}},	// J
	{{1,1,1,0},{0,0,1,0},{0,0,0,0},{0,0,0,0}},{{0,0,1,0},{0,0,1,0},{0,1,1,0},{0,0,0,0}}},
	{{{0,1,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},{{0,1,0,0},{0,1,1,0},{0,1,0,0},{0,0,0,0}},	// T
	{{1,1,1,0},{0,1,0,0},{0,0,0,0},{0,0,0,0}},{{0,0,1,0},{0,1,1,0},{0,0,1,0},{0,0,0,0}}},
	{{{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},{{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},	// O
	{{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},{{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}}},
	{{{0,1,1,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}},{{1,0,0,0},{1,1,0,0},{0,1,0,0},{0,0,0,0}},	// S
	{{0,1,1,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}},{{1,0,0,0},{1,1,0,0},{0,1,0,0},{0,0,0,0}}},
	{{{1,1,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},{{0,1,0,0},{1,1,0,0},{1,0,0,0},{0,0,0,0}},	// Z
	{{1,1,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},{{0,1,0,0},{1,1,0,0},{1,0,0,0},{0,0,0,0}}}
};

int key = 0;
bool crushFlag = false;

void CursorView();
void gotoxy(short, short);
void reset();
void draw();
void newBlock();
void downBlock();
bool crush();
void keyCheck();
void blockMove(int move);
bool canMove(int x, int y, int sp);
void lineClear();
void checkGameOver();
void gameOver();

int main() {

	srand(time(0));
	CursorView();
	reset();

	while (true) {
		for (int i = 0; i < 5; i++) {
			keyCheck();
			draw();
			Sleep(100);
		}
		downBlock();
		lineClear();
		draw();
		checkGameOver();
		if (crushFlag) newBlock();
	}

	return 0;
}


void CursorView()
{
	CONSOLE_CURSOR_INFO cursorInfo;
	cursorInfo.dwSize = 1;
	cursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}


void gotoxy(short x, short y) {
	COORD pos = { 2 * x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}


void reset() {
	system("cls");
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			screen[i][j] = EMPTY;
			screenCopy[i][j] = 10;
		}
	}
	for (int j = 0; j < WIDTH; j++) {
		screen[HEIGHT - 1][j] = WALL;
	}
	for (int i = 0; i < HEIGHT; i++) {
		screen[i][0] = WALL;
		screen[i][WIDTH - 1] = WALL;
	}

	draw();
	newBlock();
}

void draw() {
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			if (screen[i][j] != screenCopy[i][j]) {
				gotoxy(j, i);
				switch (screen[i][j]) {
				case EMPTY:
					printf("  ");
					break;
				case WALL:
					printf("в╠");
					break;
				case ACTIVE_BLOCK:
					printf("бр");
					break;
				case STOP_BLOCK:
					printf("бс");
					break;
				}
			}
		}
	}
}


void newBlock() {
	bx = (WIDTH / 2) - 1;
	by = 0;
	blockType = rand() % 7;
	crushFlag = false;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (blocks[blockType][spin][i][j] == 1)
				screen[by + i][bx + j] = ACTIVE_BLOCK;
		}
	}

}


void downBlock() {
	if (crush()) {
		crushFlag = true;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (blocks[blockType][spin][i][j] == 1)
					screen[by + i][bx + j] = STOP_BLOCK;
			}
		}
	}
	else {
		blockMove(DOWN);
	}
}

bool crush() {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (blocks[blockType][spin][i][j] == 1) {
				if (screen[by + 1 + i][bx + j] != ACTIVE_BLOCK && screen[by + 1 + i][bx + j] != EMPTY)
					return true;
			}
		}
	}
	return false;
}


void keyCheck() {
	if (_kbhit()) {
		key = _getch();
		if (key == 224) {
			key = _getch();
			switch (key) {
			case LEFT:
				if (canMove(bx - 1, by, spin))
					blockMove(LEFT);
				break;
			case RIGHT:
				if (canMove(bx + 1, by, spin))
					blockMove(RIGHT);
				break;
			case UP:
				if (canMove(bx, by, (spin + 1) % 4))
					blockMove(UP);
				break;
			case DOWN:
				if (canMove(bx, by + 1, spin))
					blockMove(DOWN);
				break;
			}
		}
	}
}

void blockMove(int move) {
	switch (move) {
	case LEFT:
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (blocks[blockType][spin][i][j] == 1)
					screen[by + i][bx + j] = EMPTY;
			}
		}
		bx--;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (blocks[blockType][spin][i][j] == 1)
					screen[by + i][bx + j] = ACTIVE_BLOCK;
			}
		}
		break;
	case RIGHT:
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (blocks[blockType][spin][i][j] == 1)
					screen[by + i][bx + j] = EMPTY;
			}
		}
		bx++;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (blocks[blockType][spin][i][j] == 1)
					screen[by + i][bx + j] = ACTIVE_BLOCK;
			}
		}
		break;
	case UP:
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (blocks[blockType][spin][i][j] == 1)
					screen[by + i][bx + j] = EMPTY;
			}
		}
		spin = (spin + 1) % 4;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (blocks[blockType][spin][i][j] == 1)
					screen[by + i][bx + j] = ACTIVE_BLOCK;
			}
		}
		break;
	case DOWN:
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (blocks[blockType][spin][i][j] == 1)
					screen[by + i][bx + j] = EMPTY;
			}
		}
		by++;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (blocks[blockType][spin][i][j] == 1)
					screen[by + i][bx + j] = ACTIVE_BLOCK;
			}
		}
		break;
	}
}


bool canMove(int x, int y, int sp) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (blocks[blockType][sp][i][j] == 1 && screen[y + i][x + j] > 0)
				return false;
		}
	}
	return true;
}


void lineClear() {
	bool clear = true;
	for (int i = HEIGHT - 2; i >= 0; i--) {
		for (int j = 1; j < WIDTH - 1; j++) {
			if (screen[i][j] != STOP_BLOCK) {
				clear = false;
				break;
			}
		}
		if (clear) {
			for (int k = i; k > 0; k--) {
				for (int l = 1; l < WIDTH - 1; l++) {
					screen[k][l] = screen[k - 1][l];
				}
			}
			i++;
		}
		clear = true;
	}
}


void checkGameOver() {
	for (int j = 1; j < WIDTH - 1; j++) {
		if (screen[1][j] == STOP_BLOCK) {
			gameOver();
			reset();
		}
	}
}


void gameOver() {
	gotoxy(WIDTH + 5, HEIGHT / 2); printf("G A M E  O V E R..");
	Sleep(1000);
	while (_kbhit()) key = _getch();
	key = _getch();
}
