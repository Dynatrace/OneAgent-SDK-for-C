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

#ifndef SAMPLE1_WEB_SERVICE_IMPL_H_INCLUDED
#define SAMPLE1_WEB_SERVICE_IMPL_H_INCLUDED

#include "config_database.h"
#include "transformer_service_client_proxy.h"

#include <exception>
#include <future>
#include <string>
#include <vector>

#include "onesdk/onesdk_agent.h"
#include "onesdk/onesdk_string.h"

/*========================================================================================================================================*/

class web_service_impl {
public:
    web_service_impl(web_service_impl const&) = delete; // We're non-copyable.
    web_service_impl& operator =(web_service_impl const&) = delete; // We're non-copyable.

    web_service_impl() = default;

    std::string get_response_body(std::string const& request_body) {
        // Get in-process link so we can continue tracing in another thread.
		// Note that, just like when creating new tracers, we don't need a parent tracer handle for this. The retrieved link will
		// automatically connect to the innermost active tracer of the current thread.
		// (Or, if there is no active tracer on the current thread, we'll get an empty link.)
        std::vector<unsigned char> in_process_link;
        {
			onesdk_size_t required_buffer_size = 0;
            // Call once to get required buffer size.
            onesdk_inprocesslink_create(nullptr, 0, &required_buffer_size);
            // Call again to fetch data.
            in_process_link.resize(required_buffer_size);
            onesdk_size_t const link_size = onesdk_inprocesslink_create(in_process_link.data(), in_process_link.size(), nullptr);
            // Use return value to resize vector.
            in_process_link.resize(link_size);
        }

        // Start database request in another thread.
        std::future<std::string> output_prefix_future = std::async(std::launch::async, [this, in_process_link]() -> std::string {
            std::string result;

            // Create in-process link tracer so we can continue tracing in this thread.
            onesdk_tracer_handle_t const tracer = onesdk_inprocesslinktracer_create(in_process_link.data(), in_process_link.size());

            try {
                // Start tracer ("activates" the in-process link).
                onesdk_tracer_start(tracer);

                result = m_config_database.get_output_prefix();

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

            // End tracer ("deactivates" the in-process link and deletes/releases the tracer).
            onesdk_tracer_end(tracer);

            return result;
        });

        // Call transformer service in this thread (in parallel to the database request).
        std::string const transformed_data = m_transformer_service.transform(request_body);

        // Wait for database request, combine and return result.
        return output_prefix_future.get() + ":" + transformed_data;
    }

private:
    config_database m_config_database;
    transformer_service_client_proxy m_transformer_service;
};

/*========================================================================================================================================*/

#endif
