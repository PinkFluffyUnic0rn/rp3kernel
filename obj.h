#ifndef OBJ_H
#define OBJ_H

#include <stddef.h>

#define OBJ_MAX_BUFFERS 1024

struct obj_buffer {
	struct obj_block *block;
	size_t objsize;
};

struct obj_block {
	struct obj_block *prev;
	struct obj_block *next;
	int freecount;
	void **free;
	void *data;
};

int obj_bufinit();

void *obj_alloc(int bufid);

void obj_destroy(int bufid, void *obj);

#endif
