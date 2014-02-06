//
//  exampleAppDelegate.h
//  example
//
//  Created by Anaël Seghezzi on 10/03/11.
//  Copyright __MyCompanyName__ 2011. All rights reserved.
//

#import <UIKit/UIKit.h>

@class EAGLView;
@class exampleViewController;

@interface exampleAppDelegate : NSObject <UIApplicationDelegate>
{
    UIWindow *window;
    EAGLView *glView;
    exampleViewController *viewController;
}

@property (nonatomic, retain) UIWindow *window;
@property (nonatomic, retain) EAGLView *glView;
@property (nonatomic, retain) exampleViewController *viewController;

@end

