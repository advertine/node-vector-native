#if !defined(VECTORARRAY_HEADER)
#define VECTORARRAY_HEADER

#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <float.h>

#ifndef FORCE_INLINE
#  if defined(_MSC_VER)
#    define FORCE_INLINE __forceinline
#  else
#    define FORCE_INLINE \
        inline __attribute__((always_inline))
#  endif
#endif

typedef double vec_value_t;
typedef uint32_t vec_dim_t;

typedef struct {
  vec_dim_t dim;
  vec_value_t value;
} vec_dim_pair_t;

static int vec_compare_by_dim(const void *ap, const void *bp)
{
  const vec_dim_t adim = *(vec_dim_t *) ap;
  const vec_dim_t bdim = *(vec_dim_t *) bp;
  if (adim < bdim)
      return -1;
  else if(adim > bdim)
      return 1;
  else
      return 0;
}

static int vec_compare_by_value_desc(const void *ap, const void *bp)
{
  const vec_value_t aval = ((vec_dim_pair_t *) ap)->value;
  const vec_value_t bval = ((vec_dim_pair_t *) bp)->value;
  if (aval > bval)
      return -1;
  else if(aval < bval)
      return 1;
  else
    return vec_compare_by_dim(ap, bp);
}

static int vec_compare_by_value_asc(const void *ap, const void *bp)
{
  const vec_value_t aval = ((vec_dim_pair_t *) ap)->value;
  const vec_value_t bval = ((vec_dim_pair_t *) bp)->value;
  if (aval < bval)
      return -1;
  else if(aval > bval)
      return 1;
  else {
    return vec_compare_by_dim(ap, bp);
  }
}

static FORCE_INLINE void vec_sort_by_value_asc(vec_dim_pair_t *vector, size_t vectorsize)
{
  qsort( (void *)vector, vectorsize, sizeof(vec_dim_pair_t), vec_compare_by_value_asc);
}

static FORCE_INLINE void vec_sort_by_value_desc(vec_dim_pair_t *vector, size_t vectorsize)
{
  qsort( (void *)vector, vectorsize, sizeof(vec_dim_pair_t), vec_compare_by_value_desc);
}

#endif
