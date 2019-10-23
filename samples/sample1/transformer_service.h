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

#ifndef SAMPLE1_TRANSFORMER_SERVICE_H_INCLUDED
#define SAMPLE1_TRANSFORMER_SERVICE_H_INCLUDED

#include "billing_queue.h"

#include <cctype>
#include <chrono>
#include <stdexcept>
#include <string>
#include <stddef.h>

#include "onesdk/onesdk.h"


/*========================================================================================================================================*/

class transformer_service {
public:
    transformer_service(transformer_service const&) = delete; // We're non-copyable.
    transformer_service& operator =(transformer_service const&) = delete; // We're non-copyable.

    transformer_service()
        : m_messagingsysteminfo_handle(onesdk_messagingsysteminfo_create(
            onesdk_asciistr("sample1_inprocess_messaging"), // vendor name
            onesdk_asciistr(BillingQueueName),                     // destination name
            ONESDK_MESSAGING_DESTINATION_TYPE_QUEUE,        // destination type
            ONESDK_CHANNEL_TYPE_IN_PROCESS,                 // channel type
            onesdk_nullstr()))                              // channel endpoint
        , m_msg_queue(connect_queue(BillingQueueName))
    {
    }

    ~transformer_service() {
        onesdk_messagingsysteminfo_delete(m_messagingsysteminfo_handle);
    }

    std::string transform(std::string str) {
        // Simulate processing time (busy CPU time).
        auto const duration = std::chrono::milliseconds(str.size());
        using clock = std::chrono::steady_clock;
        auto const end_time = clock::now() + duration;
        while (clock::now() < end_time) {
            // Just spin.
        }

        size_t changed_count = 0;

        for (std::string::size_type i = 0; i < str.size(); i++) {
            char const ch = str[i];
            // Let's say '!' is an invalid input character so we can simulate processing errors.
            if (ch == '!')
                throw std::invalid_argument(std::string("Invalid input character '") + ch + "'");
            str[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));

            if (str[i] != ch)
                changed_count++;
        }

        // Add custom request attributes.
        onesdk_customrequestattribute_add_integer(onesdk_asciistr("transformer_service.total_count"), static_cast<onesdk_int64_t>(str.size()));
        onesdk_customrequestattribute_add_integer(onesdk_asciistr("transformer_service.changed_count"), static_cast<onesdk_int64_t>(changed_count));

        // Send a message to our queue about the request, for billing purposes.
        onesdk_tracer_handle_t const tracer = onesdk_outgoingmessagetracer_create(m_messagingsysteminfo_handle);
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


            message_queue::queue_message msg;
            msg.headers[ONESDK_DYNATRACE_MESSAGE_PROPERTY_NAME] = std::move(tag);
            msg.payload.changed_chars = static_cast<unsigned>(changed_count);
            msg.payload.total_chars = static_cast<unsigned>(str.size());
            m_msg_queue.send(msg);

            onesdk_outgoingmessagetracer_set_vendor_message_id(tracer, onesdk_asciistr(msg.message_id.c_str()));

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

        return str;
    }

private:
    onesdk_messagingsysteminfo_handle_t m_messagingsysteminfo_handle;
    message_queue& m_msg_queue;
};

/*========================================================================================================================================*/

#endif
