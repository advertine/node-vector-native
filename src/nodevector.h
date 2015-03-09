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
  using v8::Persistent;
  using v8::Object;
  using v8::Value;

  class NativeVector: public node::ObjectWrap, public Vector {
    public:
      static Persistent<FunctionTemplate> constructor;

      static void Init(Handle<Object> exports);
      NativeVector(void);
      NativeVector(NativeVector const& ovec);
      NativeVector(vec_dim_pair_t const * ovec, size_t const vsize);
      virtual ~NativeVector(void);
      static NAN_METHOD(New);
      static NAN_METHOD(Add);
      // static NAN_METHOD(Subtract);
      // static NAN_METHOD(Multiply);
      // static NAN_METHOD(ScalarMultiply);
      static NAN_GETTER(GetSize);
      static NAN_INDEX_GETTER(GetDimension);
      static NAN_INDEX_SETTER(SetDimension);
      static NAN_METHOD(ValueOf);
      static NAN_METHOD(ToObject);
      static NAN_METHOD(GetBuffer);

    private:
      bool Add(Local<Value> value);
      void AssignFromJSObject(Handle<Object> object);
  };

}

#endif
