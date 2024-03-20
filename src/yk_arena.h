#ifndef YK_ARENA_H
#define YK_ARENA_H

#include <yk_common.h>

struct Arena
{
	size_t size;
	u8* base;
	size_t used;
};


internal void* _arena_alloc(struct Arena* arena, size_t size)
{
	Assert(arena->used + size <= arena->size);
	arena->used += size;
	void* out = arena->base + arena->used;
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