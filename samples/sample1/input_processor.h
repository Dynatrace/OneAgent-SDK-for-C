/*
    Copyright 2017 Dynatrace LLC

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

#ifndef SAMPLE1_INPUT_PROCESSOR_H_INCLUDED
#define SAMPLE1_INPUT_PROCESSOR_H_INCLUDED

#include "config_database.h"
#include "transformer_service_client_proxy.h"

#include <string>
#include <exception>

#include <onesdk/onesdk_agent.h>

/*========================================================================================================================================*/

class input_processor {
public:
    input_processor(input_processor const&) = delete; // We're non-copyable.
    input_processor& operator =(input_processor const&) = delete; // We're non-copyable.

    input_processor() {}

    std::string process(std::string const& data) {
        std::string result;

        // Create tracer for processing this chunk of input data.
        // We use an "incoming remote call" tracer to trace the execution of this method. When you think about it, it actually makes sense
        // to interpret processing a chunk of input data as a "service call". And since the data is coming from outside our application,
        // the "remote" part isn't inappropriate either.
        onesdk_tracer_handle_t const tracer = onesdk_incomingremotecalltracer_create(
            onesdk_asciistr("process"),                             // service method
            onesdk_asciistr("sample1.input_processor"),             // service name
            onesdk_asciistr("localhost/sample1/input_processor"));  // service endpoint

        try {
            // Start tracer (starts time measurement).
            onesdk_tracer_start(tracer);

            // Process the data.
            std::string const output_prefix = m_config_database.get_output_prefix();
            std::string const transformed_data = m_transformer_service.transform(data);

            result = output_prefix + ":" + transformed_data;

        } catch (std::exception const& e) {
            // Set error information on our tracer.
            onesdk_tracer_error(tracer, onesdk_asciistr("std::exception"), onesdk_asciistr(e.what()));
            result = "ERROR";
        } catch (...) {
            // Set error information on our tracer.
            onesdk_tracer_error(tracer, onesdk_asciistr("unknown exception"), onesdk_asciistr("unknown error"));
            result = "ERROR";
        }

        // End tracer (stops time measurement and deletes/releases tracer).
        onesdk_tracer_end(tracer);

        return result;
    }

private:
    config_database m_config_database;
    transformer_service_client_proxy m_transformer_service;
};

/*========================================================================================================================================*/

#endif
