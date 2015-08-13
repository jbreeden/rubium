#define _CRT_SECURE_NO_DEPRECATE

#include <iostream>
#include "stdio.h"
#include "errno.h"
#include "mruby.h"
#include "mruby_rubium.h"
#include "mruby/compile.h"
#include "mruby_cef.h"
#include "RubiumRenderProcessHandler.h"
#include "ruby_fn_handler.h"

RubiumRenderProcessHandler::RubiumRenderProcessHandler() {}

void RubiumRenderProcessHandler::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) {
   auto mrb = mrb_for_thread();
   CefRefPtr<RubyFnHandler> rubyHandler = new RubyFnHandler(mrb);
   auto ruby_fn = CefV8Value::CreateFunction("ruby", rubyHandler);
   context->GetGlobal()->SetValue(CefString("ruby"), ruby_fn, CefV8Value::PropertyAttribute::V8_PROPERTY_ATTRIBUTE_NONE);
}
