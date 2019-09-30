#ifndef STACK
#define STACK
typedef struct _stack {
	char *pos, *num, len, size;
} stack;

stack* newStack() {
	stack *s = malloc(sizeof(*s));
	s->len = 5;
	s->size = 0;
	s->pos = malloc(sizeof(s->pos)*s->len);
	s->num = malloc(sizeof(s->num)*s->len);
	return s;
}
void expand(stack *s) {
	s->len += 5;
	s->pos = realloc(s->pos, sizeof(s->pos)*s->len);
	s->num = realloc(s->pos, sizeof(s->num)*s->len);
}
void push(stack *s, char pos, char num) {
	if (s->size+1 == s->len) expand(s);
	*(s->pos + s->size) = pos;
	*(s->num + s->size++) = num;
}
char* pop(stack *s) {
	if (s->size <= 0) return (char *) 0;
	s->size--;
	char *data = malloc(sizeof(*data)*2);
	*data = *(s->pos + s->size);
	*(data + 1) = *(s->num + s->size);
	return data;
}
void unStack(stack *s) {
	free(s->pos);
	free(s->num);
	free(s);
}
#endif

