#include <string>
#include "mruby.h"
#include "mruby/compile.h"
#include "mruby_cef.h"
#include "mruby_v8_handler.h"

using namespace std;

void free_cef_ref_ptr_to_v8_exception(mrb_state* mrb, void* ptr) {
  /*
   * The destructor of the CefRefPtr will decrement V8's ref count on the
   * pointed-to object, possibly freeing it for GC.
   */
  delete static_cast<CefRefPtr<CefV8Exception>*>(ptr);
};

const mrb_data_type cef_ref_ptr_to_v8_exception_type = {
   "cef_ref_ptr_type", free_cef_ref_ptr_to_v8_exception
};

mrb_value mruby_box_cef_v8_exception_ref(mrb_state* mrb, CefRefPtr<CefV8Exception> ref) {
   CefRefPtr<CefV8Exception>* ptr = new CefRefPtr<CefV8Exception>();
   *ptr = ref;
   RData* data = mrb_data_object_alloc(mrb, JSException_class(mrb), ptr, &cef_ref_ptr_to_v8_exception_type);
   return mrb_obj_value(data);
}

CefRefPtr<CefV8Exception>
mruby_unbox_cef_v8_exception_ref(mrb_state* mrb, mrb_value cef_v8_exception) {
   return *(CefRefPtr<CefV8Exception>*) DATA_PTR(cef_v8_exception);
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
   CefRefPtr<CefV8Exception> v8_exception = mruby_unbox_cef_v8_exception_ref(mrb, self);
#if defined(_WIN32) || defined(_WIN64)
   return mrb_str_new_cstr(mrb, v8_exception->GetMessageA().ToString().c_str());
#else
  return mrb_str_new_cstr(mrb, v8_exception->GetMessage().ToString().c_str());
#endif
}

#ifdef __cplusplus
extern "C" {
#endif

   void mrb_mruby_js_exception_init(mrb_state* mrb) {
      RClass* JSException = mrb_define_class_under(mrb, JS_module(mrb), "Exception", mrb->eStandardError_class);
      mrb_define_method(mrb, JSException, "message", mrb_cef_v8_exception_get_message, MRB_ARGS_NONE());
   }

#ifdef __cplusplus
}
#endif
