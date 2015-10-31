/* TODO
 * Separate the components in this file.
 */

/* System Includes */
#if defined(_WIN32) || defined(_WIN64)
  #include <ws2tcpip.h>
#elif !defined(__APPLE__)
  #include <X11/Xlib.h>
#endif
#include <string>
#include <stdexcept>

/* CEF Includes */
#include "include/cef_app.h"
#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#if defined(_WIN32) || defined(_WIN64)
  #include "include/cef_sandbox_win.h"
#endif

/* APR Includes */
#include "apr_file_io.h"
#include "apr_portable.h"
#include "apr_thread_proc.h"
#include "apr_thread_mutex.h"
#include "apr_network_io.h"
#include "apr_env.h"

/* MRuby Includes */
#include "mruby.h"
#include "mruby/string.h"
#include "mruby/compile.h"

/* Rubium Includes */
#include "mruby_rubium.h"
#include "RubiumHandler.h"
#include "RubiumApp.h"

using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

using namespace std;

int g_argc = 0;
char** g_argv = NULL;
CefRefPtr<CefCommandLine> g_command_line;

// Notes on sandboxing:
//  - On windows, this currently only works if the sub-process exe is the same exe as the main process
//  - With sandboxing enabled, render processes cannot access system resources like networking/files
//  - Currently running without sandboxing to allow the render process to load mruby scripts
//    + This also allows javascript extensions to access resources without going through IPC
//    + This isn't great from a security perspective. Will need to work around this and re-enable sandboxing
//      as soon as a decent story is available for creating JS extensions with mruby scripts in a sandboxed environment.
//#ifdef _WIN32
//#define CEF_USE_SANDBOX
//#endif

/******************************************************************
 * XError Handlers (so app doesn't crash for no *good* reason)
 ******************************************************************/

#if !defined(_WIN32) && !defined(__APPLE__)
namespace {

int XErrorHandlerImpl(Display *display, XErrorEvent *event) {
  LOG(WARNING)
        << "X error received: "
        << "type " << event->type << ", "
        << "serial " << event->serial << ", "
        << "error_code " << static_cast<int>(event->error_code) << ", "
        << "request_code " << static_cast<int>(event->request_code) << ", "
        << "minor_code " << static_cast<int>(event->minor_code);
  return 0;
}

int XIOErrorHandlerImpl(Display *display) {
  return 0;
}

}
#endif

/*********************************
 * MRB Thread Instance Management
 *********************************/

class MrbThreadPair {
public:
  mrb_state * mrb;
  apr_os_thread_t thread;

  MrbThreadPair(apr_os_thread_t t, mrb_state* mrb) {
    this->mrb = mrb;
    this->thread = t;
  }
};

class MrbThreadManager {
public:
  MrbThreadManager() {
    this->mrb_thread_pairs = std::vector<MrbThreadPair>();
  }

  void check_mutex() {
    // LAMINA_LOG("MrbThreadManager: Making sure mutex exists");
    if (MrbThreadManager::mutex == NULL) {
      // LAMINA_LOG("MrbThreadManager: Mutex is NULL, creating it");
      apr_pool_t* mutex_pool;
      apr_pool_create(&mutex_pool, NULL);
      apr_thread_mutex_create(&MrbThreadManager::mutex, APR_THREAD_MUTEX_DEFAULT, mutex_pool);
    }
    // LAMINA_LOG("MrbThreadManager: Mutex check done");
  }

  mrb_state* get_mrb_for_thread() {
    // LAMINA_LOG("MrbThreadManager: Getting MRB for current thread");
    check_mutex();
    apr_thread_mutex_lock(MrbThreadManager::mutex);
    apr_os_thread_t t = apr_os_thread_current();
    mrb_state* mrb = NULL;

    for (int i = 0; i < mrb_thread_pairs.size(); i++) {
      if (apr_os_thread_equal(t, mrb_thread_pairs[i].thread)) {
        // LAMINA_LOG("Found existing MRB instance for current thread. #" << i);
        mrb = mrb_thread_pairs[i].mrb;
      }
    }

    if (mrb == NULL) {
      // LAMINA_LOG("No MRB instance found for current thread, creating one");
      mrb_state* mrb = mrb_open();
      this->set_mrb_for_thread_no_lock(mrb);
    }

    apr_thread_mutex_unlock(MrbThreadManager::mutex);
    return mrb;
  }

private:
  void set_mrb_for_thread_no_lock(mrb_state* mrb) {
    apr_os_thread_t t = apr_os_thread_current();
    mrb_thread_pairs.push_back(MrbThreadPair(t, mrb));
  }

  std::vector<MrbThreadPair> mrb_thread_pairs;
  static apr_thread_mutex_t* mutex;
};

apr_thread_mutex_t* MrbThreadManager::mutex = NULL;

static MrbThreadManager mrb_thread_manager;

mrb_state* mrb_for_thread() {
   return mrb_thread_manager.get_mrb_for_thread();
}

/*********************************
 * Rubium Ruby Module Functions
 *********************************/

void mrb_mruby_rubium_gem_init(mrb_state* mrb) {
  // Nothing to do
}

void mrb_mruby_rubium_gem_final(mrb_state* mrb) {}

void rubium_check_usage() {
  if (g_command_line->HasSwitch("help")) {
    printf(
      "\n"
      "Rubium\n"
      "\n"
      "Usage: rubium [OPTIONS...] \n"
      "\n"
      "DESCRIPTION\n"
      "\n"
      "  Desktop application development with Ruby, JavaScript, HTML, SVG, and CSS.\n"
      "\n"
      "OPTIONS\n"
      "\n"
      "  --cache-path=DIR\n"
      "    The directory where rubium will persist cookies, local storage, etc.\n"
      "    If this option is not supplied, no local storage will be saved.\n"
      "    The directory, but no parents, will be created if it does not exist.\n"
      "\n"
      "  --content-script=JS_FILE\n"
      "    Path to a javascript file to be injected into each browser window.\n"
      "\n"
      "  --dev-tools\n"
      "    Open the Chromium dev tools in a separate window on launch.\n"
      "\n"
      "  --help\n"
      "    Show this help text.\n"
      "\n"
      "  --url=URL\n"
      "    On startup, open URL instead of searching for an index.html file.\n"
      "    URL may be a local file, using the file:// protocol, or a remote resource\n"
      "    addressed with http:// or https://\n"
      "\n"
      "APPLICATION STARTUP\n"
      "\n"
      "  The options for executing an application with Rubium are as follows:\n"
      "\n"
      "  1) Specify the --url=URL parameter.\n"
      "     Described above, under the \"OPTIONS\" heading."
      "\n"
      "  2) Run the `rubium` command in a folder with an index.html file.\n"
      "     If this file is found, it is opened in a new window, the contents are\n"
      "     displayed, and any scripts within are executed exactly as in a typical\n"
      "     web application. This includes loading external scripts with <script>\n"
      "     tags.\n"
      "\n"
      "  3) Run the `rubium` command in a folder with no index.html file.\n"
      "     In this case, the folder contents are displayed. An application may\n"
      "     be launched by navigating to the file and selecting it.\n"
      "\n"
      "RUNTIME ENVIRONMENT\n"
      "\n"
      "  Multiple windows may be opened by the application. Each window will have it's\n"
      "  own Ruby context as well as the usual JavaScript context. It is important to\n"
      "  note that each Ruby context is indeed an independent VM instance, and so no\n"
      "  variables may be shared among them. You may, however, use the browser's built\n"
      "  in window messaging API to communicate among windows.\n"
      "\n"
      "JAVASCRIPT & RUBY INTEROP\n"
      "\n"
      "  To call Ruby from JavaScript, pass a string to the `ruby` function.\n"
      "\n"
      "    Ex: Writing the working directory contents to the document\n"
      "    ----------------------------------------------------------\n"
      "    |  document.write(ruby('`ls`'));\n"
      "    ----------------------------------------------------------\n"
      "\n"
      "  Note that any value returned from Ruby to JavaScript must be converted to a\n"
      "  JavaScript object. Rubium will do this automatically for built in types like\n"
      "  Fixnums & booleans. If it does not know how to convert a value, undefined is\n"
      "  returned instead. You can convert values manually using the JS.create_*\n"
      "  family of functions.\n"
      "\n"
      "  To interact with JavaScript objects - including the DOM - from Ruby,\n"
      "  use the `JS` module. (Property access, method calls, and block arguments\n"
      "  should all behave the way you expect.)\n"
      "\n"
      "    Ex: Make an alert call on `window` explicitly from Ruby\n"
      "    ----------------------------------------------------------\n"
      "    |  JS.window.alert('Hell, from Ruby!')\n"
      "    ----------------------------------------------------------\n"
      "\n"
      "  As with pure JavaScript, any methods called on the JS module are delegated\n"
      "  to the window object by default.\n"
      "\n"
      "    Ex: Make an alert call on `window` implicitly from Ruby\n"
      "    ----------------------------------------------------------\n"
      "    |  JS.alert('Hell, from Ruby!')\n"
      "    ----------------------------------------------------------\n"
      "\n"
      "  Any block parameters to a JavaScript function are converted to function\n"
      "  arguments automatically.\n"
      "\n"
      "    Ex: Attaching a DOM event handler from Ruby\n"
      "    ----------------------------------------------------------\n"
      "    |  JS.document.addEventListener('onload') do |event|\n"
      "    |    JS.alert('Document loaded!')\n"
      "    |  end\n"
      "    ----------------------------------------------------------\n"
      "\n"
      "SEE ALSO:\n"
      "\n"
      "  1) http://github.com/mruby/mruby\n"
      "  The Ruby implementation embedded into Rubium.\n"
      "\n"
      "  2) http://github.com/jbreeden/mruby-apr\n"
      "  A re-implementation of much of the CRuby/MRI standard library for MRuby,\n"
      "  used by Rubium to provide access to system resources like files & sockets.\n"
      "\n");
    exit(0);
  }
}

/*********************************
 * Platform agnostic start routine
 *********************************/
int rubium_main()
{
  //  LAMINA_LOG("rubium_main: Entering");
   apr_initialize();
   mrb_state* mrb = mrb_for_thread();

   void* sandbox_info = NULL;

 #if defined(CEF_USE_SANDBOX)
  // Manage the life span of the sandbox information object. This is necessary
  // for sandbox support on Windows. See cef_sandbox_win.h for complete details.
  CefScopedSandboxInfo scoped_sandbox;
  sandbox_info = scoped_sandbox.sandbox_info();
 #endif

    // Provide CEF with command-line arguments.
 #ifdef WINDOWS
    CefMainArgs main_args;
 #else
    CefMainArgs main_args(g_argc, g_argv);
 #endif

  // SimpleApp implements application-level callbacks. It will create the first
  // browser instance in OnContextInitialized() after CEF has initialized.
  //  LAMINA_LOG("rubium_main: Creating App");
  CefRefPtr<RubiumApp> app(new RubiumApp);

  if (g_command_line->HasSwitch("content-script")) {
    std::cout << "Setting CONTENT_SCRIPT" << std::endl;
    mrb_load_string(mrb,  (string("ENV['CONTENT_SCRIPT'] = '") + g_command_line->GetSwitchValue("content-script").ToString() + "'").c_str());
  }

  // CEF applications have multiple sub-processes (render, plugin, GPU, etc)
  // that share the same executable. This function checks the command-line and,
  // if this is a sub-process, executes the appropriate logic.

  //  LAMINA_LOG("rubium_main: Executing CEF Process");
  int exit_code = CefExecuteProcess(main_args, app.get(), sandbox_info);
  if (exit_code >= 0) {
    // LAMINA_LOG("rubium_main: Subprocess exited");
    // The sub-process has completed so return here.
    return exit_code;
  }

  //  LAMINA_LOG("rubium_main: This is a CEF browser process");

  // Specify CEF global settings here.
  CefSettings settings;

#ifndef DEBUG /* Suppresses debug.log output when building in release mode */
  //settings.log_severity = LOGSEVERITY_DISABLE;
#endif

  if (g_command_line->HasSwitch("cache-path")) {
    CefString(&settings.cache_path) = g_command_line->GetSwitchValue("cache-path");
  }

#if !defined(CEF_USE_SANDBOX)
  settings.no_sandbox = true;
#endif

#if !defined(_WIN32) && !defined(__APPLE__)
  // Install xlib error handlers so that the application won't be terminated
  // on non-fatal errors.
  XSetErrorHandler(XErrorHandlerImpl);
  XSetIOErrorHandler(XIOErrorHandlerImpl);
#endif

  // LAMINA_LOG("rubium_main: Initializing CEF");
  CefInitialize(main_args, settings, app.get(), sandbox_info);

  // LAMINA_LOG("rubium_main: Running the message loop");
  CefRunMessageLoop();

  // LAMINA_LOG("rubium_main: Shutting down CEF");
  CefShutdown();

  return 0;
}

#ifdef __cplusplus
}
#endif
