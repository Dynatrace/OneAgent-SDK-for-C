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

#ifndef ONESDK_MSVC_H_INCLUDED
#define ONESDK_MSVC_H_INCLUDED

/* IWYU pragma: private */

/** @file
    @brief Workaround for Visual C++ CRT version differences.
*/

#if defined(_MSC_VER)

#include "onesdk/onesdk_common.h"

#include <stdio.h>

/*========================================================================================================================================*/

#if defined(ONESDK_AUTO_LINK) && !defined(ONESDK_BUILD_LIBRARY)

#if _MSC_VER >= 1900
/* The CRTs of Visual C++ 2015 and later provide the printf/scanf family as inline functions, but our static LIB requires them as normal
   functions. => Link with legacy_stdio_definitions.lib where Visual C++ still provides them. */
#pragma comment(lib, "legacy_stdio_definitions.lib")
#endif

#if defined(ONESDK_SHARED)
#pragma comment(lib, "onesdk_shared.lib")
#else
#pragma comment(lib, "onesdk_static.lib")
#endif

#endif /* defined(ONESDK_AUTO_LINK) && !defined(ONESDK_BUILD_LIBRARY) */

/*========================================================================================================================================*/

/* The CRTs of Visual C++ 2015 and later use a different internal helper function for locating stdin/stdout/stderr than older Visual C++
   versions. That makes static libraries incompatible between the two "generations".

   We can work around that by defining our own helper function in the consuming program which locates stderr for us.
   Since we potentially do this in multiple translation units, we need __declspec(selectany), and since __declspec(selectany) only works
   on variables and not on functions, we use a function pointer. */

#if !defined(ONESDK_SHARED)
#if defined(ONESDK_BUILD_LIBRARY)

/* Declare the pointer to our helper function for building our static library. */

/** @internal */
#if defined(__cplusplus)
extern "C"
#else
extern
#endif
FILE* (ONESDK_CALL* const onesdk_get_stderr)(void);

#else

/* Define the pointer to our helper function in the consuming application. */

/** @internal */
static FILE* ONESDK_CALL onesdk_get_stderr_impl(void) {
    return stderr;
}

/** @internal */
#if defined(__cplusplus)
extern "C"
#else
extern
#endif
__declspec(selectany) FILE* (ONESDK_CALL* const onesdk_get_stderr)(void) = onesdk_get_stderr_impl;

#endif
#endif

/*========================================================================================================================================*/

#endif /* defined(_MSC_VER) */

#endif /* ONESDK_MSVC_H_INCLUDED */
