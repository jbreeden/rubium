#include "mruby_v8_handler.h"
#include "mruby/array.h"
#include "mruby_cef.h"
#include "include/cef_v8.h"

using namespace std;

MRubyV8Handler::MRubyV8Handler(mrb_state* mrb, string name, mrb_value block) {
   this->mrb = mrb;
   this->name = name;
   this->block = block;
}

bool
MRubyV8Handler::Execute(const CefString& name,
   CefRefPtr<CefV8Value> object,
   const CefV8ValueList& arguments,
   CefRefPtr<CefV8Value>& retval,
   CefString& exception) {

   if (name == this->name) {
      mrb_value rb_args = mrb_ary_new_capa(mrb, 5);

      for (auto it = arguments.begin(); it != arguments.end(); ++it) {
         mrb_ary_push(mrb, rb_args, mrb_cef_v8_value_wrap(mrb, *it));
      }

      mrb_value ret;
      const char* return_class = NULL;

      if (this->block.tt == MRB_TT_PROC) {
         // Hack, or standard practice? Need to talk to mruby guys
         // (This prevents any exceptions from throwing all the way up to
         //  the mrb_load_file call that starts rubium. See mruby/vm.c mrb_funcall_with_block)
         auto prev_jmp = mrb->jmp;
         mrb->jmp = NULL;
         ret = mrb_funcall(this->mrb, this->block, "call", 1, rb_args);
         mrb->jmp = prev_jmp;
      }

      if (mrb->exc) {
         exception = mrb_str_to_cstr(mrb, mrb_funcall(mrb, mrb_obj_value(mrb->exc), "to_s", 0));
         mrb->exc = NULL;
      }
      else {
         return_class = mrb_obj_classname(this->mrb, ret);
      }

      if (return_class) {
         if (0 == strcmp(return_class, "Cef::V8::JsObject")) {
            retval = mrb_cef_v8_value_unwrap(mrb, ret);
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
