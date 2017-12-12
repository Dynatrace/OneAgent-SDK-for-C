/*
    Copyright 2017 Dynatrace LLC

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

#ifndef ONESDK_COMMON_H_INCLUDED
#define ONESDK_COMMON_H_INCLUDED

/** @file
    @brief Defines common types/constants/macros. Applications should not include this file directly.
*/

/*========================================================================================================================================*/

#include "onesdk_config.h"

/*========================================================================================================================================*/

/** @anchor error_code_constants
    @name Error Code Constants
    @{
*/

/** @hideinitializer @brief The operation failed, no further detail is available. */
#define ONESDK_ERROR_GENERIC                        ((onesdk_result_t)(ONESDK_ERROR_BASE + 1))
/** @hideinitializer @brief A function was called with an invalid argument. */
#define ONESDK_ERROR_INVALID_ARGUMENT               ((onesdk_result_t)(ONESDK_ERROR_BASE + 2))
/** @hideinitializer @brief The called function is not implemented. */
#define ONESDK_ERROR_NOT_IMPLEMENTED                ((onesdk_result_t)(ONESDK_ERROR_BASE + 3))
/** @hideinitializer @brief The SDK has not been initialized. */
#define ONESDK_ERROR_NOT_INITIALIZED                ((onesdk_result_t)(ONESDK_ERROR_BASE + 4))
/** @hideinitializer @brief Out of memory. */
#define ONESDK_ERROR_OUT_OF_MEMORY                  ((onesdk_result_t)(ONESDK_ERROR_BASE + 5))
/** @hideinitializer @brief The SDK stub was configured to _not_ try to load the actual agent module. */
#define ONESDK_ERROR_AGENT_NOT_ACTIVE               ((onesdk_result_t)(ONESDK_ERROR_BASE + 6))
/** @hideinitializer @brief The loader module was unable to load the actual SDK agent module. */
#define ONESDK_ERROR_LOAD_AGENT                     ((onesdk_result_t)(ONESDK_ERROR_BASE + 7))
/** @hideinitializer @brief The loader module found an SDK agent binary that didn't have the expected exports. */
#define ONESDK_ERROR_INVALID_AGENT_BINARY           ((onesdk_result_t)(ONESDK_ERROR_BASE + 8))
/** @hideinitializer @brief The operation failed because of an unexpected error. */
#define ONESDK_ERROR_UNEXPECTED                     ((onesdk_result_t)(ONESDK_ERROR_BASE + 9))
/** @hideinitializer @brief The command line argument/stub variable definition was ignored because an entry with the same key was already
    present. */
#define ONESDK_ERROR_ENTRY_ALREADY_EXISTS           ((onesdk_result_t)(ONESDK_ERROR_BASE + 10))
/** @hideinitializer @brief The SDK agent module doesn't support the feature level required by this version of the SDK stub. */
#define ONESDK_ERROR_FEATURE_LEVEL_NOT_SUPPORTED    ((onesdk_result_t)(ONESDK_ERROR_BASE + 11))
/** @hideinitializer @brief The SDK agent module doesn't support the SDK interface required by this version of the SDK stub. */
#define ONESDK_ERROR_INTERFACE_NOT_SUPPORTED        ((onesdk_result_t)(ONESDK_ERROR_BASE + 12))
/** @hideinitializer @brief The operation failed because this is the child process of a fork that occurred while the SDK was initialized. */
#define ONESDK_ERROR_FORK_CHILD                     ((onesdk_result_t)(ONESDK_ERROR_BASE + 13))
/* NOTE: Extend get_onesdk_specific_error_message() when adding error codes. */

/** @hideinitializer @brief The operation completed successfully. */
#define ONESDK_SUCCESS                              ((onesdk_result_t)0)

/** @} */

/*========================================================================================================================================*/

/** @brief Agent logging function prototype.
    @param message      The message.
    
    On Windows, the message is UTF-8 encoded. On other platforms, the message uses the system default encoding.

    Applications should not try to parse or interpret the message or in any way rely on the wording/format of the message.

    @see @ref onesdk_agent_set_logging_callback
*/
typedef void ONESDK_CALL onesdk_agent_logging_callback_t(char const* message);

/*========================================================================================================================================*/

/** @anchor logging_level_constants
    @name Logging Level Constants
    @{
*/

#define ONESDK_LOGGING_LEVEL_FINEST		0   /**< @brief Most verbose logging level. */
#define ONESDK_LOGGING_LEVEL_FINER		1
#define ONESDK_LOGGING_LEVEL_FINE		2
#define ONESDK_LOGGING_LEVEL_CONFIG		3
#define ONESDK_LOGGING_LEVEL_INFO		4   /**< @brief Default SDK stub logging level. */
#define ONESDK_LOGGING_LEVEL_WARNING	5
#define ONESDK_LOGGING_LEVEL_SEVERE		6   /**< @brief Least verbose regular logging level. */
#define ONESDK_LOGGING_LEVEL_DEBUG		7   /**< @brief Only used for debug messages which are not enabled by default. */
#define ONESDK_LOGGING_LEVEL_NONE       8   /**< @brief Setting this log level deactivates all logging. */

/** @} */

/** @brief Integer type for logging levels.
    @see @ref logging_level_constants
*/
typedef onesdk_int32_t onesdk_logging_level_t;

/*========================================================================================================================================*/

/** @anchor handle_types
    @name Handle Types
    @{
*/

/** @brief Type for all SDK handles.

    The "derived" typedefs are solely used for the purpose of documentation.
*/
typedef onesdk_uint64_t onesdk_handle_t;

typedef onesdk_handle_t onesdk_tracer_handle_t;         /**< @brief A handle that refers to a tracer object. */
typedef onesdk_handle_t onesdk_databaseinfo_handle_t;   /**< @brief A handle that refers to a database-info object. */

/** @} */

/** @brief Invalid handle value (zero).

    Functions that create objects will return @ref ONESDK_INVALID_HANDLE if the object could not be created. Passing
    @ref ONESDK_INVALID_HANDLE as argument to a required parameter will usually result in a silent error. That means no warning
    will be logged and the function will simply do nothing.
*/
#define ONESDK_INVALID_HANDLE 0

/*========================================================================================================================================*/

/** @anchor agent_state_constants
    @name Agent State Constants
    @{
*/

/** @brief The SDK stub is connected to the agent, which is currently active.

    In this state...
    - New tracers will be created.
    - Data is captured and reported.
    - Creating "passive" objects is possible (e.g. @ref onesdk_databaseinfo_create).
*/
#define ONESDK_AGENT_STATE_ACTIVE					0

/** @brief The SDK stub is connected to the agent, which is temporarily inactive.

    The agent may become active again at any time though. If your application uses the agent state to "skip" SDK calls, it should re-check
    the status periodically.

    In this state...
    - No new tracers will be created.
    - No data is captured/reported.
    - Creating "passive" objects is possible though (e.g. @ref onesdk_databaseinfo_create).
*/
#define ONESDK_AGENT_STATE_TEMPORARILY_INACTIVE		1

/** @brief The SDK stub is connected to the agent, which is permanently inactive.

    The agent will not become active again. This can happen e.g. if the application process has been forked while the SDK was initialized
    and the calling process is the fork child.

    The effects are the same as with @ref ONESDK_AGENT_STATE_NOT_INITIALIZED.
*/
#define ONESDK_AGENT_STATE_PERMANENTLY_INACTIVE		2

/** @brief The agent has not been initialized.

    E.g. because the application didn't call @ref onesdk_initialize, @ref onesdk_initialize failed or the agent has already
    been shut down (@ref onesdk_shutdown).

    In this state...
    - No new tracers will be created.
    - No data is captured/reported.
    - Creating "passive" objects will also fail (e.g. @ref onesdk_databaseinfo_create).
*/
#define ONESDK_AGENT_STATE_NOT_INITIALIZED			3

/** @brief Some unexpected error occurred while trying to determine the agent state. */
#define ONESDK_AGENT_STATE_ERROR					-1

/** @} */

/*========================================================================================================================================*/

/** @addtogroup channels Channel Types and Endpoints

    When we talk about channel types and channel endpoints in the SDK, we're talking about a "communication channel" which is used
    to access/communicate with some service or resource. Mapped to the OSI model or Internet protocol suite, our kind of channels would be
    located in the transport layer. So, unsurprisingly, there is a channel type @ref ONESDK_CHANNEL_TYPE_TCP_IP. Communication doesn't
    always happen over the network though, so for e.g. in-process communication there's also @ref ONESDK_CHANNEL_TYPE_IN_PROCESS.

    So if, for example, your application is sending SOAP requests via HTTP via TCP/IP, then the channel we're talking about would be the
    TCP/IP connection.

    The channel endpoint then is the endpoint of that connection. E.g. for a TCP/IP connection that endpoint is usually expressed by an
    `IP:PORT` or `HOSTNAME:PORT` pair, so that's what your application should use if the channel type is @ref ONESDK_CHANNEL_TYPE_TCP_IP.

    Unless specified otherwise, the channel type is a mandatory parameter and the channel endpoint is optional. If there is no suitable
    constant for the type of communication channel your application uses, it should use @ref ONESDK_CHANNEL_TYPE_OTHER.

    @note Although the channel endpoint is optional, it's highly advised to include it for all channel types that specify a format for the
    channel endpoint string. Having the channel endpoint allows us to better map which hosts/services/processes are talking to each other.

    @{
*/

/** @anchor channel_type_constants
    @name Channel Type Constants
    @see @ref channels
    @{
*/

/** @brief Some other channel type or unknown channel type.

    Any string can be used as channel endpoint for this type.
*/
#define ONESDK_CHANNEL_TYPE_OTHER				0

/** @brief The channel is a TCP/IP connection.

    The channel endpoint string should be the host name, followed by a colon, followed by the port number (in decimal).
    E.g. `localhost:1234` or `example.com:80`.
*/
#define ONESDK_CHANNEL_TYPE_TCP_IP				1

/** @brief The channel is a connection via Unix domain sockets.

    The channel endpoint string should be the path of the Unix domain sockets.
*/
#define ONESDK_CHANNEL_TYPE_UNIX_DOMAIN_SOCKET	2

/** @brief The channel is a named pipe.

    The channel endpoint string should be the pipe name.
*/
#define ONESDK_CHANNEL_TYPE_NAMED_PIPE			3

/** @brief The channel is some in-process means of communication.

    Any string can be used as channel endpoint for this type. An application should use this channel type e.g. for database requests
    serviced by an in-process database like SQLite.
*/
#define ONESDK_CHANNEL_TYPE_IN_PROCESS			4

/** @} */
/** @} */

/*========================================================================================================================================*/

#if defined(_MSC_VER)
#include "onesdk_msvc.h"
#endif

/*========================================================================================================================================*/

#endif /* ONESDK_COMMON_H_INCLUDED */
