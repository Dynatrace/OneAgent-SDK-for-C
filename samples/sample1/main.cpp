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

#include "http_request.h"
#include "http_response.h"
#include "web_client.h"

#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <random>
#include <stdio.h>


#include "onesdk/onesdk.h"
#include <cassert>

#if defined(_WIN32)
#include <wchar.h>
#endif

#if defined(unix) || defined(__unix__) || defined(__unix)
#include <unistd.h>
#include <signal.h>
#include <strings.h>
#if _XOPEN_SOURCE >= 500 || _XOPEN_SOURCE && _XOPEN_SOURCE_EXTENDED
#define SAMPLE1_HAVE_FORK_FUNCTIONS
#endif
#endif

/*========================================================================================================================================*/

void run_main_loop(bool use_fork);
void handle_request(std::string const& input);
void perform_cleanup(message_queue& queue, onesdk_messagingsysteminfo_handle_t queue_info);
void poll_process_messages(message_queue& queue, onesdk_messagingsysteminfo_handle_t queue_info);
void on_billing_message(message_queue::queue_message& msg, onesdk_messagingsysteminfo_handle_t queue_info);
char const* agent_state_to_string(onesdk_int32_t agent_state);
void ONESDK_CALL onesdk_agent_warning_callback(char const* message);
void ONESDK_CALL onesdk_agent_verbose_callback(char const* message);
onesdk_result_t checkresult(onesdk_result_t r, char const* message);

/*========================================================================================================================================*/

#if defined(_WIN32)
int wmain(int argc, wchar_t** argv)
#else
int main(int argc, char** argv)
#endif
{
#if defined(ONESDK_SHARED)
    printf("ONESDK stub: shared library\n");
#else
    printf("ONESDK stub: static library\n");
#endif

    // Process and strip out ONESDK command line arguments.
    checkresult(onesdk_stub_process_cmdline_args(argc, argv, true), "stub_process_cmdline_args");
    checkresult(onesdk_stub_strip_sdk_cmdline_args(&argc, argv), "stub_strip_sdk_cmdline_args");

    bool use_fork = false;
    // Process command line arguments.
    // (Since we called onesdk_stub_strip_sdk_cmdline_args we won't see ONESDK arguments here anymore.)
    for (int i = 0; i < argc; i++) {
#if defined(_WIN32)
        printf("argv[%d]: '%S'\n", i, argv[i]);
        if (wcscmp(argv[i], L"--fork") == 0)
            use_fork = true;
#else
        printf("argv[%d]: '%s'\n", i, argv[i]);
        if (strcasecmp(argv[i], "--fork") == 0)
            use_fork = true;
#endif
    }

    uint32_t onesdk_init_flags = 0;
    if (use_fork) {
#if defined(SAMPLE1_HAVE_FORK_FUNCTIONS)
        puts("Using per-request forked child processes.\n");
        sigignore(SIGCHLD);
        onesdk_init_flags |= ONESDK_INIT_FLAG_FORKABLE;
#else
        fputs("WARNING: --fork is not supported.\n", stderr);
        use_fork = false;
#endif
    }

    // Try to initialize the OneAgent SDK for C/C++.
    onesdk_result_t const onesdk_init_result = onesdk_initialize_2(onesdk_init_flags);
    printf("ONESDK initialized:   %s\n", (onesdk_init_result == ONESDK_SUCCESS) ? "yes" : "no");
    checkresult(onesdk_init_result, "  initialize");

    // Set logging callbacks (as soon after initialize as possible) so we get info/warning/error messages from the agent.
    checkresult(onesdk_agent_set_warning_callback(&onesdk_agent_warning_callback), "agent_set_warning_callback");
    checkresult(onesdk_agent_set_verbose_callback(&onesdk_agent_verbose_callback), "agent_set_verbose_callback");

    printf("ONESDK agent version: '%" ONESDK_STR_PRI_XSTR "'\n", onesdk_agent_get_version_string());
    onesdk_bool_t agent_found, agent_compatible;
    onesdk_stub_get_agent_load_info(&agent_found, &agent_compatible);
    printf("ONESDK agent load info:\n");
    printf("    agent was found: %s\n", agent_found ? "yes" : "no");
    printf("    agent is compatible: %s\n", agent_compatible ? "yes" : "no");


    // Run the main service loop.
    run_main_loop(use_fork);

    // Shut down ONESDK.
    if (onesdk_init_result == ONESDK_SUCCESS)
        checkresult(onesdk_shutdown(), "shutdown");

    return 0;
}

/*========================================================================================================================================*/

void run_main_loop(bool use_fork) {
    message_queue& queue = connect_queue(BillingQueueName);
    onesdk_messagingsysteminfo_handle_t const messagingsysteminfo_handle = onesdk_messagingsysteminfo_create(
        onesdk_asciistr("sample1_inprocess_messaging"), // vendor name
        onesdk_asciistr(BillingQueueName),              // destination name
        ONESDK_MESSAGING_DESTINATION_TYPE_QUEUE,        // destination type
        ONESDK_CHANNEL_TYPE_IN_PROCESS,                 // channel type
        onesdk_nullstr());                              // channel endpoint
    try {
        std::cout <<
            "\n"
            "Enter request body data or command.\n"
            "Hint 1: '!' is an invalid input character and will cause the service to fail.\n"
            "Hint 2: Otherwise, the message text will be uppercased, and a message will be added to the billing queue.\n"
            "Hint 3: A message of length zero will cause a failure when the cleanup command processes the corresponding message.\n"
            "Available commands:\n"
            "    cleanup: perform cleanup\n"
            "    exit:    stop and exit the application\n"
            "\n";

        while (true) {
            // Read input.
            std::string input;
            getline(std::cin, input);

            if (input == "exit")
                break;

            if (input == "cleanup") {
                perform_cleanup(queue, messagingsysteminfo_handle);
            } else if (use_fork) {
#if defined(SAMPLE1_HAVE_FORK_FUNCTIONS)
                pid_t const child_pid = fork();
                if (child_pid == -1) {
                    int const ec = errno;
                    fprintf(stderr, "ERROR: Forking request handler process failed (error %d).\n", ec);
                } else if (child_pid == 0) {
                    // Handle request in child process.
                    handle_request(input);
                    return;
                } else {
                    // Child process was forked successfully.
                }
#endif
            } else {
                // Handle request in main process.
                handle_request(input);
            }
        }

        std::cout << "Shutting down...\n";
    } catch (...) {
        onesdk_messagingsysteminfo_delete(messagingsysteminfo_handle);
        throw;
    }
    onesdk_messagingsysteminfo_delete(messagingsysteminfo_handle);
}

void handle_request(std::string const& input) {
    http_request request;
    request.method = "POST";
    request.url = "http://example.com/sample1/web-service/transform";
    request.headers.emplace_back(std::make_pair("Host", "example.com"));
    request.headers.emplace_back(std::make_pair("Connection", "close"));
    request.headers.emplace_back(std::make_pair("Content-Type", "text/html; charset=utf-8"));
    request.headers.emplace_back(std::make_pair("Pragma", "no-cache"));
    // We don't have parameters (we're not using application/x-www-form-urlencoded) -> leave request.parameters empty.
    request.body = input;

    // Let our "web service" process the request.
    web_client client;
    http_response response = client.send_request(request);

    // Just print the response body.
    std::cout << "Response body: " << response.body << "\n";
}

void perform_cleanup(message_queue& queue, onesdk_messagingsysteminfo_handle_t queue_info) {
    // perform_cleanup simulates an expensive method that takes a long time. It could run
    // on its own thread, executed periodically, or, as in this example, be executed
    // manually (if it was triggered by another service, a remote call or in-process
    // link tracer is a better fit).
    onesdk_tracer_handle_t const tracer = onesdk_customservicetracer_create(
        onesdk_asciistr("perform_cleanup"), onesdk_asciistr("CleanupService"));

    try {
        onesdk_tracer_start(tracer);
        std::cout << "Performing cleanup...\n";
        poll_process_messages(queue, queue_info);
        std::cout << "Cleanup completed.\n";
    } catch (std::exception const& e) {
        // Set error information on our tracer.
        onesdk_tracer_error(tracer, onesdk_asciistr("std::exception"), onesdk_asciistr(e.what()));
        std::cerr << "Cleanup failed: " << e.what() << "\n";
    } catch (...) {
        // Set error information on our tracer.
        onesdk_tracer_error(tracer, onesdk_asciistr("unknown exception"), onesdk_asciistr("unknown error"));
        std::cerr << "Cleanup failed!\n";
    }
    onesdk_tracer_end(tracer);
}

void poll_process_messages(message_queue& queue, onesdk_messagingsysteminfo_handle_t queue_info) {
    // Use one IncomingMessageReceiveTracer per (potential) message.
    onesdk_tracer_handle_t const tracer = onesdk_incomingmessagereceivetracer_create(queue_info);
    try {
        // Start tracer (starts time measurement).
        onesdk_tracer_start(tracer);
        // For messaging, it is allowed to start and end (one after the other)
        // multiple process tracers in a single receive tracer ("bulk receive").
        for (;;) {
            message_queue::queue_message msg = queue.poll_receive_one();
            if (msg.message_id.empty())
                break;
            // If you use both an IncomingMessageProcessTracer and an IncomingMessageReceiveTracer, you should start and end tracing
            // the message processing while the corresponding receive tracer is started.
            on_billing_message(msg, queue_info);
        }
    } catch (std::exception const& e) {
        // Set error information and end tracer.
        onesdk_tracer_error(tracer, onesdk_asciistr("std::exception"), onesdk_asciistr(e.what()));
        onesdk_tracer_end(tracer);
        throw;
    } catch (...) {
        // Set error information and end tracer.
        onesdk_tracer_error(tracer, onesdk_asciistr("unknown exception"), onesdk_asciistr("unknown error"));
        onesdk_tracer_end(tracer);
        throw;
    }
    onesdk_tracer_end(tracer);
}

void on_billing_message(message_queue::queue_message& msg, onesdk_messagingsysteminfo_handle_t queue_info) {
    onesdk_tracer_handle_t const tracer = onesdk_incomingmessageprocesstracer_create(queue_info);
    try {
        auto const tag_iterator = msg.headers.find(ONESDK_DYNATRACE_MESSAGE_PROPERTY_NAME);
        if (tag_iterator != msg.headers.end()) {
            // The tag must be set before starting the tracer.
            onesdk_tracer_set_incoming_dynatrace_string_tag(tracer, onesdk_asciistr(tag_iterator->second.c_str()));
        }
        // Start tracer (starts time measurement).
        onesdk_tracer_start(tracer);
        onesdk_incomingmessageprocesstracer_set_vendor_message_id(tracer, onesdk_asciistr(msg.message_id.c_str()));

        auto const cost = msg.payload.changed_chars * 2 + msg.payload.total_chars;
        std::cout << "Paying up the " << cost << " cents for you...\n";
        if (cost <= 0)
            throw std::runtime_error("Cannot pay zero cents.");
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        msg.mark_processed();
    } catch (std::exception const& e) {
        // Set error information and end tracer.
        onesdk_tracer_error(tracer, onesdk_asciistr("std::exception"), onesdk_asciistr(e.what()));
        onesdk_tracer_end(tracer);
        throw;
    } catch (...) {
        // Set error information and end tracer.
        onesdk_tracer_error(tracer, onesdk_asciistr("unknown exception"), onesdk_asciistr("unknown error"));
        onesdk_tracer_end(tracer);
        throw;
    }

    // End tracer (stops time measurement and deletes/releases tracer).
    onesdk_tracer_end(tracer);
}

/*========================================================================================================================================*/

char const* agent_state_to_string(onesdk_int32_t agent_state) {
    switch (agent_state) {
    case ONESDK_AGENT_STATE_ACTIVE:
        return "active";
    case ONESDK_AGENT_STATE_TEMPORARILY_INACTIVE:
        return "temporarily_inactive";
    case ONESDK_AGENT_STATE_PERMANENTLY_INACTIVE:
        return "permanently_inactive";
    case ONESDK_AGENT_STATE_NOT_INITIALIZED:
        return "not_initialized";
    case ONESDK_AGENT_STATE_ERROR:
        return "error";

    default:
        return "(unknown state value)";
    }
}

/*========================================================================================================================================*/

void ONESDK_CALL onesdk_agent_warning_callback(char const* message) {
    printf("ONESDK log message: %s\n", message);
}

void ONESDK_CALL onesdk_agent_verbose_callback(char const* message) {
    printf("ONESDK log message (verbose): %s\n", message);
}


onesdk_result_t checkresult(onesdk_result_t r, char const* message) {
    if (r != ONESDK_SUCCESS) {
        static const size_t message_buffer_len = 2048;
        onesdk_xchar_t message_buffer[message_buffer_len];
        fprintf(stderr, "%s: %" ONESDK_STR_PRI_XSTR "\n",
            message, onesdk_stub_xstrerror(r, message_buffer, message_buffer_len));
    }
    return r;
}

/*========================================================================================================================================*/
