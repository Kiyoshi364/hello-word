#include <stdio.h>
#include <pthread.h>

typedef unsigned char byte;
#define rbyte register byte

// Addresses
#define WRITE_RAMPTR 		0x00000001
#define READ_RAMPTR 		0x00000002
#define WRITE_RAM 		0x00000004
#define READ_RAM 		0x00000008
#define WRITE_PROGPTR 		0x00000010
#define READ_PROGPTR 		0x00000020
#define INC_PROGPTR 		0x00000040
#define HALT_FLAG 		0x00000080

#define WRITE_REGA 		0x00000100
#define READ_REGA 		0x00000200
#define WRITE_REGB 		0x00000400
#define READ_REGB 		0x00000800
#define WRITE_REGC 		0x00001000
#define READ_REGC 		0x00002000
#define SUBTRACT_FLAG 		0x00004000
#define READ_SUM 		0x00008000

#define WRITE_INSTRUCT		0x00010000
#define READ_INSTRUCT 		0x00020000
#define WRITE_ARGS 		0x00040000
#define READ_ARGS 		0x00080000
#define INC_BUS 		0x00100000
#define DEC_BUS 		0x00200000
//#define ADD 		0x00800000

#define INC_STACK 		0x01000000
#define DEC_STACK 		0x02000000
#define ADD 		0x04000000
#define ADD 		0x08000000
/* unused
#define ADD 		0x10000000
#define ADD 		0x20000000
#define ADD 		0x40000000
#define ADD 		0x80000000
*/

// Flags
#define CARRY			0x01
#define ZERO			0x02
//#define FLAG		0x04
//#define FLAG		0x08

#define OVERFLOW		0x10
#define UNDERFLOW		0x20
//#define FLAG		0x40
//#define FLAG		0x80

byte myclock;
byte flags;
byte bus;
unsigned int add;

void loadRAM(char *filename);
unsigned int intructionList(byte intruction, byte step);

void cpu(byte rw);
void ram(byte rw);
void progCounter(byte rw);
void alu(byte rw);
void addOne(byte rw);

void all(byte rw) {
	ram(rw);
	progCounter(rw);
	alu(rw);
	cpu(rw);
	addOne(rw);
}

void cycle() {
	myclock = 1;
	all(0);
	all(1);
	myclock = 0;
	cpu(0);
}

void reset() {
	bus = 0; add = ~0;
	all(1);
	for (rbyte i = 0; i < 0xFF; i++) {
		bus = i+1; add = WRITE_RAMPTR;
		all(1);
		bus = 0; add = WRITE_RAM;
	}
	add = 0; bus = 0;
}

int main() {
	reset();
	loadRAM("");

	myclock = 1;
//for (rbyte i = 0; i < 2; i++)
//for (rbyte j = 0; j < 17; j++) {
byte i = 128, j = 128;
	add = WRITE_REGA;
	bus = i;
	cycle();
	add = WRITE_REGB;
	bus = j;
	cycle();
	add = READ_SUM | SUBTRACT_FLAG;
	cycle();

	printf("%02d - %02d = %02d <|> o: %d, z: %d\n", i, j, bus, flags&1, (flags&2)>>1);
//}

	return 0;
}

void cpu(byte rw) {
	static byte instruction = 0, args = 0;
	if (myclock) {
		if (rw && add&WRITE_INSTRUCT) {
			instruction = bus;
		}
		if (!rw && add&READ_INSTRUCT) {
			bus = instruction;
		}
		if (rw && add&WRITE_ARGS) {
			args = bus;
		}
		if (!rw && add&READ_ARGS) {
			bus = args;
		}
	} else {
		static byte step = 0;
		add = instructionList(instruction, step++);
	}
}

void ram(byte rw) {
	static byte ram[0xFF], ramptr = 0;
	if (myclock) {
		if (rw && add&WRITE_RAMPTR) {
			ramptr = bus;
		}
		if (!rw && add&READ_RAMPTR) {
			bus = ramptr;
		}
		if (rw && add&WRITE_RAM) {
			ram[ramptr] = bus;
		}
		if (!rw && add&READ_RAM) {
			bus = ram[ramptr];
		}
	}
}

void progCounter(byte rw) {
	static byte progptr = 0;
	if (myclock) {
		if (rw && add&WRITE_PROGPTR) {
			progptr = bus;
		}
		if (!rw && add&READ_PROGPTR) {
			bus = progptr;
		}
		if (!rw && add&INC_PROGPTR) {
			progptr++;
		}
	}
}

void alu(byte rw) {
	static byte ra = 0, rb = 0, rc = 0;
	if (myclock) {
		if (rw && add&WRITE_REGA) {
			ra =  bus;
		}
		if (!rw && add&READ_REGA) {
			bus = ra;
		}
		if (rw && add&WRITE_REGB) {
			rb = bus;
		}
		if (!rw && add&READ_REGB) {
			bus = rb;
		}
		if (rw && add&WRITE_REGC) {
			rc = bus;
		}
		if (!rw && add&READ_REGC) {
			bus = rc;
		}
		if (!rw && add&READ_SUM) {
			rbyte a = ra, b = (add&SUBTRACT_FLAG?~rb:rb),
			      s = 0, c = (add&SUBTRACT_FLAG?1:0);
			rbyte overflow = 0;
			do {
				s = a ^ b ^ c;
				c = a & b | a & c | b & c;
				overflow |= c;
				a = s;
				b = c<<1;
				c = 0;
			} while (b);
			rbyte zero = !s;
			overflow = (add&SUBTRACT_FLAG?!!(overflow&0x80):!!(overflow&0x80));

			flags = overflow | zero<<1;
			bus = s;
		}
	}
}

void addOne(byte rw) {
	if (myclock) {
		if (!rw && add&INC_BUS) {
			bus++;
		}
		if (!rw && add&DEC_BUS) {
			bus--;
		}
	}

void loadRAM(char* filename) {
	byte commandlen = COMMAND_COUNT;
	char *commands[] = COMMANDS;
}

unsigned int intructionList(byte intruction, byte step) {
	if (step < 4) {
		switch(step) {
			case 0:
				return READ_PROGPTR | WRITE_RAMPTR;
				break;
			case 1:
				return READ_RAM | WRITE_INSTRUCT;
				break;
			case 2:
				return READ_PROGPTR | WRITE_RAMPTR;
				break;
			case 3:
				return READ_RAM | WRITE_ARGS;
				break;
		}
	}
	switch(instruction) {
		default:
	}
	return 0;
}
//  0,  1,  2,  3 ||  4,  5,  6,   7 ||   8
// 01, 02, 04, 08 || 16, 32, 64, 128 || 256
// 01, 02, 04, 08 || 10, 20, 40,  80 || 100
