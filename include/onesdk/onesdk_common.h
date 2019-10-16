/*
    Copyright 2017-2019 Dynatrace LLC

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

#include "onesdk/onesdk_config.h"
#include "onesdk/onesdk_version.h"

/*========================================================================================================================================*/


/** @brief Stores the stub version number.
    @see @ref onesdk_stub_get_version */
typedef struct onesdk_stub_version {
    onesdk_uint32_t version_major;      /**< @brief ONESDK_STUB_VERSION_MAJOR */
    onesdk_uint32_t version_minor;      /**< @brief ONESDK_STUB_VERSION_MINOR */
    onesdk_uint32_t version_patch;      /**< @brief ONESDK_STUB_VERSION_PATCH */
} onesdk_stub_version_t;

/*========================================================================================================================================*/

/** @name Error Code Constants
    @{
    @anchor error_code_constants
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

/** @name Logging Level Constants
    @brief Allows choosing a log level. Lower levels include higher ones.
    @see @ref onesdk_stub_set_logging_level
    @{
    @anchor logging_level_constants
*/

#define ONESDK_LOGGING_LEVEL_FINEST		0   /**< @brief Most verbose logging level. */
#define ONESDK_LOGGING_LEVEL_FINER		1   /**< @brief Log even finer messages. */
#define ONESDK_LOGGING_LEVEL_FINE		2   /**< @brief Log fine messages. */
#define ONESDK_LOGGING_LEVEL_CONFIG		3   /**< @brief Log config-related messages. */
#define ONESDK_LOGGING_LEVEL_INFO		4   /**< @brief Log info messages. */
#define ONESDK_LOGGING_LEVEL_WARNING	5   /**< @brief Log warning messages. */
#define ONESDK_LOGGING_LEVEL_SEVERE		6   /**< @brief Least verbose regular logging level. */
#define ONESDK_LOGGING_LEVEL_DEBUG		7   /**< @brief Only used for debug messages which are not enabled by default. */
#define ONESDK_LOGGING_LEVEL_NONE       8   /**< @brief Default SDK stub logging level. Setting this log level deactivates all logging. */

/** @} */

/** @brief Integer type for logging levels.
    @see @ref logging_level_constants
*/
typedef onesdk_int32_t onesdk_logging_level_t;

/*========================================================================================================================================*/

/** @name Handle Types
    @{
    @anchor handle_types
*/

/** @brief Type for all SDK handles.

    The "derived" typedefs are solely used for the purpose of documentation.
*/
typedef onesdk_uint64_t onesdk_handle_t;

typedef onesdk_handle_t onesdk_tracer_handle_t;              /**< @brief A handle that refers to a tracer object. */
typedef onesdk_handle_t onesdk_databaseinfo_handle_t;        /**< @brief A handle that refers to a database info object. */
typedef onesdk_handle_t onesdk_webapplicationinfo_handle_t;  /**< @brief A handle that refers to a web application info object. */
typedef onesdk_handle_t onesdk_messagingsysteminfo_handle_t; /**< @brief A handle that refers to a messaging system info object. */

/** @} */

/** @brief Invalid handle value (zero).

    Functions that create objects will return @ref ONESDK_INVALID_HANDLE if the object could not be created. Passing
    @ref ONESDK_INVALID_HANDLE as argument to a required parameter will usually result in a silent error. That means no warning
    will be logged and the function will simply do nothing.
*/
#define ONESDK_INVALID_HANDLE 0

/*========================================================================================================================================*/

/** @name Agent State Constants
    @{
    @anchor agent_state_constants
    @see @ref onesdk_agent_get_current_state
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
#define ONESDK_AGENT_STATE_ERROR	              (-1)

/** @} */


/** @anchor agent_fork_state_constants
    @name Agent Forking State Constants
    @{
    @brief The fork-related state the agent is in, only relevant if you used @ref ONESDK_INIT_FLAG_FORKABLE.
    @see @ref ONESDK_INIT_FLAG_FORKABLE
    @see @ref onesdk_agent_get_fork_state
*/

/** @brief SDK cannot be used, forked processes may use SDK. */
#define ONESDK_AGENT_FORK_STATE_PARENT_INITIALIZED    1

/** @brief Forked processes can use the SDK. Using the SDK in this process is allowed but
           changes the state to @ref ONESDK_AGENT_FORK_STATE_FULLY_INITIALIZED */
#define ONESDK_AGENT_FORK_STATE_PRE_INITIALIZED       2

/** @brief SDK can be used, forked processes may not use the SDK. */
#define ONESDK_AGENT_FORK_STATE_FULLY_INITIALIZED     3

/** @brief SDK can be used, forked processes may not use the SDK, the agent was initialized without @ref ONESDK_INIT_FLAG_FORKABLE. */
#define ONESDK_AGENT_FORK_STATE_NOT_FORKABLE          4


/** @brief Some error occurred while trying to determine the agent fork state. */
#define ONESDK_AGENT_FORK_STATE_ERROR	              (-1)

/** @} */


/*========================================================================================================================================*/

/** @defgroup channels Channel Types and Endpoints
    @brief Describes communication (networking, I/O) channels and endpoints.

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
          channel endpoint string. Having the channel endpoint allows us to better map which hosts/services/processes are talking to each
          other.

    For further information, see the high level SDK documentation at https://github.com/Dynatrace/OneAgent-SDK/#endpoints

    @{
*/

/** @name Channel Type Constants
    @see @ref channels
    @{
    @anchor channel_type_constants
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

/** @ingroup database_requests
    @{

    @name Database Vendor Strings
    @see @ref database_requests, https://github.com/Dynatrace/OneAgent-SDK/#database
    @{
    @anchor database_vendor_strings
*/

#define ONESDK_DATABASE_VENDOR_APACHE_HIVE      "ApacheHive"        /**< @brief Database vendor string for Apache Hive. */
#define ONESDK_DATABASE_VENDOR_CLOUDSCAPE       "Cloudscape"        /**< @brief Database vendor string for Apache Derby
                                                                                (aka. IBM Cloudscape). */
#define ONESDK_DATABASE_VENDOR_HSQLDB           "HSQLDB"            /**< @brief Database vendor string for HyperSQL DB. */
#define ONESDK_DATABASE_VENDOR_PROGRESS         "Progress"          /**< @brief Database vendor string for OpenEdge Database
                                                                                (aka. Progress). */
#define ONESDK_DATABASE_VENDOR_MAXDB            "MaxDB"             /**< @brief Database vendor string for SAP MaxDB. */
#define ONESDK_DATABASE_VENDOR_HANADB           "HanaDB"            /**< @brief Database vendor string for SAP HANA DB. */
#define ONESDK_DATABASE_VENDOR_INGRES           "Ingres"            /**< @brief Database vendor string for Ingres Database. */
#define ONESDK_DATABASE_VENDOR_FIRST_SQL        "FirstSQL"          /**< @brief Database vendor string for FirstSQL. */
#define ONESDK_DATABASE_VENDOR_ENTERPRISE_DB    "EnterpriseDB"      /**< @brief Database vendor string for EnterpriseDB. */
#define ONESDK_DATABASE_VENDOR_CACHE            "Cache"             /**< @brief Database vendor string for InterSystems Cache. */
#define ONESDK_DATABASE_VENDOR_ADABAS           "Adabas"            /**< @brief Database vendor string for ADABAS. */
#define ONESDK_DATABASE_VENDOR_FIREBIRD         "Firebird"          /**< @brief Database vendor string for Firebird Database. */
#define ONESDK_DATABASE_VENDOR_DB2              "DB2"               /**< @brief Database vendor string for IBM Db2. */
#define ONESDK_DATABASE_VENDOR_DERBY_CLIENT     "Derby Client"      /**< @brief Database vendor string for JDBC connections to Apache Derby
                                                                                (aka. IBM Cloudscape). */
#define ONESDK_DATABASE_VENDOR_DERBY_EMBEDDED   "Derby Embedded"    /**< @brief Database vendor string for Derby Embedded. */
#define ONESDK_DATABASE_VENDOR_FILEMAKER        "Filemaker"         /**< @brief Database vendor string for FileMaker Pro. */
#define ONESDK_DATABASE_VENDOR_INFORMIX         "Informix"          /**< @brief Database vendor string for IBM Informix. */
#define ONESDK_DATABASE_VENDOR_INSTANT_DB       "InstantDb"         /**< @brief Database vendor string for InstantDB. */
#define ONESDK_DATABASE_VENDOR_INTERBASE        "Interbase"         /**< @brief Database vendor string for Embarcadero InterBase. */
#define ONESDK_DATABASE_VENDOR_MYSQL            "MySQL"             /**< @brief Database vendor string for MySQL. */
#define ONESDK_DATABASE_VENDOR_MARIADB          "MariaDB"           /**< @brief Database vendor string for MariaDB. */
#define ONESDK_DATABASE_VENDOR_NETEZZA          "Netezza"           /**< @brief Database vendor string for IBM Netezza. */
#define ONESDK_DATABASE_VENDOR_ORACLE           "Oracle"            /**< @brief Database vendor string for Oracle Database. */
#define ONESDK_DATABASE_VENDOR_PERVASIVE        "Pervasive"         /**< @brief Database vendor string for Pervasive PSQL. */
#define ONESDK_DATABASE_VENDOR_POINTBASE        "Pointbase"         /**< @brief Database vendor string for PointBase. */
#define ONESDK_DATABASE_VENDOR_POSTGRESQL       "PostgreSQL"        /**< @brief Database vendor string for PostgreSQL. */
#define ONESDK_DATABASE_VENDOR_SQLSERVER        "SQL Server"        /**< @brief Database vendor string for Microsoft SQL Server. */
#define ONESDK_DATABASE_VENDOR_SQLITE           "sqlite"            /**< @brief Database vendor string for SQLite. */
#define ONESDK_DATABASE_VENDOR_SYBASE           "Sybase"            /**< @brief Database vendor string for SAP ASE
                                                                                (aka. Sybase SQL Server, Sybase DB, Sybase ASE). */
#define ONESDK_DATABASE_VENDOR_TERADATA         "Teradata"          /**< @brief Database vendor string for Teradata Database. */
#define ONESDK_DATABASE_VENDOR_VERTICA          "Vertica"           /**< @brief Database vendor string for Vertica. */
#define ONESDK_DATABASE_VENDOR_CASSANDRA        "Cassandra"         /**< @brief Database vendor string for Cassandra. */
#define ONESDK_DATABASE_VENDOR_H2               "H2"                /**< @brief Database vendor string for H2 Database Engine. */
#define ONESDK_DATABASE_VENDOR_COLDFUSION_IMQ   "ColdFusion IMQ"    /**< @brief Database vendor string for ColdFusion In-Memory Query
                                                                                (aka. Query of Queries). */
#define ONESDK_DATABASE_VENDOR_REDSHIFT         "Amazon Redshift"   /**< @brief Database vendor string for Amazon Redshift. */
#define ONESDK_DATABASE_VENDOR_COUCHBASE        "Couchbase"         /**< @brief Database vendor string for Couchbase. */

/** @} */
/** @} */

/*========================================================================================================================================*/

/** @ingroup outgoing_web_requests
    @brief HTTP header name for the Dynatrace string tag.

    Sending an `"X-dynaTrace"` request header with a Dynatrace string tag along with an HTTP request enables any Dynatrace OneAgent on the
    server side to continue tracing (connect the server side trace to the traced operation from which the tag was obtained).
*/
#define ONESDK_DYNATRACE_HTTP_HEADER_NAME       "X-dynaTrace"

/*========================================================================================================================================*/

/** @ingroup init
    @{

    @name Initialization flag constants
    @anchor init_flags
    @see @ref onesdk_initialize_2
    @{

    @brief Flags to use with @ref onesdk_initialize_2.
*/

/** @hideinitializer
    @brief Do not fully initialize the SDK now but instead allow it to be used in forked child processes.

    When setting this flag, @ref onesdk_initialize_2 will only partially initialize the SDK agent. In this special _parent-initialized_
    initialization state, only the following functions can be called:

    - All functions that are valid to call before calling initialize remain valid.
    - @ref onesdk_agent_get_version_string and @ref onesdk_stub_get_agent_load_info work as expected.
    - @ref onesdk_agent_get_current_state will return @ref ONESDK_AGENT_STATE_TEMPORARILY_INACTIVE - but see the note below.
    - @ref onesdk_agent_set_warning_callback and @ref onesdk_agent_set_verbose_callback work as expected,
      the callback will be carried over to forked child processes.
    - It is recommended you call @ref onesdk_shutdown when the original process will not fork any more children that want to use the
      SDK.

    After you fork, the child becomes _pre-initialized_: the first call to an SDK function that needs a _fully initialized_ agent will
    automatically complete the initialization.

    You can still fork another child (e.g. in a double-fork scenario) in the _pre-initialized_ state. However if you fork another child
    in the _fully initialized_ state, it will not be able to use the SDK - not even if it tries to shut down the SDK and initialize it
    again.

    @note Calling @ref onesdk_agent_get_current_state in the _pre-initialized_ state will cause the agent to become _fully initialized_.

    @see @ref onesdk_agent_get_fork_state can be used to query which of these states the agent is in.

    All children forked from a _parent-initialized_ process will use the same agent. That agent will shut down when all child processes and
    the original _parent-initialized_ process have terminated or called shutdown. Calling @ref onesdk_shutdown in a _pre-initialized_
    process is not required otherwise.

    @note If you use POSIX `wait` or similar functions, you might observe an additional child process in the original _parent-initialized_
          process.

    @since This flag requires at least agent version 1.151.
*/
#define ONESDK_INIT_FLAG_FORKABLE ((onesdk_uint32_t) 1)

/** @} */
/** @} */

/*========================================================================================================================================*/

/** @ingroup messaging
    @{
*/

/**
    @brief Message property name for the Dynatrace byte tag.

    Sending an `"dtdTraceTagInfo"` message property with a Dynatrace byte tag along with an message enables any Dynatrace OneAgent on the
    server side to continue tracing (connect the server side trace to the traced operation from which the tag was obtained). This works
    only if the receiving application is monitored by a Dynatrace agent and the messaging system used for receiving is supported.
*/
#define ONESDK_DYNATRACE_MESSAGE_PROPERTY_NAME "dtdTraceTagInfo"

/** @brief DEPRECATED alias for @ref ONESDK_DYNATRACE_MESSAGE_PROPERTY_NAME.
    @deprecated Use @ref ONESDK_DYNATRACE_MESSAGE_PROPERTY_NAME instead.
*/
#define ONESDK_DYNATRACE_MESSAGE_PROPERTYNAME ONESDK_DYNATRACE_MESSAGE_PROPERTY_NAME

/**
    @name Messaging Destination Type Constants
    @anchor messaging_destination_type
    @see @ref messaging
    @{
*/

#define ONESDK_MESSAGING_DESTINATION_TYPE_QUEUE 1  /**< @brief A message queue: a message sent to this destination will be
                                                               (sucessfully) received by only one consumer. */
#define ONESDK_MESSAGING_DESTINATION_TYPE_TOPIC 2  /**< @brief A message topic: a message sent to this destination will be
                                                               received by all subscribed consumers. */

/** @} */

/**
    @name Messaging System Vendor Strings
    @anchor messaging_vendor_strings
    @see @ref messaging
    @{
*/

#define ONESDK_MESSAGING_VENDOR_HORNETQ      "HornetQ"      /**< @brief Messaging system vendor string for HornetQ. */
#define ONESDK_MESSAGING_VENDOR_ACTIVE_MQ    "ActiveMQ"     /**< @brief Messaging system vendor string for ActiveMQ. */
#define ONESDK_MESSAGING_VENDOR_RABBIT_MQ    "RabbitMQ"     /**< @brief Messaging system vendor string for RabbitMQ. */
#define ONESDK_MESSAGING_VENDOR_ARTEMIS      "Artemis"      /**< @brief Messaging system vendor string for Artemis. */
#define ONESDK_MESSAGING_VENDOR_WEBSPHERE    "WebSphere"    /**< @brief Messaging system vendor string for WebSphere. */
#define ONESDK_MESSAGING_VENDOR_MQSERIES_JMS "MQSeries JMS" /**< @brief Messaging system vendor string for MQSeries JMS. */
#define ONESDK_MESSAGING_VENDOR_MQSERIES     "MQSeries"     /**< @brief Messaging system vendor string for MQSeries. */
#define ONESDK_MESSAGING_VENDOR_TIBCO        "Tibco"        /**< @brief Messaging system vendor string for Tibco. */

/** @} */
/** @} */

/** @ingroup ex_metrics */
/** @{ */
typedef onesdk_handle_t onesdk_metric_handle_t;             /**< @brief A handle that refers to a metric object. */
/** @} */

/*========================================================================================================================================*/

#if defined(_MSC_VER)
#include "onesdk/onesdk_msvc.h"
#endif

/*========================================================================================================================================*/

#endif /* ONESDK_COMMON_H_INCLUDED */
