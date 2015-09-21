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
     return rubium_main();
  }

#else

  #include <iostream>
  #include "Windows.h"
  #include "mruby_rubium.h"
  #include "include/cef_command_line.h"

  int main()
  {
     LAMINA_LOG("main: enter");
     g_command_line = CefCommandLine::CreateCommandLine();
     g_command_line->InitFromString(GetCommandLine());
     return rubium_main();
  }

#endif
