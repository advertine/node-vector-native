#if !defined(VECTOR_HEADER)
#define VECTOR_HEADER

#include "vectorarray.h"
#include <map>

struct DimValueMap {
  typedef std::map<vec_dim_t,vec_value_t> type;
};

class Vector {
  public:
    Vector(void);
    Vector(Vector const& ovec);
    Vector(vec_dim_pair_t const * ovec, size_t const vsize);
    virtual ~Vector(void);

    size_t CopyToArray(vec_dim_pair_t * const out, size_t const capacity) const;
    size_t Size(void) const;

    void Set(vec_dim_pair_t const * ovec, size_t const vsize);
    void Set(vec_dim_t dim, vec_value_t value);
    void Add(vec_dim_pair_t const * ovec, size_t const vsize);
    void Subtract(vec_dim_pair_t const * ovec, size_t const vsize);

    Vector& operator =(Vector const & ovec);
    Vector &operator +=(Vector const& ovec);
    Vector &operator -=(Vector const& ovec);
    Vector &operator *=(Vector const& ovec);
    vec_value_t operator *(Vector const& ovec) const;
    vec_value_t operator +() const;

    vec_value_t operator [](vec_dim_t dim) const;

    // static int CompareDim(const void *ap, const void *bp);

  protected:
    DimValueMap::type vec;

  private:
    void Assign(vec_dim_pair_t const * ovec, size_t const vsize);

};

#endif
