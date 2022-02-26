/*------------------------------------------------------------------------
 *  Copyright 2023 (c) Leo CH Li <jafee201153@gmail.com>
 *------------------------------------------------------------------------*/

#ifndef __USER_ALLOC_H__
#define __USER_ALLOC_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ENABLE_MIN_SIZE_CHECK 0  // 0: off, 1: open
#define USER_ALLOC_ARRAY_SIZE (114 * 1024)

void *user_malloc_t(size_t __size);
void user_free_t(void *__p);
void *user_calloc_t(size_t __nmemb, size_t __size);
void *user_realloc_t(void *__p, size_t __size);

/**
 * @brief Define the memory allocation functions.
 *
 */
#if 0
#define zbar_malloc malloc
#define zbar_free free
#define zbar_calloc calloc
#define zbar_realloc realloc
#else
#define zbar_malloc user_malloc_t
#define zbar_free user_free_t
#define zbar_calloc user_calloc_t
#define zbar_realloc user_realloc_t
#endif

#endif /* __USER_ALLOC_H__ */
