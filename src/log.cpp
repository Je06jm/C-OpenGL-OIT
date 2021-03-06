#include "log.hpp"

#include <stdio.h>
#include <stdarg.h>

#ifdef IS_RELEASE

#ifdef _WIN32
#include <windows.h>
#endif

// This does nothing in a release build
void info(const char* format, ...) {}

void critical(const char* format, ...) {
    va_list args;
    va_start(args, format);

#ifdef _WIN32
    // Formats the string and displays it in a message box
    size_t strLength = strlen(format);
    char* msg = new char[(size_t)(strLength*1.25f)];
    vsprintf(msg, format, args);

    MessageBoxA(0, msg, "Critical Error", MB_OK);
#else
    vfprintf(stderr, format, args);
#endif
    va_end(args);

    exit(1);
}

#else

void info(const char* format, ...) {
    // Basicly printf
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
}

void critical(const char* format, ...) {
    // Basicly printf and exit
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    exit(1);
}

#endif
