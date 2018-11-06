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

#ifndef SAMPLE1_WEB_CLIENT_H_INCLUDED
#define SAMPLE1_WEB_CLIENT_H_INCLUDED

#include "http_request.h"
#include "http_response.h"
#include "web_service.h"
#include "util.h"

#include <chrono>
#include <exception>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "onesdk/onesdk_agent.h"
#include "onesdk/onesdk_string.h"

/*========================================================================================================================================*/

class web_client {
public:
    web_client(web_client const&) = delete; // We're non-copyable.
    web_client& operator =(web_client const&) = delete; // We're non-copyable.

    web_client() = default;

    http_response send_request(http_request const& request) {
        // Create a tracer for processing this web request.
        onesdk_tracer_handle_t const tracer = onesdk_outgoingwebrequesttracer_create(
            onesdk_utf8str(request.url.c_str()),
            onesdk_utf8str(request.method.c_str()));

        http_response response;

        try {
            // Feed additional information about the request into the tracer.
            for (auto&& header : request.headers)
                onesdk_outgoingwebrequesttracer_add_request_header(tracer,
                    onesdk_utf8str(header.first.c_str()), onesdk_utf8str(header.second.c_str()));

            // Start tracer (starts time measurement).
            onesdk_tracer_start(tracer);

            // Get the string representation of the outgoing tag from our tracer.
            // (Must be done after starting the tracer, otherwise the returned tag would be empty.)
            std::string tag;
            {
                // Call once to get required buffer size (including space for the terminating NULL character).
                onesdk_size_t required_buffer_size = 0;
                onesdk_tracer_get_outgoing_dynatrace_string_tag(tracer, nullptr, 0, &required_buffer_size);
                // Call again to fetch data.
                tag.resize(required_buffer_size);
                onesdk_size_t const tag_size = onesdk_tracer_get_outgoing_dynatrace_string_tag(tracer, &tag[0], tag.size(), nullptr);
                // Use return value to resize string (the return value does NOT include the terminating NULL character).
                tag.resize(tag_size);
            }

            // Add the Dynatrace tag header.
            http_request request_with_tag = request;
            if (!tag.empty())
                request_with_tag.headers.emplace_back(ONESDK_DYNATRACE_HTTP_HEADER_NAME, tag);

            // Send the request to the server.
            do_send_request(request_with_tag, response);

            // Feed information about the response into the tracer.
            for (auto&& header : response.headers)
                onesdk_outgoingwebrequesttracer_add_response_header(tracer,
                    onesdk_utf8str(header.first.c_str()), onesdk_utf8str(header.second.c_str()));

            onesdk_outgoingwebrequesttracer_set_status_code(tracer, response.status_code);
        } catch (std::exception const& e) {
            // Set error information on our tracer.
            onesdk_tracer_error(tracer, onesdk_asciistr("std::exception"), onesdk_asciistr(e.what()));
            response = http_response();
            response.status_code = -1;
        } catch (...) {
            // Set error information on our tracer.
            onesdk_tracer_error(tracer, onesdk_asciistr("unknown exception"), onesdk_asciistr("unknown error"));
            response = http_response();
            response.status_code = -1;
        }

        // End tracer (stops time measurement and deletes/releases tracer).
        onesdk_tracer_end(tracer);

        return response;
    }

private:
    void do_send_request(http_request const& request, http_response& response) {
        // Send the HTTP request to the server/service.
        //
        // Normally this would involve rather complicated code, but we're a sample and need to keep it simple, so we'll cheat.
        // We can't call the web-service code directly though, because that would mean executing the web-service in "our" thread, which 
        // again would mean that any tracers created by the web-service would automatically attach to "our" trace.
        // Which is not what we want - after all we want to show how to use tagging to connect traces.
        // => Spawn a new thread in which we call the web-service implementation.
        {
            std::thread th([&request, &response, this] {
                try {
                    std::this_thread::sleep_for(std::chrono::milliseconds(5)); // Simulate network communication delay.

                    // Only the path & query parts of the URL are sent to the server => strip the rest.
                    http_request server_side_request = request;
                    server_side_request.url = strip_url_for_http_request(request.url);
                    // The server has access to the "remote address" though (=the address on the other side of the TCP/IP connection)
                    // => set it
                    server_side_request.remote_address = "127.0.0.1:12345";

                    response = m_web_service.process(server_side_request);

                    std::this_thread::sleep_for(std::chrono::milliseconds(5)); // Simulate network communication delay.
                } catch (...) {
                    response = http_response();
                    response.status_code = -1;
                }
            });

            th.join();

            if (response.status_code < 0)
                throw std::runtime_error("could not send HTTP request");
        }
    }

    web_service m_web_service;
};

/*========================================================================================================================================*/

#endif
