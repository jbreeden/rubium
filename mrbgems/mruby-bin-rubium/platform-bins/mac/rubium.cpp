#include "mruby_rubium.h"
#include "RubiumLifeSpanHandler.h"
#include <cstdio>
#include <cstdlib>
#import "include/cef_application_mac.h"
#include "include/cef_command_line.h"
#import <Cocoa/Cocoa.h>

// Provide the CefAppProtocol implementation required by CEF.
@interface RubiumApplication : NSApplication<CefAppProtocol> {
@private
  BOOL handlingSendEvent_;
}
@end

@interface RubiumAppDelegate : NSObject<NSApplicationDelegate> {
  NSWindow *window;
}
@property (assign) IBOutlet NSWindow *window;
- (void)tryToTerminateApplication:(NSApplication*)app;
@end

@implementation RubiumApplication
- (BOOL)isHandlingSendEvent {
  return handlingSendEvent_;
}

- (void)setHandlingSendEvent:(BOOL)handlingSendEvent {
  handlingSendEvent_ = handlingSendEvent;
}

- (void)sendEvent:(NSEvent*)event {
  CefScopedSendingEvent sendingEventScoper;
  [super sendEvent:event];
}

- (void)terminate:(id)sender {
  // Could have been a force close, or the user simply closed the last window.
  // In case of the former, need to "tryToTerminateApplication" to close all windows.
  // In case of the latter, they are already closed.
  // In either case, it's time to exit.
  RubiumAppDelegate* delegate =
      static_cast<RubiumAppDelegate*>([NSApp delegate]);
  [delegate tryToTerminateApplication:self];
  exit(0);
}
@end

@implementation RubiumAppDelegate

@synthesize window;

-(void)dealloc
{
    [super dealloc];
}

-(BOOL)applicationShouldTerminateAfterLastWindowClosed: (NSApplication *)sender
{
    return YES;
}

-(void)applicationDidFinishLaunching: (NSNotification *)aNotification
{
  printf("Starting rubium_main\n");
  rubium_main();
  printf("Finished: rubium_main\n");
  [self release];
}

- (void)tryToTerminateApplication:(NSApplication*)app {
  RubiumLifeSpanHandler* handler = RubiumLifeSpanHandler::GetInstance();
  if (handler && !handler->IsClosing())
    handler->CloseAllBrowsers(false);
}

@end

int main(int argc, char *argv[])
{
  g_argc = argc;
  g_argv = argv;
  g_command_line = CefCommandLine::CreateCommandLine();
  g_command_line->InitFromArgv(argc, argv);

  // return NSApplicationMain(argc, (const char **) argv);
  @autoreleasepool {
    NSApplication * application = [RubiumApplication sharedApplication];
    RubiumAppDelegate* appDelegate = [[RubiumAppDelegate alloc] init];

    [application setDelegate:appDelegate];
    [application run];
  }

  return EXIT_SUCCESS;
}
