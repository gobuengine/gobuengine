#if defined(_WIN32)
#if defined(__TINYC__)
#define __declspec(x) __attribute__((x))
#endif
#define GOBUAPI __declspec(dllexport) // We are building the library as a Win32 shared library (.dll)
#endif

#ifndef GOBUAPI
#define GOBUAPI // Functions defined as 'extern' by default (implicit specifiers)
#endif
