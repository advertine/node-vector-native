// #include <stdio.h>
#include "vector.h"
#include <math.h>

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

size_t Vector::CopyToArray(vec_dim_pair_t * const out, size_t const capacity, vec_value_t cutoff) const
{
  vec_dim_pair_t * over = out + capacity, * ptr = out;
  vec_value_t value;
  DimValueMap::type::const_iterator it = vec.begin();
  for (; ptr != over && it != vec.end(); ++it ) {
    if ((value = it->second) > cutoff) {
      ptr->dim = it->first;
      ptr->value = value;
      ++ptr;
    }
  }
  return (size_t)(ptr - out);
}

size_t Vector::Size() const
{
  return vec.size();
}

bool Vector::operator ==(Vector const& ovec)
{
  return vec == ovec.vec;
}

bool Vector::operator !=(Vector const& ovec)
{
  return vec != ovec.vec;
}

vec_value_t Vector::operator [](vec_dim_t dim) const
{
  DimValueMap::type::const_iterator it = vec.find(dim);
  double ret = it != vec.end() ? it->second : 0.0;
  return ret;
}

Vector &Vector::operator +=(Vector const& ovec)
{
  const DimValueMap::type &ovecvec = ovec.vec;
  DimValueMap::type::const_iterator oit = ovecvec.begin();
  for (; oit != ovecvec.end(); ++oit ) {
    if ( 0.0 == (vec[oit->first] += oit->second) )
      vec.erase(oit->first);
  }
  return *this;
}

Vector &Vector::operator -=(Vector const& ovec)
{
  const DimValueMap::type &ovecvec = ovec.vec;
  DimValueMap::type::const_iterator oit = ovecvec.begin();
  for (; oit != ovecvec.end(); ++oit ) {
    if ( 0.0 == (vec[oit->first] -= oit->second) )
      vec.erase(oit->first);
  }
  return *this;
}

Vector &Vector::operator *=(Vector const& ovec)
{
  const DimValueMap::type &ovecvec = ovec.vec;
  DimValueMap::type::iterator it = vec.begin();
  vec_value_t nval;
  while ( it != vec.end() ) {
    DimValueMap::type::const_iterator oit = ovecvec.find(it->first);
    if ( oit == ovecvec.end() || (nval = it->second * oit->second) == 0.0 ) {
      vec.erase(it++);
    } else {
      (it++)->second = nval;
    }
  }
  return *this;
}

Vector &Vector::operator *=(vec_value_t scale)
{
  DimValueMap::type::iterator it = vec.begin();
  vec_value_t nval;
  while ( it != vec.end() ) {
    if ( (nval = it->second * scale) == 0.0 ) {
      vec.erase(it++);
    } else {
      (it++)->second = nval;
    }
  }
  return *this;
}

Vector &Vector::operator /=(vec_value_t scale)
{
  DimValueMap::type::iterator it = vec.begin();
  vec_value_t nval;
  while ( it != vec.end() ) {
    if ( (nval = it->second / scale) == 0.0 ) {
      vec.erase(it++);
    } else {
      (it++)->second = nval;
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
  vec_value_t sum = 0.0, value;
  DimValueMap::type::const_iterator it = vec.begin();
  for (; it != vec.end(); ++it ) {
    value = it->second;
    sum += value * value;
  }
  return sqrt(sum);
}

vec_value_t Vector::sum() const
{
  vec_value_t sum = 0.0;
  DimValueMap::type::const_iterator it = vec.begin();
  for (; it != vec.end(); ++it ) {
    sum += it->second;
  }
  return sum;
}

vec_value_t Vector::average() const
{
  vec_value_t sum = this->sum();
  return sum / (vec_value_t) vec.size();
}

vec_value_t Vector::sigma(vec_value_t *average) const
{
  return sqrt( this->variance(average) );
}

vec_value_t Vector::variance(vec_value_t *average) const
{
  vec_value_t sum = 0.0, sum2 = 0.0, size = (vec_value_t) vec.size(), value;
  DimValueMap::type::const_iterator it = vec.begin();
  for (; it != vec.end(); ++it ) {
    value = it->second;
    sum += value;
    sum2 += value*value;
  }
  sum /= size;
  if ( average != NULL )
    *average = sum;
  return sum2/size - (sum*sum);
}
