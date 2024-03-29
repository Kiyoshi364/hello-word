﻿#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Importing string constants and a stack library
#include "tableLayouts/table.txt"
#include "language/portugues_acento.txt"
#include "files.txt"

#include "myStack.c"

// Loading Methods
void startSudoku(char difficulty);
void loadSudoku(FILE *file);
unsigned int randInt();
char next(FILE *file, char times);
void initFixed();

// Sudoku Methods
void run();
void print();
char isFixed(char pos);
char* input();
void update();
char verifyPos(char *pack);
char gameEnd();

// Global Variables
char table[9][9],	// Stores the sudoku table
    *fixed,		// Stores the inicial positions (can't be alterated)
    fixed_len,		// Stores the length of the 'fixed' array
    hili;		// Stores some variables related to printing the table

stack *undo,		// A stack that stores the moves to be undone
      *redo;		// A stack that stores the moves to be redone

int main() {
	run();
	return 0;
}

// Asks for the difficulty (1, 2, 3), loads and runs the sudoku game
void run() {
	char difficulty;
	printf(CHOOSE_DIF_MSG);
	scanf("%hhd", &difficulty);

	startSudoku(difficulty);
	initFixed();

	while (!gameEnd()) {
		print();
		update();
	}

	hili = 16; // Sets the printing state
	print();
	printf(WIN_MSG);
}

// Sets the variables needed to load a random sudoku game
void startSudoku(char difficulty) {
	FILE *file;
	switch(difficulty) {
		case 1:
			file = fopen(FEASY, "rb");
			break;
		case 2:
			file = fopen(FNORMAL, "rb");
			break;
		case 3: default:
			file = fopen(FHARD, "rb");
			break;
	}
	if (!file) {
		printf(OPENFILEERROR_MSG);
		exit(-1);
	}

	next(file, randInt());
	loadSudoku(file);
	fclose(file);

	hili = STD_HILI;
	undo = newStack();
	redo = newStack();
}

// Generates a random unsigned int between 0 and 4096
unsigned int randInt() {
	time_t t;
	srand((unsigned int) time(&t));
	return (unsigned int) rand() % 0x1000;
}

// Skips to the next sudoku table 'times' times
char next(FILE *file, char times) {
	unsigned char buffer = 0, twoStateCounter = 1;
	while (times) {
		if(twoStateCounter++&1) {
			fread(&buffer, sizeof(buffer), 1, file);
			if (!buffer) {
				fseek(file, 0, SEEK_SET);
				return next(file, times);
			}
		} else {
			buffer <<= 4;
		}
		unsigned char temp = buffer>>4;
		if (!temp) {
			times--;
			long int now = ftell(file);
			fseek(file, 0, SEEK_END);
			if (now+1 == ftell(file)) {
				fseek(file, 0, SEEK_SET);
				return next(file, times);
			}
			fseek(file, now, SEEK_SET);
		}
	}
	return 0;
}

// Loads sudoku table
void loadSudoku(FILE *file) {
	fseek(file, -1, SEEK_CUR);
	unsigned char buffer = 0, temp = 1, twoStateCounter = 1, i = 0;

		// Preprocessing to make sure that
		// the twoStateCounter and the buffer are OK
		fread(&buffer, 1, 1, file);
		if (!(buffer>>4)) {
			twoStateCounter++;
		} else if (!(buffer%0x10)) { // 15 = 0x0F = 0000 1111
			buffer = 0;
		} else {
			fseek(file, -1, SEEK_CUR);
		}

	while (temp) {
		if(twoStateCounter++&1) {
			fread(&buffer, sizeof(buffer), 1, file);
		} else {
			buffer <<= 4;
		}
		temp = buffer>>4;
		if (temp < 0) {
			printf(LOADINGERROR_MSG, i, temp);
			exit(-2);
		} else if (temp > 9) {
			temp -= 9;
			while (temp--) {
				table[i/9][i%9] = 0;
				i++;
			}
		} else if (temp > 0 && temp < 10) {
			table[i/9][i%9] = temp;
			i++;
		}
	}
}

// Prints the sudoku table state
void print() {
	system(CLS);
	printf("\n\t\t  ");
	for (register char i = 1; i <=9; i++) {
		printf("  %d  ", i);
		if (!(i%3))
			printf("   ");
	}
	printf(UP_LINE);
	for (register char i = 0; i<9; i++) {
		printf(RIGHT_COL, i+1);
		for (register char j = 0; j<9; j++) {
			if (hili&16 && hili&15 && table[i][j] == (hili&15)) {// if highlight is toggled but fixed is untoggled
				printf(" %c%c%c ", HFL_TOKEN, table[i][j]?table[i][j]+'0':EMPTY, HFR_TOKEN);
			} else if (hili&15 && table[i][j] == (hili&15)) {// if highlight and fixed are toggled
				printf(" %c%c%c ", isFixed(j+9*i)?HFL_TOKEN:HUL_TOKEN, table[i][j]?table[i][j]+'0':EMPTY, isFixed(j+9*i)?HFR_TOKEN:HUR_TOKEN);
			} else if (hili>>4) {// if fixed is untoggled
				printf(" %c%c%c ", UUL_TOKEN, table[i][j]?table[i][j]+'0':EMPTY, UUR_TOKEN);
			} else {
				printf(" %c%c%c ", isFixed(j+9*i)?UFL_TOKEN:UUL_TOKEN, table[i][j]?table[i][j]+'0':EMPTY, isFixed(j+9*i)?UFR_TOKEN:UUR_TOKEN);
			}
			if (!((j+1)%3))
				printf(STD_COL);
		}
		printf(MID_LIN);
		if (!((i+1)%3) && i+1!=9)
		printf(MID_LINE);
	}
	printf(DOWN_LINE);
	putchar(10);
}

// Saves all the positions that contains a number in array table
// stores this info in array fixed
void initFixed() {
	for (register char i = 0; i < 81; i++) {
		if (table[i/9][i%9]) fixed_len++;
	}
	register char j = 0;
	fixed = malloc(sizeof(*fixed)*fixed_len);
	for (register char i = 0; i < 81 && j < fixed_len; i++) {
		if (table[i/9][i%9]) *(fixed + j++) = i;
	}
}

// Checks if pos is inside the array fixed: returns 1 if true, 0 otherwise
char isFixed(char pos) {
    for (register char i = 0; i < fixed_len; i++) {
        if (fixed[i] == pos)
            return 1;
        if (fixed[i] > pos)
            break;
    }
    return 0;
}

// Asks input from the user; Returns a pointer with x and y positions
// and the number in the following order {x, y, number}
char* input() {
	char *pack = malloc(sizeof(*pack)*3), i = 0;
	printf(INPUT_MSG); // x, y, n
	while (i < 3) {
		int c = getchar();
		if (!i && c == 'u' && undo->size > 0) {
			pack[0] = 10;
			pack[1] = 10;
			pack[2] = 10;
			i = 3;
		} else if (!i && c == 'r' && redo->size > 0) {
			pack[0] = 10;
			pack[1] = 10;
			pack[2] = 9;
			i = 3;
		} else if (!i && c == 'f') {
			pack[0] = 10;
			pack[1] = 10;
			pack[2] = 8;
			i = 3;
		} else if (!i && c == 'h') {
			pack[0] = 10;
			pack[1] = 9;
			i = 2;
		} else if (c >= '0' && c <= '9') {
			pack[i++] = c - '0';
		}
	}
	int c = getchar();
	while (c != '\n') c = getchar();
	return pack;
}

// Updates the table with x, y and number received
void update() {
    char *pack = input(), pos = -1;
	if (pack[0] == 10) { // Macros
		if (pack[1] == 10 && pack[2] == 10) { // Undo
			char *b = pop(undo);
			pack[0] = *b%9+1;
			pack[1] = *b/9+1;
			pack[2] = *(b+1);
			push(redo, *b, table[pack[1]-1][pack[0]-1]);
		} else if (pack[1] == 10 && pack[2] == 9) { // Redo
			char *b = pop(redo);
			pack[0] = *b%9+1;
			pack[1] = *b/9+1;
			pack[2] = *(b+1);
			pos = (pack[0])-1/*x-1*/ + (pack[1]-1)/*y-1*/*9;
		} else if (pack[1] == 10 && pack[2] == 8) { // Toggle Fixed On/Off
			hili ^= 16; // 16 = 0x10 = 0000 1000
			pack[2] = 10;
		} else if (pack[1] == 9) { // Highlight
			hili += pack[2] - (hili&15);
			pack[2] = 10;
		}
	} else {
		pos = pack[0]-1 + (pack[1]-1)*9; // pos = (x-1)+(y-1)*9;
		unStack(redo);
		redo = newStack();
	}
	for (register char i = 0; i < fixed_len; i++) {
		if (fixed[i] == pos) {
			printf(FIXED_POS_MSG);
			int c = getchar();
			while (c != '\n') c = getchar();
			return;
		}
		if (fixed[i] > pos) {
			break;
		}
	}
	if (pack[2] == 10); 
	else {
		if (pos+1) { // if pos != -1
			push(undo, pos, table[pack[1]-1][pack[0]-1]);
		}
		if (pack[2] == 0)
			table[pack[1]-1][pack[0]-1] = pack[2];
		else if (verifyPos(pack))
			table[pack[1]-1][pack[0]-1] = pack[2];
	}
	free(pack);
}

// Verify if position and number are able to be placed in the table
char verifyPos(char *pack) {
	char flag = 1;
	for (register char i = 0; i < 9; i++) {
		if (table[pack[1]-1][i] == pack[2]) {
			flag = 0;
			printf(SAME_POS_MSG, i+1, pack[1]);
		}
		if (table[i][pack[0]-1] == pack[2]) {
			flag = 0;
			printf(SAME_POS_MSG, pack[0], i+1);
		}
		if (table [(pack[1]-1) /3*3 + i/3] [pack[1] /3*3 + (i%3)] == pack[2]) {
			flag = 0;
			printf(SAME_POS_MSG, pack[1]/3*3+(i%3) +1, (pack[1]-1)/3*3+i/3 +1);
		}
	}

	if (!flag) {
		int c = getchar();
		while (c != '\n') c = getchar();
	}
	return flag;
}

// Verify if all positions have a number different from 0 in it, return 1 if true, 0 otherwise
char gameEnd() {
    for (register char i = 0; i < 9; i++) {
        for (register char j = 0; j < 9; j++) {
            if (!table[i][j]) return 0;
        }
    }
    return 1;
}
