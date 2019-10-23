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

#ifndef ONESDK_STRING_H_INCLUDED
#define ONESDK_STRING_H_INCLUDED

/** @file
    @brief Defines @ref onesdk_string_t, the string type used for tracing, plus related utility functions.
*/

/*========================================================================================================================================*/

#include "onesdk/onesdk_common.h"

#include <string.h>     /* for strlen */

#if defined(_WIN32)
#include <wchar.h>      /* for wcslen */
#endif

/*========================================================================================================================================*/

/** @defgroup strings Strings
    @brief See @ref onesdk_string_t for more information about SDK strings.
    @{
*/

/*========================================================================================================================================*/

/** @brief Used to store CCSID values. */
typedef uint16_t onesdk_ccsid_t;

/** @brief Represents a reference to string data in a user specified encoding.

    For normal (non-null, non-empty) strings, simply set #data to point to the string data, #byte_length to the length of the string data
    in bytes and #ccsid to the CCSID value for the used encoding.

    For an empty string, set #byte_length to zero and #ccsid to any valid, supported CCSID value (i.e. _not_ @ref ONESDK_CCSID_NULL).

    If you set #byte_length to zero and #ccsid to @ref ONESDK_CCSID_NULL, the SDK will interpret this as a "null string".
    To use @ref ONESDK_CCSID_NULL together with a non-zero length is an error.

    When calling a function that takes a @ref onesdk_string_t pointer argument, you can also pass a NULL pointer to specify
    a "null string".
*/
typedef struct onesdk_string {
    void const* data;           /**< @brief Pointer to the beginning of the string data. May be `NULL` if #byte_length is zero. */
    onesdk_size_t byte_length;  /**< @brief The length of the buffer pointed to by #data, in bytes. */
    onesdk_ccsid_t ccsid;       /**< @brief Specifies the encoding of the string data. */
} onesdk_string_t;

/*========================================================================================================================================*/

/** @name CCSID Constants
    @{
    @anchor ccsid_constants
*/

/** @brief Special "NULL" CCSID value.

    Used to build "NULL strings". If @ref ONESDK_CCSID_NULL is used in a @ref onesdk_string_t.ccsid, @ref onesdk_string_t.byte_length must
    be zero.

    @see @ref onesdk_string_t.
*/
#define ONESDK_CCSID_NULL       ((onesdk_ccsid_t)   0)

#define ONESDK_CCSID_ASCII      ((onesdk_ccsid_t) 367)  /**< @brief CCSID value for 7 bit ASCII text. */
#define ONESDK_CCSID_ISO8859_1  ((onesdk_ccsid_t) 819)  /**< @brief CCSID value for ISO/IEC 8859-1 aka Latin-1 encoded text. */
#define ONESDK_CCSID_UTF8       ((onesdk_ccsid_t)1209)  /**< @brief CCSID value for UTF-8 encoded text. */
#define ONESDK_CCSID_UTF16_BE   ((onesdk_ccsid_t)1201)  /**< @brief CCSID value for UTF-16 Big Endian encoded text. */
#define ONESDK_CCSID_UTF16_LE   ((onesdk_ccsid_t)1203)  /**< @brief CCSID value for UTF-16 Little Endian encoded text. */

/** @hideinitializer @brief CCSID for UTF-16 text with the native endianness
    (either @ref ONESDK_CCSID_UTF16_LE or @ref ONESDK_CCSID_UTF16_BE).
*/
#if defined(ONESDK_BUILD_DOC)
#define ONESDK_CCSID_UTF16_NATIVE
#endif

#if defined(ONESDK_LITTLE_ENDIAN)
#define ONESDK_CCSID_UTF16_NATIVE ONESDK_CCSID_UTF16_LE
#endif

#if defined(ONESDK_BIG_ENDIAN)
#define ONESDK_CCSID_UTF16_NATIVE ONESDK_CCSID_UTF16_BE
#endif

/** @} */

/*========================================================================================================================================*/

/** @name String Constructors
    @{
*/

/** @brief Creates a @ref onesdk_string_t.
    @param data         See @ref onesdk_string_t.data
    @param byte_length  See @ref onesdk_string_t.byte_length
    @param ccsid        See @ref onesdk_string_t.ccsid

    @return A @ref onesdk_string_t with the specified fields.

    @see @ref onesdk_string_t.
*/
ONESDK_DEFINE_INLINE_FUNCTION(onesdk_string_t) onesdk_str(void const* data, onesdk_size_t byte_length, onesdk_ccsid_t ccsid) {
    onesdk_string_t str = { NULL, 0, 0 };
    str.data = data;
    str.byte_length = byte_length;
    str.ccsid = ccsid;
    return str;
}

/** @brief Creates a @ref onesdk_string_t for a byte string.
    @param data     Pointer to the string data. Must be `NULL` or point to a null-terminated buffer.
    @param ccsid    Specifies the encoding of the string data.

    @return A @ref onesdk_string_t that points to the string.

    This function computes the length of the string as `byte_length = strlen(data)` and then builds the return value
    by calling `onesdk_str(data, byte_length, ccsid)`.

    @see @ref onesdk_str
*/
ONESDK_DEFINE_INLINE_FUNCTION(onesdk_string_t) onesdk_bytestr(char const* data, onesdk_ccsid_t ccsid) {
    return onesdk_str(data, data ? ((onesdk_size_t)strlen(data)) : 0, ccsid);
}

/** @brief Creates a @ref onesdk_string_t for an ASCII string.
    @param data     Pointer to the ASCII string data. Must be `NULL` or point to a null-terminated buffer.

    @return `onesdk_bytestr(data, ONESDK_CCSID_ASCII)`

    @see @ref onesdk_bytestr
*/
ONESDK_DEFINE_INLINE_FUNCTION(onesdk_string_t) onesdk_asciistr(char const* data) {
    return onesdk_bytestr(data, ONESDK_CCSID_ASCII);
}

/** @brief Creates a @ref onesdk_string_t for a UTF-8 string.
    @param data     Pointer to the UTF-8 string data. Must be `NULL` or point to a null-terminated buffer.

    @return `onesdk_bytestr(data, ONESDK_CCSID_UTF8)`

    @see @ref onesdk_bytestr
*/
ONESDK_DEFINE_INLINE_FUNCTION(onesdk_string_t) onesdk_utf8str(char const* data) {
    return onesdk_bytestr(data, ONESDK_CCSID_UTF8);
}

/** @brief Creates a @ref onesdk_string_t designating a "null string".

    @return `onesdk_str(NULL, 0, ONESDK_CCSID_NULL)`

    @see @ref onesdk_str
*/
ONESDK_DEFINE_INLINE_FUNCTION(onesdk_string_t) onesdk_nullstr(void) {
    return onesdk_str(0, 0, ONESDK_CCSID_NULL);
}

#if (defined(_WIN32) && defined(ONESDK_CCSID_UTF16_NATIVE)) || defined(ONESDK_BUILD_DOC)

/** @brief Creates a @ref onesdk_string_t for a `wchar_t` UTF-16 string using the native endianness.
    @param data     Pointer to the UTF-16 string data. Must be `NULL` or point to a null-terminated buffer.

    @return A @ref onesdk_string_t that points to the string.

    This function is only available on Windows.

    This function computes the length of the string as `byte_length = wcslen(data) * 2` and then builds the return value
    by calling `onesdk_str(data, byte_length, ONESDK_CCSID_UTF16_NATIVE)`.

    @note The string pointed to by @p data must be terminated by a `wchar_t` with value zero (=two bytes with value zero).

    @see @ref onesdk_str
*/
ONESDK_DEFINE_INLINE_FUNCTION(onesdk_string_t) onesdk_wstr(wchar_t const* data) {
    return onesdk_str(data, data ? ((onesdk_size_t)wcslen(data) * 2) : 0, ONESDK_CCSID_UTF16_NATIVE);
}

#endif

/** @} */

/*========================================================================================================================================*/

/** @} */

/*========================================================================================================================================*/

#endif /* ONESDK_STRING_H_INCLUDED */
