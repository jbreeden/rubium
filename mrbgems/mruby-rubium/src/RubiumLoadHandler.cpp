#define _CRT_SECURE_NO_WARNINGS

#include "RubiumLoadHandler.h"
#include "include/wrapper/cef_helpers.h"
#include "include/cef_v8.h"
#include "mruby.h"
#include "mruby/compile.h"

namespace {
   RubiumLoadHandler* g_instance = NULL;
}

RubiumLoadHandler::RubiumLoadHandler() {
   DCHECK(!g_instance);
}

RubiumLoadHandler::~RubiumLoadHandler() {
   g_instance = NULL;
}

// static
RubiumLoadHandler* 
RubiumLoadHandler::GetInstance() {
   if (g_instance == NULL) {
      g_instance = new RubiumLoadHandler;
   }
   return g_instance;
}

void RubiumLoadHandler::OnLoadError(CefRefPtr<CefBrowser> browser,
   CefRefPtr<CefFrame> frame,
   ErrorCode errorCode,
   const CefString& errorText,
   const CefString& failedUrl) {
   CEF_REQUIRE_UI_THREAD();

   // Don't display an error for downloaded files.
   if (errorCode == ERR_ABORTED)
      return;

   // Display a load error message.
   std::stringstream ss;
   ss << "<html><body bgcolor=\"white\">"
      "<h2>Failed to load URL " << std::string(failedUrl) <<
      " with error " << std::string(errorText) << " (" << errorCode <<
      ").</h2></body></html>";
   frame->LoadString(ss.str(), failedUrl);
}
