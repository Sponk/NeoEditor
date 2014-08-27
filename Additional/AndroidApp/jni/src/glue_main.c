#include <stdio.h>
#include <SDL.h>

extern int player_main(int argc, char* argv[]);

const char* asset_directory = "/sdcard/neo-android-demo";

int main(int argc, char* argv[])
{
	return player_main(argc, argv);
} 

