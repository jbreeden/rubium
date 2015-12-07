#include <string>
#include "mruby.h"
#include "mruby/compile.h"
#include "mruby_cef.h"
#include "mruby_v8_handler.h"

using namespace std;

#define ASSERT_JS_NOT_NULL(js) \
if (js.get() == NULL) { \
  mrb_raise(mrb, mrb->eStandardError_class, "RUBIUM BUG: Null JS object"); \
  return mrb_nil_value(); \
}

#define ASSERT_IS_JS_VALUE(val, msg) \
if (!mrb_obj_is_kind_of(mrb, val, JSValue_class(mrb))) { \
  mrb_raise(mrb, mrb->eStandardError_class, msg); \
  return mrb_nil_value(); \
}

void free_cef_ref_ptr_to_v8_value(mrb_state* mrb, void* ptr) {
  /*
   * The destructor of the CefRefPtr will decrement V8's ref count on the
   * pointed-to object, possibly freeing it for GC.
   */
  delete static_cast<CefRefPtr<CefV8Value>*>(ptr);
};

const mrb_data_type cef_ref_ptr_to_v8_value_type = {
   "cef_ref_ptr_type", free_cef_ref_ptr_to_v8_value
};

mrb_value mruby_box_cef_v8_value_ref(mrb_state* mrb, CefRefPtr<CefV8Value> ref) {
   /*
    * CefRefPtr is a smart pointer, so this will add a new ref to the js object,
    * preventing it from being GC'ed by V8.
    */
   CefRefPtr<CefV8Value>* ptr = new CefRefPtr<CefV8Value>();
   *ptr = ref;
   RData* data = mrb_data_object_alloc(mrb, JSValue_class(mrb), ptr, &cef_ref_ptr_to_v8_value_type);
   return mrb_obj_value(data);
}

CefRefPtr<CefV8Value>
mruby_unbox_cef_v8_value_ref(mrb_state* mrb, mrb_value cef_v8value){
   return *(CefRefPtr<CefV8Value>*) DATA_PTR(cef_v8value);
}

//<
// class Cef::V8::Value
// =======================
//>

//<
// ### `#bool_value`
// - Returns the bool value of this Value. (Should check type with `is_bool?` first)
//>
static mrb_value
mrb_cef_v8_value_get_bool_value(mrb_state *mrb, mrb_value self) {
   CefRefPtr<CefV8Value> js = mruby_unbox_cef_v8_value_ref(mrb, self);
   ASSERT_JS_NOT_NULL(js);
   if (js->GetBoolValue()) {
      return mrb_true_value();
   }
   return mrb_false_value();
}

//<
// ### `#int_value`
// - Returns the int value of this Value. (Should check type with `is_int?` first)
//>
static mrb_value
mrb_cef_v8_value_get_int_value(mrb_state *mrb, mrb_value self) {
  CefRefPtr<CefV8Value> js = mruby_unbox_cef_v8_value_ref(mrb, self);
  ASSERT_JS_NOT_NULL(js);
  return mrb_fixnum_value(js->GetIntValue());
}

//<
// ### `#double_value`
// - Returns the double value of this Value. (Should check type with `is_double?` first)
//>
static mrb_value
mrb_cef_v8_value_get_double_value(mrb_state *mrb, mrb_value self) {
  CefRefPtr<CefV8Value> js = mruby_unbox_cef_v8_value_ref(mrb, self);
  ASSERT_JS_NOT_NULL(js);
  return mrb_float_value(mrb, js->GetDoubleValue());
}

//<
// ### `#string_value`
// - Returns the string value of this Value. (Should check type with `is_string?` first)
//>
static mrb_value
mrb_cef_v8_value_get_string_value(mrb_state *mrb, mrb_value self) {
   CefRefPtr<CefV8Value> js = mruby_unbox_cef_v8_value_ref(mrb, self);
   ASSERT_JS_NOT_NULL(js);
   return mrb_str_new_cstr(mrb, js->GetStringValue().ToString().c_str());
}

#define TYPE_CHECK_FN(fn, checkMethod) \
   static mrb_value \
   fn (mrb_state *mrb, mrb_value self) { \
      CefRefPtr<CefV8Value> js = mruby_unbox_cef_v8_value_ref(mrb, self); \
      ASSERT_JS_NOT_NULL(js); \
      if (js->checkMethod()) { \
         return mrb_true_value(); \
      } \
      return mrb_false_value(); \
   }

//<
// ### `#is_undefined?`
// - Returns true if this Value is a JavaScript undefined, else false
//>
TYPE_CHECK_FN(mrb_cef_v8_value_is_undefined, IsUndefined)
//<
// ### `#is_null?`
// - Returns true if this Value is a JavaScript null, else false
//>
TYPE_CHECK_FN(mrb_cef_v8_value_is_null, IsNull)
//<
// ### `#is_bool?`
// - Returns true if this Value is a JavaScript bool, else false
//>
TYPE_CHECK_FN(mrb_cef_v8_value_is_bool, IsBool)
//<
// ### `#is_int?`
// - Returns true if this Value is a JavaScript int, else false
//>
TYPE_CHECK_FN(mrb_cef_v8_value_is_int, IsInt)
//<
// ### `#is_uint?`
// - Returns true if this Value is a JavaScript uint, else false
//>
TYPE_CHECK_FN(mrb_cef_v8_value_is_uint, IsUInt)
//<
// ### `#is_double?`
// - Returns true if this Value is a JavaScript double, else false
//>
TYPE_CHECK_FN(mrb_cef_v8_value_is_double, IsDouble)
//<
// ### `#is_date?`
// - Returns true if this Value is a JavaScript date, else false
//>
TYPE_CHECK_FN(mrb_cef_v8_value_is_date, IsDate)
//<
// ### `#is_string?`
// - Returns true if this Value is a JavaScript string, else false
//>
TYPE_CHECK_FN(mrb_cef_v8_value_is_string, IsString)
//<
// ### `#is_object?`
// - Returns true if this Value is a JavaScript object, else false
//>
TYPE_CHECK_FN(mrb_cef_v8_value_is_object, IsObject)
//<
// ### `#is_array?`
// - Returns true if this Value is a JavaScript array, else false
//>
TYPE_CHECK_FN(mrb_cef_v8_value_is_array, IsArray)
//<
// ### `#is_function?`
// - Returns true if this Value is a JavaScript function, else false
//>
TYPE_CHECK_FN(mrb_cef_v8_value_is_function, IsFunction)

#undef TYPE_CHECK_FN

//<
// ### `#[](key)`
// - Returns the value of the property named by `key` on the underlying JavaScript object
//>
mrb_value
mrb_cef_v8_js_object_get_property(mrb_state* mrb, mrb_value self) {
  mrb_value key_param;
  mrb_get_args(mrb, "o", &key_param);

  CefRefPtr<CefV8Value> jsThis = mruby_unbox_cef_v8_value_ref(mrb, self);
  ASSERT_JS_NOT_NULL(jsThis);

  if (mrb_obj_is_kind_of(mrb, key_param, mrb->fixnum_class)) {
    int key = mrb_fixnum(key_param);
    if (jsThis->HasValue(key)) {
      return mruby_box_cef_v8_value_ref(mrb, jsThis->GetValue(key));
    }
  }
  else {
    CefString key(mrb_string_value_cstr(mrb, &key_param));
    if (jsThis->HasValue(key)) {
      return mruby_box_cef_v8_value_ref(mrb, jsThis->GetValue(key));
    }
  }
  return mruby_box_cef_v8_value_ref(mrb, CefV8Value::CreateUndefined());
}

//<
// ### `#[](key, value)`
// - Sets the value of the property named by `key` on the underlying JavaScript object to `value`
//   (which must be a Value)
//>
mrb_value
mrb_cef_v8_js_object_set_property(mrb_state* mrb, mrb_value self) {
   mrb_value key_param;
   mrb_value value_param;

   mrb_get_args(mrb, "oo", &key_param, &value_param);

   CefRefPtr<CefV8Value> jsObjPtr = mruby_unbox_cef_v8_value_ref(mrb, self);

   // TODO: Implicit conversions if value param is a ruby type
   CefRefPtr<CefV8Value> jsValuePtr = mruby_unbox_cef_v8_value_ref(mrb, value_param);

   CefString key(
      mrb_str_to_cstr(
        mrb,
        mrb_funcall(mrb, key_param, "to_s", 0)
      )
    );

   ASSERT_JS_NOT_NULL(jsObjPtr);
   ASSERT_JS_NOT_NULL(jsValuePtr);
   jsObjPtr->SetValue(key, jsValuePtr, CefV8Value::PropertyAttribute::V8_PROPERTY_ATTRIBUTE_NONE);
   return self;
}

//<
// ### `#apply(context, args, &exception_handler)`
// - Used to call a JavaScript function
// - Pretty much just like `apply` from javascript
// - Args
//   + `context`: What to bind `this` to in the invoked function
//   + `args`: An array of Value values to supply as parameters to the function
//   + `exception_handler`: A block accepting a single parameter. <br/>
//                          If the JavaScript function throws, the exception is wrapped
//                          in a Cef::V8::JsException and pass to this block.
// - Return
//   + The return value of the JavaScript function, wrapped in a `Cef::V8::Value`
// - Notes
//   + If the JavaScript function throws, you must either handle the exception or
//     bail out immediately.
//   + If you do not supply the `exception_handler` param, any thrown exceptions are
//     immediately raised in JavaScript land. Continuing to execute code that interacts
//     with the JavaScript context is an error (semantically speaking). For this reason,
//     if there is any chance an exception will be thrown by the called function, you
//     should always provid an `exception_handler` block.
//   + If you can't handler the `exception` passed to the `exception_handler` block,
//     simply `raise exception.message` to re-throw it in JavaScript land.
//>
mrb_value
mrb_cef_v8_js_object_apply(mrb_state* mrb, mrb_value self) {
   mrb_value context;
   mrb_value* args;
   int argc;
   mrb_get_args(mrb, "oa", &context, &args, &argc);

   ASSERT_IS_JS_VALUE(context, "JS::Value#apply expects a JS::Value as the first argument");

   CefV8ValueList js_args;
   for (int i = 0; i < argc; ++i) {
      ASSERT_IS_JS_VALUE(args[i], "JS::Value#apply expects an array of JS::Value objects as the second argument");
      js_args.push_back(
        // TODO: May not be a Value. Need error checking
        mruby_unbox_cef_v8_value_ref(mrb, *(args + i))
      );
   }

   CefRefPtr<CefV8Value> js_context = mruby_unbox_cef_v8_value_ref(mrb, context);
   CefRefPtr<CefV8Value> js_fn = mruby_unbox_cef_v8_value_ref(mrb, self);

   if (!js_fn->IsFunction()) {
     mrb_raise(mrb, mrb->eStandardError_class, "JS::Value#apply is only valid for functions");
     return mrb_nil_value();
   }

   js_fn->SetRethrowExceptions(false);
   CefRefPtr<CefV8Value> result = js_fn->ExecuteFunction(js_context, js_args);

   if (js_fn->HasException()) {
      mrb_value wrapped_exception = mruby_box_cef_v8_exception_ref(mrb, js_fn->GetException());
      js_fn->ClearException();
      mrb_exc_raise(mrb, wrapped_exception);
      return mrb_nil_value();
   }
   else {
      return mruby_box_cef_v8_value_ref(mrb, result);
   }
}

#ifdef __cplusplus
extern "C" {
#endif

   void mrb_mruby_js_value_init(mrb_state* mrb) {
      RClass* JSValue = mrb_define_class_under(mrb, JS_module(mrb), "Value", mrb->object_class);

#define TYPE_CHECK_BINDING(ruby_fn) mrb_define_method(mrb, JSValue, #ruby_fn "?", mrb_cef_v8_value_ ## ruby_fn, MRB_ARGS_NONE());
      TYPE_CHECK_BINDING(is_undefined);
      TYPE_CHECK_BINDING(is_null);
      TYPE_CHECK_BINDING(is_bool);
      TYPE_CHECK_BINDING(is_int);
      TYPE_CHECK_BINDING(is_uint);
      TYPE_CHECK_BINDING(is_double);
      TYPE_CHECK_BINDING(is_date);
      TYPE_CHECK_BINDING(is_string);
      TYPE_CHECK_BINDING(is_object);
      TYPE_CHECK_BINDING(is_array);
      TYPE_CHECK_BINDING(is_function);
#undef TYPE_CHECK_BINDING

      mrb_define_method(mrb, JSValue, "bool_value", mrb_cef_v8_value_get_bool_value, MRB_ARGS_NONE());
      mrb_define_method(mrb, JSValue, "int_value", mrb_cef_v8_value_get_int_value, MRB_ARGS_NONE());
      mrb_define_method(mrb, JSValue, "double_value", mrb_cef_v8_value_get_double_value, MRB_ARGS_NONE());
      mrb_define_method(mrb, JSValue, "string_value", mrb_cef_v8_value_get_string_value, MRB_ARGS_NONE());
      mrb_define_method(mrb, JSValue, "[]", mrb_cef_v8_js_object_get_property, MRB_ARGS_REQ(1));
      mrb_define_method(mrb, JSValue, "[]=", mrb_cef_v8_js_object_set_property, MRB_ARGS_REQ(2));
      mrb_define_method(mrb, JSValue, "apply", mrb_cef_v8_js_object_apply, MRB_ARGS_REQ(2));
   }
#ifdef __cplusplus
}
#endif
