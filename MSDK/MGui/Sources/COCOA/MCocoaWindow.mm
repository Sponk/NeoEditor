//========================================================================
// Following code created from GLFW cocoa_window : http://www.glfw.org/
//------------------------------------------------------------------------
// Copyright (c) 2009-2010 Camilla Berglund <elmindreda@elmindreda.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

/*
 * THIS FILE WAS ALTERED BY Anaël Seghezzi, 1 Nov 2010
 *
 * - adaptation for MWindow Cocoa
 */

#include <MWindow.h>

#import <Cocoa/Cocoa.h>
#import <Carbon/Carbon.h>

#import <OpenGL/OpenGL.h>
#import <OpenGL/gl.h>

#include <pthread.h>
#include <sys/time.h>


id window = nil;
id pixelFormat = nil;
id context = nil;
id delegate = nil;
unsigned int modifierFlags;

id DesktopMode;
id AutoreleasePool = nil;



// GLFWApplication
@interface GLFWApplication : NSApplication
@end

@implementation GLFWApplication

- (id)init
{
	self = [super init];
	return self;
}

- (void)dealloc
{
	[super dealloc];
}

- (void)sendEvent:(NSEvent *)event
{
	if([event type] == NSTabletPoint)
	{
		MMouse::getInstance()->setPressure([event pressure]);
		
		/*
		//if(([event type] == NSTabletPoint) || ([event subtype] == NSTabletPointEventSubtype))
		{
			SInt32 mAbsX = [event absoluteX];
			SInt32 mAbsY = [event absoluteY];
			
			NSPoint tilt = [event tilt];
			mTiltX = tilt.x;
			mTiltY = tilt.y;
			
			mRotDeg = [event rotation];
			
			//UInt16 mDeviceID =  [theEvent deviceID];
		}*/
	}
	
    if([event type] == NSKeyUp && ([event modifierFlags] & NSCommandKeyMask))
    {
        [[self keyWindow] sendEvent:event];
    }
    else
    {
        [super sendEvent:event];
    }
}

@end



// GLFWWindowDelegate
@interface GLFWWindowDelegate : NSObject
@end

@implementation GLFWWindowDelegate

- (BOOL)windowShouldClose:(id)window
{
	MWinEvent mevent;
	mevent.type = MWIN_EVENT_WINDOW_CLOSE;
	MWindow::getInstance()->sendEvents(&mevent);
	
    return NO;
}

- (void)windowDidBecomeMain:(NSNotification *)notification
{
	MWindow::getInstance()->setFocus(true);
}

- (void)windowDidResignMain:(NSNotification *)notification
{
	MWindow::getInstance()->setFocus(false);
}

- (void)windowDidResize:(NSNotification *)notification
{
    [context update];
	
    NSRect contentRect =
	[window contentRectForFrameRect:[window frame]];

	MWinEvent mevent;
	mevent.type = MWIN_EVENT_WINDOW_RESIZE;
	mevent.data[0] = contentRect.size.width;
	mevent.data[1] = contentRect.size.height;
	MWindow::getInstance()->sendEvents(&mevent);
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender
{
	MWinEvent mevent;
	mevent.type = MWIN_EVENT_WINDOW_CLOSE;
	MWindow::getInstance()->sendEvents(&mevent);
	
    return NSTerminateCancel;
}

@end


//========================================================================
// Converts a Mac OS X keycode
//========================================================================

static const unsigned int MAC_KEYCODE_MAPPING[128] =
{
    /* 00 */ 'A',
    /* 01 */ 'S',
    /* 02 */ 'D',
    /* 03 */ 'F',
    /* 04 */ 'H',
    /* 05 */ 'G',
    /* 06 */ 'Z',
    /* 07 */ 'X',
    /* 08 */ 'C',
    /* 09 */ 'V',
    /* 0a */ -1,
    /* 0b */ 'B',
    /* 0c */ 'Q',
    /* 0d */ 'W',
    /* 0e */ 'E',
    /* 0f */ 'R',
    /* 10 */ 'Y',
    /* 11 */ 'T',
    /* 12 */ '1',
    /* 13 */ '2',
    /* 14 */ '3',
    /* 15 */ '4',
    /* 16 */ '6',
    /* 17 */ '5',
    /* 18 */ '=',
    /* 19 */ '9',
    /* 1a */ '7',
    /* 1b */ '-',
    /* 1c */ '8',
    /* 1d */ '0',
    /* 1e */ ']',
    /* 1f */ 'O',
    /* 20 */ 'U',
    /* 21 */ '[',
    /* 22 */ 'I',
    /* 23 */ 'P',
    /* 24 */ MKEY_RETURN,
    /* 25 */ 'L',
    /* 26 */ 'J',
    /* 27 */ '\'',
    /* 28 */ 'K',
    /* 29 */ ';',
    /* 2a */ '\\',
    /* 2b */ ',',
    /* 2c */ '/',
    /* 2d */ 'N',
    /* 2e */ 'M',
    /* 2f */ '.',
    /* 30 */ MKEY_TAB,
    /* 31 */ MKEY_SPACE,
    /* 32 */ '`',
    /* 33 */ MKEY_BACKSPACE,
    /* 34 */ -1,
    /* 35 */ MKEY_ESCAPE,
    /* 36 */ MKEY_RSUPER, // RSUPER
    /* 37 */ MKEY_LSUPER, // LSUPER
    /* 38 */ MKEY_LSHIFT,
    /* 39 */ -1, // CAPS_LOCK
    /* 3a */ MKEY_LALT,
    /* 3b */ MKEY_LCONTROL, // LCTRL
    /* 3c */ MKEY_RSHIFT,
    /* 3d */ MKEY_RALT,
    /* 3e */ MKEY_RCONTROL, // RCTRL
    /* 3f */ -1, /*Function*/
    /* 40 */ -1, // F17
    /* 41 */ -1, // KP_DECIMAL
    /* 42 */ -1,
    /* 43 */ '*',
    /* 44 */ -1,
    /* 45 */ MKEY_ADD,
    /* 46 */ -1,
    /* 47 */ MKEY_CLEAR,
    /* 48 */ -1, /*VolumeUp*/
    /* 49 */ -1, /*VolumeDown*/
    /* 4a */ -1, /*Mute*/
    /* 4b */ '/',
    /* 4c */ MKEY_KP_ENTER,
    /* 4d */ -1,
    /* 4e */ '-',
    /* 4f */ -1, // F18
    /* 50 */ -1, // F19
    /* 51 */ '=',
    /* 52 */ MKEY_KP0,
    /* 53 */ MKEY_KP1,
    /* 54 */ MKEY_KP2,
    /* 55 */ MKEY_KP3,
    /* 56 */ MKEY_KP4,
    /* 57 */ MKEY_KP5,
    /* 58 */ MKEY_KP6,
    /* 59 */ MKEY_KP7,
    /* 5a */ -1, // F20
    /* 5b */ MKEY_KP8,
    /* 5c */ MKEY_KP9,
    /* 5d */ -1,
    /* 5e */ -1,
    /* 5f */ -1,
    /* 60 */ MKEY_F5,
    /* 61 */ MKEY_F6,
    /* 62 */ MKEY_F7,
    /* 63 */ MKEY_F3,
    /* 64 */ MKEY_F8,
    /* 65 */ MKEY_F9,
    /* 66 */ -1,
    /* 67 */ MKEY_F11,
    /* 68 */ -1,
    /* 69 */ -1, // F13
    /* 6a */ -1, // F16
    /* 6b */ -1, // F14
    /* 6c */ -1,
    /* 6d */ MKEY_F10,
    /* 6e */ -1,
    /* 6f */ MKEY_F12,
    /* 70 */ -1,
    /* 71 */ -1, // F15
    /* 72 */ MKEY_INSERT, /*Help*/
    /* 73 */ MKEY_HOME,
    /* 74 */ MKEY_PAGEUP,
    /* 75 */ MKEY_DELETE,
    /* 76 */ MKEY_F4,
    /* 77 */ MKEY_END,
    /* 78 */ MKEY_F2,
    /* 79 */ MKEY_PAGEDOWN,
    /* 7a */ MKEY_F1,
    /* 7b */ MKEY_LEFT,
    /* 7c */ MKEY_RIGHT,
    /* 7d */ MKEY_DOWN,
    /* 7e */ MKEY_UP,
    /* 7f */ -1,
};

static int convertMacKeyCode(unsigned int macKeyCode)
{
    if(macKeyCode >= 128)
    {
        return -1;
    }
	
    // This treats keycodes as *positional*; that is, we'll return 'a'
    // for the key left of 's', even on an AZERTY keyboard.  The charInput
    // function should still get 'q' though.
    return MAC_KEYCODE_MAPPING[macKeyCode];
}

//========================================================================
// Content view class for the GLFW window
//========================================================================

@interface GLFWContentView : NSView
@end

@implementation GLFWContentView

- (BOOL)isOpaque
{
    return YES;
}

- (BOOL)canBecomeKeyView
{
    return YES;
}

- (BOOL)acceptsFirstResponder
{
    return YES;
}

- (BOOL) canBecomeKeyWindow
{
    return YES;
}

- (void)mouseDown:(NSEvent *)event
{
	MWinEvent mevent;
	mevent.type = MWIN_EVENT_MOUSE_BUTTON_DOWN;
	mevent.data[0] = MMOUSE_BUTTON_LEFT;
	MWindow::getInstance()->sendEvents(&mevent);
	MMouse::getInstance()->setPressure([event pressure]);
}

- (void)mouseDragged:(NSEvent *)event
{
    [self mouseMoved:event];
	MMouse::getInstance()->setPressure([event pressure]);
}

- (void)mouseUp:(NSEvent *)event
{
	MWinEvent mevent;
	mevent.type = MWIN_EVENT_MOUSE_BUTTON_UP;
	mevent.data[0] = MMOUSE_BUTTON_LEFT;
	MWindow::getInstance()->sendEvents(&mevent);
	MMouse::getInstance()->setPressure([event pressure]);
}

- (void)mouseMoved:(NSEvent *)event
{
	NSPoint p = [event locationInWindow];
		
	MWinEvent mevent;
	mevent.type = MWIN_EVENT_MOUSE_MOVE;
	mevent.data[0] = p.x;
	mevent.data[1] = [[window contentView] bounds].size.height - p.y;
	MWindow::getInstance()->sendEvents(&mevent);
	MMouse::getInstance()->setPosition(p.x, [[window contentView] bounds].size.height - p.y);
	
	//float dx = (float)[event deltaX];
	//float dy = (float)[event deltaY];
}

- (void)rightMouseDown:(NSEvent *)event
{
    MWinEvent mevent;
	mevent.type = MWIN_EVENT_MOUSE_BUTTON_DOWN;
	mevent.data[0] = MMOUSE_BUTTON_RIGHT;
	MWindow::getInstance()->sendEvents(&mevent);
}

- (void)rightMouseDragged:(NSEvent *)event
{
    [self mouseMoved:event];
}

- (void)rightMouseUp:(NSEvent *)event
{
    MWinEvent mevent;
	mevent.type = MWIN_EVENT_MOUSE_BUTTON_UP;
	mevent.data[0] = MMOUSE_BUTTON_RIGHT;
	MWindow::getInstance()->sendEvents(&mevent);
}

- (void)otherMouseDown:(NSEvent *)event
{
	if([event buttonNumber] == 2)
	{
		MWinEvent mevent;
		mevent.type = MWIN_EVENT_MOUSE_BUTTON_DOWN;
		mevent.data[0] = MMOUSE_BUTTON_MIDDLE;
		MWindow::getInstance()->sendEvents(&mevent);
	}
}

- (void)otherMouseDragged:(NSEvent *)event
{
    [self mouseMoved:event];
}

- (void)otherMouseUp:(NSEvent *)event
{
	if([event buttonNumber] == 2)
	{
		MWinEvent mevent;
		mevent.type = MWIN_EVENT_MOUSE_BUTTON_UP;
		mevent.data[0] = MMOUSE_BUTTON_MIDDLE;
		MWindow::getInstance()->sendEvents(&mevent);
	}
}

- (void)keyDown:(NSEvent *)event
{
	NSUInteger length;
    NSString * characters;
    int i, code = convertMacKeyCode([event keyCode]);
	
    if(code != -1)
    {
		if(code < 128)
		{
			characters = [[event charactersIgnoringModifiers] uppercaseString];
            length = [characters length];
			if(length > 0)
				code = [characters characterAtIndex:0];
		}
		
		MWinEvent mevent;
		mevent.type = MWIN_EVENT_KEY_DOWN;
		mevent.data[0] = code;
		MWindow::getInstance()->sendEvents(&mevent);

        if([event modifierFlags] & NSCommandKeyMask)
        {
            //if( !_glfwWin.sysKeysDisabled )
            {
              //  [super keyDown:event];
            }
        }
        else
        {
            characters = [event characters];
            length = [characters length];
			
            for(i = 0; i<length; i++)
            {
				int car = [characters characterAtIndex:i];
				 
				if(car == 127 || car == 32 || car == 13 || car == 9) // not return, space, delete etc..
					continue;
				
				mevent.type = MWIN_EVENT_CHAR;
				mevent.data[0] = car;
				MWindow::getInstance()->sendEvents(&mevent);
            }
        }
    }
}

- (void)flagsChanged:(NSEvent *)event
{
    unsigned int newModifierFlags = [event modifierFlags] | NSDeviceIndependentModifierFlagsMask;
    MWinEvent mevent;
	
    if(newModifierFlags > modifierFlags)
    {
        mevent.type = MWIN_EVENT_KEY_DOWN;
    }
    else
    {
        mevent.type = MWIN_EVENT_KEY_UP;
    }
	
	modifierFlags = newModifierFlags;
	
	mevent.data[0] = MAC_KEYCODE_MAPPING[[event keyCode]];
	if(mevent.data[0] != -1)
		MWindow::getInstance()->sendEvents(&mevent);  
}

- (void)keyUp:(NSEvent *)event
{
	NSUInteger length;
    NSString * characters;
    int code = convertMacKeyCode([event keyCode]);
	
    if(code != -1)
    {
		if(code < 128)
		{
			characters = [[event charactersIgnoringModifiers] uppercaseString];
            length = [characters length];
			if(length > 0)
				code = [characters characterAtIndex:0];
		}
		
        MWinEvent mevent;
		mevent.type = MWIN_EVENT_KEY_UP;
		mevent.data[0] = code;
		MWindow::getInstance()->sendEvents(&mevent);
    }
}

- (void)scrollWheel:(NSEvent *)event
{
	MWinEvent mevent;
	mevent.type = MWIN_EVENT_MOUSE_WHEEL_MOVE;

	if([event deltaY] > 0)
		mevent.data[0] = 1;
	else if([event deltaY] < 0)
		mevent.data[0] = -1;
	else
		mevent.data[0] = 0;
	
	MWindow::getInstance()->sendEvents(&mevent);
}

@end











MWindow::MWindow(void):
m_focus(true),
m_active(true),
m_fullscreen(false),
m_width(0),
m_height(0),
m_colorBits(0),
m_pointerEvent(NULL)
{
	AutoreleasePool = [[NSAutoreleasePool alloc] init];
    [GLFWApplication sharedApplication];
	[NSApp finishLaunching];
	DesktopMode = (NSDictionary *)CGDisplayCurrentMode(CGMainDisplayID());
	
	NSString * resourcePath = [[NSBundle mainBundle] resourcePath];
    if(access([resourcePath cStringUsingEncoding:NSUTF8StringEncoding], R_OK) == 0)
    {
        chdir([resourcePath cStringUsingEncoding:NSUTF8StringEncoding]);
    }
	
	m_position[0] = 0;
	m_position[1] = 0;
	strcpy(m_workingDirectory, getCurrentDirectory());
	strcpy(m_title, "");
}

MWindow::~MWindow(void)
{
	if(window != nil && pixelFormat != nil && context != nil && delegate != nil)
	{
		[window orderOut:nil];
	
		if(m_fullscreen)
		{
			[[window contentView] exitFullScreenModeWithOptions:nil];
			CGDisplaySwitchToMode(CGMainDisplayID(), (CFDictionaryRef)DesktopMode);
			CGReleaseAllDisplays();
		}
	
		[pixelFormat release];
		pixelFormat = nil;
	
		[NSOpenGLContext clearCurrentContext];
		[context release];
		context = nil;
	
		[window setDelegate:nil];
		[NSApp setDelegate:nil];
		[delegate release];
		delegate = nil;
	
		[window close];
		window = nil;
	}
	
	if(AutoreleasePool != nil)
	{
		[AutoreleasePool release];
		AutoreleasePool = nil;
	}
}

const char * MWindow::getTempDirectory(void)
{
	static char tempDirectory[256];
	NSString * temp = NSTemporaryDirectory();
	strcpy(tempDirectory, [temp cStringUsingEncoding:NSUTF8StringEncoding]);
	return tempDirectory;
	
	//return "/tmp"; // linux style
}

const char * MWindow::getCurrentDirectory(void)
{
	static char currentDirectory[256];
	getcwd(currentDirectory, 256);
	return currentDirectory;
}

void MWindow::setCursorPos(int x, int y)
{
	CGSetLocalEventsSuppressionInterval(0.0);
	
    NSPoint localPoint = NSMakePoint(x, y);
    NSPoint globalPoint = [window convertBaseToScreen:localPoint];
    CGPoint mainScreenOrigin = CGDisplayBounds(CGMainDisplayID()).origin;
    double mainScreenHeight = CGDisplayBounds(CGMainDisplayID()).size.height;
    CGPoint targetPoint = CGPointMake(globalPoint.x - mainScreenOrigin.x, mainScreenHeight - globalPoint.y - mainScreenOrigin.y);
    CGDisplayMoveCursorToPoint(CGMainDisplayID(), targetPoint);
}

void MWindow::hideCursor(void)
{
	if(CGCursorIsVisible())
    {
        [NSCursor hide];
    }
}

void MWindow::showCursor(void)
{
	if(! CGCursorIsVisible())
	{
		[NSCursor unhide];
	}
}

void MWindow::setTitle(const char * title)
{
	[window setTitle:[NSString stringWithCString:title encoding:NSISOLatin1StringEncoding]];
}

void MWindow::setCurrentDirectory(const char * directory)
{
	//chdir(directory);
}

void MWindow::setWorkingDirectory(const char * directory)
{
	strcpy(m_workingDirectory, directory);
}

void MWindow::sendEvents(MWinEvent * events)
{
	MKeyboard * keyboard = MKeyboard::getInstance();
	MMouse * mouse = MMouse::getInstance();
	
	switch(events->type)
	{
		case MWIN_EVENT_KEY_DOWN:
			keyboard->onKeyDown(events->data[0]);
			break;
			
		case MWIN_EVENT_KEY_UP:
			keyboard->onKeyUp(events->data[0]);
			break;
			
		case MWIN_EVENT_WINDOW_RESIZE:
			m_width = (unsigned int)events->data[0];
			m_height = (unsigned int)events->data[1];
			break;
			
		case MWIN_EVENT_WINDOW_MOVE:
			m_position[0] = events->data[0];
			m_position[1] = events->data[1];
			break;
			
		case MWIN_EVENT_MOUSE_BUTTON_DOWN:
			mouse->downButton(events->data[0]);
			break;
			
		case MWIN_EVENT_MOUSE_BUTTON_UP:
			mouse->upButton(events->data[0]);
			break;
			
		case MWIN_EVENT_MOUSE_WHEEL_MOVE:
			mouse->setWheelDirection(events->data[0]);
			break;
	}
	
	if(m_pointerEvent)
		m_pointerEvent(events);
}


bool MWindow::isMouseOverWindow(void)
{
	MMouse * mouse = MMouse::getInstance();
	int x = mouse->getXPosition();
	int y = mouse->getYPosition();
	
	if(x >= 0 && y >= 0 && x < (int)getWidth() && y < (int)getHeight())
		return true;
	
	return false;
}

unsigned long MWindow::getSystemTick(void)
{
	static NSTimeInterval startTime = [NSDate timeIntervalSinceReferenceDate];
    return (unsigned long)(([NSDate timeIntervalSinceReferenceDate] - startTime ) * 1000.0);
}

bool MWindow::onEvents(void)
{	
	NSEvent *event;
	
    do
    {
        event = [NSApp nextEventMatchingMask:NSAnyEventMask
                       untilDate:[NSDate distantPast]
                       inMode:NSDefaultRunLoopMode
                       dequeue:YES];
		
        if(event)
            [NSApp sendEvent:event];
    }
    while(event);
	
    [AutoreleasePool drain];
    AutoreleasePool = [[NSAutoreleasePool alloc] init];
	
	return true;
}

void MWindow::swapBuffer(void)
{
	[context flushBuffer];
}

bool MWindow::create(const char * title, unsigned int width, unsigned int height, int colorBits, bool fullscreen, bool vsync, int samples)
{
    delegate = [[GLFWWindowDelegate alloc] init];
    if(delegate == nil)
    {
        return false;
    }
	
    [NSApp setDelegate:delegate];
	
	
    CFDictionaryRef fullscreenMode = NULL;
    if(fullscreen)
    {
        fullscreenMode =
		// I think it's safe to pass 0 to the refresh rate for this function
		// rather than conditionalizing the code to call the version which
		// doesn't specify refresh...
		CGDisplayBestModeForParametersAndRefreshRateWithProperty(
																 CGMainDisplayID(),
																 32,
																 width,
																 height,
																 0,
																 kCGDisplayModeIsSafeForHardware,
																 NULL);
		
        width = [[(id)fullscreenMode objectForKey:(id)kCGDisplayWidth] intValue];
        height = [[(id)fullscreenMode objectForKey:(id)kCGDisplayHeight] intValue];
    }
	
    unsigned int styleMask = 0;
    if(! fullscreen)
    {
		styleMask = NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask;
    }
    else
    {
        styleMask = NSBorderlessWindowMask;
    }
	
    window = [[NSWindow alloc]
			  initWithContentRect:NSMakeRect(0, 0, width, height)
			  styleMask:styleMask
			  backing:NSBackingStoreBuffered
			  defer:NO];
    [window setContentView:[[GLFWContentView alloc] init]];
    [window setDelegate:delegate];
    [window setAcceptsMouseMovedEvents:YES];
	
    if(fullscreen)
    {
        CGCaptureAllDisplays();
        CGDisplaySwitchToMode(CGMainDisplayID(), fullscreenMode);
    }
	
    unsigned int attribute_count = 0;
	#define ADD_ATTR(x) attributes[attribute_count++] = x
	#define ADD_ATTR2(x, y) (void)({ ADD_ATTR(x); ADD_ATTR(y); })
	#define MAX_ATTRS 24
    NSOpenGLPixelFormatAttribute attributes[MAX_ATTRS];
	
	// attributes
    ADD_ATTR(NSOpenGLPFADoubleBuffer);
	
    if(fullscreen)
    {
        ADD_ATTR(NSOpenGLPFAFullScreen);
        ADD_ATTR(NSOpenGLPFANoRecovery);
        ADD_ATTR2(NSOpenGLPFAScreenMask, CGDisplayIDToOpenGLDisplayMask(CGMainDisplayID()));
    }
	
    ADD_ATTR2(NSOpenGLPFAColorSize, 32);
	
    /*if(alphaBits > 0)
    {
        ADD_ATTR2(NSOpenGLPFAAlphaSize, alphaBits);
    }*/
	
    //if(depthBits)
    {
        ADD_ATTR2(NSOpenGLPFADepthSize, 32);
    }
	
    //if(stencilBits > 0)
    {
        ADD_ATTR2(NSOpenGLPFAStencilSize, 8);
    }
	
	/*if(accum)
    {
        ADD_ATTR2(NSOpenGLPFAAccumSize, 64);
    }*/
	
	if(samples > 0)
    {
        ADD_ATTR2(NSOpenGLPFASampleBuffers, 1);
        ADD_ATTR2(NSOpenGLPFASamples, samples);
    }

    ADD_ATTR(0);
	
	// pixel format
    pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes];
    if(pixelFormat == nil)
    {
        return false;
    }
	
    context = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:nil];
    if(context == nil)
    {
        return false;
    }
	
    [window makeKeyAndOrderFront:nil];
    [context setView:[window contentView]];
	
    if(fullscreen)
    {
        [[window contentView] enterFullScreenMode:[NSScreen mainScreen] withOptions:nil];
    }
	
    [context makeCurrentContext];
	
	// title
	if(title != NULL)
		setTitle(title);

	
	// v-sync
	if(vsync)
	{
		GLint sync = 1;
		[context setValues:&sync forParameter:NSOpenGLCPSwapInterval];
	}
	else
	{
		GLint sync = 0;
		[context setValues:&sync forParameter:NSOpenGLCPSwapInterval];
	}
	
	// window scale
	NSRect contentRect = [window contentRectForFrameRect:[window frame]];
	m_width = contentRect.size.width;
	m_height = contentRect.size.height;
	
	if(title == NULL)
	{
		[window setStyleMask:NSBorderlessWindowMask];
		[NSApp activateIgnoringOtherApps:YES]; 
	}
	else
	{
		[window center];
	}
	
	

	
	
	return true;
}

void MWindow::execute(const char * path, const char * args)
{
	NSString * _path = [NSString stringWithCString:path encoding:NSISOLatin1StringEncoding];
	NSString * _args = [NSString stringWithCString:args encoding:NSISOLatin1StringEncoding];
	
	NSArray * arguments = [NSArray arrayWithObjects: _args, nil, nil];
	
	NSTask *task;
	task = [[NSTask alloc] init];
	[task setLaunchPath:_path];
	[task setArguments:arguments];
	[task launch];
}

void MWindow::sleep(double time)
{
    if( time == 0.0 )
    {
		sched_yield();
		return;
    }
	
    struct timeval  currenttime;
    struct timespec wait;
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    long dt_sec, dt_usec;
	
    // Not all pthread implementations have a pthread_sleep() function. We
    // do it the portable way, using a timed wait for a condition that we
    // will never signal. NOTE: The unistd functions sleep/usleep suspends
    // the entire PROCESS, not a signle thread, which is why we can not
    // use them to implement glfwSleep.
	
    // Set timeout time, relatvie to current time
    gettimeofday( &currenttime, NULL );
    dt_sec  = (long) time;
    dt_usec = (long) ((time - (double)dt_sec) * 1000000.0);
    wait.tv_nsec = (currenttime.tv_usec + dt_usec) * 1000L;
    if( wait.tv_nsec > 1000000000L )
    {
        wait.tv_nsec -= 1000000000L;
        dt_sec ++;
    }
    wait.tv_sec  = currenttime.tv_sec + dt_sec;
	
    // Initialize condition and mutex objects
    pthread_mutex_init( &mutex, NULL );
    pthread_cond_init( &cond, NULL );
	
    // Do a timed wait
    pthread_mutex_lock( &mutex );
    pthread_cond_timedwait( &cond, &mutex, &wait );
    pthread_mutex_unlock( &mutex );
	
    // Destroy condition and mutex objects
    pthread_mutex_destroy( &mutex );
    pthread_cond_destroy( &cond );
}


bool MWindow::getOpenMultipleFiles(const char * title, const char * filter, string * repertory, list <string> * filesList, const char * startPath)
{ 
	int i;
	static char filename[256];
	NSOpenPanel * openDlg = [NSOpenPanel openPanel];
	
	NSString * path = nil;
	if(startPath)
		path = [NSString stringWithCString:startPath encoding:NSISOLatin1StringEncoding];
	
	[openDlg setCanChooseFiles:YES];
	[openDlg setAllowsMultipleSelection:YES];
	
	if([openDlg runModalForDirectory:path file:nil] == NSOKButton)
	{
		NSArray* files = [openDlg filenames];
		
		if([files count] > 0)
		{
			char rep[256];
			NSString * file = [files objectAtIndex:0];
			const char * string = (const char *)[file UTF8String];
			getRepertory(rep, string);
			(*repertory) = rep;
			
			for(i = 0; i<[files count]; i++)
			{
				file = [files objectAtIndex:i];
				string = (const char *)[file UTF8String];
				
				getLocalFilename(filename, rep, string);
				filesList->push_back(filename);
			}
			
			[window makeKeyAndOrderFront:window];
			return true;
		}
	}	
	
	[window makeKeyAndOrderFront:window];
	return false;
}

const char * MWindow::getOpenDir(const char * title, const char * startPath)
{
	int i;
	static char filename[256];
	NSOpenPanel * openDlg = [NSOpenPanel openPanel];
	
	NSString * path = nil;
	if(startPath)
		path = [NSString stringWithCString:startPath encoding:NSISOLatin1StringEncoding];
	
	[openDlg setCanChooseFiles:NO];
	[openDlg setCanChooseDirectories:YES];
	
	if([openDlg runModalForDirectory:path file:nil] == NSOKButton)
	{
		NSArray * files = [openDlg filenames];
		
		for(i = 0; i<[files count]; i++)
		{
			NSString * file = [files objectAtIndex:i];
			const char * string = (const char *)[file UTF8String];
			
			strcpy(filename, string);
			[window makeKeyAndOrderFront:window];
			return filename;
		}
	}	
	
	[window makeKeyAndOrderFront:window];
	return NULL;	
}

const char * MWindow::getOpenFilename(const char * title, const char * filter, const char * startPath)
{
	int i;
	static char filename[256];
	NSOpenPanel * openDlg = [NSOpenPanel openPanel];
	
	NSString * path = nil;
	if(startPath)
		path = [NSString stringWithCString:startPath encoding:NSISOLatin1StringEncoding];
	
	[openDlg setCanChooseFiles:YES];
	
	if([openDlg runModalForDirectory:path file:nil] == NSOKButton)
	{
		NSArray* files = [openDlg filenames];
		
		for(i = 0; i<[files count]; i++)
		{
			NSString * file = [files objectAtIndex:i];
			const char * string = (const char *)[file UTF8String];
			
			strcpy(filename, string);
			[window makeKeyAndOrderFront:window];
			return filename;
		}
	}	
	
	[window makeKeyAndOrderFront:window];
	return NULL;
}

const char * MWindow::getSaveFilename(const char * title, const char * filter, const char * startPath)
{ 
	static char filename[256];
	NSSavePanel * saveDlg = [NSSavePanel savePanel];
	
	NSString * path = nil;
	if(startPath)
		path = [NSString stringWithCString:startPath encoding:NSISOLatin1StringEncoding];
	
	if([saveDlg runModalForDirectory:path file:nil] == NSOKButton)
	{
		NSString * file = [saveDlg filename];
		const char * string = (const char *)[file UTF8String];
		strcpy(filename, string);
		[window makeKeyAndOrderFront:window];
		return filename;
	}
	
	[window makeKeyAndOrderFront:window];
	return NULL;
}