#include "obj.h"

#include <stdlib.h>

#define BLOCK_SZ (4 * 1024)

int Lastbuf = 0;
struct obj_buffer Obj_buffer[OBJ_MAX_BUFFERS];

struct obj_block *obj_blockcreate(size_t sz)
{
	void *mem;
	int freesz;
	struct obj_block *new;
	int i;

	mem = malloc(BLOCK_SZ);
	
	freesz = (BLOCK_SZ / sz) * sizeof(void *);

	new = (struct obj_block *) mem;
	new->free = mem + sizeof(struct obj_block);
	new->data = mem + sizeof(struct obj_block) + freesz;
	new->prev = new->next = NULL;
	new->freecount = ((mem + BLOCK_SZ) - new->data) / sz;	

	for (i = 0; i < new->freecount; ++i)
		new->free[i] = new->data + i;

	return new;
}

void obj_blockdestroy(struct obj_block *b)
{
	if (b->prev != NULL)
		b->prev->next = b->next;
	
	if (b->next != NULL)
		b->next->prev = b->prev;

	free(b);
}

int obj_bufinit(size_t sz)
{
	Obj_buffer[Lastbuf].block = NULL;
	Obj_buffer[Lastbuf].objsize = sz;

	return Lastbuf++;
}

void *obj_alloc(int bufid)
{
	struct obj_buffer *buf;
	struct obj_block *block;

	buf = Obj_buffer + bufid;

	block = buf->block;
	while (block != NULL) {
		if (block->freecount != 0)
			break;

		block = block->next;
	}
	
	if (block == NULL) {
		block = obj_blockcreate(buf->objsize);

		if (buf->block != NULL)
			buf->block->prev = block;

		block->next = Obj_buffer[bufid].block;
		Obj_buffer[bufid].block = block;
	}
	
	return block->free[--block->freecount];
}

void obj_destroy(int bufid, void *obj)
{
	struct obj_buffer *buf;
	struct obj_block *block;

	buf = Obj_buffer + bufid;
	
	block = buf->block;
	while (block != NULL) {
		if (obj >= block->data && obj < (block->data + BLOCK_SZ))
			break;
		
		block = block->next;
	}

	if (block == NULL)
		return;

	block->free[block->freecount++] = obj;

	if (block->freecount == (((void *) block + BLOCK_SZ)
		- block->data) / buf->objsize) {
		obj_blockdestroy(block);

		if (block == buf->block)
			buf->block = NULL;
	}
}
