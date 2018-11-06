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

#ifndef SAMPLE1_CONFIG_DATABASE_H_INCLUDED
#define SAMPLE1_CONFIG_DATABASE_H_INCLUDED

#include "util.h"

#include <chrono>
#include <exception>
#include <string>
#include <thread>

#include "onesdk/onesdk_agent.h"
#include "onesdk/onesdk_string.h"

/*========================================================================================================================================*/

class config_database {
public:
    config_database(config_database const&) = delete; // We're non-copyable.
    config_database& operator =(config_database const&) = delete; // We're non-copyable.

    config_database() {
        // Create database info object.
        // Assume we're using an SQLite database called "sample1-config.db".
        m_db_info_handle = onesdk_databaseinfo_create(
            onesdk_asciistr("sample1-config.db"),   // database name
            onesdk_asciistr("sqlite"),              // database type
            ONESDK_CHANNEL_TYPE_IN_PROCESS,         // channel type
            onesdk_nullstr());                      // channel endpoint
    }

    ~config_database() {
        // Release the database info object.
        onesdk_databaseinfo_delete(m_db_info_handle);
    }

    std::string get_output_prefix() {
        return query_config_value("output_prefix");
    }

private:
    std::string query_config_value(std::string const& name) {
        return query_scalar("SELECT value FROM config_values WHERE name = '" + sql_escape(name) + "';");
    }

    std::string query_scalar(std::string const& sql) {
        std::string result;

        // Create tracer for the database request.
        onesdk_tracer_handle_t const tracer = onesdk_databaserequesttracer_create_sql(
            m_db_info_handle,                                           // database info handle
            onesdk_str(sql.c_str(), sql.size(), ONESDK_CCSID_ASCII));   // SQL statement

        try {
            // Start tracer (starts time measurement).
            onesdk_tracer_start(tracer);

            // In reality we would of course actually query a database here...
            std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Simulate time spent waiting for DB request.
            result = "sample";

            // Set number of rows returned by the query (we assume one row).
            onesdk_databaserequesttracer_set_returned_row_count(tracer, 1);
            // Set number of round trips between client and database (we assume one fetch operation).
            onesdk_databaserequesttracer_set_round_trip_count(tracer, 1);

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

    onesdk_databaseinfo_handle_t m_db_info_handle;
};

/*========================================================================================================================================*/

#endif
