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

  mrb_value content_script = mrb_load_string(mrb, "ENV['CONTENT_SCRIPT']");
  if (!mrb_nil_p(content_script)) {
    const char * native_content_script = mrb_string_value_cstr(mrb, &content_script);
    LAMINA_LOG("Attempting to load content script: " << native_content_script);
    FILE* script_file = fopen(native_content_script, "r");
    if (script_file == NULL) {
      LAMINA_LOG("ERROR: Could not open content script: " << native_content_script);
      return;
    } else {
      CefRefPtr<CefV8Value> ret;
      CefRefPtr<CefV8Exception> exc;

      fseek(script_file, 0, SEEK_END);
      int length = ftell(script_file);
      rewind(script_file);

      char* buffer = (char*)calloc(1, length + 1);
      if (!buffer) {
        LAMINA_LOG("ERROR: Could not create read buffer for content script: " << native_content_script);
        return;
      }

      if (length != fread(buffer, 1, length, script_file)) {
        LAMINA_LOG("ERROR: Could not read content script: " << native_content_script);
        return;
      }

      context->Eval(buffer, ret, exc);

      if (exc.get() != NULL) {
        LAMINA_LOG("ERROR: On line "
          << exc->GetLineNumber()
          << " of file "
          << native_content_script
          << ". "
#if defined(_WIN32)
          << exc->GetMessageA().ToString());
#else
          << exc->GetMessage().ToString());
#endif
      }
    }
  }
}
