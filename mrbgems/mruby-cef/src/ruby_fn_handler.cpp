#include <string>
#include "ruby_fn_handler.h"
#include "mruby/string.h"
#include "mruby/compile.h"
#include "mruby_cef.h"
#include "include/cef_v8.h"

using namespace std;

RubyFnHandler::RubyFnHandler(mrb_state* mrb) {
   this->mrb = mrb;
}

bool
RubyFnHandler::Execute(const CefString& name,
   CefRefPtr<CefV8Value> object,
   const CefV8ValueList& arguments,
   CefRefPtr<CefV8Value>& retval,
   CefString& exception) {

#define THROW_INCORRECT_USAGE \
   exception = "ruby function requires a single argument (the script text to execute)"; \
   return true; 

   if (name == this->name) {

      if (arguments.size() != 1) {
         THROW_INCORRECT_USAGE
      }

      auto script = *arguments.begin();
      if (!script->IsString()) {
         THROW_INCORRECT_USAGE
      }

      mrb_value ret;
      const char* return_class = NULL;
      
      string script_text = script->GetStringValue().ToString();
      
      auto prev_jmp = mrb->jmp;
      mrb->jmp = NULL;
      ret = mrb_load_string(this->mrb, script_text.c_str());
      mrb->jmp = prev_jmp;

      if (this->mrb->exc) {
         exception = mrb_str_to_cstr(this->mrb, mrb_funcall(this->mrb, mrb_obj_value(this->mrb->exc), "to_s", 0));
         this->mrb->exc = NULL;
      }
      else {
         return_class = mrb_obj_classname(this->mrb, ret);
      }

      if (return_class) {
         if (0 == strcmp(return_class, "Cef::V8::JsObject")) {
            retval = mrb_cef_v8_value_unwrap(this->mrb, ret);
         }
         else {
            retval = CefV8Value::CreateUndefined();
         }
      }

      return true;
   }

   // Function does not exist.
   return false;
}
