/*
    Copyright 2018 Dynatrace LLC

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

#ifndef SAMPLE1_HTTP_RESPONSE_H_INCLUDED
#define SAMPLE1_HTTP_RESPONSE_H_INCLUDED

#include <string>
#include <vector>
#include <utility>

/*========================================================================================================================================*/

struct http_response {
    using name_value_pair = std::pair<std::string, std::string>;

    std::vector<name_value_pair> headers;
    std::string body;
    int status_code = 200;
};

/*========================================================================================================================================*/

#endif