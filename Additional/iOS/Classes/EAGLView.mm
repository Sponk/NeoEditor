//
//  EAGLView.m
//  example
//
//  Created by Anaël Seghezzi on 10/03/11.
//  Copyright __MyCompanyName__ 2011. All rights reserved.
//

#define SCREEN_HEIGHT ([UIApplication sharedApplication].statusBarOrientation == UIInterfaceOrientationPortrait ? [[UIScreen mainScreen] bounds].size.height : [[UIScreen mainScreen] bounds].size.width)
#define SCREEN_WIDTH ([UIApplication sharedApplication].statusBarOrientation == UIInterfaceOrientationPortrait ? [[UIScreen mainScreen] bounds].size.width : [[UIScreen mainScreen] bounds].size.height)


#include <MEngine.h>

#include <MContexts/MES1Context.h>
#include <MContexts/MES2Context.h>
#include <MContexts/MALContext.h>
#include <MContexts/MBulletContext.h>
#include <MLoaders/MiOSImageLoader.h>
#include <MLoaders/MWavSoundLoader.h>
#include <MLoaders/MFreetypeLoader.h>
#include <MLoaders/MBinFontLoader.h>
#include <MLoaders/MBinMeshLoader.h>

#include <MFileManager/MLevelLoad.h>
#include <MBehaviors/MBLookAt.h>
#include <MBehaviors/MBFollow.h>
#include <MScript/MScript.h>
#include <MInput/MInput.h>
#include <MFileManager/MMeshLoad.h>
#include <MFileManager/MLevelLoad.h>
#include <MProject/MProject.h>
#include <MRenderers/MiOSStandardRenderer.h>
#include <MRenderers/MFixedRenderer.h>
#include <MFileManager/MPackageManagerNPK.h>


MRenderingContext * render = NULL;
MInputContext *	input = NULL;
MLevel * level = NULL;
MGame * game = NULL;
MRenderer * mrenderer = NULL;
MSystemContext * msystem = NULL;
MPhysicsContext * physics = NULL;
MScriptContext * script = NULL;
MSoundContext * soundContext = NULL;
MPackageManager * packageManager = NULL;


class MiOSContext : public MSystemContext
{
public:
	
	// destructor
	~MiOSContext(void){}
	
	// screen
	void getScreenSize(unsigned int * width, unsigned int * height)
	{
        // Get real screen size
		if (width) (*width) = SCREEN_WIDTH;
		if (height) (*height) = SCREEN_HEIGHT;
	}
	
	// cursor
	void setCursorPosition(int x, int y){}
	
	// working directory
	const char * getWorkingDirectory(void)
	{
		static char currentDirectory[256] = "";
		return currentDirectory;
	}	
	
	// system tick
	unsigned long getSystemTick(void)
	{
		static NSTimeInterval startTime = [NSDate timeIntervalSinceReferenceDate];
		return (unsigned long)(([NSDate timeIntervalSinceReferenceDate] - startTime ) * 1000.0);
	}
};




#import "EAGLView.h"

#import "ES1Renderer.h"
#import "ES2Renderer.h"

@implementation EAGLView

@synthesize animating;
@dynamic animationFrameInterval;

// load project
- (void) loadMaratisProject:(const char *)filename;
{
	MEngine * engine = MEngine::getInstance();
	
	if(! filename)
		return;
	
	// load project file
	MProject proj;
	if(proj.loadXML(filename))
	{
		//loadGamePlugin();
		
		// set renderer
		{		
			if(renderer)
			{
				[renderer release];
				renderer = nil;
			}
			SAFE_DELETE(mrenderer);
			SAFE_DELETE(render);
			
			if(strcmp(proj.renderer.c_str(), "StandardRenderer") == 0)
			{
				// ES2 / StandardRenderer
				renderer = [[ES2Renderer alloc] init];
				if(renderer)
				{
					// rendering context
					render = new MES2Context();
					engine->setRenderingContext(render);
					
					// renderer
					mrenderer = engine->getRendererManager()->getRendererByName("StandardRenderer")->getNewRenderer();
					engine->setRenderer(mrenderer);
				}
			}
			
			if(! renderer)
			{
				// ES1 / FixedRenderer
				renderer = [[ES1Renderer alloc] init];
				if(renderer)
				{
					// rendering context
					render = new MES1Context();
					engine->setRenderingContext(render);
					
					// renderer
					mrenderer = engine->getRendererManager()->getRendererByName("FixedRenderer")->getNewRenderer();
					engine->setRenderer(mrenderer);
				}
			}
		}
		
		
		// if we have a package manager, try to load the package
		if(packageManager)
		{
			char packageFile[256];
			strcpy(packageFile, filename);
			if(char* ext = strstr(packageFile, ".mproj"))
			{
				strcpy(ext, ".npk");
				packageManager->loadPackage(packageFile);
			}
		}
		
		
		// load start level
		engine->loadLevel(proj.startLevel.c_str());
	}
}

// You must implement this method
+ (Class) layerClass
{
    return [CAEAGLLayer class];
}

// The GL view is initialized by hand, so we need to init the rendering engine
- (id) initWithFrame:(CGRect)frame
{
    if ((self = [super initWithFrame:frame]))
	{
        if (![self initRenderingContext])
        {
            [self release];
            return nil;
        }
    }
    
    return self;
}

//The GL view is stored in the nib file. When it's unarchived it's sent -initWithCoder:
- (id) initWithCoder:(NSCoder*)coder
{    
    if ((self = [super initWithCoder:coder]))
	{
        if (![self initRenderingContext])
        {
            [self release];
            return nil;
        }
    }
	
    return self;
}

// Generic metho to init the rendering context for the view
// This is called from initWithFrame and initWithCoder
- (BOOL) initRenderingContext
{
	renderer = nil;
	
    // Enable multitouch on this view
    self.multipleTouchEnabled = YES;
    
    // Get the layer
    CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
    
    UIScreen* mainScreen = [UIScreen mainScreen];
    if ([mainScreen respondsToSelector:@selector(scale)])
    {
        eaglLayer.contentsScale = mainScreen.scale;
        NSLog(@"Using scale factor %f for device", mainScreen.scale);
    }
    
    eaglLayer.opaque = TRUE;
    eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                    [NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
	
    
    // set ressource path as current directory
    NSString * resourcePath = [[NSBundle mainBundle] resourcePath];
    chdir([resourcePath cStringUsingEncoding:NSUTF8StringEncoding]);
    
   
    
    
    // Maratis
    {
        // get engine
        MEngine * engine = MEngine::getInstance();
        
        // create virtual contexts
        soundContext = new MALContext();
        physics = new MBulletContext();
        script = new MScript();
        input = new MInput();
        msystem = new MiOSContext();
		packageManager = new MPackageManagerNPK();
        
                
		// package manager
		engine->setPackageManager(packageManager);
		packageManager->init();

        // init MEngine
        engine->setSoundContext(soundContext); // sound context
        engine->setPhysicsContext(physics); // physics context
        engine->setScriptContext(script); // script context
        engine->setInputContext(input); // input context
        engine->setSystemContext(msystem); // system context
        
        engine->getImageLoader()->addLoader(M_loadImage); // image loader
		engine->getSoundLoader()->addLoader(M_loadWavSound); // sound loader
        engine->getLevelLoader()->addLoader(xmlLevelLoad); // level loader
		engine->getFontLoader()->addLoader(M_loadFont); // freetype font loader
        engine->getFontLoader()->addLoader(M_loadBinFont); // bin font loader
        
        // add some default "Maratis" behaviors : uncomment if wanted or add custom
        engine->getBehaviorManager()->addBehavior(MBLookAt::getStaticName(), M_OBJECT3D_CAMERA, MBLookAt::getNew);
        engine->getBehaviorManager()->addBehavior(MBFollow::getStaticName(), M_OBJECT3D, MBFollow::getNew);
        
        // add renderers
        engine->getRendererManager()->addRenderer(MStandardRenderer::getStaticName(), MStandardRenderer::getNew);
        engine->getRendererManager()->addRenderer(MFixedRenderer::getStaticName(), MFixedRenderer::getNew);
        
        // mesh loader
		engine->getMeshLoader()->addLoader(xmlMeshLoad);
		engine->getMeshLoader()->addLoader(M_loadBinMesh);
		engine->getArmatureAnimLoader()->addLoader(xmlArmatureAnimLoad);
		engine->getArmatureAnimLoader()->addLoader(M_loadBinArmatureAnim);
		engine->getTexturesAnimLoader()->addLoader(xmlTextureAnimLoad);
		engine->getTexturesAnimLoader()->addLoader(M_loadBinTexturesAnim);
		engine->getMaterialsAnimLoader()->addLoader(xmlMaterialAnimLoad);
        engine->getMaterialsAnimLoader()->addLoader(M_loadBinMaterialsAnim);
        
		
        // create default Level
        level = new MLevel();
        engine->setLevel(level);
        
		
        // configure accelerometer support
        [self configureMobileInput];
        
        char filename[256];
        getGlobalFilename(filename, msystem->getWorkingDirectory(), "Demos.mproj");
		[self loadMaratisProject:filename];
        
		if(! renderer)
		{
			return NO;
		}
		
		// set game
		game = new MGame();
        engine->setGame(game);
		
        // begin game
        game->begin();
    }
    
    animating = FALSE;
    displayLinkSupported = FALSE;
    animationFrameInterval = 1;
    displayLink = nil;
    animationTimer = nil;
    
    // A system version of 3.1 or greater is required to use CADisplayLink. The NSTimer
    // class is used as fallback when it isn't available.
    NSString *reqSysVer = @"3.1";
    NSString *currSysVer = [[UIDevice currentDevice] systemVersion];
    if ([currSysVer compare:reqSysVer options:NSNumericSearch] != NSOrderedAscending)
    {
        displayLinkSupported = TRUE;
    }
    
    return YES;
}

- (void) drawView:(id)sender
{
    [self processMotionEvents];
    [renderer render];
}

- (void) layoutSubviews
{
	[renderer resizeFromLayer:(CAEAGLLayer*)self.layer];
    [self drawView:nil];
}

- (NSInteger) animationFrameInterval
{
	return animationFrameInterval;
}

- (void) setAnimationFrameInterval:(NSInteger)frameInterval
{
	// Frame interval defines how many display frames must pass between each time the
	// display link fires. The display link will only fire 30 times a second when the
	// frame internal is two on a display that refreshes 60 times a second. The default
	// frame interval setting of one will fire 60 times a second when the display refreshes
	// at 60 times a second. A frame interval setting of less than one results in undefined
	// behavior.
	if (frameInterval >= 1)
	{
		animationFrameInterval = frameInterval;
		
		if (animating)
		{
			[self stopAnimation];
			[self startAnimation];
		}
	}
}

- (void) startAnimation
{
	if (!animating)
	{
		if (displayLinkSupported)
		{
			// CADisplayLink is API new to iPhone SDK 3.1. Compiling against earlier versions will result in a warning, but can be dismissed
			// if the system version runtime check for CADisplayLink exists in -initWithCoder:. The runtime check ensures this code will
			// not be called in system versions earlier than 3.1.

			displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(drawView:)];
			[displayLink setFrameInterval:animationFrameInterval];
			[displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
		}
		else
        {
			animationTimer = [NSTimer scheduledTimerWithTimeInterval:(NSTimeInterval)((1.0 / 60.0) * animationFrameInterval) target:self selector:@selector(drawView:) userInfo:nil repeats:TRUE];
		}
        
        // Start the CoreMotion updates
        [self startMobileInput];
        
		animating = TRUE;
	}
}

- (void)stopAnimation
{
	if (animating)
	{
		if (displayLinkSupported)
		{
			[displayLink invalidate];
			displayLink = nil;
		}
		else
		{
			[animationTimer invalidate];
			animationTimer = nil;
		}
        
        // Stop the CoreMotion updates
        [self stopMobileInput];
		
		animating = FALSE;
	}
}

#pragma mark -
#pragma iOS Input

- (void) configureMobileInput
{
    // Create space for the touches dictionary
    maxTouches = 5;
    touchPointAvailable = new bool[maxTouches];
    
    for (int i = 0; i < maxTouches; i++)
    {
        touchPointAvailable[i] = true;
    }
    
    touchPoints = CFDictionaryCreateMutable(kCFAllocatorDefault, 10, NULL, NULL);
    
    // Use CoreMotion to get the latest data from sensors (accelerometer+gyroscope)
    motionManager = [[CMMotionManager alloc] init];
    
    if (motionManager.deviceMotionAvailable)
    {
        // Set the update interval for device motion (gyro + accelerometer)
        motionManager.deviceMotionUpdateInterval = 1/50.0;
        
        // Create three axis to handle the gyro data
        input->createAxis("MOTION_X");
        input->createAxis("MOTION_Y");
        input->createAxis("MOTION_Z");
        
        // Create three axis to handle the accelerometer data
        input->createAxis("ACCEL_X");
        input->createAxis("ACCEL_Y");
        input->createAxis("ACCEL_Z");
        
		input->createAxis("GRAVITY_X");
        input->createAxis("GRAVITY_Y");
        input->createAxis("GRAVITY_Z");
        
        input->createAxis("YAW");
        input->createAxis("PITCH");
        input->createAxis("ROLL");
    }
    else if (motionManager.accelerometerAvailable)
    {
        // Set the update interval for the accelerometer
        motionManager.gyroUpdateInterval = 1/50.0;
        
        // Create three axis to handle the accelerometer data
        input->createAxis("ACCEL_X");
        input->createAxis("ACCEL_Y");
        input->createAxis("ACCEL_Z");
    }
}

- (void) startMobileInput
{
    if (motionManager.deviceMotionAvailable)
    {
        [motionManager startDeviceMotionUpdates];
    }
    else
    {
        [motionManager startAccelerometerUpdates];
    }
}

- (void) stopMobileInput
{
    if (motionManager.deviceMotionAvailable)
    {
        [motionManager stopDeviceMotionUpdates];
    }
    else
    {
        [motionManager stopAccelerometerUpdates];
    }
}

- (void) processMotionEvents
{
    if (motionManager.deviceMotionAvailable && motionManager.deviceMotionActive)
    {
        input->setAxis("MOTION_X", motionManager.deviceMotion.rotationRate.x);
        input->setAxis("MOTION_Y", motionManager.deviceMotion.rotationRate.y);
        input->setAxis("MOTION_Z", motionManager.deviceMotion.rotationRate.z);
        
        // Send data to the real gravity
		input->setAxis("GRAVITY_X", motionManager.deviceMotion.gravity.x);
        input->setAxis("GRAVITY_Y", motionManager.deviceMotion.gravity.y);
        input->setAxis("GRAVITY_Z", motionManager.deviceMotion.gravity.z);
		
        // Send data to accelerometer-specific axis
        input->setAxis("ACCEL_X", motionManager.deviceMotion.userAcceleration.x);
        input->setAxis("ACCEL_Y", motionManager.deviceMotion.userAcceleration.y);
        input->setAxis("ACCEL_Z", motionManager.deviceMotion.userAcceleration.z);
        
        // Get change in attitude
        CMAttitude* currentAttitude = motionManager.deviceMotion.attitude;
        
        // Send Euler-Angles
        if (referenceAttitude != nil)
        {
            [currentAttitude multiplyByInverseOfAttitude:referenceAttitude];
            
            input->setAxis("YAW", currentAttitude.yaw);
            input->setAxis("PITCH", currentAttitude.pitch);
            input->setAxis("ROLL", currentAttitude.roll);
        }
        else
        {
            referenceAttitude = [motionManager.deviceMotion.attitude retain];
        }
    }
    else if (motionManager.accelerometerAvailable && motionManager.accelerometerActive)
    {    
        // Send data to accelerometer-specific axis
        input->setAxis("ACCEL_X", motionManager.accelerometerData.acceleration.x);
        input->setAxis("ACCEL_Y", motionManager.accelerometerData.acceleration.y);
        input->setAxis("ACCEL_Z", motionManager.accelerometerData.acceleration.z);
    }
}

- (int) getAvailableTouchID
{
    for (int i = 0; i < 5; i++)
    {
        if (touchPointAvailable[i])
        {
            touchPointAvailable[i] = false;
            return i;
        }
    }
    
    return -1;
}

- (void) setTouchIDAvailable:(int)touchID
{
    touchPointAvailable[touchID] = true;
}

- (void) touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    // Traverse all touches and create a new touch for each one
    for (UITouch* touch in [touches allObjects])
    {
		CGSize size = self.bounds.size;
		
        // Create a new TouchData object
        int* touchID = (int *)malloc(sizeof(int));
        *touchID = [self getAvailableTouchID];
        CFDictionarySetValue(touchPoints, touch, touchID);
        
        // Get the touch location
        CGPoint location = [touch locationInView:self];
        
        // Send the data to input
        input->beginTouch(*touchID, MVector2((location.x * self.contentScaleFactor)/size.width, (location.y * self.contentScaleFactor)/size.height));
    }
}

- (void) touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    // Traverse all touches and update each one
    for (UITouch* touch in [touches allObjects])
    {
        CGPoint location = [touch locationInView:self];
        int* touchID = (int *)CFDictionaryGetValue(touchPoints, touch);
        
        if (touchID != NULL)
        {
			CGSize size = self.bounds.size;
			
            // Send an updated touch to input
			MVector2 pos = MVector2((location.x * self.contentScaleFactor)/size.width, (location.y * self.contentScaleFactor)/size.height);
            input->updateTouch(*touchID, pos);
        
			// simulate mouse input
			if(touchID == 0)
			{
				input->setAxis("MOUSE_X", pos.x);
				input->setAxis("MOUSE_Y", pos.y);
			}
        }
    }
}

- (void) touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    // Traverse all touches and end them
    for (UITouch* touch in [touches allObjects])
    {
        CGPoint location = [touch locationInView:self];
        int* touchID = (int *)CFDictionaryGetValue(touchPoints, touch);
        
        if (touchID != NULL)
        {
			CGSize size = self.bounds.size;
			
            // Send the end touch to input
            input->endTouch(*touchID, MVector2((location.x * self.contentScaleFactor)/size.width, (location.y * self.contentScaleFactor)/size.height));
        
            // After updating input, set the touch index as available and release the memory
            [self setTouchIDAvailable:*touchID];
            CFDictionaryRemoveValue(touchPoints, touch);
        }
    }
}

- (void) touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    // Traverse all touches and cancell them
    for (UITouch* touch in [touches allObjects])
    {
        CGPoint location = [touch locationInView:self];
        int* touchID = (int *)CFDictionaryGetValue(touchPoints, touch);
        
        if (touchID != NULL)
        {
			CGSize size = self.bounds.size;
			
            // Send the end touch to input
            input->cancelTouch(*touchID, MVector2((location.x * self.contentScaleFactor)/size.width, (location.y * self.contentScaleFactor)/size.height));
            
            // After updating input, set the touch index as available and release the memory
            [self setTouchIDAvailable:*touchID];
            CFDictionaryRemoveValue(touchPoints, touch);
        }
    }
}

- (void) dealloc
{
	// Maratis
	{
		game->end();

		SAFE_DELETE(mrenderer);
		SAFE_DELETE(game);
		SAFE_DELETE(level);
		
		SAFE_DELETE(render);
		SAFE_DELETE(soundContext);
		SAFE_DELETE(physics);
		SAFE_DELETE(script);
		SAFE_DELETE(input);
		SAFE_DELETE(msystem);
		SAFE_DELETE(packageManager);
	}
    
    // Device Motion
    [referenceAttitude release];
    [motionManager release];
    
    // Release the touches dictionary
    delete [] touchPointAvailable;
    CFRelease(touchPoints);
	
    [renderer release];
	
    [super dealloc];
}

@end
