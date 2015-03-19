#include <stdlib.h>
#include <algorithm>
#include "nodevector.h"

namespace NodeVector {
  using v8::Handle;
  using v8::Local;
  using v8::Value;
  using v8::Object;
  using v8::Array;
  using v8::Number;
  using v8::Uint32;
  using v8::String;
  using v8::Function;
  using v8::FunctionTemplate;
  using v8::ObjectTemplate;
  using v8::Persistent;
  using v8::PropertyAttribute;
  using v8::ReadOnly;
  using v8::DontDelete;
  using node::ObjectWrap;

  Persistent<FunctionTemplate> NativeVector::constructor;

  NativeVector::NativeVector(void) : Vector() {}
  NativeVector::NativeVector(NativeVector const& ovec) : Vector(ovec) {}
  NativeVector::NativeVector(vec_dim_pair_t const * ovec, size_t const vsize)
    : Vector(ovec, vsize) {};

  NativeVector::~NativeVector(void) {}

  void NativeVector::Init(Handle<Object> exports)
  {
    Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);
    NanAssignPersistent( constructor, tpl );
    tpl->SetClassName( NanNew<String>("Vector") );

    Local<ObjectTemplate> i_t = tpl->InstanceTemplate();
    i_t->SetInternalFieldCount(1);

    i_t->SetIndexedPropertyHandler(GetDimension, SetDimension);

    Local<ObjectTemplate> proto = tpl->PrototypeTemplate();
    proto->SetAccessor(NanNew<String>("length"), GetLength);
    proto->SetAccessor(NanNew<String>("average"), GetAverage);
    proto->SetAccessor(NanNew<String>("variance"), GetVariance);
    proto->SetAccessor(NanNew<String>("sigma"), GetSigma);

    NODE_SET_PROTOTYPE_METHOD(tpl, "add", Add);
    NODE_SET_PROTOTYPE_METHOD(tpl, "multiply", Multiply);
    NODE_SET_PROTOTYPE_METHOD(tpl, "scalar", ScalarMultiply);
    NODE_SET_PROTOTYPE_METHOD(tpl, "normalize", Normalize);
    NODE_SET_PROTOTYPE_METHOD(tpl, "toObject", ToObject);
    NODE_SET_PROTOTYPE_METHOD(tpl, "valueOf", ValueOf);
    NODE_SET_PROTOTYPE_METHOD(tpl, "getBuffer", GetBuffer);

    tpl->Set( NanNew<String>("BYTEARRAY_ELEMENT_SIZE"),
              NanNew<Uint32>((uint32_t) sizeof(vec_dim_pair_t)),
              static_cast<PropertyAttribute>(ReadOnly | DontDelete) );
    tpl->Set( NanNew<String>("BYTEARRAY_DIM_OFFSET"),
              NanNew<Uint32>((int32_t) offsetof(vec_dim_pair_t, dim)),
              static_cast<PropertyAttribute>(ReadOnly | DontDelete) );
    tpl->Set( NanNew<String>("BYTEARRAY_VALUE_OFFSET"),
              NanNew<Uint32>((int32_t) offsetof(vec_dim_pair_t, value)),
              static_cast<PropertyAttribute>(ReadOnly | DontDelete) );

    exports->Set( NanNew<String>("Vector"), NanNew<FunctionTemplate>(constructor)->GetFunction() );
  }

  NAN_METHOD(NativeVector::New)
  {
    NanScope();

    if ( args.IsConstructCall() ) {

      NativeVector *vector;

      if ( args.Length() > 0 ) {
        if ( node::Buffer::HasInstance(args[0]) ) {
          Local<Object> buffer = args[0].As<Object>();
          vector = new NativeVector(
            (vec_dim_pair_t *)node::Buffer::Data(buffer),
            node::Buffer::Length(buffer) / sizeof(vec_dim_pair_t) );
        } else if ( args[0]->IsObject() ) {
          if ( NanHasInstance(NativeVector::constructor, args[0]) ) {
            NativeVector const *other = ObjectWrap::Unwrap<NativeVector>( args[0].As<Object>() );
            vector = new NativeVector(*other);
          } else {
            vector = new NativeVector();
            vector->AssignFromJSObject( args[0].As<Object>() );
          }
        } else if ( args[0]->IsNull() || args[0]->IsUndefined() ) {
          vector = new NativeVector();
        } else
          return NanThrowTypeError("first argument should be a Buffer or an Object");
      } else
        vector = new NativeVector();

      vector->Wrap( args.This() );
      NanReturnValue( args.This() );

    } else {
      int argc = args.Length();
      Local<Value> argv[1];
      if (argc > 0) {
        argv[0] = args[0];
        if (argc > 1) argc = 1;
      }
      NanReturnValue(
        NanNew<FunctionTemplate>(constructor)->GetFunction()->NewInstance(argc, argv) );
    }
  }

  NAN_GETTER(NativeVector::GetLength)
  {
    NanScope();
    NativeVector const *vector = ObjectWrap::Unwrap<NativeVector>( args.This() );
    NanReturnValue( NanNew<Uint32>( (unsigned int) vector->Size() ) );
  }

  NAN_INDEX_GETTER(NativeVector::GetDimension)
  {
    NanScope();
    NativeVector const *vector = ObjectWrap::Unwrap<NativeVector>( args.This() );
    NanReturnValue( NanNew<Number>( (*vector)[index] ) );
  }

  NAN_INDEX_SETTER(NativeVector::SetDimension)
  {
    NanScope();
    NativeVector *vector = ObjectWrap::Unwrap<NativeVector>( args.This() );
    vector->Set(index, value->NumberValue());
    NanReturnValue( value );
  }

  NAN_METHOD(NativeVector::ValueOf)
  {
    NanScope();
    NativeVector const *vector = ObjectWrap::Unwrap<NativeVector>( args.This() );
    NanReturnValue( NanNew<Number>( +(*vector) ) );
  }

  NAN_METHOD(NativeVector::ToObject)
  {
    NanScope();
    Local<Object> object( NanNew<Object>() );
    NativeVector const *vector = ObjectWrap::Unwrap<NativeVector>( args.This() );

    const DimValueMap::type &vec = vector->vec;
    DimValueMap::type::const_iterator it = vec.begin();
    for (; it != vec.end(); ++it ) {
      object->Set( it->first, NanNew<Number>(it->second) );
    }

    NanReturnValue(object);
  }

  NAN_INLINE void NativeVector::FreeBufferData(char *, void *data) {
    free(data);
  }

  NAN_METHOD(NativeVector::GetBuffer)
  {
    NanScope();

    NativeVector const *vector = ObjectWrap::Unwrap<NativeVector>( args.This() );

    size_t orig_count = vector->Size(), orig_size = orig_count * sizeof(vec_dim_pair_t);

    vec_dim_pair_t *vecdata = (vec_dim_pair_t *) malloc( orig_size );

    vec_value_t cutoff = 0.0;

    if ( args.Length() > 0 && args[0]->IsNumber() ) {
      cutoff = (vec_value_t) args[0]->NumberValue();
      vec_value_t average;
      cutoff *= vector->sigma(&average);
      cutoff = average - cutoff;
    }

    size_t count = vector->CopyToArray( vecdata, orig_count, cutoff );

    if ( args.Length() > 1 && args[1]->BooleanValue() ) {
      vec_sort_by_value_desc( vecdata, count );
      if ( args[1]->IsNumber() ) {
        count = std::min( count, (size_t) args[1]->IntegerValue() );
      }
    }

    size_t size = count * sizeof(vec_dim_pair_t);

    if ( size != orig_size )
      vecdata = (vec_dim_pair_t *) realloc(vecdata, size);

    Local<Object> buffer( NanNewBufferHandle( (char *) vecdata, size,
                          (NanFreeCallback) FreeBufferData, (void *)vecdata ) );

    NanReturnValue(buffer);
  }

  void NativeVector::AssignFromJSObject(Handle<Object> object)
  {
    Local<Array> names( object->GetPropertyNames() );
    int size = names->Length();
    while (size-- > 0) {
      Local<Value> veckey(names->Get(size));
      this->Set( veckey->Uint32Value(), object->Get(veckey)->NumberValue() );
    }
  }

  bool NativeVector::Add(Local<Value> value)
  {
    NanScope();

    if (node::Buffer::HasInstance(value)) {
      Local<Object> buffer = value.As<Object>();
      this->Vector::Add(  (vec_dim_pair_t *)node::Buffer::Data(buffer),
                    node::Buffer::Length(buffer) / sizeof(vec_dim_pair_t) );
    } else if ( NanHasInstance(NativeVector::constructor, value) ) {
      Vector const *other = ObjectWrap::Unwrap<NativeVector>( value.As<Object>() );
      (*this) += (*other);
    } else if ( value->IsArray() ) {
      Local<Array> array( value.As<Array>() );
      for (int n = array->Length(); n-- != 0; ) {
        if ( ! this->Add( array->Get(n) ) )
          break;
      }
    } else
      return false;
    return true;
  }

  NAN_METHOD(NativeVector::Add)
  {
    NanScope();

    NativeVector *vector = ObjectWrap::Unwrap<NativeVector>( args.This() );

    if ( args.Length() > 0 ) {
      if ( ! vector->Add( args[0] ) )
        return NanThrowTypeError("first argument should be a Buffer, Vector or an Array");
    }

    NanReturnValue( args.This() );
  }

  bool NativeVector::Multiply(Local<Value> value)
  {
    NanScope();

    if ( NanHasInstance(NativeVector::constructor, value) ) {
      NativeVector const *other = ObjectWrap::Unwrap<NativeVector>( value.As<Object>() );
      (*this) *= (*other);
    } else if ( value->IsNumber() ) {
      (*this) *= (vec_value_t) value->NumberValue();
    } else if ( value->IsArray() ) {
      Local<Array> array( value.As<Array>() );
      for (int n = array->Length(); n-- != 0; ) {
        if ( ! this->Multiply( array->Get(n) ) )
          break;
      }
    } else
      return false;
    return true;
  }

  NAN_METHOD(NativeVector::Multiply)
  {
    NanScope();

    NativeVector *vector = ObjectWrap::Unwrap<NativeVector>( args.This() );

    if ( args.Length() > 0 ) {
      if ( ! vector->Multiply( args[0] ) )
        return NanThrowTypeError("first argument should be a Number, Vector or an Array");
    }

    NanReturnValue( args.This() );
  }

  NAN_METHOD(NativeVector::ScalarMultiply)
  {
    NanScope();

    NativeVector *vector = ObjectWrap::Unwrap<NativeVector>( args.This() );

    if ( args.Length() > 0 &&
          NanHasInstance(NativeVector::constructor, args[0]) ) {
      Vector const *other = ObjectWrap::Unwrap<NativeVector>( args[0].As<Object>() );
      NanReturnValue( NanNew<Number>( (*vector) * (*other) ) );
    }

    return NanThrowTypeError("first argument should be a Vector");
  }

  NAN_METHOD(NativeVector::Normalize)
  {
    NanScope();

    NativeVector *vector = ObjectWrap::Unwrap<NativeVector>( args.This() );

    vec_value_t sum;

    if ( args.Length() > 0 ) {
      sum = args[0]->NumberValue();
    } else {
      sum = +(*vector);
    }

    (*vector) /= sum;

    NanReturnValue( args.This() );
  }

  NAN_GETTER(NativeVector::GetAverage)
  {
    NanScope();
    NativeVector *vector = ObjectWrap::Unwrap<NativeVector>( args.This() );
    NanReturnValue( NanNew<Number>( vector->average() ) );
  }

  NAN_GETTER(NativeVector::GetVariance)
  {
    NanScope();
    NativeVector *vector = ObjectWrap::Unwrap<NativeVector>( args.This() );
    NanReturnValue( NanNew<Number>( vector->variance() ) );
  }

  NAN_GETTER(NativeVector::GetSigma)
  {
    NanScope();
    NativeVector *vector = ObjectWrap::Unwrap<NativeVector>( args.This() );
    NanReturnValue( NanNew<Number>( vector->sigma() ) );
  }

}
