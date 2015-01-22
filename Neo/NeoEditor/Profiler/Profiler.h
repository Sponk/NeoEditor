#ifndef ___PROFILER_H___
#define ___PROFILER_H___

#include <Shiny.h>

class Profiler
{
    static Profiler pr;
public:
    Profiler* getInstance() {return &pr;}

};

#endif
