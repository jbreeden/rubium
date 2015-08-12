#ifndef MRUBY_CEF_V8_H
#define MRUBY_CEF_V8_H

#include "mruby.h"
#include "mruby/string.h"
#include "mruby/variable.h"
#include "mruby/data.h"
#include "include/cef_v8.h"

struct mrb_cef {
   RClass* cef_module;
   RClass* v8_module;
   RClass* js_object_class;
   RClass* js_exception_class;
};

// TODO: Don't use global variable to store these classes/modules
extern struct mrb_cef mrb_cef;

extern const mrb_data_type cef_ref_ptr_type;

void
free_cef_ref_ptr(mrb_state* mrb, void* ptr);

mrb_value 
mrb_cef_v8_value_wrap(mrb_state* mrb, CefRefPtr<CefV8Value> ref);

CefRefPtr<CefV8Value> 
mrb_cef_v8_value_unwrap(mrb_state* mrb, mrb_value cef_v8value);

mrb_value 
mrb_cef_v8_exception_wrap(mrb_state* mrb, CefRefPtr<CefV8Exception> ref);

CefRefPtr<CefV8Exception> 
mrb_cef_v8_exception_unwrap(mrb_state* mrb, mrb_value cef_v8value);

#ifdef __cplusplus
extern "C" {
#endif

void
mrb_mruby_cef_v8_init(mrb_state* mrb, RClass* cef_module);

#ifdef __cplusplus
}
#endif

#endif
