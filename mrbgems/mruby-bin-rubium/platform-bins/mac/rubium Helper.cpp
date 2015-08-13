#include "mruby_rubium.h"
#include "RubiumLifeSpanHandler.h"
#include <cstdio>
#include <cstdlib>
#import "include/cef_application_mac.h"
#include "include/cef_command_line.h"

int main(int argc, char *argv[])
{
  g_argc = argc;
  g_argv = argv;
  g_command_line = CefCommandLine::CreateCommandLine();
  g_command_line->InitFromArgv(argc, argv);
  return rubium_main();
}
