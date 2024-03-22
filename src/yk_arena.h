#ifndef YK_ARENA_H
#define YK_ARENA_H

#include <yk_common.h>

/*
Note(oh-facts): Be very very careful when glossing over this file. At some point I swapped line 2 and 3 of arena alloc and got memory corruption (display kept getting overwritten and it took me hours to find it).
*/

struct Arena
{
	size_t size;
	u8* base;
	size_t used;
};


internal void* _arena_alloc(struct Arena* arena, size_t size)
{
	Assert(arena->used + size <= arena->size);
	
    void* out = arena->base + arena->used;
	arena->used += size;
	return out;
}

#define push_struct(arena, type) _arena_alloc(arena, sizeof(type))

#define push_array(arena,type,count) _arena_alloc(arena, sizeof(type) * count)

internal void arena_innit(struct Arena* arena, size_t size, void* base)
{
	arena->size = size;
	arena->base = (u8*)base;
	arena->used = 0;
}


#endif