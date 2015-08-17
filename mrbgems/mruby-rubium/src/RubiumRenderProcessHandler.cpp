#define _CRT_SECURE_NO_DEPRECATE

#include <iostream>
#include "stdio.h"
#include "errno.h"
#include "mruby.h"
#include "mruby_rubium.h"
#include "mruby/compile.h"
#include "mruby_cef.h"
#include "RubiumRenderProcessHandler.h"

RubiumRenderProcessHandler::RubiumRenderProcessHandler() {}

void RubiumRenderProcessHandler::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) {
   mrb_state* mrb = mrb_for_thread();
   mrb_funcall(mrb, mrb_obj_value(mrb->kernel_module), "define_ruby_js_function", 0);
}
