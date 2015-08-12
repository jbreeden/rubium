#ifndef LAMINAHANDLER_H
#define LAMINAHANDLER_H

#include <iostream>
#include <string>
#include <iostream>
#include <sstream>
#include <list>
#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#endif
#include "include/cef_client.h"
#include "include/wrapper/cef_helpers.h"
#include "mruby_rubium.h"
#include "RubiumLifeSpanHandler.h"
#include "RubiumLoadHandler.h"
#include "rubium_opt.h"

using namespace std;

// TODO: Should be "RubiumClient" and all handlers should be extracted
class RubiumHandler : public CefClient,
   public CefDisplayHandler {
public:
   RubiumHandler();
   ~RubiumHandler();

   // Provide access to the single global instance of this object.
   static RubiumHandler* GetInstance();

   // CefClient methods:
   virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE {
      return this;
   }
   virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE {
      return RubiumLifeSpanHandler::GetInstance();
   }
   virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE {
      return RubiumLoadHandler::GetInstance();
   }

   // CefDisplayHandler methods:
   virtual void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) OVERRIDE {
      // TODO: Set window title based on browsers main frame title
   }

private:
   // Include the default reference counting implementation.
   IMPLEMENT_REFCOUNTING(RubiumHandler);
};

#endif /*LAMINAHANDLER_H*/
