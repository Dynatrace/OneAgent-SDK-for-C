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

#ifndef SAMPLE1_TRANSFORMER_SERVICE_DISPATCHER_H_INCLUDED
#define SAMPLE1_TRANSFORMER_SERVICE_DISPATCHER_H_INCLUDED

#include "transformer_service.h"

#include <exception>
#include <stdexcept>
#include <string>

#include "onesdk/onesdk_agent.h"
#include "onesdk/onesdk_string.h"

/*========================================================================================================================================*/

class transformer_service_dispatcher {
public:
    transformer_service_dispatcher(transformer_service_dispatcher const&) = delete; // We're non-copyable.
    transformer_service_dispatcher& operator =(transformer_service_dispatcher const&) = delete; // We're non-copyable.

    transformer_service_dispatcher() = default;

    std::string dispatch(std::string const& method_name, std::string const& arguments, std::string const& tag) {
        std::string result;

        // Create tracer for processing this incoming remote method invocation.
        onesdk_tracer_handle_t const tracer = onesdk_incomingremotecalltracer_create(
            onesdk_str(method_name.c_str(), method_name.size(), ONESDK_CCSID_ASCII),    // method name
            onesdk_asciistr("sample1.transformer_service"),                             // service name
            onesdk_asciistr("localhost/sample1/transformer_service"));                  // service endpoint

        try {
            // Set string representation of the incoming tag on the tracer.
            // (We need to do this before starting the tracer, otherwise the tag won't be picked up and this remote call will not be linked
            //  correctly.)
            onesdk_tracer_set_incoming_dynatrace_string_tag(tracer, onesdk_str(tag.c_str(), tag.size(), ONESDK_CCSID_ASCII));

            // Start tracer (starts time measurement)
            onesdk_tracer_start(tracer);

            // Dispatch to our service instance.
            if (method_name == "transform")
                result = m_service.transform(arguments);
            else
                throw std::runtime_error("unknown method '" + method_name + "'");

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

public:
    transformer_service m_service;
};

/*========================================================================================================================================*/

#endif
