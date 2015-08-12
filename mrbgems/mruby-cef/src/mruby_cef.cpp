#include <string>
#include "mruby.h"
#include "mruby/compile.h"
#include "mruby_cef.h"
#include "mruby_v8_handler.h"

using namespace std;

struct mrb_cef mrb_cef;

const mrb_data_type cef_ref_ptr_type = {
   "cef_ref_ptr_type", free_cef_ref_ptr
};

void free_cef_ref_ptr(mrb_state* mrb, void* ptr) {
   free(ptr);
};

#ifdef __cplusplus
extern "C" {
#endif

   void mrb_mruby_cef_gem_init(mrb_state* mrb) {
      mrb_cef.cef_module = mrb_define_module(mrb, "Cef");
      mrb_mruby_cef_v8_init(mrb, mrb_cef.cef_module);
   }

   void mrb_mruby_cef_gem_final(mrb_state* mrb) {}

#ifdef __cplusplus
}
#endif
