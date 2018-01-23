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

#ifndef ONESDK_AGENT_H_INCLUDED
#define ONESDK_AGENT_H_INCLUDED

/** @file
    @brief Defines agent functions.
*/

/*========================================================================================================================================*/

#include "onesdk_common.h"
#include "onesdk_string.h"

/*========================================================================================================================================*/

/** @addtogroup misc Miscellaneous
    @{
*/

/*========================================================================================================================================*/

/** @brief Returns the version string of the loaded SDK agent module.

    @return
    A pointer to a null-terminated @ref onesdk_xchar_t string constant that indicates the version of the loaded SDK agent module.
    If the agent has not been initialized yet this function will return an empty string.

    Your application should not try to parse the version string or make any assumptions about it's format.
*/
ONESDK_DECLARE_FUNCTION(onesdk_xchar_t const*) onesdk_agent_get_version_string(void);

/*========================================================================================================================================*/

/** @brief Returns the current agent state.

    @return The current agent state.

    @see @ref agent_state_constants
*/
ONESDK_DECLARE_FUNCTION(onesdk_int32_t) onesdk_agent_get_current_state(void);

/** @brief Sets the agent logging callback function.
    @param agent_logging_callback   The new agent logging callback function.

    The agent logging callback is called whenever one of the following happens while executing an SDK function:
    - An SDK usage error is detected or
    - An unexpected or unusual event (e.g. out of memory) prevented an operation from completing successfully.

    The agent logging callback is only ever called while executing an SDK function, in the context of the application thread that is
    calling the SDK function. The application must not call any SDK functions while executing the callback.

    This mechanism is provided primarily as a development and debugging aid.

    There is no default logging function.

    @see @ref onesdk_agent_logging_callback_t
*/
ONESDK_DECLARE_FUNCTION(void) onesdk_agent_set_logging_callback(onesdk_agent_logging_callback_t* agent_logging_callback);

/*========================================================================================================================================*/

/** @} */

/*========================================================================================================================================*/

/** @addtogroup tracers Common Tracer Functions

    Tracers are the objects used to capture information about "operations" (e.g. remote calls or database requests) that your application
    performs.

    Tracers are created by different factory functions, depending on the type of the tracer, and destroyed/released by
    @ref onesdk_tracer_end.

    Tracers are automatically linked to other active tracers on the same thread. To link to an operation that's running in another thread,
    processes or on another system you have to use the tagging functions.

    @attention
    All tracers in this SDK have strict thread affinity - they are bound to the thread by which they were created. Calling _any_ tracer
    function with a tracer handle that was created by a different thread is an error and will not produce any effect. Note that this
    includes @ref onesdk_tracer_end. Thus trying to destroy/release a tracer from a different thread will result in a memory leak.

    For further information, see the high level SDK documentation available at https://github.com/Dynatrace/OneAgent-SDK/

    @{
*/

/*========================================================================================================================================*/

/** @brief Starts a tracer.
    @param tracer_handle    A valid tracer handle.

    Starts the specified tracer. This will capture all entry fields of the tracer and start the time measurement.
*/
ONESDK_DECLARE_FUNCTION(void) onesdk_tracer_start(onesdk_tracer_handle_t tracer_handle);

/** @brief Ends and releases a tracer.
    @param tracer_handle    A valid tracer handle.

    This function releases the specified tracer. Allocated resources are freed and the handle is invalidated.

    An application should call @ref onesdk_tracer_start exactly once for each tracer that it has created.

    If the tracer has been started, the time measurement will be stopped and it's exit fields will be captured before releasing the tracer.

    @note Just like all other tracer functions, this function must only be called from the thread that was used to create the tracer.
    This is important because it means that a tracer cannot be released on a different thread. Attempting to do so is an error and will
    result in memory leaks.
*/
ONESDK_DECLARE_FUNCTION(void) onesdk_tracer_end(onesdk_tracer_handle_t tracer_handle);

/** @internal */
ONESDK_DECLARE_FUNCTION(void) onesdk_tracer_error_p(onesdk_tracer_handle_t tracer_handle, onesdk_string_t const* error_class, onesdk_string_t const* error_message);

/** @brief Sets error information for a tracer.
    @param tracer_handle    A valid tracer handle.
    @param error_class      [optional] Pointer to a string denoting the error class/type/category.
    @param error_message    [optional] Pointer to a string with the error message.

    An application should call this function to notify a tracer that the traced operations has failed.

    @note Calling @ref onesdk_tracer_error does not end/release the tracer.

    @note @ref onesdk_tracer_error must only be called once. If a traced operation results in multiple errors and the application wants
    all of them to be captured, it must concatenate/combine them and then call @ref onesdk_tracer_error once before calling
    @ref onesdk_tracer_end.
*/
ONESDK_DEFINE_INLINE_FUNCTION(void) onesdk_tracer_error(onesdk_tracer_handle_t tracer_handle, onesdk_string_t error_class, onesdk_string_t error_message) {
    onesdk_tracer_error_p(tracer_handle, &error_class, &error_message);
}

/*========================================================================================================================================*/

/** @brief Retrieves the string representation of the tag from an "outgoing taggable" tracer.
    @param tracer_handle                A valid tracer handle.
    @param[out] buffer                  [optional] Pointer to a buffer into which the string representation shall be copied.
    @param buffer_size                  Size of the buffer pointed to by @p buffer in bytes. Must be zero if @p buffer is `NULL`.
    @param[out] required_buffer_size    [optional] Pointer to a @ref onesdk_size_t variable where the required buffer size will be stored.

    @return The number of characters copied into @p buffer, not including the terminating null character.

    An application can call this function to retrieve the string representation of the outgoing tag for a tracer. The tracer handle
    must refer to an "outgoing taggable" tracer that has already been started.

    If @p required_buffer_size is not `NULL`, the number of bytes required to store the string representation is stored in
    @p *required_buffer_size.

    If @p buffer is not `NULL` and @p buffer_size is big enough, this function will copy the string representation into the provided buffer.

    This function will always make sure that, if the application provides a non-`NULL` @p buffer with a @p buffer_size >= 1, @p buffer will
    always be null-terminated. I.e. if the string representation cannot be stored because @p buffer_size is too small, a null character will
    be written into @p buffer[0].

    The string copied into @p buffer uses ASCII encoding.

    @note
    - If called with invalid arguments, the retrieved string will be an empty string.
    - Calling this function multiple times for the same tracer is explicitly supported and will yield the same result.
    - Retrieving both the string representation and the binary representation from the same tracer is explicitly supported.

    @see @ref onesdk_tracer_get_outgoing_dynatrace_byte_tag
*/
ONESDK_DECLARE_FUNCTION(onesdk_size_t) onesdk_tracer_get_outgoing_dynatrace_string_tag(onesdk_tracer_handle_t tracer_handle, char* buffer, onesdk_size_t buffer_size, onesdk_size_t* required_buffer_size);

/** @brief Retrieves the binary representation of the tag from an "outgoing taggable" tracer.
    @param tracer_handle                A valid tracer handle.
    @param[out] buffer                  [optional] Pointer to a buffer into which the binary representation shall be copied.
    @param buffer_size                  Size of the buffer pointed to by @p buffer in bytes. Must be zero if @p buffer is `NULL`.
    @param[out] required_buffer_size    [optional] Pointer to a @ref onesdk_size_t variable where the required buffer size will be stored.

    @return The number of characters copied into @p buffer.

    An application can call this function to retrieve the binary representation of the outgoing tag for a tracer. The tracer handle
    must refer to an "outgoing taggable" tracer that has already been started.

    If @p required_buffer_size is not `NULL`, the number of bytes required to store the binary representation is stored in
    @p *required_buffer_size.

    If @p buffer is not `NULL` and @p buffer_size is big enough, this function will copy the binary representation into the provided buffer.

    @note
    - If called with invalid arguments this function will do nothing and return zero.
    - Calling this function multiple times for the same tracer is explicitly supported and will yield the same result.
    - Retrieving both the string representation and the binary representation from the same tracer is explicitly supported.

    @see @ref onesdk_tracer_get_outgoing_dynatrace_string_tag
*/
ONESDK_DECLARE_FUNCTION(onesdk_size_t) onesdk_tracer_get_outgoing_dynatrace_byte_tag(onesdk_tracer_handle_t tracer_handle, unsigned char* buffer, onesdk_size_t buffer_size, onesdk_size_t* required_buffer_size);

/*========================================================================================================================================*/

/** @internal */
ONESDK_DECLARE_FUNCTION(void) onesdk_tracer_set_incoming_dynatrace_string_tag_p(onesdk_tracer_handle_t tracer_handle, onesdk_string_t const* string_tag);

/** @brief Sets the tag of an "incoming taggable" tracer using the string representation.
    @param tracer_handle    A valid tracer handle.
    @param string_tag       The string representation of the incoming tag.

    An application can call this function to set the incoming tag of an "incoming taggable" tracer using the string representation.
    The tracer handle must refer to an "incoming taggable" tracer that has not been started yet.

    If @p string_tag is a "null string" or an empty string, the incoming tag will be reset (cleared).

    @note An "incoming taggable" tracer has _one_ tag. Calling @ref onesdk_tracer_set_incoming_dynatrace_string_tag will overwrite any tag
    that was set by either @ref onesdk_tracer_set_incoming_dynatrace_string_tag or @ref onesdk_tracer_set_incoming_dynatrace_byte_tag.

    @see ref onesdk_tracer_set_incoming_dynatrace_string_tag
    @see ref onesdk_tracer_set_incoming_dynatrace_byte_tag
*/
ONESDK_DEFINE_INLINE_FUNCTION(void) onesdk_tracer_set_incoming_dynatrace_string_tag(onesdk_tracer_handle_t tracer_handle, onesdk_string_t string_tag) {
    onesdk_tracer_set_incoming_dynatrace_string_tag_p(tracer_handle, &string_tag);
}

/** @brief Sets the tag of an "incoming taggable" tracer using the binary representation.
    @param tracer_handle    A valid tracer handle.
    @param byte_tag         Pointer to an array that holds the binary representation of the incoming tag.
    @param byte_tag_size    Size of the binary representation of the incoming tag.

    An application can call this function to set the incoming tag of an "incoming taggable" tracer using the binary representation.
    The tracer handle must refer to an "incoming taggable" tracer that has not been started yet.

    If @p byte_tag_size is zero, the incoming tag will be reset (cleared).

    @note An "incoming taggable" tracer has _one_ tag. Calling @ref onesdk_tracer_set_incoming_dynatrace_byte_tag will overwrite any tag
    that was set by either @ref onesdk_tracer_set_incoming_dynatrace_string_tag or @ref onesdk_tracer_set_incoming_dynatrace_byte_tag.

    @see ref onesdk_tracer_set_incoming_dynatrace_string_tag
*/
ONESDK_DECLARE_FUNCTION(void) onesdk_tracer_set_incoming_dynatrace_byte_tag(onesdk_tracer_handle_t tracer_handle, unsigned char const* byte_tag, onesdk_size_t byte_tag_size);

/*========================================================================================================================================*/

/** @} */

/*========================================================================================================================================*/

/** @addtogroup remote_calls Remote Call Tracers

    When tracing remote calls, we use the following parameters
    - @p service_method
    - @p service_name
    - @p service_endpoint

    And for outgoing remote calls also
    - @p channel_type
    - @p channel_endpoint

    @p service_method should be the name of the service method/operation, @p service_name the name of the service class/type and
    @p service_endpoint a string identifying the "instance" of that service class/type.

    The @p channel_ parameters are described in @ref channels.

    Further there is the optional `protocol_name` property that you can set to specify the used "wire protocol".

    For further information, see the high level SDK documentation available at https://github.com/Dynatrace/OneAgent-SDK/

    @par Example 1:
    - You're tracing an outgoing remote call to some web service `org.example.services.Repository`.
    - There are multiple repositories (multiple instances of `org.example.services.Repository`), but the one that you're using (calling) is
      located at `http://art-department.example.org:12345/services/assets/repo`.
    - You're storing a new file using the operation `storeFile`.
    - You're using HTTP over TCP/IP on port 12345 to access the service.
    @par
    In that case you'd use
    - @p service_method = `storeFile`
    - @p service_name = `org.example.services.Repository`
    - @p service_endpoint = `http://art-department.example.org:12345/services/assets/repo`
    - @p channel_type = @ref ONESDK_CHANNEL_TYPE_TCP_IP
    - @p channel_endpoint = `art-department.example.org:12345`
    
    @par Example 2:
    - You're tracing a call to a custom remote service called `MasterControlProgram`.
    - There is only one instance of that service, running distributed on all servers of your company.
    - You're sending a "derez file" request.
    - The `MasterControlProgram` is accessed by a some custom laser communication system.
    @par
    Since there is only one instance of the service, which doesn't really have a name/location/endpoint assigned to it, you don't really
    have anything meaningful to use as @p service_name argument. You have to specify @em something though, since it's an required argument
    (and empty string doesn't count). One easy convention for such cases would be to simply always use the string `instance`.
    Another one would be to use the service name. Let's go with the first.
    @par
    In that case you'd use
    - @p service_method = `derez file`
    - @p service_name = `MasterControlProgram`
    - @p service_endpoint = `instance`
    - @p channel_type = @ref ONESDK_CHANNEL_TYPE_OTHER
    - @p channel_endpoint = @ref onesdk_nullstr()

    @{
*/

/*========================================================================================================================================*/

/** @internal */
ONESDK_DECLARE_FUNCTION(onesdk_tracer_handle_t) onesdk_outgoingremotecalltracer_create_p(onesdk_string_t const* service_method, onesdk_string_t const* service_name, onesdk_string_t const* service_endpoint, onesdk_int32_t channel_type, onesdk_string_t const* channel_endpoint);

/** @brief Creates a tracer for tracing outgoing remote calls.
    @param service_method       The name of the service method.
    @param service_name         The name of the service.
    @param service_endpoint     The service endpoint.
    @param channel_type         The type of the channel used to communicate with the service.
    @param channel_endpoint     [optional] The endpoint of the channel used to communicate with the service.

    @return A handle for the newly created outgoing remote call tracer or @ref ONESDK_INVALID_HANDLE.

    For more information see @ref remote_calls and @ref channels.
*/
ONESDK_DEFINE_INLINE_FUNCTION(onesdk_tracer_handle_t) onesdk_outgoingremotecalltracer_create(onesdk_string_t service_method, onesdk_string_t service_name, onesdk_string_t service_endpoint, onesdk_int32_t channel_type, onesdk_string_t channel_endpoint) {
    return onesdk_outgoingremotecalltracer_create_p(&service_method, &service_name, &service_endpoint, channel_type, &channel_endpoint);
}

/** @internal */
ONESDK_DECLARE_FUNCTION(void) onesdk_outgoingremotecalltracer_set_protocol_name_p(onesdk_tracer_handle_t tracer_handle, onesdk_string_t const* protocol_name);

/** @brief Sets the name of the remoting protocol.
    @param tracer_handle        A valid outgoing remote call tracer handle.
    @param protocol_name        [optional] The name of the remoting protocol used to access the service.

    For more information see @ref remote_calls.

    @note Calling this function with @p protocol_name = @ref onesdk_nullstr() will reset/clear any value that was set previously.
    @note This function can not be used after the tracer was started.
*/
ONESDK_DEFINE_INLINE_FUNCTION(void) onesdk_outgoingremotecalltracer_set_protocol_name(onesdk_tracer_handle_t tracer_handle, onesdk_string_t protocol_name) {
    onesdk_outgoingremotecalltracer_set_protocol_name_p(tracer_handle, &protocol_name);
}

/*========================================================================================================================================*/

/** @internal */
ONESDK_DECLARE_FUNCTION(onesdk_tracer_handle_t) onesdk_incomingremotecalltracer_create_p(onesdk_string_t const* service_method, onesdk_string_t const* service_name, onesdk_string_t const* service_endpoint);

/** @brief Creates a tracer for tracing incoming remote calls.
    @param service_method       The name of the service method.
    @param service_name         The name of the service.
    @param service_endpoint     The service endpoint.

    @return A handle for the newly created incoming remote call tracer or @ref ONESDK_INVALID_HANDLE.

    For more information see @ref remote_calls.
*/
ONESDK_DEFINE_INLINE_FUNCTION(onesdk_tracer_handle_t) onesdk_incomingremotecalltracer_create(onesdk_string_t service_method, onesdk_string_t service_name, onesdk_string_t service_endpoint) {
    return onesdk_incomingremotecalltracer_create_p(&service_method, &service_name, &service_endpoint);
}

/** @internal */
ONESDK_DECLARE_FUNCTION(void) onesdk_incomingremotecalltracer_set_protocol_name_p(onesdk_tracer_handle_t tracer_handle, onesdk_string_t const* protocol_name);

/** @brief Sets the name of the remoting protocol.
    @param tracer_handle        A valid incoming remote call tracer handle.
    @param protocol_name        [optional] The name of the remoting protocol used to access the service.

    For more information see @ref remote_calls.

    @note Calling this function with @p protocol_name = @ref onesdk_nullstr() will reset/clear any value that was set previously.
    @note This function can not be used after the tracer was started.
*/
ONESDK_DEFINE_INLINE_FUNCTION(void) onesdk_incomingremotecalltracer_set_protocol_name(onesdk_tracer_handle_t tracer_handle, onesdk_string_t protocol_name) {
    onesdk_incomingremotecalltracer_set_protocol_name_p(tracer_handle, &protocol_name);
}

/*========================================================================================================================================*/

/** @} */

/*========================================================================================================================================*/

/** @addtogroup database_requests Database Request Tracers

    Database request tracers are used to capture information about database requests. They're not taggable.

    To create a database request tracer an application must first create a database info object which describes the database that the
    application uses - see @ref onesdk_databaseinfo_create.

    @{
*/

/** @internal */
ONESDK_DECLARE_FUNCTION(onesdk_databaseinfo_handle_t) onesdk_databaseinfo_create_p(onesdk_string_t const* database_name, onesdk_string_t const* database_vendor, onesdk_int32_t channel_type, onesdk_string_t const* channel_endpoint);

/** @brief Creates a database info object.
    @param database_name        The name of the database.
    @param database_vendor      The type of the database (e.g. "sqlite", "MySQL", "Oracle", "DB2" - see @ref database_vendor_strings).
    @param channel_type         The type of the channel used to communicate with the database.
    @param channel_endpoint     [optional] The endpoint of the channel used to communicate with the database.

    @return A handle for the newly created database info object or @ref ONESDK_INVALID_HANDLE.

    This function creates a database info object which describes a database instance and is required for tracing database requests (see
    @ref onesdk_databaserequesttracer_create_sql).

    For information about @p channel_type and @p channel_endpoint see @ref channels.
*/
ONESDK_DEFINE_INLINE_FUNCTION(onesdk_databaseinfo_handle_t) onesdk_databaseinfo_create(onesdk_string_t database_name, onesdk_string_t database_vendor, onesdk_int32_t channel_type, onesdk_string_t channel_endpoint) {
    return onesdk_databaseinfo_create_p(&database_name, &database_vendor, channel_type, &channel_endpoint);
}

/** @brief Releases a database info object.
    @param databaseinfo_handle      A valid database info handle.

    This function releases the specified database info object. Allocated resources are freed and the handle is invalidated.

    An application should call @ref onesdk_databaseinfo_delete exactly once for each database info object that it has created.

    @note Calling @ref onesdk_databaseinfo_delete with a handle to a database info object which is still referenced by existing tracers is
    supported. In that case the lifetime of the database info object is extended as necessary. The handle will always become invalid
    immediately though.
*/
ONESDK_DECLARE_FUNCTION(void) onesdk_databaseinfo_delete(onesdk_databaseinfo_handle_t databaseinfo_handle);

/** @internal */
ONESDK_DECLARE_FUNCTION(onesdk_tracer_handle_t) onesdk_databaserequesttracer_create_sql_p(onesdk_databaseinfo_handle_t databaseinfo_handle, onesdk_string_t const* database_statement);

/** @brief Creates a tracer for tracing SQL database requests.
    @param databaseinfo_handle      A valid database info handle.
    @param database_statement       The database statement (SQL).

    @return A handle for the newly created database request tracer or @ref ONESDK_INVALID_HANDLE.

    @see @ref onesdk_databaseinfo_create
*/
ONESDK_DEFINE_INLINE_FUNCTION(onesdk_tracer_handle_t) onesdk_databaserequesttracer_create_sql(onesdk_databaseinfo_handle_t databaseinfo_handle, onesdk_string_t database_statement) {
    return onesdk_databaserequesttracer_create_sql_p(databaseinfo_handle, &database_statement);
}

/*========================================================================================================================================*/

/** @} */

/*========================================================================================================================================*/

#endif /* ONESDK_AGENT_H_INCLUDED */
