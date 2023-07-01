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
#define SPACE 32
#define PAUSE1 112
#define PAUSE2 80
#define ESC 27

#define SCREEN_X_ADJ 3
#define SCREEN_Y_ADJ 1
#define INFO_X_ADJ SCREEN_X_ADJ + WIDTH + 2

int screen[HEIGHT][WIDTH];
int screenCopy[HEIGHT][WIDTH];

int bx, by;
int blockType = 0;
int nextBlockType = 0;
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
int score = 0;
int speed = 100;
int gameLevel = 0;
int last_score = 0;
int best_score = 0;
bool crushFlag = false;
bool spaceFlag = false;


void CursorView();
void gotoxy(short, short);
void reset();
void draw();
void drawInfo();
void newBlock();
void downBlock();
bool crush();
void keyCheck();
void blockMove(int);
bool canMove(int, int, int);
void lineClear();
void speedUp(int);
void checkGameOver();
void gameOver();
void drawBlockLine(int, int, int);
void pause();


int main() {

	srand(time(0));
	CursorView();
	reset();

	while (true) {
		for (int i = 0; i < 5; i++) {
			keyCheck();
			draw();
			Sleep(speed);
			if (spaceFlag) {
				spaceFlag = false;
				break;
			}
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
	FILE* file;
	fopen_s(&file, "score.dat", "rt");
	if (file == 0) best_score = 0;
	else {
		fscanf_s(file, "%d", &best_score);
		fclose(file);
	}
	

	key = 0;
	crushFlag = false;
	score = 0;
	speed = 100;
	nextBlockType = rand() % 7;

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
	drawInfo();
	newBlock();
}

void draw() {
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			if (screen[i][j] != screenCopy[i][j]) {
				gotoxy(SCREEN_X_ADJ + j, SCREEN_Y_ADJ + i);
				switch (screen[i][j]) {
				case EMPTY:
					printf("  ");
					break;
				case WALL:
					printf("¢Ì");
					break;
				case ACTIVE_BLOCK:
					printf("¡à");
					break;
				case STOP_BLOCK:
					printf("¡á");
					break;
				}
			}
		}
	}
}


void drawInfo() {
	int y = SCREEN_Y_ADJ + 5;
	
	gotoxy(INFO_X_ADJ, y + 1); printf("+-- N E X T --+");
	gotoxy(INFO_X_ADJ, y + 2); printf("|             |");
	gotoxy(INFO_X_ADJ, y + 3); printf("|             |");
	gotoxy(INFO_X_ADJ, y + 4); printf("|             |");
	gotoxy(INFO_X_ADJ, y + 5); printf("|             |");
	gotoxy(INFO_X_ADJ, y + 6); printf("+-------------+");

	gotoxy(INFO_X_ADJ, y + 8); printf(" YOUR SCORE :");
	gotoxy(INFO_X_ADJ, y + 9); printf("        %6d", score);
	gotoxy(INFO_X_ADJ, y + 10); printf(" LAST SCORE :");
	gotoxy(INFO_X_ADJ, y + 11); printf("        %6d", last_score);
	gotoxy(INFO_X_ADJ, y + 12); printf(" BEST SCORE :");
	gotoxy(INFO_X_ADJ, y + 13); printf("        %6d", best_score);
	gotoxy(INFO_X_ADJ, y + 15); printf("  ¡â   : Shift        SPACE : Hard Drop");
	gotoxy(INFO_X_ADJ, y + 16); printf("¢·  ¢¹ : Left / Right   P   : Pause");
	gotoxy(INFO_X_ADJ, y + 17); printf("  ¡ä   : Soft Drop     ESC  : Quit");

}


void newBlock() {
	bx = (WIDTH / 2) - 1;
	by = 0;
	blockType = nextBlockType;
	crushFlag = false;
	nextBlockType = rand() % 7;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (blocks[blockType][spin][i][j] == 1)
				screen[by + i][bx + j] = ACTIVE_BLOCK;
		}
	}

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 4; j++) {
			gotoxy(INFO_X_ADJ + 2 + j, 9 + i);
			if (blocks[nextBlockType][0][i][j] == 1)
				printf("¡à");
			else
				printf("  ");
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
				else if (canMove(bx + 1, by, (spin + 1) % 4)) {
					blockMove(RIGHT);
					blockMove(UP);
				}
				else if (canMove(bx - 1, by, (spin + 1) % 4)) {
					blockMove(LEFT);
					blockMove(UP);
				}
				else if (blockType == 0 && canMove(bx - 2, by, (spin + 1) % 4) && bx == 8) {
					blockMove(LEFT);
					blockMove(LEFT);
					blockMove(UP);
				}
				break;
			case DOWN:
				if (canMove(bx, by + 1, spin))
					blockMove(DOWN);
				break;
			}
		}
		else {
			int cnt = 0;
			switch (key) {
			case SPACE:
				spaceFlag = true;
				
				while (!crush()) {
					downBlock();
					cnt++;
				}
				score += cnt / 3 * (gameLevel / 2 + 1);
				gotoxy(INFO_X_ADJ, SCREEN_Y_ADJ + 14); printf("        %6d", score);
				break;
			case PAUSE1:
			case PAUSE2:
				pause();
				break;
			case ESC:
				system("cls");
				exit(0);
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
	int combo = 0;
	bool clear = true;
	for (int i = HEIGHT - 2; i >= 0; i--) {
		for (int j = 1; j < WIDTH - 1; j++) {
			if (screen[i][j] != STOP_BLOCK) {
				clear = false;
				break;
			}
		}
		if (clear) {
			combo++;
			for (int k = i; k > 0; k--) {
				for (int l = 1; l < WIDTH - 1; l++) {
					screen[k][l] = screen[k - 1][l];
				}
			}
			i++;
			score += 100 * (gameLevel * 2 + 1);
			gotoxy(INFO_X_ADJ, SCREEN_Y_ADJ + 14); printf("        %6d", score);
			if (score >= 20000) speedUp(6);
			else if (score >= 15000) speedUp(5);
			else if (score >= 10000) speedUp(4);
			else if (score >= 5000) speedUp(3);
			else if (score >= 2000) speedUp(2);
			else if (score >= 1000) speedUp(1);
		}
		clear = true;
	}
	if (combo > 1) {
		gotoxy(SCREEN_X_ADJ + (WIDTH / 2) - 1, SCREEN_Y_ADJ + by - 2); printf("%d COMBO!", combo);
		Sleep(500);
		score += (combo * 100 * (gameLevel + 1));
	}
}


void speedUp(int level) {
	switch (level) {
	case 1:
		speed = 50;
		break;
	case 2:
		speed = 30;
		break;
	case 3:
		speed = 20;
		break;
	case 4:
		speed = 10;
		break;
	case 5:
		speed = 8;
		break;
	case 6:
		speed = 5;
		break;
	}
	gameLevel = level;
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
	int x = 5;
	int y = 5;
	drawBlockLine(x, y, 17);
	gotoxy(x, y + 1); printf("¢Ç                               ¢Ç");
	gotoxy(x, y + 2); printf("¢Ç   +-----------------------+   ¢Ç");
	gotoxy(x, y + 3); printf("¢Ç   |  G A M E  O V E R..   |   ¢Ç");
	gotoxy(x, y + 4); printf("¢Ç   +-----------------------+   ¢Ç");
	gotoxy(x, y + 5); printf("¢Ç    YOUR SCORE: %6d         ¢Ç", score);
	gotoxy(x, y + 6); printf("¢Ç                               ¢Ç");
	gotoxy(x, y + 7); printf("¢Ç   Press any key to restart..  ¢Ç");
	gotoxy(x, y + 8); printf("¢Ç                               ¢Ç");
	drawBlockLine(x, y + 9, 17);
	last_score = score;

	if (score > best_score) {
		FILE* file;
		fopen_s(&file, "score.dat", "wt");

		gotoxy(x, y + 6); printf("¢Ç   ¡Ú ¡Ú ¡Ú  BEST SCORE! ¡Ú ¡Ú ¡Ú    ¢Ç ");

		if (file == 0) {
			gotoxy(0, 0);
			printf("FILE ERROR: SYSTEM CANNOT WRITE BEST SCORE ON \"SCORE.DAT\"");
		}
		else {
			fprintf(file, "%d", score);
			fclose(file);
		}
	}
	Sleep(1000);
	while (_kbhit()) key = _getch();
	key = _getch();
}

void drawBlockLine(int start_x, int start_y, int count) {
	for (int i = 0; i < count; i++) {
		gotoxy(start_x + i, start_y);
		printf("¢Ç  ");
	}
	
}


void pause() {

}