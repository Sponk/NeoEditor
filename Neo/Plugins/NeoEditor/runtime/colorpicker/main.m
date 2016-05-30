#define GNUSTEP
#import <AppKit/AppKit.h>

#define SEXPRESSO_OPT_OUT_PIKESTYLE
#include <sexpresso.hpp>
#include <sstream>
#include <iostream>

int main(int argc, char* argv[])
{
	NSApplication *app;
	NSAutoreleasePool *pool;

	std::stringstream ss;
	ss << std::cin.rdbuf();

	auto config = sexpresso::parse(ss.str());
	std::string title = config.getChildByPath("title")->arguments().begin()->value.str;
	std::string text = config.getChildByPath("text")->arguments().begin()->value.str;

	pool = [NSAutoreleasePool new];
	app = [NSApplication sharedApplication];

	NSAlert *alert = [[NSAlert alloc] init];
	[alert addButtonWithTitle:@"OK"];
	[alert setMessageText:[NSString stringWithCString:title.c_str() encoding:NSUTF8StringEncoding]];
	[alert setInformativeText:[NSString stringWithCString:text.c_str() encoding:NSUTF8StringEncoding]];
	[alert setAlertStyle:NSWarningAlertStyle];

	[alert runModal];
	return 0;
}
