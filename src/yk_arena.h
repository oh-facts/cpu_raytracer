#ifndef YK_ARENA_H
#define YK_ARENA_H

#include <yk_common.h>

struct Arena
{
	size_t size;
	u8* base;
	size_t used;
};


YK_API void* _arena_alloc(struct Arena* arena, size_t size);

#define push_struct(arena, type) _arena_alloc(arena, sizeof(type))

#define push_array(arena,type,count) _arena_alloc(arena, sizeof(type) * count)

YK_API void arena_innit(struct Arena* arena, size_t size, void* base);

#endif