/*
    Copyright 2017-2018 Dynatrace LLC

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#ifndef ONESDK_CONFIG_H_INCLUDED
#define ONESDK_CONFIG_H_INCLUDED

/* IWYU pragma: private, include "onesdk/onesdk_common.h" */

/** @file
    @brief Defines platform/compiler dependent types/constants/macros. Applications should not include this file directly.
*/

/*========================================================================================================================================*/

#include <stdint.h>     /* for fixed width integer types (int32_t, uint32_t etc.) */
#include <stddef.h>     /* for size_t */

/*========================================================================================================================================*/

#if defined(ONESDK_BUILD_LIBRARY) && !defined(ONESDK_BUILD_TEST) && defined(__cplusplus)
/* Make sure the SDK stub is not compiled as C++.
   Among other things, this prevents inline function from being visible in the SDK stub, which is a good thing.
   (In C we use 'static inline', which means if they're compiled for the SDK stub they will have internal linkage.) */
#    error The SDK stub itself must be compiled as pure C
#endif

/* Calling convention */

/** @internal */
#if defined(_WIN32)
#    define ONESDK_CALL __stdcall
#else
#    define ONESDK_CALL
#endif

/* Macros for function visibility */

/** @internal */
#if defined(ONESDK_BUILD_LIBRARY)
#    if   defined(_WIN32)
#        if defined(ONESDK_SHARED)
#            define ONESDK_EXPORT __declspec(dllexport)
#        else
#            define ONESDK_EXPORT
#        endif
#    elif defined(_AIX)
#         define ONESDK_EXPORT
#    elif defined(__GNUC__) && ((__GNUC__ + 0) >= 4)
#        if defined(ONESDK_SHARED)
#            define ONESDK_EXPORT __attribute__((visibility("default")))
#        else
#            define ONESDK_EXPORT
#        endif
#    else
#        define ONESDK_EXPORT
#    endif
#else
#    define ONESDK_EXPORT
#endif

/** @internal */
#if   defined(_WIN32)
#    define ONESDK_HIDDEN
#elif defined(_AIX)
#    define ONESDK_HIDDEN
#elif defined(__GNUC__) && ((__GNUC__ + 0) >= 4)
#    define ONESDK_HIDDEN __attribute__((visibility("hidden")))
#else
#    define ONESDK_HIDDEN
#endif

/* Macros for uniform external linkage specifications in C and C++ */

#if defined(__cplusplus)
#    define ONESDK_DECLARE_EXTERN_C extern "C"      /**< @internal */
#    define ONESDK_DEFINE_EXTERN_C extern "C"       /**< @internal */
#else
#    define ONESDK_DECLARE_EXTERN_C extern          /**< @internal */
#    define ONESDK_DEFINE_EXTERN_C                  /**< @internal */
#endif

/* Macros for inline functions */

/** @internal */
#if   defined(ONESDK_ATTRIBUTE_UNUSED)
/* OK, allow user to define this, in case our definition doesn't work with their compiler */
#elif defined(__GNUC__) && (__GNUC__ >= 4)
#    define ONESDK_ATTRIBUTE_UNUSED __attribute__((__unused__))
#elif defined(__clang__)
#    define ONESDK_ATTRIBUTE_UNUSED __attribute__((__unused__))
#else
#    define ONESDK_ATTRIBUTE_UNUSED
#endif

/** @internal */
#if   defined(ONESDK_INLINE)
/* OK, allow user to define this, in case our definition doesn't work with their compiler */
#elif defined(__cplusplus)
#    define ONESDK_INLINE inline
#elif (__STDC_VERSION__ >= 199901L)
#    define ONESDK_INLINE static inline
#elif defined(_MSC_VER) && ((_MSC_VER + 0) >= 1900) && !defined(_BullseyeCoverage)
#    define ONESDK_INLINE static inline
#else
#    define ONESDK_INLINE static
#endif

/* Macros for declaring and defining functions */

/** @internal */
#define ONESDK_DECLARE_FUNCTION(return_type) ONESDK_DECLARE_EXTERN_C ONESDK_EXPORT return_type ONESDK_CALL 
/** @internal */
#define ONESDK_DECLARE_INTERNAL_FUNCTION(return_type) ONESDK_DECLARE_EXTERN_C ONESDK_HIDDEN return_type ONESDK_CALL 
/** @internal */
#define ONESDK_DEFINE_INLINE_FUNCTION(return_type) ONESDK_INLINE return_type ONESDK_ATTRIBUTE_UNUSED 

/*========================================================================================================================================*/

/* Macros for endianness */

#if defined(ONESDK_LITTLE_ENDIAN) || defined(ONESDK_BIG_ENDIAN)
/* OK, allow user to define this, in case our detection code doesn't work with their compiler */
#else
/* Try to detect endianness. */
/* Check for little endian... (yes, we could mash all this into one big expression, but it's confusing enough as it is) */
#    if   defined(__BYTE_ORDER) && defined(__LITTLE_ENDIAN) && ((__BYTE_ORDER + 0) == (__LITTLE_ENDIAN + 0))
#        define ONESDK_LITTLE_ENDIAN
#    elif defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && ((__BYTE_ORDER__ + 0) == (__ORDER_LITTLE_ENDIAN__ + 0))
#        define ONESDK_LITTLE_ENDIAN
#    elif (defined(__LITTLE_ENDIAN__) && !defined(__BIG_ENDIAN__)) || (defined(_LITTLE_ENDIAN) && !defined(_BIG_ENDIAN))
#        define ONESDK_LITTLE_ENDIAN
#    elif defined(__ARMEL__) || defined(__THUMBEL__) || defined(__AARCH64EL__) || defined(_MIPSEL) || defined(__MIPSEL) || defined(__MIPSEL__)
#        define ONESDK_LITTLE_ENDIAN
#    elif defined(__i386__) || defined(__i386) || defined(__x86_64__) || defined(__x86_64) || defined(__amd64__) || defined(__amd64) || defined(_X86_) || defined(_M_IX86) || defined(_M_X64) || defined(_M_AMD64) || defined(__THW_INTEL__)
#        define ONESDK_LITTLE_ENDIAN
#    endif
/* Check for big endian... */
#    if   defined(__BYTE_ORDER) && defined(__BIG_ENDIAN) && ((__BYTE_ORDER + 0) == (__BIG_ENDIAN + 0))
#        define ONESDK_BIG_ENDIAN
#    elif defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && ((__BYTE_ORDER__ + 0) == (__ORDER_BIG_ENDIAN__ + 0))
#        define ONESDK_BIG_ENDIAN
#    elif (defined(__BIG_ENDIAN__) && !defined(__LITTLE_ENDIAN__)) || (defined(_BIG_ENDIAN) && !defined(_LITTLE_ENDIAN))
#        define ONESDK_BIG_ENDIAN
#    elif defined(__ARMEB__) || defined(__THUMBEB__) || defined(__AARCH64EB__) || defined(_MIPSEB) || defined(__MIPSEB) || defined(__MIPSEB__)
#        define ONESDK_BIG_ENDIAN
#    elif defined(__sparc__) || defined(__sparc)
#        define ONESDK_BIG_ENDIAN
#    elif defined(__s390__) || defined(__s390x__) || defined(__MVS__) || defined(__TOS_MVS__)
#        define ONESDK_BIG_ENDIAN
#    endif
#endif

/* Make sure we didn't end up with both, little _and_ big endian. */
#if defined(ONESDK_LITTLE_ENDIAN) && defined(ONESDK_BIG_ENDIAN)
#    error ONESDK_LITTLE_ENDIAN and ONESDK_BIG_ENDIAN defined at the same time.
#endif

/*========================================================================================================================================*/

/** @brief Type for result codes of the SDK stub.

    Since we also returns native system error codes, we use `uint32_t` on Windows (`GetLastError` -> `DWORD`) and `int32_t`
    on other systems (`errno` -> `int`).
*/
#if defined(_WIN32)
typedef uint32_t onesdk_result_t;
#else
typedef int32_t onesdk_result_t;
#endif

/** @internal
    Base value for SDK stub error codes.
    Derived by building a Windows HRESULT code with the following properties:
        - "S" bit (error) = 1
        - "R" bit (NT status' severity bit) = 0
        - "C" bit (customer) = 1
        - "N" bit (tag to indicate mapped NT status codes) = 0
        - "X" bit (reserved, should not bet set by MS error codes) = 1
        - Facility = 0x7FE, which is the second-to-last facility code
          (We want to use a high facility code, but not necessarily the last one -> second-to-last should be fine)

    Altogether, that should make collisions with real Windows error codes VERY unlikely.

    Since this also seems like a reasonable base value for POSIX systems, we simply use the same value
    (only interpreted as a signed 32 bit integer instead of unsigned).
*/
#if defined(_WIN32)
#    define ONESDK_ERROR_BASE   ((onesdk_result_t) 0xAFFE0000)
#else
#    define ONESDK_ERROR_BASE   ((onesdk_result_t)-0x50020000) // = same bit pattern as 0xAFFE0000
#endif

/*========================================================================================================================================*/

/** @name Integer Types
    @{
    @anchor integer_types
*/

typedef int32_t onesdk_bool_t;      /**< @brief Type for boolean values. Zero means `false`, any non-zero value means `true`. */

typedef int32_t onesdk_int32_t;     /**< @brief Signed 32 bit integer. */
typedef int64_t onesdk_int64_t;     /**< @brief Signed 64 bit integer. */
typedef uint32_t onesdk_uint32_t;   /**< @brief Unsigned 32 bit integer. */
typedef uint64_t onesdk_uint64_t;   /**< @brief Unsigned 64 bit integer. */

typedef size_t onesdk_size_t;       /**< @brief Unsigned integer type used for sizes (e.g. array sizes) and counts. */

/** @} */

/*========================================================================================================================================*/

/** @brief The native character type of the operating system. `wchar_t` for Windows and `char` for other systems. */
#if   defined(ONESDK_BUILD_DOC)
typedef char_or_wchar_t onesdk_xchar_t;
#elif defined(_WIN32)
typedef wchar_t onesdk_xchar_t;
#else
typedef char onesdk_xchar_t;
#endif

/** @hideinitializer @brief `printf` type string for a char string referenced by a @ref onesdk_xchar_t format string. */
#if defined(_WIN32)
#    define ONESDK_XSTR_PRI_STR L"S"
#else
#    define ONESDK_XSTR_PRI_STR "s"
#endif

/** @hideinitializer @brief `printf` type string for a @ref onesdk_xchar_t string referenced by a char format string. */
#if defined(_WIN32)
#    define ONESDK_STR_PRI_XSTR "S"
#else
#    define ONESDK_STR_PRI_XSTR "s"
#endif

/** @internal */
#if defined(_WIN32)
#    define ONESDK_XSTR_IMPL(str) L ## str
#else
#    define ONESDK_XSTR_IMPL(str) str
#endif

/** @hideinitializer @brief Macro for creating native OS character (@ref onesdk_xchar_t) string literals. */
#define ONESDK_XSTR(str) ONESDK_XSTR_IMPL(str)

/*========================================================================================================================================*/

#endif /* ONESDK_CONFIG_H_INCLUDED */

