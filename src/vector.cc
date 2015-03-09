// #include <stdio.h>
#include "vector.h"

Vector::Vector() {}

Vector::~Vector() {}

Vector::Vector(Vector const& ovec)
{
  vec = ovec.vec;
}

Vector::Vector(vec_dim_pair_t const * inp, size_t const vsize)
{
  Assign(inp, vsize);
}

void Vector::Assign(vec_dim_pair_t const * inp, size_t const vsize)
{
  vec_dim_pair_t const * over = inp + vsize, * ptr = inp;
  DimValueMap::type::const_iterator it = vec.begin();
  for (; ptr != over; ++ptr ) {
    vec.insert( DimValueMap::type::value_type(ptr->dim, ptr->value) );
  }
}

void Vector::Set(vec_dim_pair_t const * ovec, size_t const vsize)
{
  vec.clear();
  Assign(ovec, vsize);
}

void Vector::Set(vec_dim_t dim, vec_value_t value)
{
  if (value == 0.0) {
    vec.erase(dim);
  } else
    vec[dim] = value;
}

void Vector::Add(vec_dim_pair_t const * ovec, size_t const vsize)
{
  vec_dim_pair_t const * over = ovec + vsize, * ptr = ovec;
  for (; ptr != over; ++ptr ) {
    if ( 0.0 == (vec[ptr->dim] += ptr->value) )
      vec.erase(ptr->dim);
  }
}

void Vector::Subtract(vec_dim_pair_t const * ovec, size_t const vsize)
{
  vec_dim_pair_t const * over = ovec + vsize, * ptr = ovec;
  for (; ptr != over; ++ptr ) {
    if ( 0.0 == (vec[ptr->dim] -= ptr->value) )
      vec.erase(ptr->dim);
  }
}

Vector& Vector::operator =(Vector const& ovec)
{
  vec = ovec.vec;
  return *this;
}

size_t Vector::CopyToArray(vec_dim_pair_t * const out, size_t const capacity) const
{
  vec_dim_pair_t * over = out + capacity, * ptr = out;
  DimValueMap::type::const_iterator it = vec.begin();
  while ( ptr != over && it != vec.end() ) {
    ptr->dim = it->first;
    ptr->value = it->second;
    ++ptr;
    ++it;
  }
  return (size_t)(ptr - out);
}

size_t Vector::Size() const
{
  return vec.size();
}

vec_value_t Vector::operator [](vec_dim_t dim) const
{
  DimValueMap::type::const_iterator it = vec.find(dim);
  double ret = it != vec.end() ? it->second : 0.0;
  return ret;
}

Vector &Vector::operator +=(Vector const& ovec)
{
  DimValueMap::type::const_iterator oit = ovec.vec.begin();
  for (; oit != ovec.vec.end(); ++oit ) {
    if ( 0.0 == (vec[oit->first] += oit->second) )
      vec.erase(oit->first);
  }
  return *this;
}

Vector &Vector::operator -=(Vector const& ovec)
{
  DimValueMap::type::const_iterator oit = ovec.vec.begin();
  for (; oit != ovec.vec.end(); ++oit ) {
    if ( 0.0 == (vec[oit->first] -= oit->second) )
      vec.erase(oit->first);
  }
  return *this;
}

Vector &Vector::operator *=(Vector const& ovec)
{
  const DimValueMap::type &ovecvec = ovec.vec;
  DimValueMap::type::iterator it = vec.begin();
  while ( it != vec.end() ) {
    if ( ovecvec.find(it->first) == ovecvec.end() )
      vec.erase(it++);
    else
      ++it;
  }
  DimValueMap::type::const_iterator oit = ovecvec.begin();
  for (; oit != vec.end(); ++oit ) {
    if ( oit->second != 0.0 ) {
      vec[oit->first] *= oit->second;
    } else {
      vec.erase(oit->first);
    }
  }
  return *this;
}

vec_value_t Vector::operator *(Vector const& ovec) const
{
  vec_value_t sum = 0.0;
  const DimValueMap::type &ovecvec = ovec.vec;
  DimValueMap::type::const_iterator it = ovecvec.begin();
  for (; it != ovecvec.end(); ++it ) {
    sum += (vec_value_t) (*this)[it->first] * it->second;
  }
  return sum;
}

vec_value_t Vector::operator +() const
{
  vec_value_t sum = 0.0;
  DimValueMap::type::const_iterator it = vec.begin();
  for (; it != vec.end(); ++it ) {
    sum += it->second;
  }
  return sum;
}

/*
int Vector::CompareDim(const void *ap, const void *bp)
{
  const vec_dim_t a = *(vec_dim_t *) ap;
  const vec_dim_t b = *(vec_dim_t *) bp;
  if (a < b)
      return -1;
  else if(a > b)
      return 1;
  else
      return 0;
}
*/