#include <stdio.h>
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
int next(FILE *file, int times);
void initFixed();

// Sudoku Methods
void run(char difficulty);
void print();
int isFixed(int pos);
int* input();
void update();
int verifyPos(int *a);
int verifyTable(int diagonal);
int gameEnd();

// Global Variables
int table[9][9], *fixed, fixed_len, hili;
stack *undo, *redo;

int main() {
	char dif;
	printf(CHOOSE_DIF_MSG);
	scanf("%hhd", &dif);
	run(dif);
	return 0;
}

// Loads and runs the sudoku game
void run(char difficulty) {
	startSudoku(difficulty);
	initFixed();
	while (!gameEnd()) {
		print();
		update();
	}
	hili = 16;
	print();
	printf(WIN_MSG);
}

// Loads sudoku game
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

// Generates a random int between 0 and 4096
unsigned int randInt() {
	time_t t;
	srand((unsigned) time(&t));
	return (unsigned int) rand() % 0x1000;
}

// Skips to the next sudoku table
int next(FILE *file, int times) {
	unsigned char buffer = 0, a = 1, i = 0;
	while (times) {
		if(a++%2) {
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
			if(!a%2) {
				fseek(file, -1, SEEK_CUR);
			}
			times--;
			int now = ftell(file);
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
	unsigned char buffer = 0, temp = 1, a = 1, i = 0;
		fread(&buffer, 1, 1, file);
		if (!(buffer>>4)) {
			a++;
		} else if (!(buffer%0x10)) {
			buffer = 0;
		} else {
			fseek(file, -1, SEEK_CUR);
		}
	while (temp) {
		if(a++%2) {
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
	system(CLS);
	printf("\n\t\t  ");
	for (int i = 1; i <=9; i++) {
		printf("  %d  ", i);
		if (!(i%3))
			printf("   ");
	}
	printf(UP_LINE);
	for (int i = 0; i<9; i++) {
		printf(RIGHT_COL, i+1);
		for (int j = 0; j<9; j++) {
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
	for (int i = 0; i < 81; i++) {
		if (table[i/9][i%9]) fixed_len++;
	}
	int j = 0;
	fixed = malloc(sizeof(*fixed)*fixed_len);
	for (int i = 0; i < 81 && j < fixed_len; i++) {
		if (table[i/9][i%9]) *(fixed + j++) = i;
	}
}

// Checks if pos is inside the array fixed: returns 1 if true, 0 otherwise
int isFixed(int pos) {
    for (int i = 0; i < fixed_len; i++) {
        if (fixed[i] == pos)
            return 1;
        if (fixed[i] > pos)
            break;
    }
    return 0;
}

// Asks input from the user; Returns a pointer with x and y positions
// and the number in the following order {x, y, number}
int* input() {
	int *a = malloc(sizeof(a)*3), i = 0;
	printf(INPUT_MSG); // x, y, n
	while (i < 3) {
		int c = getchar();
		if (!i && c == 'u' && undo->size > 0) {
			*(a) = 10;
			*(a+1) = 10;
			*(a+2) = 10;
			i = 3;
		} else if (!i && c == 'r' && redo->size > 0) {
			*(a) = 10;
			*(a+1) = 10;
			*(a+2) = 9;
			i = 3;
		} else if (!i && c == 'f') {
			*(a) = 10;
			*(a+1) = 10;
			*(a+2) = 8;
			i = 3;
		} else if (!i && c == 'h') {
			*(a) = 10;
			*(a+1) = 9;
			i = 2;
		} else if (c >= '0' && c <= '9') {
			*(a + i++) = c - '0';
		}
	}
	int c = getchar();
	while (c != '\n') c = getchar();
	return a;
}

// Updates the table with x, y and number received
void update() {
    int *a = input(), pos = -1;
	if (*a == 10) { // Macros
		if (*(a+1) == 10 && *(a+2) == 10) { // Undo
			char *b = pop(undo);
			*a = *b%9+1;
			*(a+1) = *b/9+1;
			*(a+2) = *(b+1);
			push(redo, *b, table[*(a+1)-1][*a-1]);
		} else if (*(a+1) == 10 && *(a+2) == 9) { // Redo
			char *b = pop(redo);
			*a = *b%9+1;
			*(a+1) = *b/9+1;
			*(a+2) = *(b+1);
			pos = (*a)-1/*x-1*/ + (*(a+1)-1)/*y-1*/*9;
		} else if (*(a+1) == 10 && *(a+2) == 8) { // Toggle Fixed
			hili += (hili>>4)?-16:16; // 1<<4 = 16
			*(a+2) = 10;
		} else if (*(a+1) == 9) { // Highlight
			hili += *(a+2) - (hili&15);
			*(a+2) = 10;
		}
	} else {
		pos = (*a)-1 + (*(a+1)-1)*9; // pos = (x-1)+(y-1)*9;
		unStack(redo);
		redo = newStack();
	}
	for (int i = 0; i < fixed_len; i++) {
		if (*(fixed+i) == pos) {
			printf(FIXED_POS_MSG);
			int c = getchar();
			while (c != '\n') c = getchar();
			return;
		}
		if (*(fixed+i) > pos) {
			break;
		}
	}
	if (*(a+2) == 10); 
	else {
		if (pos+1) {
			push(undo, pos, table[*(a+1)-1][*a-1]);
		}
		if (*(a+2) == 0)
			table[*(a+1)-1][*a-1] = *(a+2);
		else if (verifyPos(a))
			table[*(a+1)-1][*a-1] = *(a+2);
	}
	free(a);
}

// Verify if position and number are able to be placed in the table
int verifyPos(int *a) {
	int flag = 1;
	for (int i = 0; i < 9; i++) {
		if (table[*(a+1)-1][i] == *(a+2)) {
			flag = 0;
			printf(SAME_POS_MSG, i+1, *(a+1));
		}
		if (table[i][*a-1] == *(a+2)) {
			flag = 0;
			printf(SAME_POS_MSG, *a, i+1);
		}
		if (table[(*(a+1)-1)/3*3+i/3][(*a-1)/3*3+(i%3)] == *(a+2)) {
			flag = 0;
			printf(SAME_POS_MSG, (*a-1)/3*3+(i%3) +1, (*(a+1)-1)/3*3+i/3 +1);
			}
	}

	if (!flag) {
		int c = getchar();
		while (c != '\n') c = getchar();
	}
	return flag;
}

// Verify if all positions have a number different from 0 in it, return 1 if true, 0 otherwise
int gameEnd() {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (!table[i][j]) return 0;
        }
    }
    return 1;
}
