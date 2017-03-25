#ifndef __BASE_BUILDCONFIG_H__
#define __BASE_BUILDCONFIG_H__

// A set of macros to use for platform detection.
#if defined(__APPLE__)
#   define OS_MACOSX 1
#elif defined(__linux__)
#   define OS_LINUX 1
#endif

#if defined(_WIN32)
#   define OS_WIN 1
#elif defined(__FreeBSD__)
#   define OS_FREEBSD 1
#elif defined(__OpenBSD__)
#   define OS_OPENBSD 1
#elif defined(__sun)
#   define OS_SOLARIS 1
#else
#   error Please add support for your platform in build/BuildConfig.h
#endif

#if defined(OS_LINUX) || defined(OS_FREEBSD) || defined(OS_OPENBSD) || defined(OS_SOLARIS)
#   define USE_NSS 1  // Use NSS for crypto.
#   define USE_X11 1  // Use X for graphics.
#endif

// For access to standard POSIXish features, use OS_POSIX instead of a
// more specific macro.
#if defined(OS_MACOSX) || defined(OS_LINUX) || defined(OS_FREEBSD) || defined(OS_OPENBSD) || defined(OS_SOLARIS)
#   define OS_POSIX 1
#endif

// Compiler detection.
#if defined(__GNUC__)
#   define COMPILER_GCC 1
#elif defined(_MSC_VER)
#   define COMPILER_MSVC 1
#else
#   error Please add support for your compiler in build/BuildConfig.h
#endif

#endif // __BASE_BUILDCONFIG_H__

//新增导出类的定义格式
#ifndef __BASE_GLOBAL_H__
#define __BASE_GLOBAL_H__

#ifdef BASE_LIB_EXPORT_API
# define BASE_LIB_EXPORT __declspec(dllexport)
#else
# define BASE_LIB_EXPORT __declspec(dllimport)
#endif

#endif // __BASE_GLOBAL_H__
