/**Creator: Daniel K Hashimoto
* 
* The objective of this program is aply simple
* encription methods to any text or file content
* 
* Command Line List:
* --------------------------------------|-----------------------|----------------------------------------
* 	Identificator			|	Complement	|		Function
* --------------------------------------|-----------------------|----------------------------------------
* 					|	[filename]	|	Sets input filename
* 	-o				|	[filename]	|	Sets output filename (*)
* 	-a				|	[filemane]	|	Loads a alphabet (*)
* 	-q				|			|	Silences the comands calls (*)
* 	-Q				|			|	Silences the comands calls and outputs (*)
* 	-s				|	[filename]	|	Loads and runs a script (*)
* 	--help				|			|	Prints the command line list (*)
* 
*
* Command List:
* - CCE: Cesar Cypher Encryption - Encrypts using cesar cypher
* - MCE: Multiplicative Cypher Encryption - Encrypts using multiplicative cypher
* - EXIT: EXIT - Exits without saving
* - HELP: HELP - Prints all commands
* - ICE: Invert Cesar Encryption - Sets the key with the inverse
* 				(for Cesar Cypher)
* - IME: Invert Multiplicative Encryption - Sets the key with the inverse
* 				(for Multiplicative Cypher)
* - LAB: Load AlphaBet - Loads a alphabet onto memory
* - LIF: Load Input Filename - Loads input filename onto memory
* 				and file onto input buffer
* - LOF: Load Output Filename - Loads output filename onto memory
* - PNI: Print Numeric Input - Prints input buffer as a sequence
*				of numbers
* - PNO: Print Numeric Output - Prints output buffer as a sequence
*				of numbers
* - PSI: Print String Input - Prints input buffer as string using
* 				the alphabet
* - PSO: Print String Output - Prints output buffer as string using
* 				the alphabet
* - PTK: PrinT Key - Prints all keys values
* - RSA: RSA encryption - Encrypts using RSA encryption
* - SCP: call SCriPt - Calls a script (a script cannot call another script) (*)
* - STK: SeT Key - Loads a new key onto memory
* - UPB: UPdate Buffer - Loads output buffer onto input buffer
* - WRT: WRiTe - Write output buffer onto output file
* 
* (*) Not yet implemented
*/
#include <stdio.h>
#include <stdlib.h>

typedef unsigned char byte;

// Global Settings
/**HGFE DCBA
* A - Quiet 0/1 Off/On*/
byte settings = 0;

#include "encript.c"
void welcome();
byte strcom(const char* a, const char* b);
void help();
void ext();

void loadAlph();
void loadInput();
void loadOutput();
void setKey();
void write();

// Interface Constants
#define COMMC 18 // Command Count
const char *COMMS[COMMC] = {"CCE", "MCE", "EXIT", "HELP", "ICE", "IME", "LAB", "LIF", "LOF", "PNI", "PNO", "PSI", "PSO", "PTK", "RSA", "STK", "UPB", "WRT"}; // Command String List
const char *COMMF[COMMC] = {"Cesar Cypher Encryption", "Multiplicative Cypher Encryption", "Exit", "Help", "Invert Cesar Encryption", "Invert Multiplicative Encryption", "Load Alphabet", "Load Input Filename", "Load Output Filename", "Print Numeric Input", "Print Numeric Output", "Print String Input", "Print String Output", "Print Key", "RSA encryption", "SeT Key", "Update Buffer", "Write"}; // Command Fullname List
const void (*COMML[COMMC])(void) = {cesar, multi, ext, help, uncesarKey, unmultiKey, loadAlph, loadInput, loadOutput, piNum, poNum, pi, po, pkey, rsa, setKey, update, write}; // Command Function List

int main(const int argc, const char **argv) {
	/*// new setup
{
	if (argc > 1) {

	}
}*/
	// setup
{
	char *in = calloc(50, 1);
	if (argc > 1) {
		register char i = 0;
		for (; *(argv[1] + i) && i < 50; i++) {
			*(in + i) = *(argv[1] + i);
		}
		*(in + i) = 0;
	}
	init(in);
	if (!*in) {
		free(in);
	}
}
	welcome();

	// running
{
	register byte *read = malloc(6), off = 0;
	while (1) {
		byte c = getchar();
		if (c != ' ' && c !='\n') {
			*(read + off) = c;
			off += 1;
		} else {
			byte i = 0;
			for (; i < COMMC; i++) {
				if (strcom(read, COMMS[i])) {
					// do command
					if (!settings&1) printf("Command %s: ", COMMF[i]);
					COMML[i]();
					// reset read
					break;
				}
			}
			for (byte j = 0; j <= off; j++) *(read + j) = 0;
			off = 0;
		}
	}
}

}

//returns 1 if egual, 0 otherwise
byte strcom(const char* a, const char* b) {
	size_t i = 0;
	for (; *(a+i) && *(b+i); i++) {
		// enter if different (caps not considered)
		if (*(a+i)-*(b+i) && *(a+i)-*(b+i)-32) {
			return 0;
		}
	}
	// enter if both ended (are '\0')
	if (*(a+i) || *(b+i)) {
		return 0;
	}
	return 1;
}

void welcome() {
	printf("Welcome to The Encryption Interface!\n\n");
}

void help() {
	printf("\n\nCCE:\tCesar Cypher Encryption\t\tEncripts using cesar cypher\n");
	printf("MCE:\tMultiplicative CypherEncryption\tEncripts using multiplicative cypher\n");
	printf("EXIT:\tEXIT\t\t\t\tExits without saving\n");
	printf("HELP:\tHELP\t\t\t\tPrints all commands\n");
	printf("ICE:\tInvert Cesar Encryption\t\tSets the inverse key (for Cesar Cypher)\n");
	printf("IME:\tInvert MultiplicativeEncryption\tSets the inverse key\n\t\t\t\t\t\t(for Multiplicative Cypher)\n");
	printf("LAB:\tLoad AlphaBet\t\t\tLoads a alphabet onto memory\n");
	printf("LIF:\tLoad Input Filename\t\tLoads input filename onto memory\n\t\t\t\t\t\tand file onto input buffer\n");
	printf("LOF:\tLoad Output Filename\t\tLoads output filename onto memory\n");
	printf("PNI:\tPrint Numeric Input\t\tPrints input buffer as a sequence\n\t\t\t\t\t\tof numbers\n");
	printf("PNO:\tPrint Numeric Output\t\tPrints output buffer as a sequence\n\t\t\t\t\t\tof numbers\n");
	printf("PSI:\tPrint String Input\t\tPrints input buffer as string using\n\t\t\t\t\t\tthe alphabet\n");
	printf("PSO:\tPrint String Output\t\tPrints output buffer as string using\n\t\t\t\t\t\tthe alphabet\n");
	printf("PTK:\tPrinT Key\t\t\t\tPrints all keys values\n");
	printf("RSA:\tRSA encryption\t\t\t\tEncrypts using RSA encryption\n");
	printf("STK:\tSeT Key\t\t\t\tLoads a new key onto memory\n");
	printf("UPB:\tUPdate Buffer\t\t\tLoads output buffer onto input buffer\n");
	printf("WRT:\tWRiTe\t\t\t\tWrite output buffer onto output file\n");
	//printf("\n\t\t(*) Not yet implemented");
	if (!settings&1) putchar(10);
}

void ext() {
	printf("Exiting...\n");
	exit(0);
}

void loadAlph() {
	char *read = malloc(20), off = 0;
	buff *ignored = newBuff();
	// find file
	while(off < 20) {
		register char c = getchar();
		if (c == ' ' || c =='\n') break;
		*(read + off) = c;
		off += 1;
	}
	*(read + off) = 0;

	FILE *file = fopen(read, "r");
	if (file) {
		// load alph
		killBuff(alph);
		alph = newBuff();
		register int c;
		while ((c = fgetc(file)) != EOF) {
			register int i = 0;
			while (i < alph->size) {
				if (*(alph->set + i) == c) break;
				i += 1;
			}
			if (i == alph->size) add(alph, c);
			else {
				printf("\nWarning: Character already inside alphabet. Character %c at index %d was ignored.", c, i);
				add(ignored, c);
			}
		}
		fclose(file);
		if (ignored->size) {
			add(ignored, 0);
			printf("All ignored characters: [%s]", ignored->set);
		}

		// reset inbuffer
		file = fopen(infile, "r");
		if (file) {
			killBuff(inbuffer);
			inbuffer = newBuff();
			while ((c = fgetc(file)) != EOF) {
				register int i = 0;
				while (i < alph->size)
					if (*(alph->set + i++) == c) {i--; break;}
				if (i < alph->size) add(inbuffer, i);
				else printf("\nWarning: Character outside alphabet. Character %c with ASCII code %hu was ignored.", c, c);
			}
			fclose(file);
		} else {
			printf("\nError: Input file not found.");
		}
	} else {
		printf("\nError: Alphabet file not found.");
	}
	if (!settings&1) putchar(10);
}

void loadInput() {
	char *read = malloc(20), off = 0;
	while(off < 20) {
		register char c = getchar();
		if (c != ' ' && c !='\n') {
			*(read + off) = c;
			off += 1;
		} else break;
	}
	*(read + off) = 0;

	FILE *file = fopen(read, "r");
	if (file) {
		// reset inbuffer
		killBuff(inbuffer);
		inbuffer = newBuff();
		register int c;
		while ((c = fgetc(file)) != EOF) {
			register int i = 0;
			while (i < alph->size)
				if (*(alph->set + i++) == c) {i--; break;}
			if (i < alph->size) add(inbuffer, i);
			else printf("\nWarning: Character outside alphabet. Character %c with ASCII code %hu was ignored.", c, c);
		}
		fclose(file);
		infile = read;
	} else {
		printf("\nError: Input file not found.");	
	}
	if (!settings&1) putchar(10);
}

void loadOutput() {
	char *read = malloc(20), off = 0;
	while(off < 20) {
		register char c = getchar();
		if (c != ' ' && c !='\n') {
			*(read + off) = c;
			off += 1;
		} else {
			*(read + off) = 0;
			outfile = read;
			break;
		}
	}
	if (!settings&1) putchar(10);
}

void setKey() {
	register int flag = 1, input = 0;
	killBuff(key);
	key = newBuff();
	while(1) { // Readand convert to numbers
		register char c = getchar();
		if (flag&1) {
			if (c == '-') {flag |= 2;}
			flag &= ~1;
		}
		if (c >= '0' && c <='9') {
			input = input*10 + c - '0';
		}
		else if (c == ' ') {
			add(key, (alph->size + input*(flag&2?-1:1))%alph->size);
			flag = 1;
			input = 0;
		}
		else if (c == '\n') {
			add(key, (alph->size + input*(flag&2?-1:1))%alph->size);
			break;
		}
	}
    if (!settings&1) putchar(10);
}

void write() {
	FILE *file = fopen(outfile, "w");
	if (!file) {
		printf("Couldn't write on \"%s\" file.\n", outfile);
	}
	register unsigned int i = 0;
	while (i<outbuffer->size)
		fputc(*(alph->set + *(outbuffer->set + i++)), file);
	fputc(10, file);
	fclose(file);
	putchar(10);
}

