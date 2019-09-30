/**Creator: Daniel K Hashimoto
* 
* The objective of this program is aply simple
* encription methods to some text or file content
* 
* Avaliables encryption methods:
* - Cesar Cypher [cesar()]
* - Multiplicative Cypher [multi()]
* - 
* 
* Avaliable helper methods:
* - Inverse key for Multi Cesar Cypher [unkey()] (*)
* - Inverse key for Multiplicative Cypher [unmultikey()]
* 
* (*) Not yet implemented
* 
*/
#include <stdio.h>
#include <stdlib.h>

#include "buffer.c"

// Global Constants
char *infile, *outfile;
buff *inbuffer, *outbuffer, *key, *alph;

// Default Constant Values
#define INFILE "new.txt"
#define OUTFILE "out.txt"
#define ALPH "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz 1234567890,.:;?/!@#$%&*()_+=-'\n\""
#define ALPHSIZE 86
#define KEY 1

// Buffers manipulation Methods
void pi();
void piNum();
void po();
void poNum();
void pkey();
void update();

// Encription Methods
void cesar();
void multi();
void rsa();

// Invert Keys Methods
void uncesarKey();
void unmultiKey();

// Other Methods
int modpow(int base, int exp, int mod);
void init();

void pi() {
	if (!settings&1) putchar(10);
	register unsigned int i = 0;
	while (i<inbuffer->size)
		putchar(*(alph->set + *(inbuffer->set + i++)));
	putchar(10);
}

void piNum() {
	if (!settings&1) putchar(10);
	register unsigned int i = 0;
	while (i<inbuffer->size) {
		printf("%u%c", *(inbuffer->set + i), i+1<inbuffer->size?32:10);
		i++;
	}
	putchar(10);
}

void po() {
	if (!settings&1) putchar(10);
	register unsigned int i = 0;
	while (i<outbuffer->size)
		putchar(*(alph->set + *(outbuffer->set + i++)));
	putchar(10);
}

void poNum() {
	if (!settings&1) putchar(10);
	register unsigned int i = 0;
	while (i<outbuffer->size) {
		printf("%u%c", *(outbuffer->set + i), i+1<outbuffer->size?32:10);
		i++;
	}
    putchar(10);
}

void pkey() {
	if (!settings&1) putchar(10);
	register unsigned int i = 0;
	while (i<key->size) {
		printf("%hhd%c", *(key->set + i), i+1<key->size?32:10);
		i++;
	}
    putchar(10);
}


void update() {
	killBuff(inbuffer);
	inbuffer = newBuff();
	register unsigned int i = 0;
	while (i<outbuffer->size) add(inbuffer, *(outbuffer->set + i++));
    if (!settings&1) putchar(10);
}

void cesar() {
	register unsigned int i = 0;
	killBuff(outbuffer);
	outbuffer = newBuff();
	while (i++<inbuffer->size) {
		register int c = *(inbuffer->set + i-1) + *(key->set + (i-1)%key->size);
		while (c < 0) c += alph->size;
		add(outbuffer, c%alph->size);
	}
    if (!settings&1) putchar(10);
}

void multi() {
	register unsigned int i = 0;
	killBuff(outbuffer);
	outbuffer = newBuff();
	while (i++<inbuffer->size) {
		register unsigned c = (*(inbuffer->set + i-1)) * (*(key->set + (i-1)%key->size));
		while (c < 0) c += alph->size;
		add(outbuffer, c%alph->size);
	}
	if (!settings&1) putchar(10);
}

void rsa() {
	register unsigned int i = 0;
	killBuff(outbuffer);
	outbuffer = newBuff();
	while (i++<inbuffer->size) {
		register int c = modpow(*(inbuffer->set + i-1), (*key->set + (i-1)%key->size), alph->size);
		while (c < 0) c += alph->size;
		add(outbuffer, c%alph->size);
	}
	if (!settings&1) putchar(10);
}

void uncesarKey() {
	register int i = 0;
	buff *ukey = newBuff();
	while (i < key->size) {
		add(ukey, alph->size - (*(key->set + i)));
		i++;
	}
	killBuff(key);
	key = ukey;
	if (!settings&1) putchar(10);
}

/** if the greatest common factor between the alphabet size and the key 
* is other than 1, it is not possible to return a good unkey, in other
* words it will not the able to undo the encript
*/
void unmultiKey() { // Euclidean Algorithm
	register int i = 0;
	buff *ukey = newBuff();
	while (i < key->size) {
		register int a = *(key->set), b = alph->size, x1 = 1, x2 = 0, y1 = 0, y2 = 1;
		while (a%b) {
			int q = a/b, r = a%b, xt = x1-x2*q, yt = y1-y2*q;
			q = b/r;
			a = b;
			b = r;
			x1 = x2;
			x2 = xt;
			y1 = y2;
			y2 = yt;
		}
		add(ukey, (x2>0?x2:alph->size+x2));
		i++;
	}
	killBuff(key);
	key = ukey;
	if (!settings&1) putchar(10);
}

int modpow(int base, int exp, int mod) {
	base %= mod;
	int ans = 1;
	while (exp) {
		if(exp&1) {
			ans *= base;
			ans %= mod;
		}
		base = base*base%mod;
		exp >>= 1;
	}
	return ans;
}

void init(char *in) {
	infile = *in ? in : INFILE;
	outfile = OUTFILE;
	inbuffer = newBuff();
    outbuffer = newBuff();
	alph = newBuff();
	register int i = 0;
	while (i < ALPHSIZE) add(alph, ALPH[i++]);
	key = newBuff();
	add(key, KEY);

	// load inbuffer
	FILE *file = fopen(infile, "r");
    if (!file) {
		i = 30;
        while (i-- && (file = fopen(infile, "r+")));
		if (!file) {
			printf("Error: Couldn't open default file.\n");
			exit(-1);
		}
    }
	register int c = 0;
	while ((c = fgetc(file)) != EOF) {
		i = 0;
		while (i < alph->size)
			if (*(alph->set + i++) == c) {i--; break;}
		if (i < alph->size) add(inbuffer, i);
		else printf("\nWarning: Character outside alphabet. Character %c with ASCII code %hu was ignored.", c, c);
	}
	fclose(file);

	if (!settings&1) putchar(10);
}

