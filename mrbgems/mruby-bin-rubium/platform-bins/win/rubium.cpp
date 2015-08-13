#include "Windows.h"
#include "mruby_rubium.h"
#include "include/cef_command_line.h"

#define LAMINA_LOG(msg) \
   /* do nothing for rubiumw */

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