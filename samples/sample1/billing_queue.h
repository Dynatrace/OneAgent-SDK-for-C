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

#ifndef SAMPLE1_BILLING_QUEUE_H_INCLUDED
#define SAMPLE1_BILLING_QUEUE_H_INCLUDED

#include <exception>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <mutex>
#include <condition_variable>
#include <queue>
#include "onesdk/onesdk.h"

/*========================================================================================================================================*/

struct billable_usage {
    unsigned changed_chars;
    unsigned total_chars;
};

class message_queue {
public:

    // For reliable messaging, as you would want for billing, some sort of
    // "processed" flag would be required that has to be set for the message
    // so that messages which were sucessfully received but where processing
    // failed can be redelivered.
    struct queue_message {
        std::unordered_map<std::string, std::string> headers;
        std::string message_id;
        billable_usage payload;

        void mark_processed() noexcept {} // Not implemented.
    };

    message_queue(message_queue const&) = delete; // We're non-copyable.
    message_queue operator =(message_queue const&) = delete; // We're non-copyable.

    explicit message_queue(std::string const& name) : m_name(name), m_size_metric(
        onesdk_integergaugemetric_create(
            onesdk_asciistr("message_queue.pending"),
            onesdk_asciistr("count"),
            onesdk_asciistr("queue_name")))
    {}

    void send(queue_message& message) {
        try {
            std::lock_guard<std::mutex> lock(m_queue_mut);
            message.message_id = std::to_string(m_queue.size());
            update_queue_metric();
            m_queue.push(message);
        } catch (...) {
            message.message_id.clear();
            throw;
        }
    }

    queue_message poll_receive_one() {
        std::lock_guard<std::mutex> lock(m_queue_mut);
        if (m_queue.empty())
            return {};
        auto result = std::move(m_queue.front());
        m_queue.pop();
        update_queue_metric();
        return result;
    }

private:
    void update_queue_metric() {
        onesdk_integergaugemetric_set_value(m_size_metric, m_queue.size(), onesdk_asciistr(m_name.c_str()));
    }

    std::string m_name;
    onesdk_metric_handle_t m_size_metric;
    std::mutex m_queue_mut;
    std::queue<queue_message> m_queue;
};

char const* const BillingQueueName = "billing-queue";

inline message_queue& connect_queue(std::string const& queue_name) {
    static std::unordered_map<std::string, std::unique_ptr<message_queue>> queues;
    static std::mutex queues_mut;
    std::lock_guard<std::mutex> lock(queues_mut);
    auto& result = queues[queue_name];
    if (!result)
        result.reset(new message_queue(queue_name));
    return *result;
}


/*========================================================================================================================================*/

#endif
