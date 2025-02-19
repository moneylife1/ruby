/**
 * @file defines.h
 *
 * Macro definitions used throughout the prism library.
 *
 * This file should be included first by any *.h or *.c in prism for consistency
 * and to ensure that the macros are defined before they are used.
 */
#ifndef PRISM_DEFINES_H
#define PRISM_DEFINES_H

#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/**
 * We want to be able to use the PRI* macros for printing out integers, but on
 * some platforms they aren't included unless this is already defined.
 */
#define __STDC_FORMAT_MACROS

#include <inttypes.h>

/**
 * By default, we compile with -fvisibility=hidden. When this is enabled, we
 * need to mark certain functions as being publically-visible. This macro does
 * that in a compiler-agnostic way.
 */
#ifndef PRISM_EXPORTED_FUNCTION
#   ifdef PRISM_EXPORT_SYMBOLS
#       ifdef _WIN32
#          define PRISM_EXPORTED_FUNCTION __declspec(dllexport) extern
#       else
#          define PRISM_EXPORTED_FUNCTION __attribute__((__visibility__("default"))) extern
#       endif
#   else
#       define PRISM_EXPORTED_FUNCTION
#   endif
#endif

/**
 * Certain compilers support specifying that a function accepts variadic
 * parameters that look like printf format strings to provide a better developer
 * experience when someone is using the function. This macro does that in a
 * compiler-agnostic way.
 */
#if defined(__GNUC__)
#   define PRISM_ATTRIBUTE_FORMAT(string_index, argument_index) __attribute__((format(printf, string_index, argument_index)))
#elif defined(__clang__)
#   define PRISM_ATTRIBUTE_FORMAT(string_index, argument_index) __attribute__((__format__(__printf__, string_index, argument_index)))
#else
#   define PRISM_ATTRIBUTE_FORMAT(string_index, argument_index)
#endif

/**
 * GCC will warn if you specify a function or parameter that is unused at
 * runtime. This macro allows you to mark a function or parameter as unused in a
 * compiler-agnostic way.
 */
#if defined(__GNUC__)
#   define PRISM_ATTRIBUTE_UNUSED __attribute__((unused))
#else
#   define PRISM_ATTRIBUTE_UNUSED
#endif

/**
 * Old Visual Studio versions do not support the inline keyword, so we need to
 * define it to be __inline.
 */
#if defined(_MSC_VER) && !defined(inline)
#   define inline __inline
#endif

/**
 * Old Visual Studio versions before 2015 do not implement sprintf, but instead
 * implement _snprintf. We standard that here.
 */
#if !defined(snprintf) && defined(_MSC_VER) && (_MSC_VER < 1900)
#   define snprintf _snprintf
#endif

/**
 * A simple utility macro to concatenate two tokens together, necessary when one
 * of the tokens is itself a macro.
 */
#define PM_CONCATENATE(left, right) left ## right

/**
 * We want to be able to use static assertions, but they weren't standardized
 * until C11. As such, we polyfill it here by making a hacky typedef that will
 * fail to compile due to a negative array size if the condition is false.
 */
#if defined(_Static_assert)
#   define PM_STATIC_ASSERT(line, condition, message) _Static_assert(condition, message)
#else
#   define PM_STATIC_ASSERT(line, condition, message) typedef char PM_CONCATENATE(static_assert_, line)[(condition) ? 1 : -1]
#endif

/**
 * In general, libc for embedded systems does not support memory-mapped files.
 * If the target platform is POSIX or Windows, we can map a file in memory and
 * read it in a more efficient manner.
 */
#ifdef _WIN32
#   define PRISM_HAS_MMAP
#else
#   include <unistd.h>
#   ifdef _POSIX_MAPPED_FILES
#       define PRISM_HAS_MMAP
#   endif
#endif

/**
 * isinf on Windows is defined as accepting a float, but on POSIX systems it
 * accepts a float, a double, or a long double. We want to mirror this behavior
 * on windows.
 */
#ifdef _WIN32
#   include <float.h>
#   undef isinf
#   define isinf(x) (sizeof(x) == sizeof(float) ? !_finitef(x) : !_finite(x))
#endif

/**
 * If you build prism with a custom allocator, configure it with
 * "-D PRISM_XALLOCATOR" to use your own allocator that defines xmalloc,
 * xrealloc, xcalloc, and xfree.
 *
 * For example, your `prism_xallocator.h` file could look like this:
 *
 * ```
 * #ifndef PRISM_XALLOCATOR_H
 * #define PRISM_XALLOCATOR_H
 * #define xmalloc      my_malloc
 * #define xrealloc     my_realloc
 * #define xcalloc      my_calloc
 * #define xfree        my_free
 * #endif
 * ```
 */
#ifdef PRISM_XALLOCATOR
    #include "prism_xallocator.h"
#else
    #ifndef xmalloc
        /**
         * The malloc function that should be used. This can be overriden with
         * the PRISM_XALLOCATOR define.
         */
        #define xmalloc malloc
    #endif

    #ifndef xrealloc
        /**
         * The realloc function that should be used. This can be overriden with
         * the PRISM_XALLOCATOR define.
         */
        #define xrealloc realloc
    #endif

    #ifndef xcalloc
        /**
         * The calloc function that should be used. This can be overriden with
         * the PRISM_XALLOCATOR define.
         */
        #define xcalloc calloc
    #endif

    #ifndef xfree
        /**
         * The free function that should be used. This can be overriden with the
         * PRISM_XALLOCATOR define.
         */
        #define xfree free
    #endif
#endif

#endif
