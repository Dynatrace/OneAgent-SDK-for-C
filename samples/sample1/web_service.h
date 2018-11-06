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

#ifndef SAMPLE1_WEB_SERVICE_H_INCLUDED
#define SAMPLE1_WEB_SERVICE_H_INCLUDED

#include "http_request.h"
#include "http_response.h"
#include "web_service_impl.h"

#include <exception>
#include <string>
#include <utility>
#include <vector>

#include "onesdk/onesdk_agent.h"
#include "onesdk/onesdk_string.h"

/*========================================================================================================================================*/

class web_service {
public:
    web_service(web_service const&) = delete; // We're non-copyable.
    web_service& operator =(web_service const&) = delete; // We're non-copyable.

    web_service() {
        // Create a web application info object that describes our web service.
        m_web_application_info_handle = onesdk_webapplicationinfo_create(
            onesdk_asciistr("example.com"),             // web server name
            onesdk_asciistr("sample1.web_service"),     // unique application/service name
            onesdk_asciistr("/sample1/web-service/"));  // context root
    }

    ~web_service() {
        onesdk_webapplicationinfo_delete(m_web_application_info_handle);
    }

    http_response process(http_request const& request) {
        // Create a tracer for processing this web request.
        onesdk_tracer_handle_t const tracer = onesdk_incomingwebrequesttracer_create(m_web_application_info_handle,
            onesdk_utf8str(request.url.c_str()),
            onesdk_utf8str(request.method.c_str()));

        http_response response;

        try {
            // Feed additional information about the request into the tracer.
            onesdk_incomingwebrequesttracer_set_remote_address(tracer, onesdk_utf8str(request.remote_address.c_str()));
            for (auto&& header : request.headers)
                onesdk_incomingwebrequesttracer_add_request_header(tracer,
                    onesdk_utf8str(header.first.c_str()), onesdk_utf8str(header.second.c_str()));
            for (auto&& parameter : request.parameters)
                onesdk_incomingwebrequesttracer_add_parameter(tracer,
                    onesdk_utf8str(parameter.first.c_str()), onesdk_utf8str(parameter.second.c_str()));

            // Start tracer (starts time measurement).
            onesdk_tracer_start(tracer);

            // Process the request, build response.
            response.body = m_impl.get_response_body(request.body);
            response.headers.emplace_back(std::make_pair("Content-Type", "text/html; charset=utf-8"));
            response.headers.emplace_back(std::make_pair("Cache-Control", "no-cache"));
            response.status_code = 200;

        } catch (std::exception const& e) {
            // Set error information on our tracer.
            onesdk_tracer_error(tracer, onesdk_asciistr("std::exception"), onesdk_asciistr(e.what()));
            response.status_code = 500;
            response.body = "Internal server error.";
            response.headers.clear();
        } catch (...) {
            // Set error information on our tracer.
            onesdk_tracer_error(tracer, onesdk_asciistr("unknown exception"), onesdk_asciistr("unknown error"));
            response.status_code = 500;
            response.body = "Internal server error.";
            response.headers.clear();
        }

        // Feed information about the response into the tracer.
        for (auto&& header : response.headers)
            onesdk_incomingwebrequesttracer_add_response_header(tracer,
                onesdk_utf8str(header.first.c_str()), onesdk_utf8str(header.second.c_str()));

        onesdk_incomingwebrequesttracer_set_status_code(tracer, response.status_code);

        // End tracer (stops time measurement and deletes/releases tracer).
        onesdk_tracer_end(tracer);

        return response;
    }

private:
    web_service_impl m_impl;

    onesdk_webapplicationinfo_handle_t m_web_application_info_handle = ONESDK_INVALID_HANDLE;
};

/*========================================================================================================================================*/

#endif
