#include "mruby.h"
#include "mruby/class.h"
#include "mruby/string.h"
#include "mruby/variable.h"
#include "mruby/data.h"
#include "mruby/compile.h"
#include "mruby/array.h"
#include "mruby_rubium.h"
#include "rubium_opt.h"
#include <cstdio>
#include <iostream>

using namespace std;

#define GET_LAMINA_OPTIONS_IV(iv) \
   auto mrb = mrb_for_thread();\
   auto rubium_module = mrb_module_get(mrb, "Rubium");\
   mrb_value var = mrb_iv_get(mrb, mrb_obj_value(rubium_module), mrb_intern_cstr(mrb, iv));\
   if (mrb->exc) { \
      LAMINA_LOG("!!! Error !!! " << mrb_str_to_cstr(mrb, mrb_funcall(mrb, mrb_obj_value(mrb->exc), "to_s", 0))); \
   }

#ifdef __cplusplus
extern "C" {
#endif

char**
rubium_opt_js_extensions() {
   GET_LAMINA_OPTIONS_IV("@js_extensions");
   int length = mrb_ary_len(mrb, var);
   auto files = (char**) malloc(sizeof(char*) * (length + 1));
   int i = 0;
   for (; i < length; ++i) {
      files[i] = mrb_str_to_cstr(mrb, mrb_ary_entry(var, i));
   }
   files[i] = NULL;
   return files;
}

#ifdef __cplusplus
}
#endif
