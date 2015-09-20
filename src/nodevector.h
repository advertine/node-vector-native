#if !defined(NODEVECTOR_HEADER)
#define NODEVECTOR_HEADER

#include <node.h>
#include <node_buffer.h>
#include <v8.h>
#include <nan.h>
#include "vector.h"

namespace NodeVector {
  using v8::Handle;
  using v8::Local;
  using v8::FunctionTemplate;
  using v8::Object;
  using v8::Value;
  using Nan::ObjectWrap;
  using Nan::Persistent;

  class NativeVector: public ObjectWrap, public Vector {
    public:
      static Persistent<FunctionTemplate> constructor;

      static NAN_MODULE_INIT(Init);
      NativeVector(void);
      NativeVector(NativeVector const& ovec);
      NativeVector(vec_dim_pair_t const * ovec, size_t const vsize);
      virtual ~NativeVector(void);
      static NAN_METHOD(New);
      static NAN_METHOD(Clear);
      static NAN_METHOD(Add);
      static NAN_METHOD(Multiply);
      static NAN_METHOD(ScalarMultiply);
      static NAN_METHOD(Normalize);
      static NAN_METHOD(ValueOf);
      static NAN_METHOD(ToObject);
      static NAN_METHOD(GetBuffer);
      static NAN_METHOD(Equals);
      static NAN_GETTER(GetLength);
      static NAN_GETTER(GetAverage);
      static NAN_GETTER(GetVariance);
      static NAN_GETTER(GetSigma);
      static NAN_INDEX_GETTER(GetDimension);
      static NAN_INDEX_SETTER(SetDimension);
      static NAN_INDEX_QUERY(QueryDimension);
      static NAN_INDEX_DELETER(DeleteDimension);
      static NAN_INDEX_ENUMERATOR(EnumerateDimension);

    private:
      NAN_INLINE static void FreeBufferData(char *, void *data);
      bool Add(Local<Value> value);
      bool Multiply(Local<Value> value);
      void AssignFromJSObject(Handle<Object> object);
  };

}

#endif
