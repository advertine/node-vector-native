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

    size_t CopyToArray(vec_dim_pair_t * const out, size_t const capacity, vec_value_t cutoff = 0.0) const;
    size_t Size(void) const;

    void Clear(void);
    void Set(vec_dim_pair_t const * ovec, size_t const vsize);
    void Set(vec_dim_t dim, vec_value_t value);
    bool Has(vec_dim_t dim) const;
    bool Delete(vec_dim_t dim);
    void Add(vec_dim_pair_t const * ovec, size_t const vsize);
    void Subtract(vec_dim_pair_t const * ovec, size_t const vsize);

    Vector& operator =(Vector const & ovec);
    Vector &operator +=(Vector const& ovec);
    Vector &operator -=(Vector const& ovec);
    Vector &operator *=(Vector const& ovec);
    Vector &operator *=(vec_value_t scale);
    Vector &operator /=(vec_value_t scale);
    vec_value_t operator *(Vector const& ovec) const;
    vec_value_t operator +() const;

    bool operator ==(Vector const& ovec);
    bool operator !=(Vector const& ovec);

    vec_value_t operator [](vec_dim_t dim) const;

    vec_value_t sum() const;
    vec_value_t average() const;
    vec_value_t sigma(vec_value_t *average = NULL) const;
    vec_value_t variance(vec_value_t *average = NULL) const;

  protected:
    DimValueMap::type vec;

  private:
    void Assign(vec_dim_pair_t const * ovec, size_t const vsize);

};

#endif
