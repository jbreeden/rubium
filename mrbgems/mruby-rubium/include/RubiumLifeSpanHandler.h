#ifndef LAMINA_LIFE_SPAN_HANDLER_H
#define LAMINA_LIFE_SPAN_HANDLER_H

#include <list>
#include "include/cef_life_span_handler.h"

typedef std::list<CefRefPtr<CefBrowser> > BrowserList;

class RubiumLifeSpanHandler : public CefLifeSpanHandler {
public:
   RubiumLifeSpanHandler();
   ~RubiumLifeSpanHandler();

   // Provide access to the single global instance of this object.
   static RubiumLifeSpanHandler* GetInstance();

   virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
   virtual bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
   virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

   // Request that all existing browser windows close.
   void CloseAllBrowsers(bool force_close);

   bool IsClosing() const { return is_closing_; }

   // TODO: Should probably just expose a GetBrowser/GetWindow function..?
   void ExecuteJavaScript(char* script, char* window_pattern, bool firstMatch);

private:
   // List of existing browser windows. Only accessed on the CEF UI thread.
   static BrowserList browser_list_;

   bool is_closing_;

   // Include the default reference counting implementation.
   IMPLEMENT_REFCOUNTING(RubiumLifeSpanHandler);
};

#endif /* LAMINA_LIFE_SPAN_HANDLER_H */
