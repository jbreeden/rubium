#ifndef RUBY_FN_HANDLER_H
#define RUBY_FN_HANDLER_H

#include <string>
#include "mruby.h"
#include "include/cef_v8.h"

// Defines a CefV8Handler for a `ruby` function, callable from js,
// that returns the result of running the ruby script passed in as
// a string. Any thrown exception is converted to a string then
// rethrown as a javascript exception.
class RubyFnHandler : public CefV8Handler {
public:
   mrb_state* mrb;
   std::string name = "ruby";

   RubyFnHandler(mrb_state* mrb);

   virtual bool Execute(const CefString& name,
      CefRefPtr<CefV8Value> object,
      const CefV8ValueList& arguments,
      CefRefPtr<CefV8Value>& retval,
      CefString& exception) OVERRIDE;

      // Provide the reference counting implementation for this class.
   IMPLEMENT_REFCOUNTING(RubyFnHandler);
};

#endif /* RUBY_FN_HANDLER_H */
