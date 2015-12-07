#include <string>
#include "mruby.h"
#include "mruby/compile.h"
#include "mruby_cef.h"
#include "mruby_v8_handler.h"

using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

   void mrb_mruby_cef_gem_init(mrb_state* mrb) {
      mrb_mruby_js_init(mrb);
      mrb_mruby_js_value_init(mrb);
      mrb_mruby_js_exception_init(mrb);
   }

   void mrb_mruby_cef_gem_final(mrb_state* mrb) {}

#ifdef __cplusplus
}
#endif
