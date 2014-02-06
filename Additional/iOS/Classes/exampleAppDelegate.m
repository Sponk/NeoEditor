//
//  exampleAppDelegate.m
//  example
//
//  Created by Anaël Seghezzi on 10/03/11.
//  Copyright __MyCompanyName__ 2011. All rights reserved.
//

#import "exampleAppDelegate.h"
#import "EAGLView.h"
#import "exampleViewController.h"

@implementation exampleAppDelegate

@synthesize window;
@synthesize glView;
@synthesize viewController;

// Using the multitasking-aware application launch options
- (BOOL) application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    // Get the screen bounds and make a window and view that cover the screen.
    // This will work on iPad and iPhone without any changes on the code 
    // (granted that the game code is resolution-independent)
    
    CGRect screenBounds = [[UIScreen mainScreen] bounds];
    window = [[UIWindow alloc] initWithFrame:screenBounds];
    
    // Create a view controller so we can have modal dialogs (GameCenter, Bluetooth, etc)
    // and support screen rotation, which whould be done with CoreAnimation (as per Apple).
    viewController = [[exampleViewController alloc] initWithNibName:nil bundle:nil];
    glView = [[EAGLView alloc] initWithFrame:screenBounds];
    viewController.view = glView;
    
    [window addSubview:glView];
    [window makeKeyAndVisible];
    
	[glView startAnimation];
    
    return YES;
}

- (void) applicationWillResignActive:(UIApplication *)application
{
	[glView stopAnimation];
}

- (void) applicationDidBecomeActive:(UIApplication *)application
{
	[glView startAnimation];
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
	exit(0);
}

- (void)applicationWillTerminate:(UIApplication *)application
{
	[glView stopAnimation];
}

- (void) dealloc
{
	[window release];
    [viewController release];
	[glView release];
	
	[super dealloc];
}

@end
