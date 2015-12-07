#include <string>
#include "mruby.h"
#include "mruby/compile.h"
#include "mruby_cef.h"
#include "mruby_v8_handler.h"

using namespace std;

//<
// Module Cef::V8
// ==============
//>

//<
// ### `::create_undefined`
// - Creates a Value containing a JavaScript undefined
//>
static mrb_value
mrb_cef_v8_create_undefined(mrb_state *mrb, mrb_value self) {
   return mruby_box_cef_v8_value_ref(mrb, CefV8Value::CreateUndefined());
}

//<
// ### `::create_null`
// - Creates a Value containing a JavaScript null
//>
static mrb_value
mrb_cef_v8_create_null(mrb_state *mrb, mrb_value self) {
   return mruby_box_cef_v8_value_ref(mrb, CefV8Value::CreateNull());
}

//<
// ### `::create_bool(value)`
// - Creates a Value containing a JavaScript bool
// - Args
//   + `value`: If `nil` or `flase`, returns a JavaScript fale, else returns a JavaScript true
//>
static mrb_value
mrb_cef_v8_create_bool(mrb_state *mrb, mrb_value self) {
   mrb_value param;
   mrb_get_args(mrb, "o", &param);
   return mruby_box_cef_v8_value_ref(mrb, CefV8Value::CreateBool(mrb_test(param)));
}

//<
// ### `::create_int(value)`
// - Creates a Value containing a JavaScript int
// - Args
//   + `value`: A number to convert to a JavaScript int
//>
static mrb_value
mrb_cef_v8_create_int(mrb_state *mrb, mrb_value self) {
   mrb_value int_param;
   mrb_get_args(mrb, "i", &int_param);
   return mruby_box_cef_v8_value_ref(mrb, CefV8Value::CreateInt(int_param.value.i));
}

//<
// ### `::create_float(value)`
// - Creates a Value containing a JavaScript float
// - Args
//   + `value`: A number to convert to a JavaScript float
//>
static mrb_value
mrb_cef_v8_create_float(mrb_state *mrb, mrb_value self) {
   mrb_value param;
   mrb_get_args(mrb, "f", &param);
   return mruby_box_cef_v8_value_ref(mrb, CefV8Value::CreateDouble(mrb_float(param)));
}

//<
// ### `::create_string(rb_string)`
// - Creates a Value containing a JavaScript string
// - Args
//   + `rb_string`, the ruby string to convert to a JavaScript value
//>
static mrb_value
mrb_cef_v8_create_string(mrb_state *mrb, mrb_value self) {
   mrb_value rbString;
   mrb_get_args(mrb, "S", &rbString);
   char* cString = mrb_str_to_cstr(mrb, rbString);
   return mruby_box_cef_v8_value_ref(mrb, CefV8Value::CreateString(cString));
}

//<
// ### `::create_object`
// - Creates a Value containing a JavaScript object
//>
static mrb_value
mrb_cef_v8_create_object(mrb_state *mrb, mrb_value self) {
   return mruby_box_cef_v8_value_ref(mrb, CefV8Value::CreateObject(NULL));
}

//<
// ### `::create_function(name, &block)`
// - Creates a Value containing a JavaScript function
// - Args
//   + `name`: The name of the function
//   + `block`: A block accepting an array of Values as the only parameter. <br/>
//              This block will be executed when the funciton is called from JavaScript. <br/>
//              If the block returns a Value, it will be unwrapped and returned to the caller. <br/>
//              If the block returns anything else, `undefined` will be returned to JavaScript. <br/>
//              If the block throws, a JavaScript Error object will be thrown containing the result <br/>
//              of calling `to_s` on the ruby exception. <br/>
//>
static mrb_value
mrb_cef_v8_create_function(mrb_state *mrb, mrb_value self) {
   const char* name;
   mrb_bool name_given;
   mrb_value block;

   mrb_get_args(mrb, "|z?&", &name, &name_given, &block);

   if (!name_given) {
     name = "(anonymous)";
   }
   if (mrb_nil_p(block)) {
     mrb_raise(mrb, mrb->eStandardError_class, "JS.create_function expects a block");
     return mrb_nil_value();
   }

   auto handler = new MRubyV8Handler(mrb, name, block);
   CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction(name, handler);
   mrb_value wrapped_function = mruby_box_cef_v8_value_ref(mrb, func);

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

   return mruby_box_cef_v8_value_ref(mrb, context->GetGlobal());
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
      rb_return_value = mruby_box_cef_v8_value_ref(mrb, retval);
   }
   else if (exc.get() && mrb_test(block)) {
      rb_return_value = mrb_funcall(mrb, block, "call", 1, mruby_box_cef_v8_exception_ref(mrb, exc));
   }
   return rb_return_value;
}

#ifdef __cplusplus
extern "C" {
#endif

  void mrb_mruby_js_init(mrb_state* mrb) {
    RClass* JS = mrb_define_module(mrb, "JS");

    // JS module
    mrb_define_class_method(mrb, JS, "window", mrb_cef_v8_get_window, MRB_ARGS_NONE());
    mrb_define_class_method(mrb, JS, "create_undefined", mrb_cef_v8_create_undefined, MRB_ARGS_NONE());
    mrb_define_class_method(mrb, JS, "create_null", mrb_cef_v8_create_null, MRB_ARGS_NONE());
    mrb_define_class_method(mrb, JS, "create_bool", mrb_cef_v8_create_bool, MRB_ARGS_ARG(1, 0));
    mrb_define_class_method(mrb, JS, "create_int", mrb_cef_v8_create_int, MRB_ARGS_ARG(1, 0));
    mrb_define_class_method(mrb, JS, "create_float", mrb_cef_v8_create_float, MRB_ARGS_ARG(1, 0));
    mrb_define_class_method(mrb, JS, "create_string", mrb_cef_v8_create_string, MRB_ARGS_ARG(1, 0));
    mrb_define_class_method(mrb, JS, "create_object", mrb_cef_v8_create_object, MRB_ARGS_ARG(0, 0));
    mrb_define_class_method(mrb, JS, "create_function", mrb_cef_v8_create_function, MRB_ARGS_ARG(0, 2));
    mrb_define_class_method(mrb, JS, "function", mrb_cef_v8_create_function, MRB_ARGS_ARG(0, 2));
    mrb_define_class_method(mrb, JS, "eval", mrb_cef_v8_eval, MRB_ARGS_ARG(1, 0));
  }

#ifdef __cplusplus
}
#endif
