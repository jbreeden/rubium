#include "mrb_lookup.h"

#define CHECKPOINT(msg) printf("%s %d " msg "\n", __FUNCTION__, __LINE__);

mrb_state *
MrbLookup::open(int64_t id) {
  CHECKPOINT();
  lock.Acquire();
  mrb_state* mrb = NULL;
  std::map<int64_t, mrb_state*>::iterator iter = mrb_map.find(id);
  if (iter == mrb_map.end()) {
    mrb = mrb_open();
    printf("mrb_open(id = %lld);\n", id, mrb);
    mrb_map[id] = mrb;
  } else {
    mrb = iter->second;
  }
  lock.Release();
  CHECKPOINT();
  return mrb;
}

bool
MrbLookup::is_open(int64_t id) {
  CHECKPOINT();
  bool result = false;
  lock.Acquire();
  std::map<int64_t, mrb_state*>::iterator iter = mrb_map.find(id);
  if (iter == mrb_map.end()) {
    result = false;
  } else {
    result = true;
  }
  lock.Release();
  CHECKPOINT();
  return result;
}

void
MrbLookup::close(int64_t id) {
  CHECKPOINT();
  lock.Acquire();
  std::map<int64_t, mrb_state*>::iterator iter = mrb_map.find(id);
  if (iter != mrb_map.end()) {
    printf("mrb_close(id = %lld);\n", id);
    mrb_close(iter->second);
    mrb_map.erase(iter);
  }
  CHECKPOINT();
  lock.Release();
}
