#include "tgsys.h"

#ifdef _WIN32
#include <Windows.h>
#else
#include <time.h>
#endif


void TGSleep(long msec){
    #ifdef _WIN32
    Sleep(msec);
    #else
    struct timespec t;
    t.tv_sec = msec / 1000;
    t.tv_nsec = (msec % 1000) * 1000000;
    nanosleep(&t, &t);
    #endif
}