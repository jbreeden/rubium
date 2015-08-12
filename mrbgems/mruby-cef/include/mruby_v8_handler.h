#ifndef MRUBY_V8_HANDLER
#define MRUBY_V8_HANDLER

#include <string>
#include "mruby.h"
#include "include/cef_v8.h"

class MRubyV8Handler : public CefV8Handler {
public:
   mrb_state* mrb;
   std::string name = "";
   mrb_value block;

   MRubyV8Handler(mrb_state* mrb, std::string name, mrb_value block);

   virtual bool Execute(const CefString& name,
      CefRefPtr<CefV8Value> object,
      const CefV8ValueList& arguments,
      CefRefPtr<CefV8Value>& retval,
      CefString& exception) OVERRIDE;

      // Provide the reference counting implementation for this class.
   IMPLEMENT_REFCOUNTING(MRubyV8Handler);
};

#endif /* MRUBY_V8_HANDLER */
