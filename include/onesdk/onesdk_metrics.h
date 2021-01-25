/*
    Copyright 2017-2019 Dynatrace LLC

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

#ifndef ONESDK_EX_METRICS_H_INCLUDED
#define ONESDK_EX_METRICS_H_INCLUDED

/** @file
    @brief Defines functions for metrics, see @ref ex_metrics.
    @since This header is available since version 1.5.
*/

/*========================================================================================================================================*/

#include "onesdk/onesdk_common.h" /* IWYU pragma: export */
#include "onesdk/onesdk_string.h"

/** @defgroup ex_metrics Metric API

    @brief APIs to report numeric metrics (aka time series data).

    @anchor ex_metric_detail

    The SDK supports two **metric value types**: integer (64 bit signed) and floating point (double precision).
    You should prefer integers as they are more efficient, unless the loss of precision is unacceptable (but
    consider using a different unit, e.g. integer microseconds instead of floating point seconds).

    There are these different **kinds of metrics**:
    
    - Counter: For all metrics that are counting something like sent/received bytes to/from network.
      Counters should only be used when tracking things in flow, as opposed to state. It reports the sum only
      and is the most lightweight metric kind.
    - Gauge: For metrics that periodically sample a current state, e.g. temperatures, total number of bytes stored on a disk.
      Gauges report a minimum, maximum and average value (but no sum).
    - Statistics: For event-driven metrics like the packet size of a network interface. The most heavyweight metric.
      Reports min, max, average and count.

    Each combination of metric value type and kind has its own create-function, named `onesdk_<value type><metric kind>metric_create`.

    When creating a metric, you must provide the following information:
    - The **metric key** is a unique identifier of the metric.
    
      Although it is not recommended, you may create multiple metric
      instances with the same name, as long as you use the same creation function
      (so that metric value type and kind are the same) and the other metric information is also the same.
      Otherwise, using the same metric name multiple times is an error.
      If you do create a metric with the same key multiple times, it is undefined whether distinct handles are returned, but
      you are required to call  @ref onesdk_metric_delete on each one (as if they were distinct).
      All metrics with the same key will be aggregated together as if you used only one metric instance.
      
      The encoding of the key string can be UTF-8, UTF-16 or ASCII, but only ASCII-compatible characters are allowed.
      It must not be longer than 100 characters.

    - The **unit** is an optional string that will be displayed when browsing for metrics in the Dynatrace UI.
    - The **dimension name**: When provided, the metric will have one additional dimension the value of which must be provided
      when adding values to that metric. When null or empty, the metric has no additional dimension (and a dimension value must
      not be provided when adding values). Dimension values are like additional labels attached to values, for example
      a "disk.written.bytes" metric could have a dimension name of "disk-id" and when adding values to it, you could use
      dimension values like "/dev/sda1".

    All metric objects (regardless of type) should be freed using @ref onesdk_metric_delete.

    @note Metrics are not supported when the SDK is intialized using @ref ONESDK_INIT_FLAG_FORKABLE.
    @since Metrics are available since version 1.5.
    @deprecated From 1.6 on, all metrics-related APIs are deprecated and will be removed in a future release.
        Refer to https://github.com/Dynatrace/OneAgent-SDK-for-c#metrics for details.
    @{
*/


/** @brief DEPRECATED. Releases a metric object.
    @param metric_handle      A valid metric handle (all types are supported).

    @since This function was added in version 1.5.
    @deprecated From 1.6 on, all metrics-related APIs are deprecated and will be removed in a future release.
        Refer to https://github.com/Dynatrace/OneAgent-SDK-for-c#metrics for details.
    
    This function releases the specified metric object. Allocated resources are freed and the handle is invalidated. 

    An application should call @ref onesdk_metric_delete exactly once for each metric object that it has created.
*/
ONESDK_DECLARE_FUNCTION(void) onesdk_metric_delete(onesdk_metric_handle_t metric_handle);

/** @internal */
ONESDK_DECLARE_FUNCTION(onesdk_metric_handle_t) onesdk_integercountermetric_create_p(onesdk_string_t const* metric_key, onesdk_string_t const* unit, onesdk_string_t const* dimension_name);

/** @brief DEPRECATED. Creates a metric to count (sum up) some 64 bit integer values, e.g., number of records inserted in a database.
    @param metric_key     An ASCII/UTF-8/UTF-16 string uniquely identifying the metric.
    @param unit           [optional] An informational string to describe the unit of the metric. Used for presentation purposes only.
    @param dimension_name [optional] If specified, the metric gets an additional dimension. The value for that should
                          be provided when calling @ref onesdk_integercountermetric_increase_by. The actual dimension name is used for presentation purposes only.
    @return A handle for the a metric instance or @ref ONESDK_INVALID_HANDLE. If you reused @p metric_key, the handle value is not guaranteed to be unique.

    @since This function was added in version 1.5.
    @deprecated From 1.6 on, all metrics-related APIs are deprecated and will be removed in a future release.
        Refer to https://github.com/Dynatrace/OneAgent-SDK-for-c#metrics for details.
 
    See @ref ex_metric_detail "the general metric API documentation" for a more detailed conceptual description.
*/
ONESDK_DEFINE_INLINE_FUNCTION(onesdk_metric_handle_t) onesdk_integercountermetric_create(onesdk_string_t metric_key, onesdk_string_t unit, onesdk_string_t dimension_name) {
    return onesdk_integercountermetric_create_p(&metric_key, &unit, &dimension_name);
}


/** @internal */
ONESDK_DECLARE_FUNCTION(onesdk_metric_handle_t) onesdk_floatcountermetric_create_p(onesdk_string_t const* metric_key, onesdk_string_t const* unit, onesdk_string_t const* dimension_name);

/** @brief DEPRECATED. Creates a metric to count (sum up) some double precision floating point values, e.g., liters pumped into a tank.
    @param metric_key     An ASCII/UTF-8/UTF-16 string uniquely identifying the metric.
    @param unit           [optional] An informational string to describe the unit of the metric. Used for presentation purposes only.
    @param dimension_name [optional] If specified, the metric gets an additional dimension. The value for that should
                          be provided when calling @ref onesdk_floatcountermetric_increase_by. The actual dimension name is used for presentation purposes only.
    @return A handle for the a metric instance or @ref ONESDK_INVALID_HANDLE. If you reused @p metric_key, the handle value is not guaranteed to be unique.

    @since This function was added in version 1.5.0.
    @deprecated From 1.6 on, all metrics-related APIs are deprecated and will be removed in a future release.
        Refer to https://github.com/Dynatrace/OneAgent-SDK-for-c#metrics for details.
 
    See @ref ex_metric_detail "the general metric API documentation" for a more detailed conceptual description.
*/
ONESDK_DEFINE_INLINE_FUNCTION(onesdk_metric_handle_t) onesdk_floatcountermetric_create(onesdk_string_t metric_key, onesdk_string_t unit, onesdk_string_t dimension_name) {
    return onesdk_floatcountermetric_create_p(&metric_key, &unit, &dimension_name);
}


/** @internal */
ONESDK_DECLARE_FUNCTION(onesdk_metric_handle_t) onesdk_integergaugemetric_create_p(onesdk_string_t const* metric_key, onesdk_string_t const* unit, onesdk_string_t const* dimension_name);

/** @brief DEPRECATED. Creates a metric to sample (periodically measure) some 64 bit integer value, e.g., number of records in a database sampled every 30 minutes.
    @param metric_key     An ASCII/UTF-8/UTF-16 string uniquely identifying the metric.
    @param unit           [optional] An informational string to describe the unit of the metric. Used for presentation purposes only.
    @param dimension_name [optional] If specified, the metric gets an additional dimension. The value for that should
                          be provided when calling @ref onesdk_integergaugemetric_set_value. The actual dimension name is used for presentation purposes only.
    @return A handle for the a metric instance or @ref ONESDK_INVALID_HANDLE. If you reused @p metric_key, the handle value is not guaranteed to be unique.

    @since This function was added experimentally in version 1.5.0.
    @deprecated From 1.6 on, all metrics-related APIs are deprecated and will be removed in a future release.
        Refer to https://github.com/Dynatrace/OneAgent-SDK-for-c#metrics for details.
 
    See @ref ex_metric_detail "the general metric API documentation" for a more detailed conceptual description.
*/
ONESDK_DEFINE_INLINE_FUNCTION(onesdk_metric_handle_t) onesdk_integergaugemetric_create(onesdk_string_t metric_key, onesdk_string_t unit, onesdk_string_t dimension_name) {
    return onesdk_integergaugemetric_create_p(&metric_key, &unit, &dimension_name);
}

/** @internal */
ONESDK_DECLARE_FUNCTION(onesdk_metric_handle_t) onesdk_floatgaugemetric_create_p(onesdk_string_t const* metric_key, onesdk_string_t const* unit, onesdk_string_t const* dimension_name);

/** @brief DEPRECATED. Creates a metric to sample (periodically measure) some double precision floating point value, e.g., number of liters in a tank, sampled every 30 minutes.
    @param metric_key     An ASCII/UTF-8/UTF-16 string uniquely identifying the metric.
    @param unit           [optional] An informational string to describe the unit of the metric. Used for presentation purposes only.
    @param dimension_name [optional] If specified, the metric gets an additional dimension. The value for that should
                          be provided when calling @ref onesdk_floatgaugemetric_set_value. The actual dimension name is used for presentation purposes only.
    @return A handle for the a metric instance or @ref ONESDK_INVALID_HANDLE. If you reused @p metric_key, the handle value is not guaranteed to be unique.

    @since This function was added experimentally in version 1.5.0.
    @deprecated From 1.6 on, all metrics-related APIs are deprecated and will be removed in a future release.
        Refer to https://github.com/Dynatrace/OneAgent-SDK-for-c#metrics for details.
 
    See @ref ex_metric_detail "the general metric API documentation" for a more detailed conceptual description.
*/
ONESDK_DEFINE_INLINE_FUNCTION(onesdk_metric_handle_t) onesdk_floatgaugemetric_create(onesdk_string_t metric_key, onesdk_string_t unit, onesdk_string_t dimension_name) {
    return onesdk_floatgaugemetric_create_p(&metric_key, &unit, &dimension_name);
}

/** @internal */
ONESDK_DECLARE_FUNCTION(onesdk_metric_handle_t) onesdk_integerstatisticsmetric_create_p(onesdk_string_t const* metric_key, onesdk_string_t const* unit, onesdk_string_t const* dimension_name);

/** @brief DEPRECATED. Creates a metric to track (measure on each change, event-driven) some 64 bit integer value, e.g., number of records in a database after each insert/delete/merge/etc.
    @param metric_key     An ASCII/UTF-8/UTF-16 string uniquely identifying the metric.
    @param unit           [optional] An informational string to describe the unit of the metric. Used for presentation purposes only.
    @param dimension_name [optional] If specified, the metric gets an additional dimension. The value for that should
                          be provided when calling @ref onesdk_integerstatisticsmetric_add_value. The actual dimension name is used for presentation purposes only.
    @return A handle for the a metric instance or @ref ONESDK_INVALID_HANDLE. If you reused @p metric_key, the handle value is not guaranteed to be unique.

    @since This function was added experimentally in version 1.5.0.
    @deprecated From 1.6 on, all metrics-related APIs are deprecated and will be removed in a future release.
        Refer to https://github.com/Dynatrace/OneAgent-SDK-for-c#metrics for details.
 
    See @ref ex_metric_detail "the general metric API documentation" for a more detailed conceptual description.
*/
ONESDK_DEFINE_INLINE_FUNCTION(onesdk_metric_handle_t) onesdk_integerstatisticsmetric_create(onesdk_string_t metric_key, onesdk_string_t unit, onesdk_string_t dimension_name) {
    return onesdk_integerstatisticsmetric_create_p(&metric_key, &unit, &dimension_name);
}

/** @internal */
ONESDK_DECLARE_FUNCTION(onesdk_metric_handle_t) onesdk_floatstatisticsmetric_create_p(onesdk_string_t const* metric_key, onesdk_string_t const* unit, onesdk_string_t const* dimension_name);

/** @brief DEPRECATED. Creates a metric to track (measure on each change, event-driven) some double precision floating point value,
           e.g., number of liters in a tank after each refilling and (discrete) consumption.
    @param metric_key     An ASCII/UTF-8/UTF-16 string uniquely identifying the metric.
    @param unit           [optional] An informational string to describe the unit of the metric. Used for presentation purposes only.
    @param dimension_name [optional] If specified, the metric gets an additional dimension. The value for that should
                          be provided when calling @ref onesdk_floatstatisticsmetric_add_value. The actual dimension name is used for presentation purposes only.
    @return A handle for the a metric instance or @ref ONESDK_INVALID_HANDLE. If you reused @p metric_key, the handle value is not guaranteed to be unique.

    @since This function was added experimentally in version 1.5.0.
    @deprecated From 1.6 on, all metrics-related APIs are deprecated and will be removed in a future release.
        Refer to https://github.com/Dynatrace/OneAgent-SDK-for-c#metrics for details.
 
  See @ref ex_metric_detail "the general metric API docs" for a more detailed conceptual description.
*/
ONESDK_DEFINE_INLINE_FUNCTION(onesdk_metric_handle_t) onesdk_floatstatisticsmetric_create(onesdk_string_t metric_key, onesdk_string_t unit, onesdk_string_t dimension_name) {
    return onesdk_floatstatisticsmetric_create_p(&metric_key, &unit, &dimension_name);
}

/** @internal */
ONESDK_DECLARE_FUNCTION(void) onesdk_integercountermetric_increase_by_p(onesdk_metric_handle_t metric_handle, onesdk_int64_t delta, onesdk_string_t const* dimension);

/** @brief DEPRECATED. Increase the counter @p metric_handle for @p dimension by the given @p delta.
    @param metric_handle A valid integer counter metric handle, as returned by @ref onesdk_integercountermetric_create.
    @param delta         Value by which to increase the counter (the total change since the last time @ref onesdk_integercountermetric_increase_by was called).
    @param dimension     [optional] Dimension value for which to increase the counter.
                         Must be @ref onesdk_nullstr if no @p dimension_name was specified when creating the metric.

    @since This function was added experimentally in version 1.5.0.
    @deprecated From 1.6 on, all metrics-related APIs are deprecated and will be removed in a future release.
        Refer to https://github.com/Dynatrace/OneAgent-SDK-for-c#metrics for details.
*/
ONESDK_DEFINE_INLINE_FUNCTION(void) onesdk_integercountermetric_increase_by(onesdk_metric_handle_t metric_handle, onesdk_int64_t delta, onesdk_string_t dimension) {
    onesdk_integercountermetric_increase_by_p(metric_handle, delta, &dimension);
}

/** @internal */
ONESDK_DECLARE_FUNCTION(void) onesdk_floatcountermetric_increase_by_p(onesdk_metric_handle_t metric_handle, double delta, onesdk_string_t const* dimension);

/** @brief DEPRECATED. Increase the counter @p metric_handle for @p dimension by the given @p delta.
    @param metric_handle A valid floating point counter metric handle, as returned by @ref onesdk_floatcountermetric_create.
    @param delta         Value by which to increase the counter (the total change since the last time @ref onesdk_floatcountermetric_increase_by was called).
    @param dimension     [optional] Dimension value for which to increase the counter.
                         Must be @ref onesdk_nullstr if no @p dimension_name was specified when creating the metric.

    @since This function was added experimentally in version 1.5.0.
    @deprecated From 1.6 on, all metrics-related APIs are deprecated and will be removed in a future release.
        Refer to https://github.com/Dynatrace/OneAgent-SDK-for-c#metrics for details.
*/
ONESDK_DEFINE_INLINE_FUNCTION(void) onesdk_floatcountermetric_increase_by(onesdk_metric_handle_t metric_handle, double delta, onesdk_string_t dimension) {
    onesdk_floatcountermetric_increase_by_p(metric_handle, delta, &dimension);
}

/** @internal */
ONESDK_DECLARE_FUNCTION(void) onesdk_integergaugemetric_set_value_p(onesdk_metric_handle_t metric_handle, onesdk_int64_t value, onesdk_string_t const* dimension);

/** @brief DEPRECATED. Set the last sampled value for the @p metric_handle for @p dimension to @p value.
    @param metric_handle A valid integer gauge metric handle, as returned by @ref onesdk_integergaugemetric_create.
    @param value         Current value to which to set the gauge (usually comes from some periodically sampled measurement).
    @param dimension     [optional] Dimension value for which to increase the counter.
                         Must be @ref onesdk_nullstr if no @p dimension_name was specified when creating the metric.

    @since This function was added experimentally in version 1.5.0.
    @deprecated From 1.6 on, all metrics-related APIs are deprecated and will be removed in a future release.
        Refer to https://github.com/Dynatrace/OneAgent-SDK-for-c#metrics for details.
*/
ONESDK_DEFINE_INLINE_FUNCTION(void) onesdk_integergaugemetric_set_value(onesdk_metric_handle_t metric_handle, onesdk_int64_t value, onesdk_string_t dimension) {
    onesdk_integergaugemetric_set_value_p(metric_handle, value, &dimension);
}

/** @internal */
ONESDK_DECLARE_FUNCTION(void) onesdk_floatgaugemetric_set_value_p(onesdk_metric_handle_t metric_handle, double value, onesdk_string_t const* dimension);

/** @brief DEPRECATED. Set the last sampled value for the @p metric_handle for @p dimension to @p value.
    @param metric_handle A valid floating point gauge metric handle, as returned by @ref onesdk_floatgaugemetric_create.
    @param value         Current value to which to set the gauge (usually comes from some periodically sampled measurement).
    @param dimension     [optional] Dimension value for which to increase the counter.
                         Must be @ref onesdk_nullstr if no @p dimension_name was specified when creating the metric.

    @since This function was added experimentally in version 1.5.0.
    @deprecated From 1.6 on, all metrics-related APIs are deprecated and will be removed in a future release.
        Refer to https://github.com/Dynatrace/OneAgent-SDK-for-c#metrics for details.
*/
ONESDK_DEFINE_INLINE_FUNCTION(void) onesdk_floatgaugemetric_set_value(onesdk_metric_handle_t metric_handle, double value, onesdk_string_t dimension) {
    onesdk_floatgaugemetric_set_value_p(metric_handle, value, &dimension);
}

/** @internal */
ONESDK_DECLARE_FUNCTION(void) onesdk_integerstatisticsmetric_add_value_p(onesdk_metric_handle_t metric_handle, onesdk_int64_t value, onesdk_string_t const* dimension);

/** @brief DEPRECATED. Records the current value for the @p metric_handle for @p dimension to @p value.
    @param metric_handle A valid integer statistics metric handle, as returned by @ref onesdk_integerstatisticsmetric_create.
    @param value         Value to record in the statistic (usually comes from a measurement taken after a discrete event that indicates a change in the value).
    @param dimension     [optional] Dimension value for which to increase the counter.
                         Must be @ref onesdk_nullstr if no @p dimension_name was specified when creating the metric.

    @since This function was added experimentally in version 1.5.0.
    @deprecated From 1.6 on, all metrics-related APIs are deprecated and will be removed in a future release.
        Refer to https://github.com/Dynatrace/OneAgent-SDK-for-c#metrics for details.
*/
ONESDK_DEFINE_INLINE_FUNCTION(void) onesdk_integerstatisticsmetric_add_value(onesdk_metric_handle_t metric_handle, onesdk_int64_t value, onesdk_string_t dimension) {
    onesdk_integerstatisticsmetric_add_value_p(metric_handle, value, &dimension);
}

/** @internal */
ONESDK_DECLARE_FUNCTION(void) onesdk_floatstatisticsmetric_add_value_p(onesdk_metric_handle_t metric_handle, double value, onesdk_string_t const* dimension);

/** @brief DEPRECATED. Records the current value for the @p metric_handle for @p dimension to @p value.
    @param metric_handle A valid floating point statistics metric handle, as returned by @ref onesdk_floatstatisticsmetric_create.
    @param value         Value to record in the statistic (usually comes from a measurement taken after a discrete event that indicates a change in the value).
    @param dimension     [optional] Dimension value for which to increase the counter.
                         Must be @ref onesdk_nullstr if no @p dimension_name was specified when creating the metric.

    @since This function was added experimentally in version 1.5.0.
    @deprecated From 1.6 on, all metrics-related APIs are deprecated and will be removed in a future release.
        Refer to https://github.com/Dynatrace/OneAgent-SDK-for-c#metrics for details.
*/
ONESDK_DEFINE_INLINE_FUNCTION(void) onesdk_floatstatisticsmetric_add_value(onesdk_metric_handle_t metric_handle, double value, onesdk_string_t dimension) {
    onesdk_floatstatisticsmetric_add_value_p(metric_handle, value, &dimension);
}


/** @} */

#endif /* ONESDK_EX_METRICS_H_INCLUDED */
