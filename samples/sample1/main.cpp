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

#include "web_service.h"

#include <string>
#include <iostream>
#include <stdio.h>

#include <onesdk/onesdk.h>

/*========================================================================================================================================*/

void run_service_loop();
char const* agent_state_to_string(onesdk_int32_t agent_state);

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
    // TODO: Process remaining arguments.

    // Try to initialize ONESDK.
    onesdk_result_t const onesdk_init_result = onesdk_initialize();
    printf("ONESDK initialized:   %s\n", (onesdk_init_result == ONESDK_SUCCESS) ? "yes" : "no");
    printf("ONESDK agent version: '%" ONESDK_STR_PRI_XSTR "'\n", onesdk_agent_get_version_string());
    printf("ONESDK agent state:   %s\n", agent_state_to_string(onesdk_agent_get_current_state()));

    // Run the main service loop.
    run_service_loop();

    // Shut down ONESDK.
    if (onesdk_init_result == ONESDK_SUCCESS)
        onesdk_shutdown();

    return 0;
}

/*========================================================================================================================================*/

void run_service_loop() {
    web_service svc;

    std::cout <<
        "\n"
        "Enter request body data or \"exit\" to stop.\n"
        "Hint: '!' is an invalid input character and will cause this service to fail.\n"
        "\n";

    while (true) {
        // Read input.
        std::string input;
        getline(std::cin, input);

        if (input == "exit")
            break;

        // We don't want this sample to have dependencies on third-party software and we don't want to blow it up by doing any
        // real network IO, HTTP parsing etc. -> just fake the input data for a web request.
        http_request request;
        request.remote_address = "127.0.0.1:12345";
        request.method = "GET";
        request.url = "/sample1/web-service/transform";
        request.headers.emplace_back(std::make_pair("Host", "example.com"));
        request.headers.emplace_back(std::make_pair("Connection", "close"));
        request.headers.emplace_back(std::make_pair("Content-Type", "text/html; charset=utf-8"));
        request.headers.emplace_back(std::make_pair("Pragma", "no-cache"));
        // GET requests have no parameters -> leave request.parameters empty
        request.body = input;

        // Let our "web service" process the request.
        http_response response = svc.process(request);

        // Just print the response body.
        std::cout << "Response body: " << response.body << "\n";
    }

    std::cout << "Shutting down...\n";
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
