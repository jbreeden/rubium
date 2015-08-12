#include <string>
#include "mruby.h"
#include "mruby/compile.h"
#include "mruby_cef.h"
#include "mruby_v8_handler.h"

using namespace std;

mrb_value mrb_cef_v8_value_wrap(mrb_state* mrb, CefRefPtr<CefV8Value> ref) {
   CefRefPtr<CefV8Value>* ptr = new CefRefPtr<CefV8Value>();
   *ptr = ref;
   RData* data = mrb_data_object_alloc(mrb, mrb->object_class, ptr, &cef_ref_ptr_type);
   mrb_value ruby_value = mrb_funcall(mrb, mrb_obj_value(mrb_cef.js_object_class), "new", 0);
   mrb_iv_set(mrb, ruby_value, mrb_intern_cstr(mrb, "value"), mrb_obj_value(data));
   return ruby_value;
}

CefRefPtr<CefV8Value>
mrb_cef_v8_value_unwrap(mrb_state* mrb, mrb_value cef_v8value){
   mrb_value wrapped_value = mrb_iv_get(mrb, cef_v8value, mrb_intern_cstr(mrb, "value"));
   return *(CefRefPtr<CefV8Value>*) DATA_PTR(wrapped_value);
}

mrb_value mrb_cef_v8_exception_wrap(mrb_state* mrb, CefRefPtr<CefV8Exception> ref) {
   CefRefPtr<CefV8Exception>* ptr = new CefRefPtr<CefV8Exception>();
   *ptr = ref;
   RData* data = mrb_data_object_alloc(mrb, mrb->object_class, ptr, &cef_ref_ptr_type);
   mrb_value ruby_value = mrb_funcall(mrb, mrb_obj_value(mrb_cef.js_exception_class), "new", 0);
   mrb_iv_set(mrb, ruby_value, mrb_intern_cstr(mrb, "value"), mrb_obj_value(data));
   return ruby_value;
}

CefRefPtr<CefV8Exception>
mrb_cef_v8_exception_unwrap(mrb_state* mrb, mrb_value cef_v8_exception) {
   mrb_value wrapped_value = mrb_iv_get(mrb, cef_v8_exception, mrb_intern_cstr(mrb, "value"));
   return *(CefRefPtr<CefV8Exception>*) DATA_PTR(wrapped_value);
}

//<
// Module Cef::V8
// ==============
//>

//<
// ### `::create_undefined`
// - Creates a JsObject containing a JavaScript undefined
//>
static mrb_value
mrb_cef_v8_create_undefined(mrb_state *mrb, mrb_value self) {
   return mrb_cef_v8_value_wrap(mrb, CefV8Value::CreateUndefined());
}

//<
// ### `::create_null`
// - Creates a JsObject containing a JavaScript null
//>
static mrb_value
mrb_cef_v8_create_null(mrb_state *mrb, mrb_value self) {
   return mrb_cef_v8_value_wrap(mrb, CefV8Value::CreateNull());
}

//<
// ### `::create_bool(value)`
// - Creates a JsObject containing a JavaScript bool
// - Args
//   + `value`: If `nil` or `flase`, returns a JavaScript fale, else returns a JavaScript true
//>
static mrb_value
mrb_cef_v8_create_bool(mrb_state *mrb, mrb_value self) {
   mrb_value param;
   mrb_get_args(mrb, "o", &param);
   return mrb_cef_v8_value_wrap(mrb, CefV8Value::CreateBool(mrb_test(param)));
}

//<
// ### `::create_int(value)`
// - Creates a JsObject containing a JavaScript int
// - Args
//   + `value`: A number to convert to a JavaScript int
//>
static mrb_value
mrb_cef_v8_create_int(mrb_state *mrb, mrb_value self) {
   mrb_value int_param;
   mrb_get_args(mrb, "i", &int_param);
   return mrb_cef_v8_value_wrap(mrb, CefV8Value::CreateInt(int_param.value.i));
}

//<
// ### `::create_float(value)`
// - Creates a JsObject containing a JavaScript float
// - Args
//   + `value`: A number to convert to a JavaScript float
//>
static mrb_value
mrb_cef_v8_create_float(mrb_state *mrb, mrb_value self) {
   mrb_value param;
   mrb_get_args(mrb, "f", &param);
   return mrb_cef_v8_value_wrap(mrb, CefV8Value::CreateDouble(mrb_float(param)));
}

//<
// ### `::create_string(rb_string)`
// - Creates a JsObject containing a JavaScript string
// - Args
//   + `rb_string`, the ruby string to convert to a JavaScript value
//>
static mrb_value
mrb_cef_v8_create_string(mrb_state *mrb, mrb_value self) {
   mrb_value rbString;
   mrb_get_args(mrb, "S", &rbString);
   char* cString = mrb_str_to_cstr(mrb, rbString);
   return mrb_cef_v8_value_wrap(mrb, CefV8Value::CreateString(cString));
}

//<
// ### `::create_object`
// - Creates a JsObject containing a JavaScript object
//>
static mrb_value
mrb_cef_v8_create_object(mrb_state *mrb, mrb_value self) {
   return mrb_cef_v8_value_wrap(mrb, CefV8Value::CreateObject(NULL));
}

//<
// ### `::create_function(name, &block)`
// - Creates a JsObject containing a JavaScript function
// - Args
//   + `name`: The name of the function
//   + `block`: A block accepting an array of JsObjects as the only parameter. <br/>
//              This block will be executed when the funciton is called from JavaScript. <br/>
//              If the block returns a JsObject, it will be unwrapped and returned to the caller. <br/>
//              If the block returns anything else, `undefined` will be returned to JavaScript. <br/>
//              If the block throws, a JavaScript Error object will be thrown containing the result <br/>
//              of calling `to_s` on the ruby exception. <br/>
//>
static mrb_value
mrb_cef_v8_create_function(mrb_state *mrb, mrb_value self) {
   mrb_value name;
   mrb_value block;

   mrb_get_args(mrb, "o&", &name, &block);

   string name_string = mrb_str_to_cstr(mrb, mrb_funcall(mrb, name, "to_s", 0));

   auto handler = new MRubyV8Handler(mrb, name_string, block);
   CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction(name_string, handler);
   mrb_value wrapped_function = mrb_cef_v8_value_wrap(mrb, func);

   // Keep a reference to the block & wrapped function in ruby-land so they're not garbage collected
   // TODO: Should associate these with the current context and destroy them when the context is destroyed
   mrb_value js_functions = mrb_gv_get(mrb, mrb_intern_cstr(mrb, "$js_functions"));
   mrb_funcall(mrb, js_functions, "push", 1, block);
   mrb_funcall(mrb, js_functions, "push", 1, wrapped_function);

   return wrapped_function;
}

//<
// ### `::window`
// - Returns the global `window` object fromt the current V8 context
//>
static mrb_value
mrb_cef_v8_get_window(mrb_state* mrb, mrb_value self) {
   CefRefPtr<CefV8Context> context = CefV8Context::GetCurrentContext();

   if (context.get() == NULL) {
      mrb_raise(mrb, mrb->eStandardError_class, "No V8 context currently available");
      return mrb_nil_value();
   }

   return mrb_cef_v8_value_wrap(mrb, context->GetGlobal());
}

//<
// ### `::eval(str)`
// - Evaluates the given string as JavaScript in the current V8 context
//>
mrb_value
mrb_cef_v8_eval(mrb_state* mrb, mrb_value self) {
   mrb_value script_param;
   mrb_value block;
   mrb_get_args(mrb, "S&", &script_param, &block);

   auto script_text = mrb_str_to_cstr(mrb, script_param);

   auto context = CefV8Context::GetCurrentContext();

   CefRefPtr<CefV8Value> retval;
   CefRefPtr<CefV8Exception> exc;

   bool succeeded = context->Eval(CefString(script_text), retval, exc);

   mrb_value rb_return_value;

   if (succeeded) {
      rb_return_value = mrb_cef_v8_value_wrap(mrb, retval);
   }
   else if (exc.get() && mrb_test(block)) {
      rb_return_value = mrb_funcall(mrb, block, "call", 1, mrb_cef_v8_exception_wrap(mrb, exc));
   }

   return rb_return_value;
}

//<
// class Cef::V8::JsObject
// =======================
//>

//<
// ### `#bool_value`
// - Returns the bool value of this JsObject. (Should check type with `is_bool?` first)
//>
static mrb_value
mrb_cef_v8_value_get_bool_value(mrb_state *mrb, mrb_value self) {
   CefRefPtr<CefV8Value> js = mrb_cef_v8_value_unwrap(mrb, self);
   if (js->GetBoolValue()) {
      return mrb_true_value();
   }
   return mrb_false_value();
}

//<
// ### `#int_value`
// - Returns the int value of this JsObject. (Should check type with `is_int?` first)
//>
static mrb_value
mrb_cef_v8_value_get_int_value(mrb_state *mrb, mrb_value self) {
   CefRefPtr<CefV8Value> js = mrb_cef_v8_value_unwrap(mrb, self);
   mrb_value rb;
   SET_INT_VALUE(rb, js->GetIntValue());
   return rb;
}

//<
// ### `#double_value`
// - Returns the double value of this JsObject. (Should check type with `is_double?` first)
//>
static mrb_value
mrb_cef_v8_value_get_double_value(mrb_state *mrb, mrb_value self) {
   CefRefPtr<CefV8Value> js = mrb_cef_v8_value_unwrap(mrb, self);
   mrb_value rb;
   SET_FLOAT_VALUE(MRB, rb, js->GetDoubleValue());
   return rb;
}

//<
// ### `#string_value`
// - Returns the string value of this JsObject. (Should check type with `is_string?` first)
//>
static mrb_value
mrb_cef_v8_value_get_string_value(mrb_state *mrb, mrb_value self) {
   CefRefPtr<CefV8Value> js = mrb_cef_v8_value_unwrap(mrb, self);
   return mrb_str_new_cstr(mrb, js->GetStringValue().ToString().c_str());
}

#define TYPE_CHECK_FN(fn, checkMethod) \
   static mrb_value                                             \
   fn (mrb_state *mrb, mrb_value self) {                        \
      if (mrb_cef_v8_value_unwrap(mrb, self)->checkMethod()) {  \
         return mrb_true_value();                               \
      }                                                         \
      return mrb_false_value();                                 \
   }

//<
// ### `#is_undefined?`
// - Returns true if this JsObject is a JavaScript undefined, else false
//>
TYPE_CHECK_FN(mrb_cef_v8_value_is_undefined, IsUndefined)
//<
// ### `#is_null?`
// - Returns true if this JsObject is a JavaScript null, else false
//>
TYPE_CHECK_FN(mrb_cef_v8_value_is_null, IsNull)
//<
// ### `#is_bool?`
// - Returns true if this JsObject is a JavaScript bool, else false
//>
TYPE_CHECK_FN(mrb_cef_v8_value_is_bool, IsBool)
//<
// ### `#is_int?`
// - Returns true if this JsObject is a JavaScript int, else false
//>
TYPE_CHECK_FN(mrb_cef_v8_value_is_int, IsInt)
//<
// ### `#is_uint?`
// - Returns true if this JsObject is a JavaScript uint, else false
//>
TYPE_CHECK_FN(mrb_cef_v8_value_is_uint, IsUInt)
//<
// ### `#is_double?`
// - Returns true if this JsObject is a JavaScript double, else false
//>
TYPE_CHECK_FN(mrb_cef_v8_value_is_double, IsDouble)
//<
// ### `#is_date?`
// - Returns true if this JsObject is a JavaScript date, else false
//>
TYPE_CHECK_FN(mrb_cef_v8_value_is_date, IsDate)
//<
// ### `#is_string?`
// - Returns true if this JsObject is a JavaScript string, else false
//>
TYPE_CHECK_FN(mrb_cef_v8_value_is_string, IsString)
//<
// ### `#is_object?`
// - Returns true if this JsObject is a JavaScript object, else false
//>
TYPE_CHECK_FN(mrb_cef_v8_value_is_object, IsObject)
//<
// ### `#is_array?`
// - Returns true if this JsObject is a JavaScript array, else false
//>
TYPE_CHECK_FN(mrb_cef_v8_value_is_array, IsArray)
//<
// ### `#is_function?`
// - Returns true if this JsObject is a JavaScript function, else false
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

   CefRefPtr<CefV8Value> jsThis = mrb_cef_v8_value_unwrap(mrb, self);

   if (key_param.tt == MRB_TT_FIXNUM) {
      return mrb_cef_v8_value_wrap(mrb, jsThis->GetValue(key_param.value.i));
   }
   else {
      CefString key(
         mrb_str_to_cstr(
         mrb,
         mrb_funcall(mrb, key_param, "to_s", 0)));

      return mrb_cef_v8_value_wrap(mrb, jsThis->GetValue(key));
   }
}

//<
// ### `#[](key, value)`
// - Sets the value of the property named by `key` on the underlying JavaScript object to `value`
//   (which must be a JsObject)
//>
mrb_value
mrb_cef_v8_js_object_set_property(mrb_state* mrb, mrb_value self) {
   mrb_value key_param;
   mrb_value value_param;

   mrb_get_args(mrb, "oo", &key_param, &value_param);

   CefRefPtr<CefV8Value> jsObjPtr = mrb_cef_v8_value_unwrap(mrb, self);

   // TODO: Implicit conversions if value param is a ruby type
   CefRefPtr<CefV8Value> jsValuePtr = mrb_cef_v8_value_unwrap(mrb, value_param);

   CefString key(
      mrb_str_to_cstr(
      mrb,
      mrb_funcall(mrb, key_param, "to_s", 0)));

   jsObjPtr->SetValue(key, jsValuePtr, CefV8Value::PropertyAttribute::V8_PROPERTY_ATTRIBUTE_NONE);
   return self;
}

//<
// ### `#apply(context, args, &exception_handler)`
// - Used to call a JavaScript function
// - Pretty much just like `apply` from javascript
// - Args
//   + `context`: What to bind `this` to in the invoked function
//   + `args`: An array of JsObject values to supply as parameters to the function
//   + `exception_handler`: A block accepting a single parameter. <br/>
//                          If the JavaScript function throws, the exception is wrapped
//                          in a Cef::V8::JsException and pass to this block.
// - Return
//   + The return value of the JavaScript function, wrapped in a `Cef::V8::JsObject`
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
   mrb_value block;
   int argc;
   mrb_get_args(mrb, "oa&", &context, &args, &argc, &block);

   CefV8ValueList js_args;
   for (int i = 0; i < argc; ++i) {
      js_args.push_back(
         mrb_cef_v8_value_unwrap(mrb, *(args + i))
         );
   }

   CefRefPtr<CefV8Value> js_context = mrb_cef_v8_value_unwrap(mrb, context);
   CefRefPtr<CefV8Value> js_fn = mrb_cef_v8_value_unwrap(mrb, self);

   if (mrb_test(block)) {
      js_fn->SetRethrowExceptions(false);
   }
   else {
      js_fn->SetRethrowExceptions(true);
   }

   CefRefPtr<CefV8Value> result = js_fn->ExecuteFunction(js_context, js_args);
   mrb_value rb_return_value;

   if (js_fn->HasException() && mrb_test(block)) {
      auto wrapped_exception = mrb_cef_v8_exception_wrap(mrb, js_fn->GetException());
      rb_return_value = mrb_funcall(mrb, block, "call", 1, wrapped_exception);
      js_fn->ClearException();
   }
   else {
      rb_return_value = mrb_cef_v8_value_wrap(mrb, result);
   }

   return rb_return_value;
}

//<
// # Class Cef::V8::JsException
//>

//<
// ### `#message`
// - Returns the message from the wrapped JavaScript exception as a string
//>
mrb_value
mrb_cef_v8_exception_get_message(mrb_state* mrb, mrb_value self) {
   CefRefPtr<CefV8Exception> v8_exception = mrb_cef_v8_exception_unwrap(mrb, self);
#if defined(_WIN32) || defined(_WIN64)
   return mrb_str_new_cstr(mrb, v8_exception->GetMessageA().ToString().c_str());
#else
  return mrb_str_new_cstr(mrb, v8_exception->GetMessage().ToString().c_str());
#endif
}

#ifdef __cplusplus
extern "C" {
#endif

   void mrb_mruby_cef_v8_init(mrb_state* mrb, RClass* cef_module) {
      mrb_load_string(mrb, "$js_functions = []");

      mrb_cef.v8_module = mrb_define_module_under(mrb, cef_module, "V8");
      mrb_cef.js_object_class = mrb_define_class_under(mrb, mrb_cef.v8_module, "JsObject", mrb->object_class);
      mrb_cef.js_exception_class = mrb_define_class_under(mrb, mrb_cef.v8_module, "JsException", mrb->eStandardError_class);

      // Cef::V8 module
      mrb_define_class_method(mrb, mrb_cef.v8_module, "window", mrb_cef_v8_get_window, MRB_ARGS_NONE());
      mrb_define_class_method(mrb, mrb_cef.v8_module, "create_undefined", mrb_cef_v8_create_undefined, MRB_ARGS_NONE());
      mrb_define_class_method(mrb, mrb_cef.v8_module, "create_null", mrb_cef_v8_create_null, MRB_ARGS_NONE());
      mrb_define_class_method(mrb, mrb_cef.v8_module, "create_bool", mrb_cef_v8_create_bool, MRB_ARGS_ARG(1, 0));
      mrb_define_class_method(mrb, mrb_cef.v8_module, "create_int", mrb_cef_v8_create_int, MRB_ARGS_ARG(1, 0));
      mrb_define_class_method(mrb, mrb_cef.v8_module, "create_float", mrb_cef_v8_create_float, MRB_ARGS_ARG(1, 0));
      mrb_define_class_method(mrb, mrb_cef.v8_module, "create_string", mrb_cef_v8_create_string, MRB_ARGS_ARG(1, 0));
      mrb_define_class_method(mrb, mrb_cef.v8_module, "create_object", mrb_cef_v8_create_object, MRB_ARGS_ARG(0, 0));
      mrb_define_class_method(mrb, mrb_cef.v8_module, "create_function", mrb_cef_v8_create_function, MRB_ARGS_ARG(2, 0));
      mrb_define_class_method(mrb, mrb_cef.v8_module, "eval", mrb_cef_v8_eval, MRB_ARGS_ARG(1, 0));

      // Cef::V8::JsObject class
#define TYPE_CHECK_BINDING(ruby_fn) mrb_define_method(mrb, mrb_cef.js_object_class, #ruby_fn "?", mrb_cef_v8_value_ ## ruby_fn, MRB_ARGS_NONE());
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

      mrb_define_method(mrb, mrb_cef.js_object_class, "bool_value", mrb_cef_v8_value_get_bool_value, MRB_ARGS_NONE());
      mrb_define_method(mrb, mrb_cef.js_object_class, "int_value", mrb_cef_v8_value_get_int_value, MRB_ARGS_NONE());
      mrb_define_method(mrb, mrb_cef.js_object_class, "double_value", mrb_cef_v8_value_get_double_value, MRB_ARGS_NONE());
      mrb_define_method(mrb, mrb_cef.js_object_class, "string_value", mrb_cef_v8_value_get_string_value, MRB_ARGS_NONE());
      mrb_define_method(mrb, mrb_cef.js_object_class, "[]", mrb_cef_v8_js_object_get_property, MRB_ARGS_REQ(1));
      mrb_define_method(mrb, mrb_cef.js_object_class, "[]=", mrb_cef_v8_js_object_set_property, MRB_ARGS_REQ(2));
      mrb_define_method(mrb, mrb_cef.js_object_class, "apply", mrb_cef_v8_js_object_apply, MRB_ARGS_REQ(2));

      // Cef::V8::JsException class
      mrb_define_method(mrb, mrb_cef.js_exception_class, "message", mrb_cef_v8_exception_get_message, MRB_ARGS_NONE());
   }
#ifdef __cplusplus
}
#endif
