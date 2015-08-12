#ifndef RUBYCHROMEAPP_H
#define RUBYCHROMEAPP_H

#include <string>
#include "include/cef_app.h"

class RubiumApp : public CefApp,
   public CefBrowserProcessHandler {
public:
   RubiumApp();

   // CefApp methods:
   virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE
   {
      return this;
   }

   virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() OVERRIDE;

   // CefBrowserProcessHandler methods:
   virtual void OnContextInitialized() OVERRIDE;

private:
   // Include the default reference counting implementation.
   IMPLEMENT_REFCOUNTING(RubiumApp);
};

#endif /* RUBYCHROMEAPP_H */
