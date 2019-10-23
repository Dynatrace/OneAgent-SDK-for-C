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

#ifndef ONESDK_AGENT_H_INCLUDED
#define ONESDK_AGENT_H_INCLUDED

/** @file
    @brief Defines agent functions.
*/

/*========================================================================================================================================*/

#include "onesdk/onesdk_common.h" /* IWYU pragma: export */
#include "onesdk/onesdk_string.h"

/*========================================================================================================================================*/

/** @defgroup misc Miscellaneous
    @{
*/

/*========================================================================================================================================*/

/** @brief Returns the version string of the loaded SDK agent module.

    @return
    A pointer to a null-terminated @ref onesdk_xchar_t string constant that indicates the version of the loaded SDK agent module.
    May also contain the version of the module that was last attempted to load if the last SDK initialization failed but the agent
    version could still be determined.
    If the agent has not been initialized yet or its version could not be determined, this function will return an empty string.

    Your application should not try to parse the version string or make any assumptions about its format.

    @see @ref onesdk_stub_get_agent_load_info can be used to query if a compatible agent was found.
*/
ONESDK_DECLARE_FUNCTION(onesdk_xchar_t const*) onesdk_agent_get_version_string(void);

/*========================================================================================================================================*/

/** @brief Returns the current agent state.

    @return The current agent state.

    @see @ref agent_state_constants "Agent state constants"
*/
ONESDK_DECLARE_FUNCTION(onesdk_int32_t) onesdk_agent_get_current_state(void);

/** @brief DEPRECATED. Sets the agent warning callback function.
    @param agent_logging_callback   The new agent warning callback function.

    @deprecated From 1.5 on, use @ref onesdk_agent_set_warning_callback instead, which has a return code but is otherwise the same
        (@c onesdk_agent_set_logging_callback, like @ref onesdk_agent_set_warning_callback, can fail but it has no way to report errors).
*/
ONESDK_DECLARE_FUNCTION(void) onesdk_agent_set_logging_callback(onesdk_agent_logging_callback_t* agent_logging_callback);

/** @brief Sets the agent warning callback function.
    @param agent_logging_callback   The new agent warning callback function.
    @return @ref ONESDK_SUCCESS if successful, an SDK stub error code otherwise.

    The agent warning callback is called whenever one of the following happens while executing an SDK function:
    - An SDK usage error is detected or
    - An unexpected or unusual event (e.g. out of memory) prevented an operation from completing successfully.

    The agent warning callback is only ever called from SDK functions (i.e., it can only be invoked on threads
    that call SDK functions, while the SDK function is executing). The application must not call any SDK functions
    from the callback.

    This mechanism is provided primarily as a development and debugging aid. Typically, you should set it once, right after initializing
    the SDK and never change it (though changing the callback is supported, it rarely makes sense).

    There is no default logging function.

    @note This function requires that the SDK is initialized (using @ref onesdk_initialize or @ref onesdk_initialize_2).
    @see @ref onesdk_agent_logging_callback_t
    @see @ref onesdk_stub_set_logging_callback sets the callback that is used for logging before and while the agent is loaded.
    @since This function was added in version 1.5.0, as a replacement for @ref onesdk_agent_set_logging_callback.
*/
ONESDK_DECLARE_FUNCTION(onesdk_result_t) onesdk_agent_set_warning_callback(onesdk_agent_logging_callback_t* agent_logging_callback);

/** @brief Sets the verbose agent logging callback function.
    @param agent_logging_callback   The new verbose agent logging callback function.
    @return @ref ONESDK_SUCCESS if successful, an SDK stub error code otherwise.

    Similar to @ref onesdk_agent_set_warning_callback but the callback supplied here will not be called with warning messages
    but with additional messages that may e.g. explain why a PurePath was not created even if the reason is (usually) benign.

    @note
     - It usually does not make sense to set this callback without also using @ref onesdk_agent_set_warning_callback in addition.
     - This function requires that the SDK is initialized (using @ref onesdk_initialize or @ref onesdk_initialize_2).
    @warning This callback can receive lots and lots of messages. You should not usually use it in production.
    @since This function was added in version 1.5.0.
*/
ONESDK_DECLARE_FUNCTION(onesdk_result_t) onesdk_agent_set_verbose_callback(onesdk_agent_logging_callback_t* agent_logging_callback);



/** @brief Returns the current agent fork state. Only relevant if you used @ref ONESDK_INIT_FLAG_FORKABLE. See @ref agent_fork_state_constants "Agent forking state constants".

    Calling this function only has a defined result when @ref onesdk_agent_get_current_state returns @ref ONESDK_AGENT_STATE_ACTIVE or
    @ref ONESDK_AGENT_STATE_TEMPORARILY_INACTIVE.

    @return The current agent fork state, i.e., one of the @ref agent_fork_state_constants.
    @see @ref agent_fork_state_constants "Agent forking state constants"
*/
ONESDK_DECLARE_FUNCTION(onesdk_int32_t) onesdk_agent_get_fork_state(void);

/*========================================================================================================================================*/

/** @} */

/*========================================================================================================================================*/

/** @defgroup tracers Common Tracer Functions
    @brief Basic functions that can be used with all or multiple tracer types.

    Tracers are the objects used to capture information about "operations" (e.g. remote calls or database requests) that your application
    performs.

    Tracers are created by different factory functions, depending on the type of the tracer, and destroyed/released by
    @ref onesdk_tracer_end.

    Tracers are automatically linked to other active tracers on the same thread. To link to an operation that's running in another thread,
    processes or on another system you have to use the tagging or @link in_process_links in-process linking@endlink functions.

    @attention
    All tracers in this SDK have strict thread affinity - they are bound to the thread by which they were created. Calling _any_ tracer
    function with a tracer handle that was created by a different thread is an error and will not produce any effect. Note that this
    includes @ref onesdk_tracer_end. Thus trying to destroy/release a tracer from a different thread will result in a memory leak.

    For further information, see the high level SDK documentation at https://github.com/Dynatrace/OneAgent-SDK/#tracers

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

    If the tracer has been started, the time measurement will be stopped and its exit fields will be captured before releasing the tracer.

    @note Just like all other tracer functions, this function must only be called from the thread that was used to create the tracer.
          This is important because it means that a tracer cannot be released on a different thread. Attempting to do so is an error and
          will result in memory leaks.
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

    @note If called with invalid arguments, the retrieved string will be an empty string.
    @note Calling this function multiple times for the same tracer is explicitly supported and will yield the same result.
    @note Retrieving both the string representation and the binary representation from the same tracer is explicitly supported.

    @see @ref onesdk_tracer_get_outgoing_dynatrace_byte_tag
*/
ONESDK_DECLARE_FUNCTION(onesdk_size_t) onesdk_tracer_get_outgoing_dynatrace_string_tag(onesdk_tracer_handle_t tracer_handle, char* buffer, onesdk_size_t buffer_size, onesdk_size_t* required_buffer_size);

/** @brief Retrieves the binary representation of the tag from an "outgoing taggable" tracer.
    @param tracer_handle                A valid tracer handle.
    @param[out] buffer                  [optional] Pointer to a buffer into which the binary representation shall be copied.
    @param buffer_size                  Size of the buffer pointed to by @p buffer in bytes. Must be zero if @p buffer is `NULL`.
    @param[out] required_buffer_size    [optional] Pointer to a @ref onesdk_size_t variable where the required buffer size will be stored.

    @return The number of bytes copied into @p buffer.

    An application can call this function to retrieve the binary representation of the outgoing tag for a tracer. The tracer handle
    must refer to an "outgoing taggable" tracer that has already been started.

    If @p required_buffer_size is not `NULL`, the number of bytes required to store the binary representation is stored in
    @p *required_buffer_size.

    If @p buffer is not `NULL` and @p buffer_size is big enough, this function will copy the binary representation into the provided buffer.

    @note If called with invalid arguments, the retrieved binary tag will be empty (have zero length).
    @note Calling this function multiple times for the same tracer is explicitly supported and will yield the same result.
    @note Retrieving both the string representation and the binary representation from the same tracer is explicitly supported.

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

    @see @ref onesdk_tracer_set_incoming_dynatrace_string_tag
    @see @ref onesdk_tracer_set_incoming_dynatrace_byte_tag
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

    @see @ref onesdk_tracer_set_incoming_dynatrace_string_tag
*/
ONESDK_DECLARE_FUNCTION(void) onesdk_tracer_set_incoming_dynatrace_byte_tag(onesdk_tracer_handle_t tracer_handle, unsigned char const* byte_tag, onesdk_size_t byte_tag_size);

/*========================================================================================================================================*/

/** @} */

/*========================================================================================================================================*/

/** @defgroup custom_request_attributes Custom Request Attributes
    @brief Attach custom key/value pairs to the active tracer.

    Custom request attributes can be used by an application to attach custom key/value pairs to the active tracer.

    For further information, see the high level SDK documentation at https://github.com/Dynatrace/OneAgent-SDK/#scav

    @since Custom request attributes were added in version 1.2.0.

    @{
*/

/*========================================================================================================================================*/

/** @internal */
ONESDK_DECLARE_FUNCTION(void) onesdk_customrequestattribute_add_integers_p(onesdk_string_t const* keys, onesdk_int64_t const* values, onesdk_size_t count);

/** @brief Adds a custom request attribute integer to the active tracer.
    @param key      The key (name, ID) of the custom request attribute.
    @param value    The value of the custom request attribute.

    For more information see @ref custom_request_attributes.

    @since This function was added in version 1.2.0.
*/
ONESDK_DEFINE_INLINE_FUNCTION(void) onesdk_customrequestattribute_add_integer(onesdk_string_t key, onesdk_int64_t value) {
    onesdk_customrequestattribute_add_integers_p(&key, &value, 1);
}

/** @internal */
ONESDK_DECLARE_FUNCTION(void) onesdk_customrequestattribute_add_floats_p(onesdk_string_t const* keys, double const* values, onesdk_size_t count);

/** @brief Adds a custom request attribute floating point value to the active tracer.
    @param key      The key (name, ID) of the custom request attribute.
    @param value    The value of the custom request attribute.

    For more information see @ref custom_request_attributes.

    @since This function was added in version 1.2.0.
*/
ONESDK_DEFINE_INLINE_FUNCTION(void) onesdk_customrequestattribute_add_float(onesdk_string_t key, double value) {
    onesdk_customrequestattribute_add_floats_p(&key, &value, 1);
}

/** @internal */
ONESDK_DECLARE_FUNCTION(void) onesdk_customrequestattribute_add_strings_p(onesdk_string_t const* keys, onesdk_string_t const* values, onesdk_size_t count);

/** @brief Adds a custom request attribute string to the active tracer.
    @param key      The key (name, ID) of the custom request attribute.
    @param value    [optional] The value of the custom request attribute.

    For more information see @ref custom_request_attributes.

    @since This function was added in version 1.2.0.
*/
ONESDK_DEFINE_INLINE_FUNCTION(void) onesdk_customrequestattribute_add_string(onesdk_string_t key, onesdk_string_t value) {
    onesdk_customrequestattribute_add_strings_p(&key, &value, 1);
}

/*========================================================================================================================================*/

/** @} */

/*========================================================================================================================================*/

/** @defgroup in_process_links In-Process Link Functions
    @brief Associate asynchronous tasks in the same process with the current operation.

    In-process links allow an application to associate (link) tasks, that will be executed asynchronously in the same process, with the
    currently running task/operation. The linked tasks may be started and completed at arbitrary times - it's not necessary for them
    to complete (or even start) before the "parent" operation to which they are linked completes.

    For further information, see the high level SDK documentation at https://github.com/Dynatrace/OneAgent-SDK/#in-process-linking

    @since In-process links were added in version 1.2.0.

    @{
*/

/*========================================================================================================================================*/

/** @brief Creates an in-process link.
    @param[out] buffer                  [optional] Pointer to a buffer into which the in-process link shall be copied.
    @param buffer_size                  Size of the buffer pointed to by @p buffer in bytes. Must be zero if @p buffer is `NULL`.
    @param[out] required_buffer_size    [optional] Pointer to a @ref onesdk_size_t variable where the required buffer size will be stored.

    @return The number of bytes copied into @p buffer.

    An application can call this function to retrieve an in-process link, which can then be used to trace related processing at a later
    time and/or in a different thread.

    If @p required_buffer_size is not `NULL`, the number of bytes required to store the in-process link is stored in
    @p *required_buffer_size.

    If @p buffer is not `NULL` and @p buffer_size is big enough, this function will copy the in-process link into the provided buffer.

    @note If no tracer is active on the current thread, the retrieved link will be empty (have zero length).
    @note Links returned by this function are not compatible with dynatrace string or byte tags, they can only be used with
          @ref onesdk_inprocesslinktracer_create.
    @note Links returned by this function can only be used in the process in which they were created.

    For more information see @ref in_process_links.

    @since This function was added in version 1.2.0.
*/
ONESDK_DECLARE_FUNCTION(onesdk_size_t) onesdk_inprocesslink_create(unsigned char* buffer, onesdk_size_t buffer_size, onesdk_size_t* required_buffer_size);

/** @brief Creates a tracer for tracing asynchronous related processing in the same process.
    @param in_process_link         Pointer to an array that holds the in-process link.
    @param in_process_link_size    Size of the in-process link.

    @return A handle for the newly created in-process link tracer or @ref ONESDK_INVALID_HANDLE.

    @note If the provided in-process link is empty or invalid, no tracer will be created and this function will return
          @ref ONESDK_INVALID_HANDLE.

    For more information see @ref in_process_links.

    @since This function was added in version 1.2.0.
*/
ONESDK_DECLARE_FUNCTION(onesdk_tracer_handle_t) onesdk_inprocesslinktracer_create(unsigned char const* in_process_link, onesdk_size_t in_process_link_size);

/*========================================================================================================================================*/

/** @} */

/*========================================================================================================================================*/

/** @defgroup remote_calls Remote Call Tracers
    @brief Trace calls to remote services (RMI, etc.).

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

    For further information, see the high level SDK documentation at https://github.com/Dynatrace/OneAgent-SDK/#remoting

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

/** @defgroup database_requests Database Request Tracers
    @brief Trace SQL-based database requests.

    Database request tracers are used to capture information about database requests. They're not taggable.

    To create a database request tracer, an application must first create a database info object which describes the database that the
    application uses - see @ref onesdk_databaseinfo_create.

    @note Note that SQL database traces are only created if they occur within some other SDK trace (e.g. incoming remote call).

    For further information, see the high level SDK documentation at https://github.com/Dynatrace/OneAgent-SDK/#database

    @{
*/

/** @internal */
ONESDK_DECLARE_FUNCTION(onesdk_databaseinfo_handle_t) onesdk_databaseinfo_create_p(onesdk_string_t const* name, onesdk_string_t const* vendor, onesdk_int32_t channel_type, onesdk_string_t const* channel_endpoint);

/** @brief Creates a database info object.
    @param name                 The name of the database.
    @param vendor               The type of the database (e.g. "sqlite", "MySQL", "Oracle", "DB2" - see @ref database_vendor_strings).
    @param channel_type         The type of the channel used to communicate with the database.
    @param channel_endpoint     [optional] The endpoint of the channel used to communicate with the database.

    @return A handle for the newly created database info object or @ref ONESDK_INVALID_HANDLE.

    This function creates a database info object which describes a database instance and is required for tracing database requests (see
    @ref onesdk_databaserequesttracer_create_sql).

    For information about @p channel_type and @p channel_endpoint see @ref channels.
*/
ONESDK_DEFINE_INLINE_FUNCTION(onesdk_databaseinfo_handle_t) onesdk_databaseinfo_create(onesdk_string_t name, onesdk_string_t vendor, onesdk_int32_t channel_type, onesdk_string_t channel_endpoint) {
    return onesdk_databaseinfo_create_p(&name, &vendor, channel_type, &channel_endpoint);
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
ONESDK_DECLARE_FUNCTION(onesdk_tracer_handle_t) onesdk_databaserequesttracer_create_sql_p(onesdk_databaseinfo_handle_t databaseinfo_handle, onesdk_string_t const* statement);

/** @brief Creates a tracer for tracing SQL database requests.
    @param databaseinfo_handle      A valid database info handle.
    @param statement                The database statement (SQL).

    @return A handle for the newly created database request tracer or @ref ONESDK_INVALID_HANDLE.

    @see @ref onesdk_databaseinfo_create
*/
ONESDK_DEFINE_INLINE_FUNCTION(onesdk_tracer_handle_t) onesdk_databaserequesttracer_create_sql(onesdk_databaseinfo_handle_t databaseinfo_handle, onesdk_string_t statement) {
    return onesdk_databaserequesttracer_create_sql_p(databaseinfo_handle, &statement);
}

/** @brief Sets the number of returned rows for a database request.
    @param tracer_handle            A valid database request tracer handle.
    @param returned_row_count       The number of returned rows. Must be >= 0.

    @since This function was added in version 1.1.0.
*/
ONESDK_DECLARE_FUNCTION(void) onesdk_databaserequesttracer_set_returned_row_count(onesdk_tracer_handle_t tracer_handle, onesdk_int32_t returned_row_count);

/** @brief Sets the number of round trips for a database request.
    @param tracer_handle            A valid database request tracer handle.
    @param round_trip_count         The number of round trips between the client and the database. Must be >= 0.

    @since This function was added in version 1.1.0.
*/
ONESDK_DECLARE_FUNCTION(void) onesdk_databaserequesttracer_set_round_trip_count(onesdk_tracer_handle_t tracer_handle, onesdk_int32_t round_trip_count);

/*========================================================================================================================================*/

/** @} */

/*========================================================================================================================================*/

/** @defgroup incoming_web_requests Incoming Web Request Tracers
    @brief Trace the server side of web requests.

    Incoming web request tracers are used to capture information about HTTP requests that the application services (processes, answers).

    To create an incoming web request tracer, an application must first create a web application info object which describes the web
    application - see @ref onesdk_webapplicationinfo_create.

    For further information, see the high level SDK documentation at https://github.com/Dynatrace/OneAgent-SDK/#webrequests

    @see @ref outgoing_web_requests
    @since Incoming web request tracers were added in version 1.1.0.

    @{
*/

/** @internal */
ONESDK_DECLARE_FUNCTION(onesdk_webapplicationinfo_handle_t) onesdk_webapplicationinfo_create_p(onesdk_string_t const* web_server_name, onesdk_string_t const* application_id, onesdk_string_t const* context_root);

/** @brief Creates a web application info object.
    @param web_server_name          The logical name of the web server that hosts the application.
                                    In case of a cluster, every node in the cluster must use the same @p web_server_name.
    @param application_id           A unique ID for the web application. This will also be used as the display name.
    @param context_root             The context root of the web application. This is the common path prefix for requests which will be
                                    routed to the web application.

    @return A handle for the newly created web application info object or @ref ONESDK_INVALID_HANDLE.

    This function creates a web application info object which is required for tracing incoming web requests
    (see @ref onesdk_incomingwebrequesttracer_create).

    The provided information determines the identity and name of the resulting web request service in Dynatrace.

    See https://www.dynatrace.com/support/help/server-side-services/introduction/how-does-dynatrace-detect-and-name-services/#web-request-services
    for more information about the meaning of the parameters.

    @since This function was added in version 1.1.0.
*/
ONESDK_DEFINE_INLINE_FUNCTION(onesdk_webapplicationinfo_handle_t) onesdk_webapplicationinfo_create(onesdk_string_t web_server_name, onesdk_string_t application_id, onesdk_string_t context_root) {
    return onesdk_webapplicationinfo_create_p(&web_server_name, &application_id, &context_root);
}

/** @brief Releases a web application info object.
    @param webapplicationinfo_handle    A valid web application info handle.

    This function releases the specified web application info object. Allocated resources are freed and the handle is invalidated.

    An application should call @ref onesdk_webapplicationinfo_delete exactly once for each web application info object that it has created.

    @note Calling @ref onesdk_webapplicationinfo_delete with a handle to a web application info object which is still referenced by existing
          tracers is supported. In that case the lifetime of the web application info object is extended as necessary. The handle will
          always become invalid immediately though.

    @since This function was added in version 1.1.0.
*/
ONESDK_DECLARE_FUNCTION(void) onesdk_webapplicationinfo_delete(onesdk_webapplicationinfo_handle_t webapplicationinfo_handle);

/** @internal */
ONESDK_DECLARE_FUNCTION(onesdk_tracer_handle_t) onesdk_incomingwebrequesttracer_create_p(onesdk_webapplicationinfo_handle_t webapplicationinfo_handle, onesdk_string_t const* url, onesdk_string_t const* method);

/** @brief Creates a tracer for tracing an incoming web request.
    @param webapplicationinfo_handle    A valid web application info handle.
    @param url                          The requested URL. Will be parsed into `scheme`, `host`/`port`, `path` and `query`.
    @param method                       The HTTP method of the request.

    @return A handle for the newly created incoming web request tracer or @ref ONESDK_INVALID_HANDLE.

    @note @p url does not have to contain a `scheme` or `host`. You can use the URL as it was sent in the HTTP request.
    @note If @p url contains a host name it will be resolved by the agent (asynchronously) after @ref onesdk_tracer_start was called.

    @since This function was added in version 1.1.0.
*/
ONESDK_DEFINE_INLINE_FUNCTION(onesdk_tracer_handle_t) onesdk_incomingwebrequesttracer_create(onesdk_webapplicationinfo_handle_t webapplicationinfo_handle, onesdk_string_t url, onesdk_string_t method) {
    return onesdk_incomingwebrequesttracer_create_p(webapplicationinfo_handle, &url, &method);
}

/** @internal */
ONESDK_DECLARE_FUNCTION(void) onesdk_incomingwebrequesttracer_set_remote_address_p(onesdk_tracer_handle_t tracer_handle, onesdk_string_t const* remote_address);

/** @brief Sets the remote address of an incoming web request.
    @param tracer_handle            A valid incoming web request tracer handle.
    @param remote_address           The remote IP address.

    The remote address is very useful to gain information about load balancers, proxies and ultimately the end user that is sending the
    request.

    @note The remote address is the peer address of the socket connection via which the request was received. In case one or more proxies
          are used, this will be the address of the last proxy in the proxy chain. To enable the agent to determine the client IP address
          (=the address where the request originated), an application should also call
          @ref onesdk_incomingwebrequesttracer_add_request_header to add any HTTP request headers.
    @note Calling this function with @p remote_address = @ref onesdk_nullstr() will reset/clear any value that was set previously.
    @note This function can not be used after the tracer was started.

    @since This function was added in version 1.1.0.
*/
ONESDK_DEFINE_INLINE_FUNCTION(void) onesdk_incomingwebrequesttracer_set_remote_address(onesdk_tracer_handle_t tracer_handle, onesdk_string_t remote_address) {
    onesdk_incomingwebrequesttracer_set_remote_address_p(tracer_handle, &remote_address);
}

/** @internal */
ONESDK_DECLARE_FUNCTION(void) onesdk_incomingwebrequesttracer_add_request_headers_p(onesdk_tracer_handle_t tracer_handle, onesdk_string_t const* names, onesdk_string_t const* values, onesdk_size_t count);

/** @brief Adds an HTTP request header of an incoming web request.
    @param tracer_handle            A valid incoming web request tracer handle.
    @param name                     The name of the HTTP request header.
    @param value                    [optional] The value of the HTTP request header.

    To allow the agent to determine various bits of useful information, an application should add all HTTP request headers.

    @note The native SDK agent will currently capture all provided headers.
    @note If an HTTP request contains multiple header lines with the same header name, an application should call this function once per
          line. Alternatively, depending on the header, the application can call this function once per header name, with an appropriately
          concatenated header value.
    @note This function can not be used after the tracer was started.

    @since This function was added in version 1.1.0.
*/
ONESDK_DEFINE_INLINE_FUNCTION(void) onesdk_incomingwebrequesttracer_add_request_header(onesdk_tracer_handle_t tracer_handle, onesdk_string_t name, onesdk_string_t value) {
    onesdk_incomingwebrequesttracer_add_request_headers_p(tracer_handle, &name, &value, 1);
}

/** @internal */
ONESDK_DECLARE_FUNCTION(void) onesdk_incomingwebrequesttracer_add_parameters_p(onesdk_tracer_handle_t tracer_handle, onesdk_string_t const* names, onesdk_string_t const* values, onesdk_size_t count);

/** @brief Adds an HTTP POST parameter of an incoming web request.
    @param tracer_handle            A valid incoming web request tracer handle.
    @param name                     The name of the HTTP POST parameter.
    @param value                    [optional] The value of the HTTP POST parameter.

    @note The native SDK agent will currently capture all provided parameters.

    @since This function was added in version 1.1.0.
*/
ONESDK_DEFINE_INLINE_FUNCTION(void) onesdk_incomingwebrequesttracer_add_parameter(onesdk_tracer_handle_t tracer_handle, onesdk_string_t name, onesdk_string_t value) {
    onesdk_incomingwebrequesttracer_add_parameters_p(tracer_handle, &name, &value, 1);
}

/** @internal */
ONESDK_DECLARE_FUNCTION(void) onesdk_incomingwebrequesttracer_add_response_headers_p(onesdk_tracer_handle_t tracer_handle, onesdk_string_t const* names, onesdk_string_t const* values, onesdk_size_t count);

/** @brief Adds an HTTP response header for an incoming web request.
    @param tracer_handle            A valid incoming web request tracer handle.
    @param name                     The name of the HTTP response header.
    @param value                    [optional] The value of the HTTP response header.

    @note The native SDK agent will currently capture all provided headers.
    @note If the HTTP response contains multiple header lines with the same header name, an application should call this function once per
          line. Alternatively, depending on the header, the application can call this function once per header name, with an appropriately
          concatenated header value.

    @since This function was added in version 1.1.0.
*/
ONESDK_DEFINE_INLINE_FUNCTION(void) onesdk_incomingwebrequesttracer_add_response_header(onesdk_tracer_handle_t tracer_handle, onesdk_string_t name, onesdk_string_t value) {
    onesdk_incomingwebrequesttracer_add_response_headers_p(tracer_handle, &name, &value, 1);
}

/** @brief Sets the HTTP status code for an incoming web request.
    @param tracer_handle            A valid incoming web request tracer handle.
    @param status_code              The HTTP status code of the response sent to the client.

    @note If an application can not send a response to the client, it should @em not call this function. Instead the application can use
          @ref onesdk_tracer_error to signal that the request could not be processed successfully.

    @since This function was added in version 1.1.0.
*/
ONESDK_DECLARE_FUNCTION(void) onesdk_incomingwebrequesttracer_set_status_code(onesdk_tracer_handle_t tracer_handle, onesdk_int32_t status_code);

/*========================================================================================================================================*/

/** @} */

/*========================================================================================================================================*/

/** @defgroup outgoing_web_requests Outgoing Web Request Tracers
    @brief Trace the client side of web requests.

    Outgoing web request tracers are used to capture information about HTTP requests that the application sends.

    To create an outgoing web request tracer, an application can simply call @ref onesdk_outgoingwebrequesttracer_create. To enable
    continuing the trace on the server/service side, the application must then retrieve the string tag from the tracer and send it along with
    the request in the HTTP request header `"X-dynaTrace"`.

    @see @ref onesdk_tracer_get_outgoing_dynatrace_string_tag
    @see @ref ONESDK_DYNATRACE_HTTP_HEADER_NAME
    @see @ref incoming_web_requests

    For further information, see the high level SDK documentation at https://github.com/Dynatrace/OneAgent-SDK/#webrequests

    @since Outgoing web request tracers were added in version 1.2.0.

    @{
*/

/*========================================================================================================================================*/

/** @internal */
ONESDK_DECLARE_FUNCTION(onesdk_tracer_handle_t) onesdk_outgoingwebrequesttracer_create_p(onesdk_string_t const* url, onesdk_string_t const* method);

/** @brief Creates a tracer for tracing an outgoing web request.
    @param url                      The requested URL. Will be parsed into `scheme`, `host`/`port`, `path` and `query`.
    @param method                   The HTTP method of the request.

    @return A handle for the newly created outgoing web request tracer or @ref ONESDK_INVALID_HANDLE.

    @note @p url should be a full, absolute URL, i.e. containing the `scheme`, `host`, `port` (unless default), `path` and `query`.
          Any `fragment` part, if present, will be ignored (fragments are only processed locally and not part of the request URL).
    @note If @p url contains a host name it will be resolved by the agent (asynchronously) after @ref onesdk_tracer_start was called.

    @since This function was added in version 1.2.0.
*/
ONESDK_DEFINE_INLINE_FUNCTION(onesdk_tracer_handle_t) onesdk_outgoingwebrequesttracer_create(onesdk_string_t url, onesdk_string_t method) {
    return onesdk_outgoingwebrequesttracer_create_p(&url, &method);
}

/** @internal */
ONESDK_DECLARE_FUNCTION(void) onesdk_outgoingwebrequesttracer_add_request_headers_p(onesdk_tracer_handle_t tracer_handle, onesdk_string_t const* names, onesdk_string_t const* values, onesdk_size_t count);

/** @brief Adds an HTTP request header of an outgoing web request.
    @param tracer_handle            A valid outgoing web request tracer handle.
    @param name                     The name of the HTTP request header.
    @param value                    [optional] The value of the HTTP request header.

    To allow the agent to determine various bits of useful information, an application should add all HTTP request headers.

    @note The native SDK agent will currently capture all provided headers.
    @note If an HTTP request contains multiple header lines with the same header name, an application should call this function once per
          line. Alternatively, depending on the header, the application can call this function once per header name, with an appropriately
          concatenated header value.
    @note This function can not be used after the tracer was started.

    @since This function was added in version 1.2.0.
*/
ONESDK_DEFINE_INLINE_FUNCTION(void) onesdk_outgoingwebrequesttracer_add_request_header(onesdk_tracer_handle_t tracer_handle, onesdk_string_t name, onesdk_string_t value) {
    onesdk_outgoingwebrequesttracer_add_request_headers_p(tracer_handle, &name, &value, 1);
}

/** @internal */
ONESDK_DECLARE_FUNCTION(void) onesdk_outgoingwebrequesttracer_add_response_headers_p(onesdk_tracer_handle_t tracer_handle, onesdk_string_t const* names, onesdk_string_t const* values, onesdk_size_t count);

/** @brief Adds an HTTP response header for an outgoing web request.
    @param tracer_handle            A valid outgoing web request tracer handle.
    @param name                     The name of the HTTP response header.
    @param value                    [optional] The value of the HTTP response header.

    @note The native SDK agent will currently capture all provided headers.
    @note If the HTTP response contains multiple header lines with the same header name, an application should call this function once per
          line. Alternatively, depending on the header, the application can call this function once per header name, with an appropriately
          concatenated header value.

    @since This function was added in version 1.2.0.
*/

ONESDK_DEFINE_INLINE_FUNCTION(void) onesdk_outgoingwebrequesttracer_add_response_header(onesdk_tracer_handle_t tracer_handle, onesdk_string_t name, onesdk_string_t value) {
    onesdk_outgoingwebrequesttracer_add_response_headers_p(tracer_handle, &name, &value, 1);
}

/** @brief Sets the HTTP status code for an outgoing web request.
    @param tracer_handle            A valid outgoing web request tracer handle.
    @param status_code              The HTTP status code of the response that was received.

    @note If an application fails to receive or decode the reply, it should @em not call this function. Instead the application can use
          @ref onesdk_tracer_error to signal that the request was not completed successfully.

    @since This function was added in version 1.2.0.
*/
ONESDK_DECLARE_FUNCTION(void) onesdk_outgoingwebrequesttracer_set_status_code(onesdk_tracer_handle_t tracer_handle, onesdk_int32_t status_code);

/*========================================================================================================================================*/

/** @} */

/*========================================================================================================================================*/

/** @defgroup customservice Custom Service Tracers
    @brief Trace services that are important but for which there is no more specific tracer.

    Custom service tracers are used to trace service calls for which there is no other suitable tracer.

    To create a custom service tracer, an application can simply call @ref onesdk_customservicetracer_create.

    For further information, see the high level SDK documentation at https://github.com/Dynatrace/OneAgent-SDK/#customservice

    @since Custom service tracers were added in version 1.4.0.

    @{
*/

/** @internal */
ONESDK_DECLARE_FUNCTION(onesdk_tracer_handle_t) onesdk_customservicetracer_create_p(onesdk_string_t const* service_method, onesdk_string_t const* service_name);

/** @brief Creates a tracer for tracing a custom service.
    @param service_method           The name of the service method.
    @param service_name             The name of the service.

    @return A handle for the newly created custom service tracer or @ref ONESDK_INVALID_HANDLE.

    @since This function was added in version 1.4.0.
*/
ONESDK_DEFINE_INLINE_FUNCTION(onesdk_tracer_handle_t) onesdk_customservicetracer_create(onesdk_string_t service_method, onesdk_string_t service_name) {
    return onesdk_customservicetracer_create_p(&service_method, &service_name);
}

/*========================================================================================================================================*/

/** @} */

/*========================================================================================================================================*/

/** @defgroup messaging Messaging 
    @brief Trace messages sent or received via a messaging system.

    When tracing messages, we distinguish between:

    - Sending a message: See @ref onesdk_outgoingmessagetracer_create.
    - Receiving a message: See @ref onesdk_incomingmessagereceivetracer_create
    - Processing a received message: See @ref onesdk_incomingmessageprocesstracer_create

    To create an outgoing message tracer, an application can simply call @ref onesdk_outgoingmessagetracer_create. To enable
    continuing the trace on the consumer side, the application must then retrieve the byte tag from the tracer and
    send it along with the message in a property named `"dtdTraceTagInfo"` (if the other side is also traced by the SDK,
    any other method of transferring the tag is also possible as long as the receiving application's tracing cooperates,
    but the aforementioned method is highly recommended and also works with non-SDK OneAgents if the receiving message
    system is supported).

    @see @ref onesdk_tracer_get_outgoing_dynatrace_byte_tag
    @see @ref ONESDK_DYNATRACE_MESSAGE_PROPERTY_NAME

    For further information, see the high level SDK documentation at https://github.com/Dynatrace/OneAgent-SDK/#messaging

    @since Messaging tracers were added in version 1.4.0.

    @{
*/

/** @internal */
ONESDK_DECLARE_FUNCTION(onesdk_messagingsysteminfo_handle_t) onesdk_messagingsysteminfo_create_p(
    onesdk_string_t const* vendor_name, onesdk_string_t const* destination_name, onesdk_int32_t destination_type, onesdk_int32_t channel_type, onesdk_string_t const* channel_endpoint);

/** @brief Creates a messaging system info object.
    @param vendor_name          One of @ref messaging_vendor_strings for well known vendors, or a custom string otherwise.
    @param destination_name     The "destination" name, i.e. queue name or topic name.
    @param destination_type     One of the @ref messaging_destination_type.
    @param channel_type         The type of the channel used to communicate with the messaging system.
    @param channel_endpoint     [optional] The endpoint of the channel used to communicate with the messaging system.

    @return A handle for the newly created messaging system info object or @ref ONESDK_INVALID_HANDLE.

    This function creates a messaging system info object which is required for tracing sending, receiving and processing messages.
    (see @ref messaging).

    For information about @p channel_type and @p channel_endpoint see @ref channels.

    @since This function was added in version 1.4.0.
*/
ONESDK_DEFINE_INLINE_FUNCTION(onesdk_messagingsysteminfo_handle_t) onesdk_messagingsysteminfo_create(
    onesdk_string_t vendor_name, onesdk_string_t destination_name, onesdk_int32_t destination_type, onesdk_int32_t channel_type, onesdk_string_t channel_endpoint
) {
    return onesdk_messagingsysteminfo_create_p(&vendor_name, &destination_name, destination_type, channel_type, &channel_endpoint);
}

/** @brief Releases a messaging system info object.
    @param messagingsysteminfo_handle   A valid messaging system info handle.

    This function releases the specified messaging system info object. Allocated resources are freed and the handle is invalidated.

    An application should call @ref onesdk_messagingsysteminfo_delete exactly once for each messaging system info object that it has created.

    @note Calling @ref onesdk_messagingsysteminfo_delete with a handle to a messaging system info object which is still referenced by existing
          tracers is supported. In that case the lifetime of the messaging system info object is extended as necessary. The handle will
          always become invalid immediately though.

    @since This function was added in version 1.4.0.
*/
ONESDK_DECLARE_FUNCTION(void) onesdk_messagingsysteminfo_delete(onesdk_messagingsysteminfo_handle_t messagingsysteminfo_handle);

/** @brief Creates a tracer for tracing an outgoing message.
    @param messagingsysteminfo_handle   A valid messaging system info handle.

    @return A handle for the newly created outgoing message tracer or @ref ONESDK_INVALID_HANDLE.

    @since This function was added in version 1.4.0.
*/
ONESDK_DECLARE_FUNCTION(onesdk_tracer_handle_t) onesdk_outgoingmessagetracer_create(onesdk_messagingsysteminfo_handle_t messagingsysteminfo_handle);

/** @internal */
ONESDK_DECLARE_FUNCTION(void) onesdk_outgoingmessagetracer_set_vendor_message_id_p(onesdk_tracer_handle_t tracer_handle, onesdk_string_t const* vendor_message_id);

/** @brief Sets the vendor message ID of an outgoing message.
    @param tracer_handle            A valid incoming message process tracer handle.
    @param vendor_message_id        The message ID provided by the messaging system.

    @note This information is often only available after the message was sent. Thus, calling
          this function is also supported after starting the tracer.

    @since This function was added in version 1.4.0.
*/
ONESDK_DEFINE_INLINE_FUNCTION(void) onesdk_outgoingmessagetracer_set_vendor_message_id(onesdk_tracer_handle_t tracer_handle, onesdk_string_t vendor_message_id) {
    onesdk_outgoingmessagetracer_set_vendor_message_id_p(tracer_handle, &vendor_message_id);
}

/** @internal */
ONESDK_DECLARE_FUNCTION(void) onesdk_outgoingmessagetracer_set_correlation_id_p(onesdk_tracer_handle_t tracer_handle, onesdk_string_t const* correlation_id);

/** @brief Sets the corrrelation ID of an outgoing message.
    @param tracer_handle            A valid incoming message process tracer handle.
    @param correlation_id           The correlation ID for the message, usually application-defined.

    @note This information is often only available after the message was sent. Thus, calling
          this function is also supported after starting the tracer.

    @since This function was added in version 1.4.0.
*/
ONESDK_DEFINE_INLINE_FUNCTION(void) onesdk_outgoingmessagetracer_set_correlation_id(onesdk_tracer_handle_t tracer_handle, onesdk_string_t correlation_id) {
    onesdk_outgoingmessagetracer_set_correlation_id_p(tracer_handle, &correlation_id);
}

/** @brief Creates a tracer for tracing the receipt of an incoming message.
    @param messagingsysteminfo_handle   A valid messaging system info handle.

    @return A handle for the newly created incoming message receive tracer or @ref ONESDK_INVALID_HANDLE.

    Tracing the receipt of the message is optional but may make sense if receiving may take a significant amount of time,
    e.g. when doing a blocking receive. It might make less sense when tracing a polling receive. If you do use a receive
    tracer, start and end the corresponding incoming message process tracer while the receive tracer is still active.

    @see onesdk_incomingmessageprocesstracer_create
    @since This function was added in version 1.4.0.
*/
ONESDK_DECLARE_FUNCTION(onesdk_tracer_handle_t) onesdk_incomingmessagereceivetracer_create(onesdk_messagingsysteminfo_handle_t messagingsysteminfo_handle);

/** @brief Creates a tracer for tracing the processing of an incoming message.
    @param messagingsysteminfo_handle   A valid messaging system info handle.

    @return A handle for the newly created incoming message process tracer or @ref ONESDK_INVALID_HANDLE.

    Use this tracer to trace the actual, logical processing of the message as opposed to the time it takes to receive it.
    
    If you use an incoming message receive tracer to trace the receipt of the processed message, start
    and end the corresponding incoming message process tracer while the receive tracer is still active.

    @see onesdk_incomingmessagereceivetracer_create
    @since This function was added in version 1.4.0.
*/
ONESDK_DECLARE_FUNCTION(onesdk_tracer_handle_t) onesdk_incomingmessageprocesstracer_create(onesdk_messagingsysteminfo_handle_t messagingsysteminfo_handle);

/** @internal */
ONESDK_DECLARE_FUNCTION(void) onesdk_incomingmessageprocesstracer_set_vendor_message_id_p(onesdk_tracer_handle_t tracer_handle, onesdk_string_t const* vendor_message_id);

/** @brief Sets the vendor message ID of an incoming message.
    @param tracer_handle            A valid incoming message process tracer handle.
    @param vendor_message_id        The message ID provided by the messaging system.

    @since This function was added in version 1.4.0.
*/
ONESDK_DEFINE_INLINE_FUNCTION(void) onesdk_incomingmessageprocesstracer_set_vendor_message_id(onesdk_tracer_handle_t tracer_handle, onesdk_string_t vendor_message_id) {
    onesdk_incomingmessageprocesstracer_set_vendor_message_id_p(tracer_handle, &vendor_message_id);
}

/** @internal */
ONESDK_DECLARE_FUNCTION(void) onesdk_incomingmessageprocesstracer_set_correlation_id_p(onesdk_tracer_handle_t tracer_handle, onesdk_string_t const* correlation_id);

/** @brief Sets the corrrelation ID of an incoming message.
    @param tracer_handle            A valid incoming message process tracer handle.
    @param correlation_id           The correlation ID for the message, usually application-defined.

    @since This function was added in version 1.4.0.
*/
ONESDK_DEFINE_INLINE_FUNCTION(void) onesdk_incomingmessageprocesstracer_set_correlation_id(onesdk_tracer_handle_t tracer_handle, onesdk_string_t correlation_id) {
    onesdk_incomingmessageprocesstracer_set_correlation_id_p(tracer_handle, &correlation_id);
}

/*========================================================================================================================================*/

/** @} */

/*========================================================================================================================================*/

#endif /* ONESDK_AGENT_H_INCLUDED */
