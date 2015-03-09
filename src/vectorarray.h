#if !defined(VECTORARRAY_HEADER)
#define VECTORARRAY_HEADER

#include <stdint.h>
#include <sys/types.h>
#include <float.h>

typedef double vec_value_t;
typedef uint32_t vec_dim_t;

typedef struct {
  vec_dim_t dim;
  vec_value_t value;
} vec_dim_pair_t;

#endif
