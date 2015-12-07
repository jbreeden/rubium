#ifndef MRUBY_CEF_V8_H
#define MRUBY_CEF_V8_H

#include "mruby.h"
#include "mruby/string.h"
#include "mruby/variable.h"
#include "mruby/data.h"
#include "include/cef_v8.h"

#define JS_module(mrb) mrb_module_get(mrb, "JS")
#define JSValue_class(mrb) mrb_class_get_under(mrb, JS_module(mrb), "Value")
#define JSException_class(mrb) mrb_class_get_under(mrb, JS_module(mrb), "Exception")

mrb_value
mruby_box_cef_v8_value_ref(mrb_state* mrb, CefRefPtr<CefV8Value> ref);

CefRefPtr<CefV8Value>
mruby_unbox_cef_v8_value_ref(mrb_state* mrb, mrb_value cef_v8value);

mrb_value
mruby_box_cef_v8_exception_ref(mrb_state* mrb, CefRefPtr<CefV8Exception> ref);

CefRefPtr<CefV8Exception>
mruby_unbox_cef_v8_exception_ref(mrb_state* mrb, mrb_value cef_v8value);

#ifdef __cplusplus
extern "C" {
#endif

void
mrb_mruby_js_init(mrb_state* mrb);

void
mrb_mruby_js_value_init(mrb_state* mrb);

void
mrb_mruby_js_exception_init(mrb_state* mrb);

#ifdef __cplusplus
}
#endif

#endif
