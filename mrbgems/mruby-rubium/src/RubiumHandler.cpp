#include "RubiumHandler.h"
#include "include/wrapper/cef_helpers.h"

using namespace std;

namespace {
   RubiumHandler* g_instance = NULL;
}  // namespace

RubiumHandler::RubiumHandler() {
   DCHECK(!g_instance);
   g_instance = this;
}

RubiumHandler::~RubiumHandler() {
   g_instance = NULL;
}

// static
RubiumHandler* RubiumHandler::GetInstance() {
   return g_instance;
}

