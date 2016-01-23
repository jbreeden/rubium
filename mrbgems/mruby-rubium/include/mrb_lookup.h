#ifndef MRB_LOOKUP_H
#define MRB_LOOKUP_H

#include <stdint.h>
#include <map>
#include "mruby.h"
#include "include/base/cef_lock.h"

class MrbLookup {
public:
  mrb_state* open(int64_t id);
  bool is_open(int64_t id);
  void close(int64_t id);

private:
  std::map<int64_t, mrb_state *> mrb_map;
  base::Lock lock;
};

#endif
