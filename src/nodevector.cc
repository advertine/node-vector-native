#include <stdlib.h>
#include <algorithm>
#include "nodevector.h"

namespace NodeVector {
  using v8::None;
  using v8::Handle;
  using v8::Local;
  using v8::Value;
  using v8::Object;
  using v8::Array;
  using v8::Number;
  using v8::Integer;
  using v8::Uint32;
  using v8::String;
  using v8::Function;
  using v8::FunctionTemplate;
  using v8::ObjectTemplate;
  using v8::PropertyAttribute;
  using v8::ReadOnly;
  using v8::DontDelete;

  Persistent<FunctionTemplate> NativeVector::constructor;

  NativeVector::NativeVector(void) : Vector() {}
  NativeVector::NativeVector(NativeVector const& ovec) : ObjectWrap(), Vector(ovec) {}
  NativeVector::NativeVector(vec_dim_pair_t const * ovec, size_t const vsize)
    : ObjectWrap(), Vector(ovec, vsize) {};

  NativeVector::~NativeVector(void) {}

  NAN_MODULE_INIT(NativeVector::Init)
  {
    Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->SetClassName( Nan::New<String>("Vector").ToLocalChecked() );

    Local<ObjectTemplate> i_t = tpl->InstanceTemplate();
    i_t->SetInternalFieldCount(1);

    Nan::SetIndexedPropertyHandler( i_t,
                                    GetDimension,
                                    SetDimension,
                                    QueryDimension,
                                    DeleteDimension,
                                    EnumerateDimension );

    Local<ObjectTemplate> proto = tpl->PrototypeTemplate();
    Nan::SetAccessor(proto, Nan::New<String>("length").ToLocalChecked(),
                            GetLength);
    Nan::SetAccessor(proto, Nan::New<String>("average").ToLocalChecked(),
                            GetAverage);
    Nan::SetAccessor(proto, Nan::New<String>("variance").ToLocalChecked(),
                            GetVariance);
    Nan::SetAccessor(proto, Nan::New<String>("sigma").ToLocalChecked(),
                            GetSigma);

    Nan::SetPrototypeMethod(tpl, "clear", Clear);
    Nan::SetPrototypeMethod(tpl, "add", Add);
    Nan::SetPrototypeMethod(tpl, "multiply", Multiply);
    Nan::SetPrototypeMethod(tpl, "scalar", ScalarMultiply);
    Nan::SetPrototypeMethod(tpl, "normalize", Normalize);
    Nan::SetPrototypeMethod(tpl, "toObject", ToObject);
    Nan::SetPrototypeMethod(tpl, "valueOf", ValueOf);
    Nan::SetPrototypeMethod(tpl, "getBuffer", GetBuffer);
    Nan::SetPrototypeMethod(tpl, "equals", Equals);

    Nan::SetTemplate(tpl,
              Nan::New<String>("BYTEARRAY_ELEMENT_SIZE").ToLocalChecked(),
              Nan::New<Uint32>((uint32_t) sizeof(vec_dim_pair_t)),
              static_cast<PropertyAttribute>(ReadOnly | DontDelete) );
    Nan::SetTemplate(tpl,
              Nan::New<String>("BYTEARRAY_DIM_OFFSET").ToLocalChecked(),
              Nan::New<Uint32>((int32_t) offsetof(vec_dim_pair_t, dim)),
              static_cast<PropertyAttribute>(ReadOnly | DontDelete) );
    Nan::SetTemplate(tpl,
              Nan::New<String>("BYTEARRAY_VALUE_OFFSET").ToLocalChecked(),
              Nan::New<Uint32>((int32_t) offsetof(vec_dim_pair_t, value)),
              static_cast<PropertyAttribute>(ReadOnly | DontDelete) );

    constructor.Reset( tpl );
    Nan::Set(target, Nan::New<String>("Vector").ToLocalChecked(),
                     Nan::GetFunction(tpl).ToLocalChecked());
  }

  NAN_METHOD(NativeVector::New)
  {

    if ( info.IsConstructCall() ) {

      NativeVector *vector;

      if ( info.Length() > 0 ) {
        if ( node::Buffer::HasInstance(info[0]) ) {
          Local<Object> buffer = info[0].As<Object>();
          vector = new NativeVector(
            (vec_dim_pair_t *)node::Buffer::Data(buffer),
            node::Buffer::Length(buffer) / sizeof(vec_dim_pair_t) );
        } else if ( info[0]->IsObject() ) {
          if ( Nan::New(constructor)->HasInstance( info[0] ) ) {
            NativeVector const *other = ObjectWrap::Unwrap<NativeVector>( info[0].As<Object>() );
            vector = new NativeVector(*other);
          } else {
            vector = new NativeVector();
            vector->AssignFromJSObject( info[0].As<Object>() );
          }
        } else if ( info[0]->IsNull() || info[0]->IsUndefined() ) {
          vector = new NativeVector();
        } else
          return Nan::ThrowTypeError("first argument should be a Buffer or an Object");
      } else
        vector = new NativeVector();

      vector->Wrap( info.This() );
      info.GetReturnValue().Set( info.This() );

    } else {
      int argc = info.Length();
      Local<Value> argv[1];
      if (argc > 0) {
        argv[0] = info[0];
        if (argc > 1) argc = 1;
      }
      Local<Function> cons = Nan::GetFunction(Nan::New(constructor)).ToLocalChecked();
      info.GetReturnValue().Set( Nan::NewInstance(cons, argc, &argv[0]).ToLocalChecked() );
    }
  }

  NAN_METHOD(NativeVector::ValueOf)
  {
    NativeVector const *vector = ObjectWrap::Unwrap<NativeVector>( info.This() );
    info.GetReturnValue().Set( +(*vector) );
  }

  NAN_METHOD(NativeVector::ToObject)
  {
    Local<Object> object( Nan::New<Object>() );
    NativeVector const *vector = ObjectWrap::Unwrap<NativeVector>( info.This() );

    const DimValueMap::type &vec = vector->vec;
    DimValueMap::type::const_iterator it = vec.begin();
    for (; it != vec.end(); ++it ) {
      Nan::Set(object, it->first, Nan::New<Number>(it->second) );
    }

    info.GetReturnValue().Set(object);
  }

  NAN_INLINE void NativeVector::FreeBufferData(char *, void *data) {
    free(data);
  }

  NAN_METHOD(NativeVector::GetBuffer)
  {

    NativeVector const *vector = ObjectWrap::Unwrap<NativeVector>( info.This() );

    size_t orig_count = vector->Size(), orig_size = orig_count * sizeof(vec_dim_pair_t);

    vec_dim_pair_t *vecdata = (vec_dim_pair_t *) malloc( orig_size );

    vec_value_t cutoff = 0.0;

    if ( info.Length() > 0 && info[0]->IsNumber() ) {
      cutoff = (vec_value_t) Nan::To<double>(info[0]).FromMaybe(0.0);
      vec_value_t average;
      cutoff *= vector->sigma(&average);
      cutoff = average - cutoff;
    }

    ssize_t count = vector->CopyToArray( vecdata, orig_count, cutoff );

    if ( info.Length() > 1 && Nan::To<bool>(info[1]).FromMaybe(false) ) {
      vec_sort_by_value_desc( vecdata, count );
      if ( info[1]->IsNumber() ) {
        count = std::min( count,
                  std::max(
                    static_cast<ssize_t>(Nan::To<int64_t>(info[1]).FromMaybe(static_cast<int64_t>(count))),
                    static_cast<ssize_t>(0)
                  )
                );
      }
    }

    size_t size = (unsigned) count * sizeof(vec_dim_pair_t);

    if ( size != orig_size )
      vecdata = (vec_dim_pair_t *) realloc(vecdata, size);

    Nan::MaybeLocal<Object> buffer( Nan::NewBuffer( (char *) vecdata, size,
                          (Nan::FreeCallback) FreeBufferData,
                          (void *)vecdata ));

    if (buffer.IsEmpty())
      return Nan::ThrowError("couldn't allocate buffer for vector");
    else
      info.GetReturnValue().Set(buffer.ToLocalChecked());
  }

  void NativeVector::AssignFromJSObject(Handle<Object> object)
  {
    Local<Array> names;
    if ( Nan::GetPropertyNames(object).ToLocal(&names) ) {
      int size = names->Length();
      Local<Value> vecdim, vecval;
      while (size-- > 0) {
        if ( Nan::Get(names, size).ToLocal(&vecdim) && vecdim->IsUint32() &&
             Nan::Get(object, vecdim).ToLocal(&vecval) ) {

          this->Set( Nan::To<vec_dim_t>(vecdim).FromJust(),
                     Nan::To<vec_value_t>(vecval).FromMaybe(0.0) );
        }
      }
    }
  }

  bool NativeVector::Add(Local<Value> value)
  {

    if (node::Buffer::HasInstance(value)) {
      Local<Object> buffer = value.As<Object>();
      this->Vector::Add(  (vec_dim_pair_t *)node::Buffer::Data(buffer),
                    node::Buffer::Length(buffer) / sizeof(vec_dim_pair_t) );
    } else if ( Nan::New(constructor)->HasInstance(value) ) {
      Vector const *other = ObjectWrap::Unwrap<NativeVector>( value.As<Object>() );
      (*this) += (*other);
    } else if ( value->IsArray() ) {
      Local<Array> array( value.As<Array>() );
      for (int n = array->Length(); n-- != 0; ) {
        if ( ! this->Add( Nan::Get(array, n).ToLocalChecked() ) )
          break;
      }
    } else
      return false;

    return true;
  }

  NAN_METHOD(NativeVector::Add)
  {

    NativeVector *vector = ObjectWrap::Unwrap<NativeVector>( info.This() );

    if ( info.Length() > 0 ) {
      if ( ! vector->Add( info[0] ) )
        return Nan::ThrowTypeError("first argument should be a Buffer, Vector or an Array");
    }

    info.GetReturnValue().Set( info.This() );
  }

  NAN_METHOD(NativeVector::Clear)
  {

    NativeVector *vector = ObjectWrap::Unwrap<NativeVector>( info.This() );

    vector->Vector::Clear();

    info.GetReturnValue().Set( info.This() );
  }

  bool NativeVector::Multiply(Local<Value> value)
  {

    if ( Nan::New(constructor)->HasInstance(value) ) {
      NativeVector const *other = ObjectWrap::Unwrap<NativeVector>( value.As<Object>() );
      (*this) *= (*other);
    } else if ( value->IsNumber() ) {
      (*this) *= Nan::To<vec_value_t>(value).FromMaybe(0.0);
    } else if ( value->IsArray() ) {
      Local<Array> array( value.As<Array>() );
      for (int n = array->Length(); n-- != 0; ) {
        if ( ! Nan::Get(array, n).ToLocal(&value) || ! this->Multiply(value) )
          break;
      }
    } else
      return false;

    return true;
  }

  NAN_METHOD(NativeVector::Multiply)
  {

    NativeVector *vector = ObjectWrap::Unwrap<NativeVector>( info.This() );

    if ( info.Length() > 0 ) {
      if ( ! vector->Multiply( info[0] ) )
        return Nan::ThrowTypeError("first argument should be a Number, Vector or an Array");
    }

    info.GetReturnValue().Set( info.This() );
  }

  NAN_METHOD(NativeVector::ScalarMultiply)
  {

    NativeVector *vector = ObjectWrap::Unwrap<NativeVector>( info.This() );

    if ( info.Length() > 0 &&
          Nan::New(constructor)->HasInstance( info[0] ) ) {
      Vector const *other = ObjectWrap::Unwrap<NativeVector>( info[0].As<Object>() );
      info.GetReturnValue().Set( (*vector) * (*other) );
    } else
      return Nan::ThrowTypeError("first argument should be a Vector");
  }

  NAN_METHOD(NativeVector::Normalize)
  {

    NativeVector *vector = ObjectWrap::Unwrap<NativeVector>( info.This() );

    vec_value_t sum;

    if ( info.Length() > 0 ) {
      sum = Nan::To<vec_value_t>(info[0]).FromMaybe(0.0);
    } else {
      sum = +(*vector);
    }

    (*vector) /= sum;

    info.GetReturnValue().Set( info.This() );
  }

  NAN_METHOD(NativeVector::Equals)
  {
    NativeVector *vector = ObjectWrap::Unwrap<NativeVector>( info.This() );

    if ( info.Length() > 0 && Nan::New(constructor)->HasInstance( info[0] ) ) {
      NativeVector const *other = ObjectWrap::Unwrap<NativeVector>( info[0].As<Object>() );
      info.GetReturnValue().Set( *vector == *other );
    } else
      info.GetReturnValue().Set(false);
  }

  NAN_GETTER(NativeVector::GetLength)
  {
    NativeVector const *vector = ObjectWrap::Unwrap<NativeVector>( info.This() );
    info.GetReturnValue().Set( Nan::New<Uint32>( (unsigned int) vector->Size() ) );
  }

  NAN_GETTER(NativeVector::GetAverage)
  {
    NativeVector *vector = ObjectWrap::Unwrap<NativeVector>( info.This() );
    info.GetReturnValue().Set( vector->average() );
  }

  NAN_GETTER(NativeVector::GetVariance)
  {
    NativeVector *vector = ObjectWrap::Unwrap<NativeVector>( info.This() );
    info.GetReturnValue().Set( vector->variance() );
  }

  NAN_GETTER(NativeVector::GetSigma)
  {
    NativeVector *vector = ObjectWrap::Unwrap<NativeVector>( info.This() );
    info.GetReturnValue().Set( vector->sigma() );
  }

  NAN_INDEX_GETTER(NativeVector::GetDimension)
  {
    NativeVector const *vector = ObjectWrap::Unwrap<NativeVector>( info.This() );
    info.GetReturnValue().Set( (*vector)[index] );
  }

  NAN_INDEX_SETTER(NativeVector::SetDimension)
  {
    NativeVector *vector = ObjectWrap::Unwrap<NativeVector>( info.This() );
    vector->Set(index, Nan::To<vec_value_t>(value).FromMaybe(0.0));
    info.GetReturnValue().Set( info.This() );
  }

  NAN_INDEX_QUERY(NativeVector::QueryDimension)
  {
    NativeVector const *vector = ObjectWrap::Unwrap<NativeVector>( info.This() );
    if ( vector->Has(index) ) {
      info.GetReturnValue().Set( Nan::New<Integer>(None) );
    }
  }

  NAN_INDEX_DELETER(NativeVector::DeleteDimension)
  {
    NativeVector *vector = ObjectWrap::Unwrap<NativeVector>( info.This() );
    info.GetReturnValue().Set( vector->Delete( (vec_dim_t) index ) );
  }

  NAN_INDEX_ENUMERATOR(NativeVector::EnumerateDimension)
  {
    NativeVector const *vector = ObjectWrap::Unwrap<NativeVector>( info.This() );
    Local<Array> dimensions = Nan::New<Array>( (int) vector->Size() );
    const DimValueMap::type &vec = vector->vec;
    DimValueMap::type::const_iterator it = vec.begin();
    for (uint32_t index = 0; it != vec.end(); ++it )
      Nan::Set(dimensions, index++, Nan::New<Uint32>(it->first) );
    info.GetReturnValue().Set(dimensions);
  }
}
