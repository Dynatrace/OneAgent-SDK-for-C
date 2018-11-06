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

#include "http_request.h"
#include "http_response.h"
#include "web_client.h"

#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <stdio.h>

#include "onesdk/onesdk.h"

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
char const* agent_state_to_string(onesdk_int32_t agent_state);
void ONESDK_CALL onesdk_agent_logging_callback(char const* message);

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
    onesdk_stub_process_cmdline_args(argc, argv, true);
    onesdk_stub_strip_sdk_cmdline_args(&argc, argv);

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

    // Try to initialize ONESDK.
    onesdk_result_t const onesdk_init_result = onesdk_initialize_2(onesdk_init_flags);
    printf("ONESDK initialized:   %s\n", (onesdk_init_result == ONESDK_SUCCESS) ? "yes" : "no");
    printf("ONESDK agent version: '%" ONESDK_STR_PRI_XSTR "'\n", onesdk_agent_get_version_string());
    printf("ONESDK agent state:   %s\n", agent_state_to_string(onesdk_agent_get_current_state()));
    // Set logging callback so we get warning/error messages from ONESDK.
    onesdk_agent_set_logging_callback(&onesdk_agent_logging_callback);

    // Run the main service loop.
	run_main_loop(use_fork);

    // Shut down ONESDK.
    if (onesdk_init_result == ONESDK_SUCCESS)
        onesdk_shutdown();

    return 0;
}

/*========================================================================================================================================*/

void run_main_loop(bool use_fork) {
    std::cout <<
        "\n"
        "Enter request body data or \"exit\" to stop.\n"
        "Hint: '!' is an invalid input character and will cause the service to fail.\n"
        "\n";

    while (true) {
        // Read input.
        std::string input;
        getline(std::cin, input);

        if (input == "exit")
            break;

        if (use_fork) {
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

void ONESDK_CALL onesdk_agent_logging_callback(char const* message) {
    printf("ONESDK log message: %s\n", message);
}

/*========================================================================================================================================*/
