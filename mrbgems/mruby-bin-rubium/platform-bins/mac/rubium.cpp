#include "mruby_rubium.h"
#include <stdio.h>
#import "include/cef_application_mac.h"
#include "include/cef_command_line.h"
#import <Cocoa/Cocoa.h>

// Provide the CefAppProtocol implementation required by CEF.
@interface RubiumApplication : NSApplication<CefAppProtocol> {
@private
  BOOL handlingSendEvent_;
}
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

// - (void)terminate:(id)sender {
//   ClientAppDelegate* delegate =
//       static_cast<ClientAppDelegate*>([NSApp delegate]);
//   [delegate tryToTerminateApplication:self];
//   // Return, don't exit. The application is responsible for exiting on its own.
// }
@end

@interface RubiumAppDelegate : NSObject<NSApplicationDelegate> {
    NSWindow *window;
}

@property (assign) IBOutlet NSWindow *window;

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
  rubium_main();
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
