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

#include <cctype>
#include <chrono>
#include <stdexcept>
#include <string>
#include <stddef.h>

#include "onesdk/onesdk_agent.h"
#include "onesdk/onesdk_string.h"

/*========================================================================================================================================*/

class transformer_service {
public:
    transformer_service(transformer_service const&) = delete; // We're non-copyable.
    transformer_service& operator =(transformer_service const&) = delete; // We're non-copyable.

    transformer_service() = default;

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

        return str;
    }
};

/*========================================================================================================================================*/

#endif
