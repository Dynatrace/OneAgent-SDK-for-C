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

#ifndef SAMPLE1_TRANSFORMER_SERVICE_CLIENT_PROXY_H_INCLUDED
#define SAMPLE1_TRANSFORMER_SERVICE_CLIENT_PROXY_H_INCLUDED

#include "transformer_service_dispatcher.h"

#include <chrono>
#include <exception>
#include <string>
#include <thread>

#include "onesdk/onesdk_agent.h"
#include "onesdk/onesdk_string.h"

/*========================================================================================================================================*/

class transformer_service_client_proxy {
public:
    transformer_service_client_proxy(transformer_service_client_proxy const&) = delete; // We're non-copyable.
    transformer_service_client_proxy& operator =(transformer_service_client_proxy const&) = delete; // We're non-copyable.

    transformer_service_client_proxy() = default;

    std::string transform(std::string const& str) {
        return invoke_remote_method("transform", str);
    }

private:
    std::string invoke_remote_method(std::string const& method_name, std::string const& arguments) {
        std::string result;

        // Create tracer for the outgoing remote call we're about to do.
        onesdk_tracer_handle_t const tracer = onesdk_outgoingremotecalltracer_create(
            onesdk_str(method_name.c_str(), method_name.size(), ONESDK_CCSID_ASCII),    // method name
            onesdk_asciistr("sample1.transformer_service"),                             // service name
            onesdk_asciistr("localhost/sample1/transformer_service"),                   // service endpoint
            ONESDK_CHANNEL_TYPE_IN_PROCESS,                                             // channel type
            onesdk_nullstr());                                                          // channel endpoint (optional)

        try {
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

            // Send the method invocation message to the remote service.
            //
            // Normally this would involve rather complicated code, but we're a sample and need to keep it simple, so we'll cheat.
            // We can't call the dispatcher directly though, because that would mean executing the service call in "our" thread, which 
            // again would mean that any tracers created by the service call would automatically attach to "our" trace.
            // Which is not what we want - after all we want to show how to use tagging to connect traces.
            // => Spawn a new thread in which we call the service implementation.
            {
                std::exception_ptr exception;
                std::thread th([&result, &exception, this, method_name, arguments, tag] {
                    try {
                        std::this_thread::sleep_for(std::chrono::milliseconds(5)); // Simulate network communication delay.

                        result = m_remote_dispatcher.dispatch(method_name, arguments, tag);

                        std::this_thread::sleep_for(std::chrono::milliseconds(5)); // Simulate network communication delay.
                    } catch (...) {
                        exception = std::current_exception();
                    }
                });
                
                th.join();

                if (exception)
                    std::rethrow_exception(exception);
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

        // End tracer (stops time measurement and deletes/releases tracer).
        onesdk_tracer_end(tracer);

        return result;
    }

    transformer_service_dispatcher m_remote_dispatcher;
};

/*========================================================================================================================================*/

#endif
