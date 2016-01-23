#include "mrb_lookup.h"

mrb_state *
MrbLookup::open(int64_t id) {
  lock.Acquire();
  mrb_state* mrb = NULL;
  std::map<int64_t, mrb_state*>::iterator iter = mrb_map.find(id);
  if (iter == mrb_map.end()) {
    // printf("Creating mrb %lld\n", id);
    mrb = mrb_open();
    mrb_map[id] = mrb;
  } else {
    mrb = iter->second;
  }
  lock.Release();
  return mrb;
}

bool
MrbLookup::is_open(int64_t id) {
  bool result = false;
  lock.Acquire();
  std::map<int64_t, mrb_state*>::iterator iter = mrb_map.find(id);
  if (iter == mrb_map.end()) {
    result = false;
  } else {
    result = true;
  }
  lock.Release();
  return result;
}

void
MrbLookup::close(int64_t id) {
  lock.Acquire();
  std::map<int64_t, mrb_state*>::iterator iter = mrb_map.find(id);
  if (iter != mrb_map.end()) {
    // printf("Closing mrb %lld\n", id);
    mrb_close(iter->second);
    mrb_map.erase(iter);
  }
  lock.Release();
}
