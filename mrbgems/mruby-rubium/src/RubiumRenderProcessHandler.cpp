#define _CRT_SECURE_NO_DEPRECATE

#include <iostream>
#include "stdio.h"
#include "errno.h"
#include "mruby.h"
#include "mrb_lookup.h"
#include "mruby_rubium.h"
#include "mruby/compile.h"
#include "mruby_cef.h"
#include "RubiumRenderProcessHandler.h"

static MrbLookup mrb_lookup;

RubiumRenderProcessHandler::RubiumRenderProcessHandler() {}

void
RubiumRenderProcessHandler::OnContextCreated(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefV8Context> context) {
  if (!context->IsValid()) return;
  
  mrb_state* mrb = mrb_lookup.open(frame->GetIdentifier());
  
  mrb_funcall(mrb, mrb_obj_value(mrb->kernel_module), "define_ruby_function", 0);
  if (mrb->exc) {
     char* exception = mrb_str_to_cstr(mrb, mrb_funcall(mrb, mrb_obj_value(mrb->exc), "to_s", 0));
     LAMINA_LOG("ERROR: Failed to define ruby function: " << exception);
     mrb->exc = NULL;
  }

  mrb_value content_script = mrb_load_string(mrb, "ENV['CONTENT_SCRIPT']");
  if (!mrb_nil_p(content_script)) {
    const char * native_content_script = mrb_string_value_cstr(mrb, &content_script);
    // LAMINA_LOG("Attempting to load content script: " << native_content_script);
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
      free(buffer);


#if defined(_WIN32)
      if (exc.get() != NULL) {
        LAMINA_LOG("ERROR: On line "
          << exc->GetLineNumber()
          << " of file "
          << native_content_script
          << ". "
          << exc->GetMessageA().ToString());
#else
      if (exc.get() != NULL) {
        LAMINA_LOG("ERROR: On line "
          << exc->GetLineNumber()
          << " of file "
          << native_content_script
          << ". "
          << exc->GetMessage().ToString());
#endif
      }
    }
  }
}

void
RubiumRenderProcessHandler::OnContextReleased(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefV8Context> context) {
  if (!context->IsValid()) return;
  mrb_lookup.close(frame->GetIdentifier());
}
