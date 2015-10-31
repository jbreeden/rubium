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
  if ([event type] == NSKeyDown) {
    // Note: Brackets checks [self mainWindow] firstResponder] == [self mainWindow]
    //       as well. This seems to stop all undo events for rubium. Need to
    //       investigate, as the current solution prevents intercepting the events
    //       in JavaScript
    if ([event modifierFlags] & NSCommandKeyMask) {
      // There is no edit menu, so handle these shortcuts manually
      SEL theSelector = nil;
      NSString *keyStr = [event charactersIgnoringModifiers];
      unichar keyChar = [keyStr characterAtIndex:0];
      if ( keyChar == 'c') {
        theSelector = NSSelectorFromString(@"copy:");
      } else if (keyChar == 'v'){
        theSelector = NSSelectorFromString(@"paste:");
      } else if (keyChar == 'x'){
        theSelector = NSSelectorFromString(@"cut:");
      } else if (keyChar == 'a'){
        theSelector = NSSelectorFromString(@"selectAll:");
      } else if (keyChar == 'z'){
        theSelector = NSSelectorFromString(@"undo:");
      } else if (keyChar == 'Z'){
        theSelector = NSSelectorFromString(@"redo:");
      }
      if (theSelector != nil) {
        [[NSApplication sharedApplication] sendAction:theSelector to:nil from:nil];
      }
    }
  }
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
  rubium_main();
  [self release];
}

- (void)tryToTerminateApplication:(NSApplication*)app
{
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

  rubium_check_usage();

  // return NSApplicationMain(argc, (const char **) argv);
  @autoreleasepool {
    NSApplication * application = [RubiumApplication sharedApplication];
    RubiumAppDelegate* appDelegate = [[RubiumAppDelegate alloc] init];

    [application setDelegate:appDelegate];
    [application run];
  }

  return EXIT_SUCCESS;
}
