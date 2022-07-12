#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "main.h"

static inline size_t reader_offset(Reader *r) {
	return r->offset;
}

static inline bool getbytes(Reader *r, ut8 *buf, size_t len) {
	if (fread (buf, 1, len, r->fp) != len) {
		return false;
	}
	r->offset += len;
	return true;
}

static inline bool read_byte_as_int(Reader *r, int *out) {
	char byte;
	if (!getbytes (r, &byte, 1)) {
		return false;
	}
	*out = byte;
	return true;
}

static inline Py_ssize_t get_size(Reader *r, int l) {
	if (l > sizeof (size_t) || l <= 0) {
		return -1;
	}

	Py_ssize_t ret = 0;
	while (l--) {
		char c;
		if (!getbytes (r, &c, 1)) {
			return -1;
		}
		ret << 8;
		ret |= c;
	}
	return ret;
}

static inline bool read_sizet(Reader *r, Py_ssize_t *out) {
	char byte;
	if (!getbytes (r, &byte, 1)) {
		return false;
	}
	*out = byte;
	return true;
}

bool load_frame(Reader *r, size_t offset) {
	Py_ssize_t len;
	if (!getbytes (r, (char *)&len, sizeof (len))) {
		return false;
	}
	printf ("[0x%03lx] FRAME 0x%lx\n", offset, len);
	return true;
}

static char *get_uni_str(Reader *r, int int_size) {
	Py_ssize_t len = get_size (r, int_size);
	if (len > 0) {
		char *ret = malloc (len + 2);
		if (ret && getbytes (r, ret, len)) {
			return ret; 
		}
		free (ret);
	}
	return NULL;
}

static int load_counted_binunicode(Reader *r, size_t offset, int size, const char *n) {
	char *uni = get_uni_str (r, size);
	if (uni) {
		printf ("[0x%03lx] %s %s\n", offset, n, uni);
		free (uni);
		return true;
	}
	return false;
}

static inline bool handle_int(Reader *r, size_t offset, int size, const char *n) {
	Py_ssize_t num = get_size (r, size); // TODO: this is wrong function prbly
	if (num > 0) {
		printf ("[0x%03lx] %s 0x%x\n", offset, n, num);
		return true;
	}
	printf ("[0x%3lx] Failed to parse %s at \n", offset, n);
	return false;
}

#define trivial_op(x) { \
	printf ("[0x%03lx] " #x "\n", start); \
	return true; \
}

#define unhandled(x) { \
	printf ("[0x%03lx] UNHANDLED -> " #x "\n", start); \
	return true; \
}
static bool process_next_op(Reader *r) {
	size_t start = reader_offset (r);
	char op;
	if (!getbytes (r, &op, 1)) {
		return false;
	}

	int num;
	switch (op) {
	case MARK:
		trivial_op (MARK);
	case STOP: unhandled(STOP);
	case POP: unhandled(POP);
	case POP_MARK: unhandled(POP_MARK);
	case DUP: unhandled(DUP);
	case FLOAT: unhandled(FLOAT);
	case INT: unhandled(INT);
	case BININT: unhandled(BININT);
	case BININT1: 
		if (handle_int (r, start, 1, "BININT1")) {
			return true;
		}
		break;
	case LONG: unhandled(LONG);
	case BININT2: unhandled(BININT2);
	case NONE: unhandled(NONE);
	case PERSID: unhandled(PERSID);
	case BINPERSID: unhandled(BINPERSID);
	case REDUCE: unhandled(REDUCE);
	case STRING: unhandled(STRING);
	case BINSTRING: unhandled(BINSTRING);
	case SHORT_BINSTRING: unhandled(SHORT_BINSTRING);
	case UNICODE: unhandled(UNICODE);
	case BINUNICODE: unhandled(BINUNICODE);
	case APPEND: unhandled(APPEND);
	case BUILD: unhandled(BUILD);
	case GLOBAL: unhandled(GLOBAL);
	case DICT: unhandled(DICT);
	case EMPTY_DICT:
		trivial_op (EMPTY_DICT);
	case APPENDS: unhandled(APPENDS);
	case GET: unhandled(GET);
	case BINGET: unhandled(BINGET);
	case INST: unhandled(INST);
	case LONG_BINGET: unhandled(LONG_BINGET);
	case LIST: unhandled(LIST);
	case EMPTY_LIST: 
		trivial_op (EMPTY_LIST);
	case OBJ: unhandled(OBJ);
	case PUT: unhandled(PUT);
	case BINPUT: unhandled(BINPUT);
	case LONG_BINPUT: unhandled(LONG_BINPUT);
	case SETITEM: unhandled(SETITEM);
	case TUPLE: unhandled(TUPLE);
	case EMPTY_TUPLE: unhandled(EMPTY_TUPLE);
	case SETITEMS: unhandled(SETITEMS);
	case BINFLOAT: unhandled(BINFLOAT);
	case PROTO:
		if (read_byte_as_int (r, &num)) {
			printf ("[0x%03lx] PROTO %d\n", start, num);
			return true;
		}
		break;
	case NEWOBJ: unhandled(NEWOBJ);
	case EXT1: unhandled(EXT1);
	case EXT2: unhandled(EXT2);
	case EXT4: unhandled(EXT4);
	case TUPLE1: unhandled(TUPLE1);
	case TUPLE2: unhandled(TUPLE2);
	case TUPLE3: unhandled(TUPLE3);
	case NEWTRUE: unhandled(NEWTRUE);
	case NEWFALSE: unhandled(NEWFALSE);
	case LONG1: unhandled(LONG1);
	case LONG4: unhandled(LONG4);
	case BINBYTES: unhandled(BINBYTES);
	case SHORT_BINBYTES: unhandled(SHORT_BINBYTES);
	case SHORT_BINUNICODE:
		if (load_counted_binunicode (r, start, 1, "SHORT_BINUNICODE")) {
			return true;
		}
		break;
	case BINUNICODE8: unhandled(BINUNICODE8);
	case BINBYTES8: unhandled(BINBYTES8);
	case EMPTY_SET: unhandled(EMPTY_SET);
	case ADDITEMS: unhandled(ADDITEMS);
	case FROZENSET: unhandled(FROZENSET);
	case NEWOBJ_EX: unhandled(NEWOBJ_EX);
	case STACK_GLOBAL:
		trivial_op (STACK_GLOBAL);
	case MEMOIZE:
		trivial_op (MEMOIZE);
	case FRAME:
		if (load_frame (r, start)) {
			return true;
		}
		break;
	case BYTEARRAY8: unhandled(BYTEARRAY8);
	case NEXT_BUFFER: unhandled(NEXT_BUFFER);
	case READONLY_BUFFER: unhandled(READONLY_BUFFER);
	}

	fprintf (stderr, "Unkown op 0x%02x at 0x%lx\n", op & 0xff, start);
	return false;
}

int main(int argc, char *argv[]) {
	FILE *fp = NULL;
	if (argc == 2) {
		fp = fopen (argv[1], "r");
	} else if (!isatty (fileno (stdin))) {
		fp = stdin;
	}
	if (!fp) {
		fprintf(stderr, "can't read file\n");
		return -1;
	}

	Reader r = {0};
	r.fp = fp;
	while (process_next_op (&r)) {}
	return 0;
}