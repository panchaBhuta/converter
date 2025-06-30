// File borrowed and updated by Gautam Dhar Jun. 2025
//   additional information updated from url source ...
//   https://web.archive.org/web/20191012035921/http://nadeausoftware.com/articles/2012/01/c_c_tip_how_use_compiler_predefined_macros_detect_operating_system

// platform.h
// By Gabriel Staples
// Apr. 2025
// 
// From: 
// 1. https://stackoverflow.com/a/79228659/4561887
// 2. https://github.com/ElectricRCAircraftGuy/eRCaGuy_Linux_Windows_CMake_Sockets_MSYS2/blob/main/src/platform.h
// 
// Print out some messages at compile time to show which platform we are 
// compiling on.
// - This helps us debug so we can get the correct static asserts on sizes 
//   for the various platforms on which we compile. 
// - See also: 
//   1. https://sourceforge.net/p/predef/wiki/OperatingSystems/
// 
// Note that on my 64-bit x86-64 computer, the sizes of types are the same on 
// `__CYGWIN__` (using the base MSYS2 terminal) on Windows as they are on
// `__gnu_linux__` on Linux. 

#pragma once

#define STRINGIZE2(s) #s
#define STRINGIZE(s) STRINGIZE2(s)

#pragma message(">>> Detecting your platform... <<<")

// Automatically defined by your compiler/OS/platform

#ifdef unix
    #pragma message("UNIX-style(Environment) OS -> unix")
#endif
#ifdef __unix
    #pragma message("UNIX-style(Environment) OS -> __unix")
#endif
#ifdef __unix__
    #pragma message("UNIX-style(Environment) OS -> __unix__")
#endif

#if !defined(_WIN32) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
	/* UNIX-style OS. ------------------------------------------- */
#include <unistd.h>
#if defined(_POSIX_VERSION)
	/* POSIX compliant */
    #pragma message("POSIX-compliant OS -> _POSIX_VERSION = " STRINGIZE(_POSIX_VERSION))
#endif
#endif

#ifdef linux
    #pragma message("OS based on the Linux kernel -> linux  :  Macro-Obsolete (it's not POSIX compliant)")
#endif
#ifdef __linux
    #pragma message("OS based on the Linux kernel -> _linux  :  Macro-Obsolete (it's not POSIX compliant)")
#endif
#ifdef __linux__
    #pragma message("OS based on the Linux kernel -> __linux__")
#endif

#ifdef __GNU__
    #pragma message("Compiling on __GNU__")
#endif
#ifdef __gnu_linux__
    #pragma message("Compiling on __gnu_linux__")
#endif
#ifdef __gnu_hurd__
    #pragma message("Compiling on __gnu_hurd__")
#endif

#ifdef __APPLE__
    #pragma message("Apple-platform __APPLE__")
#endif
#ifdef __MACH__
    #pragma message("MacOS __MACH__")
#endif
#if defined(__APPLE__) && defined(__MACH__)
  #if defined(__clang_major__) && defined(__clang_minor__)
      #pragma message("Compiling using Apple-Clang : __clang_major__(" STRINGIZE(__clang_major__) ").__clang_minor__(" STRINGIZE(__clang_minor__) ")" )
  #endif
  #ifdef __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__
      #pragma message("__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ = " STRINGIZE(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) )
  #endif
  #ifdef __ENVIRONMENT_MAC_OS_X_VERSION_MAX_ALLOWED__
      #pragma message("__ENVIRONMENT_MAC_OS_X_VERSION_MAX_ALLOWED__ = " STRINGIZE(__ENVIRONMENT_MAC_OS_X_VERSION_MAX_ALLOWED__) )
  #endif
#endif


#ifdef WIN32
    #pragma message("Compiling on WIN32")
#endif
#ifdef _WIN32
    #pragma message("Compiling on _WIN32")
#endif
#ifdef __WIN32
    #pragma message("Compiling on __WIN32")
#endif
#ifdef __WIN32__
    #pragma message("Compiling on __WIN32__")
#endif
#ifdef WIN64
    #pragma message("Compiling on WIN64")
#endif
#ifdef _WIN64
    #pragma message("Compiling on _WIN64")
#endif
#ifdef __WIN64
    #pragma message("Compiling on __WIN64")
#endif
#ifdef __WIN64__
    #pragma message("Compiling on __WIN64__")
#endif

#ifdef __CYGWIN__
    #pragma message("Compiling on __CYGWIN__")
#endif
#ifdef __CYGWIN32__
    #pragma message("Compiling on __CYGWIN32__")
#endif
#ifdef __MINGW32__
    #pragma message("Compiling on __MINGW32__")
#endif
#ifdef __MINGW64__
    #pragma message("Compiling on __MINGW64__")
#endif
#ifdef __MSYS__
    #pragma message("Compiling on __MSYS__")
#endif
#ifdef __GNUC__
    #pragma message("Compiling on __GNUC__")
#endif

// Custom `MSYSTEM_*` definitions added by me directly via CMakeLists.txt.
// - This is based on the fact that possible `MSYSTEM` environment variable 
//   values set by the MSYS2 terminal environment on Windows include:
//  
//   1. MSYS
//   2. MINGW32
//   3. MINGW64
//   4. UCRT64
//   5. CLANG64
//   6. CLANG32
//   7. CLANGARM64
// 
// - See my answer here: https://stackoverflow.com/a/79201770/4561887

//   also refer https://sourceforge.net/p/msys2/discussion/general/thread/b7dfdac8/#3939
//              https://www.msys2.org/docs/environments/
#ifdef MSYSTEM_MSYS
    #pragma message("environment on MSYSTEM_MSYS")
#endif
#ifdef MSYSTEM_MINGW32
    #pragma message("environment on MSYSTEM_MINGW32")
#endif
#ifdef MSYSTEM_MINGW64
    #pragma message("environment on MSYSTEM_MINGW64")
#endif
#ifdef MSYSTEM_UCRT64
    #pragma message("environment on MSYSTEM_UCRT64")
#endif
#ifdef MSYSTEM_CLANG64
    #pragma message("environment on MSYSTEM_CLANG64")
#endif
#ifdef MSYSTEM_CLANG32
    #pragma message("environment on MSYSTEM_CLANG32")
#endif
#ifdef MSYSTEM_CLANGARM64
    #pragma message("environment on MSYSTEM_CLANGARM64")
#endif
#ifdef MSYSTEM_NOTSET
    #pragma message("environment on MSYSTEM_NOTSET (ie: unlisted Linux based)")
#endif


//  Architectures
//  https://sourceforge.net/p/predef/wiki/Architectures/

// AMD64
#ifdef __amd64__
    #pragma message("architecture : __amd64__")
#endif
#ifdef __amd64
    #pragma message("architecture : __amd64")
#endif
#ifdef __x86_64__
    #pragma message("architecture : __x86_64__")
#endif
#ifdef __x86_64
    #pragma message("architecture : __x86_64")
#endif
#ifdef _M_X64
    #pragma message("architecture : _M_X64")
#endif
#ifdef _M_AMD64
    #pragma message("architecture : _M_AMD64")
#endif

//  ARM
#ifdef __arm
    #pragma message("architecture : __arm")
#endif
#ifdef __arm__
    #pragma message("architecture : __arm__")
#endif
#ifdef _ARM
    #pragma message("architecture : _ARM")
#endif
#ifdef _M_ARM
    #pragma message("architecture : _M_ARM = " STRINGIZE(_M_ARM) )
#endif
//  ARM64
#ifdef __aarch64__
    #pragma message("architecture : __aarch64__")
#endif


// Intel x86
#ifdef i386
    #pragma message("architecture : i386")
#endif
#ifdef __i386
    #pragma message("architecture : __i386")
#endif
#ifdef __i386__
    #pragma message("architecture : __i386__")
#endif
#ifdef __i486__
    #pragma message("architecture : __i486__")
#endif
#ifdef __i586__
    #pragma message("architecture : __i586__")
#endif
#ifdef __i686__
    #pragma message("architecture : __i686__")
#endif
#ifdef _M_I86
    #pragma message("architecture : _M_I86")
#endif
#ifdef _M_IX86
    #pragma message("architecture : _M_IX86 = " STRINGIZE(_M_IX86) )
#endif
#ifdef __X86__
    #pragma message("architecture : __X86__")
#endif
#ifdef _X86_
    #pragma message("architecture : _X86_")
#endif
#ifdef __I86__
    #pragma message("architecture : __I86__ = " STRINGIZE(__I86__) )
#endif
#ifdef __INTEL__
    #pragma message("architecture : __INTEL__")
#endif
