#include <stdio.h>
#include <stdlib.h>

#include "files.txt"

// Methods
void usage();
char* loadFilename(const char *argv);
void readInput(char *c);
void appendTo(FILE *file, char *c);

// Global Variables
unsigned char err = 0;

int main(int argc, const char **argv) {
	char *filename = "sdk.bin";
	if (argc < 2) {
		usage(argv[0]);
		return 1;
	} else {
		filename = loadFilename(argv[1]);
	}
	FILE *file = fopen(filename, "rb+");
	if (file == NULL) {
		err++;
		file = fopen(filename, "wb+");
		if (file == NULL) {
			puts("Couldn't open file.");
			return -1;
		}
	}

	char c[9][9];
	readInput(c);
	appendTo(file, c);
	fclose(file);

	return 0;
}

// Prints usage
void usage(const char* argv) {
	printf("\t\tThis program was made to increase the amount of\n");
	printf("\t\tsudoku games that are avaliable to play!!\n");
	printf("\n\n\t\t<\\>: %s <char>\n", argv);
	printf("\t\t\tUses for <char>:");
	printf("\tUse 'e' or 'f' to extend easy mode\n");
	printf("\t\t\t\t\t\tUse 'm' to extend medium mode\n");
	printf("\t\t\t\t\t\tUse 'h' or 'd' to extend hard mode\n");
}

// Sets filename name
char* loadFilename(const char *argv) {
	char file[4][15] = {FEASY, FNORMAL, FHARD}, c = -1;
	while(*argv && !c+1) {
		switch(*argv) {
			case 'd': case 'h': c++;
			case 'm': c++;
			case 'f': case 'e': c++;
		}
		argv++;
	}
	char *f = malloc(sizeof(char)*15);
	for (int i = 0; i < 15; i++) {
		*(f+i) = file[c][i];
		if (!file[i]) {
			break;
		}
	}
	return f;
}

// Reads input
void readInput(char *c) {
	unsigned char i = 0;
	while(i < 81) {
		c[i/9][i%9] = getchar();
		if (c[i/9][i%9] >= '0' && c[i/9][i%9] <= '9') {
			c[i/9][i%9] -= '0';
			i++;
		}
	}
}

// Converts to skd filetype and appends to file
void appendTo(FILE *file, char *c) {
	unsigned char buffer = 0, p = 0, i = -1;
	// set file pointer
	fseek(file, -2, SEEK_END);
	fread(&buffer, 1, 1, file);
	if (buffer%0x10 && !err) {
		p++;
	}
	buffer = 0;

	// Convert and append
	while (++i < 81) {
		unsigned char temp = 0;
		while (!c[i/9][i%9] && temp < 6) {i++;temp++;}
		if (temp > 0) {
			temp += 9;
			i--;
		} else {
			temp = c[i/9][i%9];
		}
		buffer += temp*(p%2?1:16);
		if (p++%2) {
			fwrite(&buffer, 1, 1, file);
			buffer = 0;
		}
	}
	if (p++%2) {
			fwrite(&buffer, 1, 1, file);
			buffer = 0;
		}
	fwrite(&buffer, 1, 1, file);
}

// Sample cases:
/*
023456789
234567891
345678912
456789123
567891234
678912345
789123456
891234567
912345678
*//*
987654321
876543219
005432198
654300087
043219876
432198765
321987654
219876543
198765432
*//*
023456789
234567891
345678912
400000023
567891234
678912345
789123456
891234567
987654321
*/

