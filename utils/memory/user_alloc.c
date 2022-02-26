/*------------------------------------------------------------------------
 *  Copyright 2023 (c) Leo CH Li <jafee201153@gmail.com>
 *------------------------------------------------------------------------*/

#include "user_alloc.h"

typedef struct user_alloc_t {
  struct {
    struct user_alloc_t *ptr;
    unsigned int size;
  } s;
  unsigned int align;
  unsigned int pad;
} user_alloc_t;

char __zbar_alloc_array[USER_ALLOC_ARRAY_SIZE];

static user_alloc_t __base;
static user_alloc_t *__free_ptr = NULL;

unsigned int __alloc_count = 0;

void *user_malloc_t(size_t __size) {
  char *__HEAP_START = __zbar_alloc_array;
  char *__HEAP_END = __HEAP_START + USER_ALLOC_ARRAY_SIZE;

  user_alloc_t *p, *prevp;
  unsigned nunits;

  user_alloc_t *bp = NULL;
  user_alloc_t *bprevp;

  nunits = ((__size + sizeof(user_alloc_t) - 1) / sizeof(user_alloc_t)) + 1;

  __alloc_count += nunits;
#if (ENABLE_MIN_SIZE_CHECK == 1)
  printf("user_malloc_t __alloc_count: %d KB\r\n",
         (__alloc_count * sizeof(user_alloc_t)) / 1024);
#endif

  if ((prevp = __free_ptr) == NULL) {
    p = (user_alloc_t *)__HEAP_START;
#if defined(_WIN32)
    p->s.size = (((intptr_t)__HEAP_END - (intptr_t)__HEAP_START) /
                 sizeof(user_alloc_t));
#else
    p->s.size = (((unsigned int)__HEAP_END - (unsigned int)__HEAP_START) /
                 sizeof(user_alloc_t));
#endif
    p->s.ptr = &__base;
    __base.s.ptr = p;
    __base.s.size = 0;
    prevp = __free_ptr = &__base;
  }

  for (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
    if (p->s.size == nunits) {
      prevp->s.ptr = p->s.ptr;
      __free_ptr = prevp;
      return (void *)(p + 1);
    } else if (p->s.size > nunits) {
      if (bp == NULL) {
        bp = p;
        bprevp = prevp;
      }

      if (bp->s.size > p->s.size) {
        bprevp = prevp;
        bp = p;
      }
    }

    if (p == __free_ptr) {
      if (bp != NULL) {
        __free_ptr = bprevp;
        p = bp;

        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;

        return (void *)(p + 1);
      }
      return NULL;
    }
  }
}

void user_free_t(void *__p) {
  if (__p == NULL) return;

  user_alloc_t *bp, *p;

#if defined(_WIN32)
  if (__p == NULL || (__p < (void *)&__zbar_alloc_array[0]) ||
      (__p > (void *)(&__zbar_alloc_array[USER_ALLOC_ARRAY_SIZE] -
                      sizeof(user_alloc_t))))
#else
  if (__p == NULL || (__p < &__zbar_alloc_array[0]) ||
      (__p >
       (&__zbar_alloc_array[USER_ALLOC_ARRAY_SIZE] - sizeof(user_alloc_t))))
#endif
    return;

  bp = (user_alloc_t *)__p - 1; /* point to block header */

  __alloc_count -= bp->s.size;
#if (ENABLE_MIN_SIZE_CHECK == 1)
  printf("user_free_t __alloc_count: %d KB\r\n",
         (__alloc_count * sizeof(user_alloc_t)) / 1024);
#endif

  for (p = __free_ptr; !((bp > p) && (bp < p->s.ptr)); p = p->s.ptr) {
    if ((p >= p->s.ptr) && ((bp > p) || (bp < p->s.ptr))) {
      break; /* freed block at start or end of arena */
    }
  }

  if ((bp + bp->s.size) == p->s.ptr) {
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } else {
    bp->s.ptr = p->s.ptr;
  }

  if ((p + p->s.size) == bp) {
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } else {
    p->s.ptr = bp;
  }

  __free_ptr = p;
}

void *user_calloc_t(size_t __nmemb, size_t __size) {
  void *p = zbar_malloc(__nmemb * __size);
  if (p != NULL) {
    memset((char *)p, 0, __nmemb * __size);
  }
  return p;
}

void *user_realloc_t(void *__p, size_t __size) {
  user_alloc_t *bp, *p, *np;

  unsigned nunits;
  unsigned aunits;
  unsigned int oldsize;

  if (__p == NULL) {
    bp = zbar_malloc(__size);
    return bp;
  }

  if (__size == 0) {
    zbar_free(__p);
    return NULL;
  }
  nunits = ((__size + sizeof(user_alloc_t) - 1) / sizeof(user_alloc_t)) + 1;

  bp = (user_alloc_t *)__p - 1; /* point to block header */
  if (nunits <= bp->s.size) {
    return __p;
  }

  oldsize = (bp->s.size - 1) * sizeof(user_alloc_t);
  bp = zbar_malloc(__size);
  memcpy(bp, __p, oldsize);
  zbar_free(__p);
  return bp;
}
