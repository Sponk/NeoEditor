//
//  EAGLView.h
//  example
//
//  Created by Anaël Seghezzi on 10/03/11.
//  Copyright __MyCompanyName__ 2011. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import <CoreMotion/CoreMotion.h>

#import "ESRenderer.h"

// This class wraps the CAEAGLLayer from CoreAnimation into a convenient UIView subclass.
// The view content is basically an EAGL surface you render your OpenGL scene into.
// Note that setting the view non-opaque will only work if the EAGL surface has an alpha channel.
@interface EAGLView : UIView <UIAccelerometerDelegate>
{    
@private

	id <ESRenderer> renderer;
	
	BOOL animating;
	BOOL displayLinkSupported;
	NSInteger animationFrameInterval;
	// Use of the CADisplayLink class is the preferred method for controlling your animation timing.
	// CADisplayLink will link to the main display and fire every vsync when added to a given run-loop.
	// The NSTimer class is used only as fallback when running on a pre 3.1 device where CADisplayLink
	// isn't available.
	id displayLink;
    NSTimer *animationTimer;

    int maxTouches;
    bool *touchPointAvailable;
    CFMutableDictionaryRef touchPoints;
    
    CMMotionManager* motionManager;
    CMAttitude* referenceAttitude;
}

@property (readonly, nonatomic, getter=isAnimating) BOOL animating;
@property (nonatomic) NSInteger animationFrameInterval;

- (void) loadMaratisProject:(const char *)filename;

- (BOOL) initRenderingContext;

- (void) configureMobileInput;
- (void) startMobileInput;
- (void) stopMobileInput;
- (void) processMotionEvents;

- (void) startAnimation;
- (void) stopAnimation;
- (void) drawView:(id)sender;

@end
