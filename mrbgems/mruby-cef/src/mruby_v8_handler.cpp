#include "mruby_v8_handler.h"
#include "mruby/array.h"
#include "mruby_cef.h"
#include "include/cef_v8.h"

using namespace std;

MRubyV8Handler::MRubyV8Handler(mrb_state* mrb, string name, mrb_value block) {
  this->mrb = mrb;
  this->name = name;
  this->block = block;
  mrb_gc_register(this->mrb, this->block);
}

MRubyV8Handler::~MRubyV8Handler() {
  mrb_gc_unregister(this->mrb, this->block);
}

bool
MRubyV8Handler::Execute(const CefString& name,
  CefRefPtr<CefV8Value> object,
  const CefV8ValueList& arguments,
  CefRefPtr<CefV8Value>& retval,
  CefString& exception) {

  if (name == this->name) {
    int argc = arguments.size();
    mrb_value* argv = (mrb_value*)malloc(sizeof(mrb_value) * argc);

    // for (auto it = arguments.begin(); it != arguments.end(); ++it) {
    //   mrb_ary_push(mrb, rb_args, mruby_box_cef_v8_value_ref(mrb, *it));
    // }

    for (int i = 0; i < argc; i++) {
      argv[i] = mruby_box_cef_v8_value_ref(mrb, arguments.at(i));
    }

    mrb_value ret;

    // TODO: Mruby has implemented mrb_protect, at long last. Need to port this.
    auto prev_jmp = mrb->jmp;
    mrb->jmp = NULL;
    ret = mrb_funcall_argv(this->mrb, this->block, mrb_intern_lit(mrb, "call"), argc, argv);
    free(argv);
    mrb->jmp = prev_jmp;

    if (mrb->exc) {
      exception = mrb_str_to_cstr(mrb, mrb_funcall(mrb, mrb_obj_value(mrb->exc), "to_s", 0));
      mrb->exc = NULL;
    } else {
      if (mrb_obj_is_kind_of(mrb, ret, JSValue_class(mrb))) {
        retval = mruby_unbox_cef_v8_value_ref(mrb, ret);
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
