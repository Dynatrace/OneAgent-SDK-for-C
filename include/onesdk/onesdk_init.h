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

#ifndef ONESDK_INIT_H_INCLUDED
#define ONESDK_INIT_H_INCLUDED

/** @file
    @brief Defines SDK initialization and shutdown functions.
*/

/*========================================================================================================================================*/

#include "onesdk/onesdk_common.h" /* IWYU pragma: export */

/** @defgroup init Initialization and Shutdown
    @brief APIs to configure, initialize and shutdown the SDK and agent and related basic functions.

    A typical use of these functions could be:
    
    @code{.c}
    #include <onesdk/onesdk.h>
    #include <stdio.h>

    // optional but higly recommended (see below)
    static void mycallback(char const* message) {
        fputs(message, stderr);
    }

    int main(int argc, char** argv) {
        onesdk_stub_process_cmdline_args(argc, argv, 1);  // optional: let the SDK process command line arguments 
        onesdk_stub_strip_sdk_cmdline_args(&argc, argv);  // optional: remove SDK command line arguments from argv

        // Initialize SDK
        onesdk_result_t const onesdk_init_result = onesdk_initialize();

        // optional: Set logging callbacks (see @ref misc)
        onesdk_agent_set_warning_callback(mycallback); // Highly recommended.
        onesdk_agent_set_verbose_callback(mycallback); // Recommended for development & debugging.

        // ... use SDK ...

        // Shut down SDK only if initialization suceeded.
        if (onesdk_init_result == ONESDK_SUCCESS)
            onesdk_shutdown();

        return 0;
    }
    @endcode

    Note that most of the functions in this module have return codes and @ref onesdk_stub_xstrerror can be used to translate
    them to human-readable messages. After initialization, errors are reported via the logging callbacks (`mycallback`
    in the example above).

    @{
*/

/*========================================================================================================================================*/

/** @brief Retrieves the stub version number.
    @param[out] out_stub_version    Pointer to a @ref onesdk_stub_version_t struct that will be filled with the stub version number.

    An application can use this function to make sure the stub binary matches the header files that were used to compile the application.
    Especially useful when using the shared stub version (DLL/SO).

    @since This function was added in version 1.2.0.
*/
ONESDK_DECLARE_FUNCTION(void) onesdk_stub_get_version(onesdk_stub_version_t* out_stub_version);

/*========================================================================================================================================*/

/** @brief Retrieves a human readable error message corresponding to an error code that was returned by the SDK stub.
    @param error_code       An error code returned by an SDK stub function.
    @param[out] buffer      Pointer to a buffer into which the error message shall be copied.
    @param buffer_length    The length of the buffer pointed to by @p buffer in @ref onesdk_xchar_t characters.

    @return @p buffer

    If @p buffer_length is at least one, this function will make sure that the string copied to @p buffer is always null-terminated.
    If the provided buffer is too small, the error message will be truncated.
*/
ONESDK_DECLARE_FUNCTION(onesdk_xchar_t const*) onesdk_stub_xstrerror(onesdk_result_t error_code, onesdk_xchar_t* buffer, onesdk_size_t buffer_length);

/** @brief Determines whether a command line argument is an SDK argument.
    @param arg      A command line argument.

    @return A non-zero value if @p arg starts with `--dt_`, zero otherwise.
*/
ONESDK_DECLARE_FUNCTION(onesdk_bool_t) onesdk_stub_is_sdk_cmdline_arg(onesdk_xchar_t const* arg);

/** @brief Processes one command line argument.
    @param arg                  A command line argument.
    @param replace_existing     Tells the function whether it should overwrite an existing value (non-zero) or not (zero).

    @return @ref ONESDK_SUCCESS if successful, an SDK stub error code otherwise.

    @return
    - If @p arg is an SDK command line argument with invalid format (e.g. not in the form `key=value`) this function will return
    @ref ONESDK_ERROR_INVALID_ARGUMENT.
    - If @p arg is an SDK command line argument with a key for which a value has already been set and @p replace_existing is zero this
    function will return @ref ONESDK_ERROR_ENTRY_ALREADY_EXISTS.

    If @p arg is an SDK command line argument, this function will store the `key=value` pair defined by that argument.
    If called with @p arg set to `NULL` or a string that isn't an SDK command line arguments this function will do nothing and return
    @ref ONESDK_SUCCESS.
*/
ONESDK_DECLARE_FUNCTION(onesdk_result_t) onesdk_stub_process_cmdline_arg(onesdk_xchar_t const* arg, onesdk_bool_t replace_existing);

/** @brief Processes multiple command line arguments.
    @param argc                 The number of entries in @p argv.
    @param argv                 Points to an array of pointers to command line argument strings.
    @param replace_existing     Tells the function whether it should overwrite existing values (non-zero) or not (zero).

    @return @ref ONESDK_SUCCESS if successful, an SDK error code otherwise.

    @return
    - If one of the processed strings is an SDK command line argument with invalid format this function will return
      @ref ONESDK_ERROR_INVALID_ARGUMENT.
    - If one of the processed strings was an SDK command line argument with a key for which a value has already been set,
      @p replace_existing is zero and no other error occurred, this function will return @ref ONESDK_ERROR_ENTRY_ALREADY_EXISTS.

    This function will process the arguments `argv[1]` ... `argv[argc - 1]` as if by calling `onesdk_stub_process_cmdline_arg(argv[i])`.
    `argv[0]` is ignored. That means it's possible to call it with the `argc` and `argv` arguments of a typical C `main` function.
    Neither @p argc or @p argv will be modified.

    @see @ref onesdk_stub_process_cmdline_arg
    @see @ref onesdk_stub_strip_sdk_cmdline_args
*/
ONESDK_DECLARE_FUNCTION(onesdk_result_t) onesdk_stub_process_cmdline_args(int argc, onesdk_xchar_t const* const* argv, onesdk_bool_t replace_existing);

/** @brief Removes SDK command line arguments from an `argv` array.
    @param[in,out] argc     Points to an `int` which specifies the number of entries in @p argv.
    @param[in,out] argv     Points to an array of pointers to command line argument strings.

    @return @ref ONESDK_SUCCESS if successful, an SDK stub error code otherwise.

    This function modifies the array pointed to by @p argv by removing entries that are SDK command line arguments. `argv[0]` is ignored.
    The value pointed to by @p argc is then updated to reflect the new array size.
*/
ONESDK_DECLARE_FUNCTION(onesdk_result_t) onesdk_stub_strip_sdk_cmdline_args(int* argc, onesdk_xchar_t** argv);

/** @brief Sets an SDK initialization variable.
    @param var                  A string with an SDK initialization variable definition in the form `key=value`.
    @param replace_existing     Tells the function whether it should overwrite an existing value (non-zero) or not (zero).

    @return @ref ONESDK_SUCCESS if successful, an SDK stub error code otherwise.

    @return
    - If @p var is `NULL` or points to an empty string this function will do nothing and return @ref ONESDK_SUCCESS.
    - If @p var has an invalid format this function will return @ref ONESDK_ERROR_INVALID_ARGUMENT.
    - If @p var defines a variable with a key for which a value has already been set and @p replace_existing is zero this function will
      return @ref ONESDK_ERROR_ENTRY_ALREADY_EXISTS.
*/
ONESDK_DECLARE_FUNCTION(onesdk_result_t) onesdk_stub_set_variable(onesdk_xchar_t const* var, onesdk_bool_t replace_existing);

/** @brief Clears all SDK initialization variables.

    This function clears all SDK initialization variables and releases the memory used to store them.
*/
ONESDK_DECLARE_FUNCTION(void) onesdk_stub_free_variables(void);

/*========================================================================================================================================*/

/** @brief SDK stub logging function prototype.
    @param level        The logging level of the message. See @ref logging_level_constants.
    @param message      The undecorated log message.

    @note Messages which do not pass the logging level test (see @ref onesdk_stub_set_logging_level) will not be forwarded to the logging
          function.

    @note The message string will not contain any decoration as described in @ref onesdk_stub_default_logging_function.
*/
typedef void ONESDK_CALL onesdk_stub_logging_callback_t(onesdk_logging_level_t level, onesdk_xchar_t const* message);

/** @brief The default SDK stub logging function - writes to `stderr`.
    @param level        See @ref onesdk_stub_logging_callback_t.
    @param message      See @ref onesdk_stub_logging_callback_t.

    This function will decorate the log message with the following items:
    - The current date and time (UTC)
    - An identifier for the current thread
    - The string representation of the message's logging level
    - The string "[onesdk]" to mark the log output as coming from the SDK stub
    - A trailing newline character

    The resulting string will the be written to `stderr`.

    An application can
    - Set this function using @ref onesdk_stub_set_logging_callback to restore the default or
    - Call this function from its own logging function (e.g. to filter log messages or forward them to some other place in addition to
    writing them to `stderr`).

    @see @ref onesdk_stub_set_logging_callback
*/
ONESDK_DECLARE_FUNCTION(void) onesdk_stub_default_logging_function(onesdk_logging_level_t level, onesdk_xchar_t const* message);

/** @brief Sets the SDK stub logging level.
    @param level        The new SDK stub logging level.

    The default SDK stub logging level is @ref ONESDK_LOGGING_LEVEL_NONE which means no messages will be logged.
    Messages with a level < the current SDK stub logging level will be suppressed (=not sent to the logging function).

    @note This only affects log messages generated in the SDK stub itself. The actual agent uses a different logging mechanism.

    @see @ref logging_level_constants
    @see @ref onesdk_stub_set_logging_callback
*/
ONESDK_DECLARE_FUNCTION(void) onesdk_stub_set_logging_level(onesdk_logging_level_t level);

/** @brief Sets the SDK stub logging callback function.
    @param stub_logging_callback     The new SDK stub logging callback function.

    The stub logging callback function is used to log messages while initializing (locating and loading the agent) and shutting down the
    SDK (unloading the agent).

    The default logging function is @ref onesdk_stub_default_logging_function.

    An application can use this function to change how SDK stub log messages are formatted and where they're written to.

    If @p stub_logging_callback is set to `NULL`, all log messages will be discarded.

    @see @ref onesdk_stub_logging_callback_t
    @see @ref onesdk_stub_default_logging_function
    @see @ref onesdk_agent_set_logging_callback
*/
ONESDK_DECLARE_FUNCTION(void) onesdk_stub_set_logging_callback(onesdk_stub_logging_callback_t* stub_logging_callback);

/*========================================================================================================================================*/

/** @brief Load and initialize the SDK agent.

    @return @ref ONESDK_SUCCESS if successful, an SDK stub error code otherwise.

    This function tries to locate, load and initialize the SDK agent. See @ref onesdk_shutdown for shutting down and unloading
    the agent. If this function is called after the agent has already been initialized, an internal reference count will be incremented.
    In that case the application must call @ref onesdk_shutdown once for each successful call to @ref onesdk_initialize.

    This function behaves just as `onesdk_initialize_2(0)`.

    @note If your application forks while executing @ref onesdk_initialize, @ref onesdk_initialize_2 or @ref onesdk_shutdown on another
          thread, it is not safe to call SDK functions in the forked child process without calling `exec` first. Doing so may result in a
          deadlock.

    @see @ref onesdk_initialize_2
    @see @ref onesdk_stub_get_agent_load_info can give more information about initialization failures.
*/
ONESDK_DECLARE_FUNCTION(onesdk_result_t) onesdk_initialize(void);

/** @brief Load and initialize the SDK agent, with additional flags.
    @param init_flags   A bitwise combination of flags from @ref init_flags or zero.

    @return @ref ONESDK_SUCCESS if successful, an SDK stub error code otherwise.

    This function tries to locate, load and initialize the SDK agent. See @ref onesdk_shutdown for shutting down and unloading
    the agent. If this function is called after the agent has already been initialized, an internal reference count will be incremented.
    In that case the application must call @ref onesdk_shutdown once for each successful call to @ref onesdk_initialize.

    @note If your application forks while executing @ref onesdk_initialize, @ref onesdk_initialize_2 or @ref onesdk_shutdown on another
          thread, it is not safe to call SDK functions in the forked child process without calling `exec` first. Doing so may result in a
          deadlock. (This restriction also applies when using @ref ONESDK_INIT_FLAG_FORKABLE.)

    @since This function was added in version 1.3.0.

    @see @ref init_flags
    @see @ref onesdk_initialize
    @see @ref onesdk_stub_get_agent_load_info can give more information about initialization failures.
*/
ONESDK_DECLARE_FUNCTION(onesdk_result_t) onesdk_initialize_2(onesdk_uint32_t init_flags);

/** @brief Shut down and unload the SDK agent.

    @return @ref ONESDK_SUCCESS if successful, an SDK stub error code otherwise.

    This function will shut down and unload the SDK agent.

    @note If your application forks while executing @ref onesdk_initialize, @ref onesdk_initialize_2 or @ref onesdk_shutdown on another
          thread, it is not safe to call SDK functions in the forked child process without calling `exec` first. Doing so may result in a
          deadlock.
    @note Unloading the actual SDK agent module (DLL/SO/...) may not be possible on all platforms.
*/
ONESDK_DECLARE_FUNCTION(onesdk_result_t) onesdk_shutdown(void);

/*========================================================================================================================================*/

/** @brief Retrieves debug information about the currently used agent.
    @param[out] agent_found      [optional] Pointer to a bool indicating whether the agent was found or not.
    @param[out] agent_compatible [optional] Pointer to a bool indicating whether the agent was both found and compatible or not.

    This function only yields meaningful results between calling @ref onesdk_initialize (or @ref onesdk_initialize_2) and
    @ref onesdk_shutdown. It is useful to get more information about why initialization failed. Sucessful initialization always implies
    that both @p *agent_found and @p *agent_compatible are true.

    Note that @p *agent_found is also false if initialization failed before even trying to find the agent and
    @p *agent_compatible is also false if the agent was found but its compatibility could not be checked.
    @see @ref onesdk_agent_get_version_string may have the version of the agent, which is interesting if
         @p *agent_found is true but @p *agent_compatible is not.

    @since This function was added in version 1.3.0.
*/
ONESDK_DECLARE_FUNCTION(void) onesdk_stub_get_agent_load_info(onesdk_bool_t* agent_found, onesdk_bool_t* agent_compatible);

/*========================================================================================================================================*/

/** @} */

/*========================================================================================================================================*/

#endif /* ONESDK_INIT_H_INCLUDED */
