#ifndef NEO_PLUGIN_H
#define NEO_PLUGIN_H

#include <Canvas.h>

#ifdef _WIN32
extern __declspec(dllexport) Neo2D::Canvas MainCanvas;
#else
extern Neo2D::Canvas MainCanvas;
#endif

#endif //NEO_PLUGIN_H
