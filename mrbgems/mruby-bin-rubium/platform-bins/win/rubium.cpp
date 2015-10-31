#define RUBIUM_CONSOLE_MODE

#ifndef RUBIUM_CONSOLE_MODE

  #include "Windows.h"
  #include "mruby_rubium.h"
  #include "include/cef_command_line.h"

  #define LAMINA_LOG(msg) \
     /* do nothing for gui mode */

  HINSTANCE app_handle;

  int APIENTRY WinMain(HINSTANCE hInstance,
     HINSTANCE hPrevInstance,
     LPSTR    lpCmdLine,
     int       nCmdShow)
  {
     app_handle = hInstance;
     g_command_line = CefCommandLine::CreateCommandLine();
     g_command_line->InitFromString(lpCmdLine);
     rubium_check_usage();
     return rubium_main();
  }

#else

  #include <iostream>
  #include "Windows.h"
  #include "mruby_rubium.h"
  #include "include/cef_command_line.h"

  int main()
  {
     g_command_line = CefCommandLine::CreateCommandLine();
     g_command_line->InitFromString(GetCommandLine());
     rubium_check_usage();
     return rubium_main();
  }

#endif
