# Dynatrace OneAgent SDK for C/C++

This SDK enables Dynatrace customers to extend request level visibility into any native process. The SDK is C based and thus can be used in any C or C++ application. It can also be used in other languages via language bindings.
In order to use the development kit you need to have access to the source code of the application in question.

This is the official C/C++ implementation of the [Dynatrace OneAgent SDK](https://github.com/Dynatrace/OneAgent-SDK/).

<!-- Generate with https://github.com/jonschlinkert/markdown-toc -->

<!-- toc -->

- [Package contents](#package-contents)
- [Features](#features)
- [Documentation](#documentation)
- [Getting started](#getting-started)
  * [Getting the SDK](#getting-the-sdk)
  * [Building and linking against the Dynatrace OneAgent SDK](#building-and-linking-against-the-dynatrace-oneagent-sdk)
    + [Using CMake](#using-cmake)
    + [Auto-linking with Visual Studio](#auto-linking-with-visual-studio)
    + [Other build systems](#other-build-systems)
  * [Using CMake to build the samples](#using-cmake-to-build-the-samples)
  * [Initializing the Dynatrace OneAgent SDK](#initializing-the-dynatrace-oneagent-sdk)
    + [Special considerations for Solaris SPARC](#special-considerations-for-solaris-sparc)
- [How to instrument your application](#how-to-instrument-your-application)
  * [General notes](#general-notes)
  * [Trace remote calls](#trace-remote-calls)
  * [Trace SQL based database calls](#trace-sql-based-database-calls)
  * [Trace incoming web requests](#trace-incoming-web-requests)
  * [Trace outgoing web requests](#trace-outgoing-web-requests)
  * [Trace asynchronous activities](#trace-asynchronous-activities)
  * [Trace messaging](#trace-messaging)
  * [Trace custom service methods](#trace-custom-service-methods)
  * [Add custom request attributes](#add-custom-request-attributes)
  * [Retrieve a W3C trace context](#retrieve-a-w3c-trace-context)
- [Using the Dynatrace OneAgent SDK with forked child processes (only available on Linux)](#using-the-dynatrace-oneagent-sdk-with-forked-child-processes-only-available-on-linux)
- [Troubleshooting](#troubleshooting)
  * [Problems with initializing the SDK](#problems-with-initializing-the-sdk)
  * [Problems occuring after initialization](#problems-occuring-after-initialization)
- [Requirements](#requirements)
  * [Version support and compatibility table](#version-support-and-compatibility-table)
- [Help & Support](#help--support)
  * [Read the manual](#read-the-manual)
  * [Let us help you](#let-us-help-you)
- [Release Notes](#release-notes)

<!-- tocstop -->

<a name="package-contents"></a>

## Package contents

The SDK package includes
- `lib` and `include`: The libraries and header files necessary for instrumenting applications.
- `*.cmake`: Optional support files to use the libraries more easily with the CMake build system.
- `samples/sample1`: A simple sample application.
- `docs`: Reference documentation.

<a name="features"></a>

## Features

- [Trace any remote call end-to-end across processes and different programming languages.](#trace-remote-calls)
- [Trace any SQL-based database call.](#trace-sql-based-database-calls)
- [Trace incoming and outgoing web requests.](#trace-incoming-web-requests)
- [Trace asynchronous processing within one process.](#trace-outgoing-web-requests)
- [Trace messaging systems and messaging queues.](#trace-messaging)
- [Trace custom service methods.](#trace-custom-service-methods)
- [Add custom request attributes to any currently traced service.](#add-custom-request-attributes)

When tracing incoming or outgoing calls, requests or messages, this SDK is compatible with other OneAgent SDKs and OneAgent code modules in general.

See [Planned features for OneAgent
SDK](https://answers.dynatrace.com/spaces/483/dynatrace-product-ideas/idea/198106/planned-features-for-oneagent-sdk.html) for details on
upcoming features.

<a name="documentation"></a>

## Documentation

The reference documentation is included in this package. The most recent version is also available online at <https://dynatrace.github.io/OneAgent-SDK-for-C/>.

A high level documentation/description of OneAgent SDK concepts is available at <https://github.com/Dynatrace/OneAgent-SDK/>.

<a name="getting-started"></a>

## Getting started

<a name="getting-the-sdk"></a>

### Getting the SDK

To start using the Dynatrace OneAgent SDK for C/C++, simply download the latest source archive from [releases](https://github.com/Dynatrace/OneAgent-SDK-for-C/releases).
The source archive also includes all necessary artifacts (e.g. the static and dynamic library files), so this is all you need.
Extract the archive to a local folder on your machine and then add the appropriate "include" and "lib" paths to your build system.

To see if your platform is supported, refer to [requirements](#requirements).

<a name="building-and-linking-against-the-dynatrace-oneagent-sdk"></a>

### Building and linking against the Dynatrace OneAgent SDK

The SDK doesn't have to be compiled, you only need to link your application to the SDK libraries.

<a name="using-cmake"></a>

#### Using CMake

If you use CMake to generate build files for your application, you should be able to use the provided `onesdk-config.cmake` script ala

```CMake
include("path/to/sdk-package/onesdk-config.cmake")
target_link_libraries(your_application onesdk_static)
```

<a name="auto-linking-with-visual-studio"></a>

#### Auto-linking with Visual Studio

If you use Visual Studio to build a Windows application, you can use the SDK's auto-linking feature. To do this, simply define the preprocessor macro `ONESDK_AUTO_LINK` before including any SDK header file.
Aside from that, you only have to add the appropriate "include" and "lib" paths.

<a name="other-build-systems"></a>

#### Other build systems

If you use another build system you have to configure it to
- add an "include" path to `path/to/sdk-package/include`
- add a "lib" path to the appropriate platform subdirectory under `lib` (e.g. `path/to/sdk-package/lib/linux-x86_64`)
- link the appropriate library (e.g. `libonesdk_static.a`)

The SDK contains code that dynamically loads the agent library (`.dll`/`.so`/...), so depending on your platform you might need to link
additional libraries (e.g. under Linux you would typically add `-ldl` to the linker command line).

On Windows, when using Visual Studio 2015 or later, you also have to link `legacy_stdio_definitions.lib`.

<a name="using-cmake-to-build-the-samples"></a>

### Using CMake to build the samples

Assuming that you have a C++11 compiler and suitable build system installed (e.g. Visual Studio or g++ & make), which are supported and correctly detected by CMake, creating build files for the samples can be as easy as

```
C:\onesdk\samples>mkdir build
C:\onesdk\samples>cd build
C:\onesdk\samples\build>cmake ..
  *snip* a lot of CMake output
-- Build files have been written to: C:/onesdk/samples/build
C:\onesdk\samples\build>
```

Then simply use your build system to build the samples (e.g. "make" or open & build the generated solution in Visual Studio).

<a name="initializing-the-dynatrace-oneagent-sdk"></a>

### Initializing the Dynatrace OneAgent SDK

To initialize the OneAgent SDK, you call [`onesdk_initialize`][refd_initialize], like in the following sample. It is higly recommended that
you call [`onesdk_shutdown`][refd_shutdown] when the application is done using the SDK (typically just before exiting):

```C
#include <onesdk/onesdk.h>
#include <stdio.h>

static void mycallback(char const* message) {
    fputs(message, stderr);
}

int main(int argc, char** argv) {
    onesdk_stub_process_cmdline_args(argc, argv, 1);  /* optional: let the SDK process command line arguments   */
    onesdk_stub_strip_sdk_cmdline_args(&argc, argv);  /* optional: remove SDK command line arguments from argv  */

    /* Initialize SDK */
    onesdk_result_t const onesdk_init_result = onesdk_initialize();

    /* optional: Set logging callbacks. */
    onesdk_agent_set_warning_callback(mycallback); /* Highly recommended. */
    onesdk_agent_set_verbose_callback(mycallback); /* Recommended for development & debugging. */

    /* ... use SDK ... */

    /* Shut down SDK */
    if (onesdk_init_result == ONESDK_SUCCESS)
        onesdk_shutdown();

    return 0;
}
```

> 📕 [Reference documentation for initialization and shutdown](https://dynatrace.github.io/OneAgent-SDK-for-C/group__init.html)  
> 📕 [Miscellaneous functions](https://dynatrace.github.io/OneAgent-SDK-for-C/group__misc.html)

<a name="special-considerations-for-solaris-sparc"></a>

#### Special considerations for Solaris SPARC

For agents older than 1.173, auto-configuration is not supported for the OneAgent SDK for C/C++ on Solaris SPARC. Thus you must either use
[`onesdk_stub_set_variable`][refd_set_variable] before calling [`initialize`][refd_initialize], or set certain environment variables. If you
use [`onesdk_stub_process_cmdline_args`][refd_process_cmdline_args], you can also use command line options. If you are familiar with [manual
injection for Apache or Java on
Solaris](https://www.dynatrace.com/support/help/setup-and-configuration/oneagent/solaris/installation/install-oneagent-on-solaris/#expand-464manual-oneagent-injection),
this may sound familiar to you.

The following options must be set (to specify on the command line, use the `set_variable`-name but prepend `--dt_`):

|`set_variable`|Environment variable |Value                                                                                      |
|:-------------|:--------------------|:------------------------------------------------------------------------------------------|
|`home`        |`DT_HOME`            |Your Dynatrace OneAgent installation folder, e.g. `/opt/dynatrace/oneagent/`.              |
|`tenant`      |`DT_TENANT`          |The environment ID of your Dynatrace environment.                                          |
|`tenantToken` |`DT_TENANTTOKEN`     |The token that OneAgent uses to connect to Dynatrace Server. **Not** an API or PaaS token! |
|`server`      |`DT_CONNECTION_POINT`|One or multiple HTTP addresses that represent Dynatrace Servers or ActiveGates.            |

Please obtain the values of all these variables (except for `home` / `DT_HOME`) from `$DT_HOME/dynatrace-env.sh`. You can also use this
script directly to set the environment variables (except `DT_HOME`), or start your SDK-using application with
`$DT_HOME/dynatrace-agent<bitness>.sh <executable> <other command line arguments>` (but you still need to set `home` / `DT_HOME`).

For example, to set `home` via [`onesdk_stub_set_variable`][refd_set_variable], do something like
`onesdk_stub_set_variable(ONESDK_XSTR("home=/opt/dynatrace/oneagent"), 0);` in your code OR set the `DT_HOME` environment variable, OR, if
your application calls [`onesdk_stub_process_cmdline_args`][refd_process_cmdline_args], you can pass `--dt_home=/opt/dynatrace/oneagent` on
the command line.

There is an additional option you may want to set: `loglevelcon` / `DT_LOGLEVELCON` can be set to `none` to stop the agent from writing to
stderr (see also [Troubleshooting](#troubleshooting)).

[refd_initialize]: https://dynatrace.github.io/OneAgent-SDK-for-C/group__init.html#gac3d473d2899bdb54196f864ae0ccf3eb
[refd_shutdown]: https://dynatrace.github.io/OneAgent-SDK-for-C/group__init.html#gab65ee07ae9c61fae6d0ca0b4afbd8bb1
[refd_set_variable]: https://dynatrace.github.io/OneAgent-SDK-for-C/group__init.html#ga1271327bb21c71ed5f8d92de0629ebfc
[refd_process_cmdline_args]: https://dynatrace.github.io/OneAgent-SDK-for-C/group__init.html#ga77bf723c281e4e2963933a57ff1ec51c

<a name="using-the-dynatrace-oneagent-sdk-to-trace-remote-calls"></a>

<a name="how-to-instrument-your-application"></a>

## How to instrument your application

This section gives samples of how to instrument your application for each supported feature. Refer to the [documentation](#documentation)
for more details.

### General notes

The core API for instrumenting your application is the "tracer". Depending on the kind of operation you want to trace,
you use one of the `onesdk_*tracer_create` functions to create a tracer, then potentially set additional information using
setter functions before you start the tracer using `onesdk_tracer_start`. If an error or exception happens, you can capture it
using `onesdk_tracer_error`. Finally, you need to call `onesdk_tracer_end` to signal the end of the operation and also free up
any resources allocated for the tracer.

Whenever you start a tracer (i.e., call `onesdk_tracer_start`), the tracer becomes a child of the previously active tracer
on this thread and the new tracer then becomes the active tracer. You may only end the active tracer.
If you do, the tracer that was active before it (its parent) becomes active again.
Put another way, tracers must be ended in reverse order of starting them
(you can think of this being like HTML tags where you must also close the child tag before you can close the parent tag).

While the tracer's automatic parent-child relationship works very intuitively in most cases,
it does not work with **asynchronous patterns**, where the same thread handles multiple logically
separate operations in an interleaved way on the same thread. If you need to instrument
such patterns with the SDK, you need to end your tracer before the thread is potentially reused
by any other operation (e.g., before yielding to the event loop). To later continue the trace,
capture an in-process link before and later resume using the in-process link tracer, as explained in
[Trace asynchronous activities](#trace-asynchronous-activities). This approach is rather awkward and
may lead to complex and difficult to interpret traces. If your application makes extensive use of
asynchronous patterns of the kind that is difficult to instrument with the SDK, consider using
the [OpenTelemetry support of Dynatrace](https://www.dynatrace.com/support/help/shortlink/opent-cpp) instead.

> See also:
>
> 📕 [Reference documentation for common tracer functions](https://dynatrace.github.io/OneAgent-SDK-for-C/group__tracers.html)

<a name="trace-remote-calls"></a>

### Trace remote calls

You can use the SDK to trace proprietary IPC communication from one process to the other. This will enable you to see full Service Flow,
PurePath and Smartscape topology for remoting technologies that Dynatrace is not aware of.

Instrumenting an outgoing remote call:

```C
    /* create tracer */
    onesdk_tracer_handle_t const tracer = onesdk_outgoingremotecalltracer_create(
        onesdk_asciistr("remote service method"),
        onesdk_asciistr("logical service name"),
        onesdk_asciistr("deployed service endpoint"),
        ONESDK_CHANNEL_TYPE_TCP_IP,           /* channel type     */
        onesdk_asciistr("localhost:12345")    /* channel endpoint, host/ip:port in case of TCP_IP */ );

    /* start tracer */
    onesdk_tracer_start(tracer);

    /* get byte representation of tag */
    onesdk_size_t byte_tag_size = 0;
    onesdk_tracer_get_outgoing_dynatrace_byte_tag(tracer, NULL, 0, &byte_tag_size);
    unsigned char* byte_tag = NULL;
    if (byte_tag_size != 0) {
        byte_tag = (unsigned char*)malloc(byte_tag_size);
        if (byte_tag != NULL)
            byte_tag_size = onesdk_tracer_get_outgoing_dynatrace_byte_tag(tracer, byte_tag, byte_tag_size, NULL);
    }

    /* ... do the actual remote call (send along `byte_tag` so the other side can continue tracing) ... */

    /* release tag memory */
    free(byte_tag);

    /* set error information */
    if (something_went_wrong)
        onesdk_tracer_error(tracer, onesdk_asciistr("error type"), onesdk_asciistr("error message"));

    /* end and release tracer */
    onesdk_tracer_end(tracer);
```

Instrumenting an incoming remote call:

```C
    unsigned char const* byte_tag = ...;    /* pointer to the byte tag that we received from the caller */
    onesdk_size_t byte_tag_size = ...;      /* size of the byte tag that we received from the caller    */

    /* create tracer */
    onesdk_tracer_handle_t const tracer = onesdk_incomingremotecalltracer_create(
        onesdk_asciistr("remote service method"),
        onesdk_asciistr("logical service name"),
        onesdk_asciistr("deployed service endpoint"));

    /* set the tag that we got from the caller */
    if (byte_tag_size != 0)
        onesdk_tracer_set_incoming_dynatrace_byte_tag(tracer, byte_tag, byte_tag_size);

    /* start tracer */
    onesdk_tracer_start(tracer);

    /* ... do the actual work ... */

    /* set error information */
    if (something_went_wrong)
        onesdk_tracer_error(tracer, onesdk_asciistr("error type"), onesdk_asciistr("error message"));

    /* end & release tracer */
    onesdk_tracer_end(tracer);
```

> 📕 [Reference documentation for remote call tracers](https://dynatrace.github.io/OneAgent-SDK-for-C/group__remote__calls.html)

<a name="using-the-dynatrace-oneagent-sdk-to-trace-sql-based-database-calls"></a>
<a name="trace-sql-based-database-calls"></a>

### Trace SQL based database calls

To trace database requests you need a database info object which stores the information about your database which does not change between
individual requests. This will typically be created somewhere in your initialization code (after initializing the SDK):

```C
onesdk_databaseinfo_handle_t db_info_handle = ONESDK_INVALID_HANDLE;

/* ... */

    db_info_handle = onesdk_databaseinfo_create(
        onesdk_asciistr("database name"),   /* the name of the database that you connect to */
        onesdk_asciistr(ONESDK_DATABASE_VENDOR_POSTGRESQL),   /* the type of the database   */
        ONESDK_CHANNEL_TYPE_TCP_IP,         /* channel type     */
        onesdk_asciistr("localhost:12345")  /* channel endpoint */ );
```

Then you can trace the SQL database requests:

```C
    /* create tracer */
    onesdk_tracer_handle_t const tracer = onesdk_databaserequesttracer_create_sql(
        db_info_handle,
        onesdk_asciistr("SELECT foo FROM bar;"));

    /* start tracer */
    onesdk_tracer_start(tracer);

    /* ... perform the database request, consume results ... */

    /* optional: set number of returned rows */
    onesdk_databaserequesttracer_set_returned_row_count(tracer, 42);
    /* optional: set number of round trips between client and database */
    onesdk_databaserequesttracer_set_round_trip_count(tracer, 3);

    /* set error information */
    if (something_went_wrong)
        onesdk_tracer_error(tracer, onesdk_asciistr("error type"), onesdk_asciistr("error message"));

    /* end & release tracer */
    onesdk_tracer_end(tracer);
```

Finally, release the database info object in your cleanup code (before shutting down the SDK):

```C
    onesdk_databaseinfo_delete(db_info_handle);
    db_info_handle = ONESDK_INVALID_HANDLE;
```

Please note that SQL database traces are only created if they occur within some other SDK trace (e.g. incoming remote call).

> 📕 [Reference documentation for database request tracers](https://dynatrace.github.io/OneAgent-SDK-for-C/group__database__requests.html)

<a name="using-the-dynatrace-oneagent-sdk-to-trace-incoming-web-requests"></a>
<a name="trace-incoming-web-requests"></a>

### Trace incoming web requests

To trace incoming web requests you first need to create a web application info object which describes your web application:

```C
onesdk_webapplicationinfo_handle_t web_application_info_handle = ONESDK_INVALID_HANDLE;

/* ... */

    web_application_info_handle = onesdk_webapplicationinfo_create(
        onesdk_asciistr("example.com"),         /* name of the web server that hosts your application */
        onesdk_asciistr("MyWebApplication"),    /* unique name for your web application               */
        onesdk_asciistr("/my-web-app/")         /* context root of your web application               */ );
```

Then you can trace incoming web requests:

```C
    /* create tracer */
    onesdk_tracer_handle_t const tracer = onesdk_incomingwebrequesttracer_create(
        web_application_info_handle,
        onesdk_asciistr("/my-web-app/content.html?q1=1&q2=2#frag"),
        onesdk_asciistr("GET"));

    /* add information about the incoming request */
    onesdk_incomingwebrequesttracer_set_remote_address(tracer, onesdk_asciistr("1.2.3.4:56789"));
    onesdk_incomingwebrequesttracer_add_request_header(tracer,
        onesdk_asciistr("Connection"), onesdk_asciistr("keep-alive"));
    onesdk_incomingwebrequesttracer_add_request_header(tracer,
        onesdk_asciistr("Pragma"), onesdk_asciistr("no-cache"));
    /* ... */

    /* start tracer */
    onesdk_tracer_start(tracer);

    /* ... service the web request ... */

    /* add information about the response */
    onesdk_incomingwebrequesttracer_add_response_header(tracer,
        onesdk_asciistr("Transfer-Encoding"), onesdk_asciistr("chunked"));
    onesdk_incomingwebrequesttracer_add_response_header(tracer,
        onesdk_asciistr("Content-Length"), onesdk_asciistr("1234"));
    onesdk_incomingwebrequesttracer_set_status_code(tracer, 200);
    /* ... */

    /* set error information */
    if (something_went_wrong)
        onesdk_tracer_error(tracer, onesdk_asciistr("error type"), onesdk_asciistr("error message"));

    /* end & release tracer */
    onesdk_tracer_end(tracer);
```

And release the web application info object before shutting down the SDK:

```C
    onesdk_webapplicationinfo_delete(web_application_info_handle);
    web_application_info_handle = ONESDK_INVALID_HANDLE;
```

> 📕 [Reference documentation for incoming web request tracers](https://dynatrace.github.io/OneAgent-SDK-for-C/group__incoming__web__requests.html)
>
> ➡️ [Trace outgoing web requests](#trace-outgoing-web-requests)

<a name="using-the-dynatrace-oneagent-sdk-to-trace-outgoing-web-requests"></a>
<a name="trace-outgoing-web-requests"></a>

### Trace outgoing web requests

You can use the SDK to trace web requests sent by your application:

```C
    /* create tracer */
    onesdk_tracer_handle_t const tracer = onesdk_outgoingwebrequesttracer_create(
        onesdk_asciistr("http://example.org:1234/my/rest-service/resources?filter=foo"),
        onesdk_asciistr("GET"));

    /* add information about the request we're about to send */
    onesdk_outgoingwebrequesttracer_add_request_header(tracer,
        onesdk_asciistr("Accept-Charset"), onesdk_asciistr("utf-8"));
    onesdk_outgoingwebrequesttracer_add_request_header(tracer,
        onesdk_asciistr("Pragma"), onesdk_asciistr("no-cache"));
    /* ... */

    /* start tracer */
    onesdk_tracer_start(tracer);

    /* get string representation of tag */
    onesdk_size_t string_tag_size = 0;
    onesdk_tracer_get_outgoing_dynatrace_string_tag(tracer, NULL, 0, &string_tag_size);
    char* string_tag = NULL;
    if (string_tag_size != 0) {
        string_tag = (char*)malloc(string_tag_size);
        if (string_tag != NULL)
            string_tag_size = onesdk_tracer_get_outgoing_dynatrace_string_tag(tracer, string_tag, string_tag_size, NULL);
    }

    /* ... actually send the HTTP request, sending along `string_tag` as an HTTP header
           (use the macro `ONESDK_DYNATRACE_HTTP_HEADER_NAME` for the header name),
           receive the reply and decode it ... */

    /* release tag memory */
    free(string_tag);

    /* add information about the response */
    onesdk_outgoingwebrequesttracer_add_response_header(tracer,
        onesdk_asciistr("Transfer-Encoding"), onesdk_asciistr("chunked"));
    onesdk_outgoingwebrequesttracer_add_response_header(tracer,
        onesdk_asciistr("Content-Length"), onesdk_asciistr("1234"));
    onesdk_outgoingwebrequesttracer_set_status_code(tracer, 200);
    /* ... */

    /* set error information */
    if (something_went_wrong)
        onesdk_tracer_error(tracer, onesdk_asciistr("error type"), onesdk_asciistr("error message"));

    /* end and release tracer */
    onesdk_tracer_end(tracer);
```

> 📕 [Reference documentation for outgoing web request tracers](https://dynatrace.github.io/OneAgent-SDK-for-C/group__outgoing__web__requests.html)
>
> ➡️ [Trace incoming web requests](#trace-incoming-web-requests)

<a name="using-the-dynatrace-oneagent-sdk-to-trace-asynchronous-activities"></a>
<a name="trace-asynchronous-activities"></a>

### Trace asynchronous activities

Many applications schedule work in some asynchronous fashion. Automatic linking of tracers will not work in such scenarios - it can only link to the innermost active tracer on the current thread.
To link the asynchronous parts to the currently active tracer, you first have to create an in-process link:

```C
    /* create in-process link */
    onesdk_size_t in_process_link_size = 0;
    onesdk_inprocesslink_create(NULL, 0, &in_process_link_size);
    unsigned char* in_process_link = NULL;
    if (in_process_link_size != 0) {
        in_process_link = (unsigned char*)malloc(in_process_link_size);
        if (in_process_link != NULL)
            in_process_link_size = onesdk_inprocesslink_create(in_process_link, in_process_link_size, NULL);
    }

    /* ... start/queue asynchronous work (send along `in_process_link` so the other side can continue tracing) ... */

    /* release in-process link memory */
    free(in_process_link);
```

Once you have the in-process link, you can create an in-process link tracer to continue tracing in another thread:

```C
    unsigned char const* in_process_link = ...;  /* pointer to the in-process link */
    onesdk_size_t in_process_link_size = ...;    /* size of the in-process link    */

    /* create in-process link tracer */
    onesdk_tracer_handle_t const tracer = onesdk_inprocesslinktracer_create(
        in_process_link,
        in_process_link_size);

    /* start tracer ("activates" the in-process link) */
    onesdk_tracer_start(tracer);

    /* ... do the work - new tracers started here will be linked to wherever the in-process link was created ... */

    /* end & release tracer ("deactivates" the in-process link) */
    onesdk_tracer_end(tracer);
```

Note that you can re-use in-process links to create multiple in-process link tracers.

> 📕 [Reference documentation for in-process link functions](https://dynatrace.github.io/OneAgent-SDK-for-C/group__in__process__links.html)

<a name="using-the-dynatrace-oneagent-sdk-to-trace-messaging"></a>
<a name="trace-messaging"></a>

### Trace messaging

To trace interaction with a messaging system, such as sending and receiving messages from message queues, you need a messaging system info
object which stores the information about your messaging system that does not change between individual requests. This will typically be
created somewhere in your initialization code (after initializing the SDK):

```C
    onesdk_messagingsysteminfo_handle_t messagingsysteminfo_handle = onesdk_messagingsysteminfo_create(
        onesdk_asciistr(ONESDK_MESSAGING_VENDOR_RABBIT_MQ), // vendor name
        onesdk_asciistr("myqueue"),                         // destination name
        ONESDK_MESSAGING_DESTINATION_TYPE_QUEUE,            // destination type
        ONESDK_CHANNEL_TYPE_TCP_IP,                         // channel type
        onesdk_asciistr("example.com:1234"));               // channel endpoint
```

Then you can trace sending, receiving and processing of messages.

Tracing the sending of messages is straightforward and works like other tracers:

```C
    /* create tracer */
    onesdk_tracer_handle_t const tracer = onesdk_outgoingmessagetracer_create(messagingsysteminfo_handle);

    /* start tracer */
    onesdk_tracer_start(tracer);

    /* get byte representation of tag (an ASCII string representation is also supported) */
    onesdk_size_t byte_tag_size = 0;
    onesdk_tracer_get_outgoing_dynatrace_byte_tag(tracer, NULL, 0, &byte_tag_size);
    unsigned char* byte_tag = NULL;
    if (byte_tag_size != 0) {
        byte_tag = (unsigned char*)malloc(byte_tag_size);
        if (byte_tag != NULL)
            byte_tag_size = onesdk_tracer_get_outgoing_dynatrace_byte_tag(tracer, byte_tag, byte_tag_size, NULL);
    }

    /* ... do the actual message sending (send along `byte_tag` so the other side can continue tracing) ... */
    mymessage_add_header(mymessage, ONESDK_DYNATRACE_MESSAGE_PROPERTY_NAME, byte_tag, byte_tag_size);
    mymessage_send(mymessage);

    /* release tag memory */
    free(byte_tag);

    /* optional: set message ID, if provided by the messaging system */
    onesdk_outgoingmessagetracer_set_vendor_message_id(tracer, onesdk_asciistr(mymessage_get_id_str(mymessage)));

    /* optional: set correlation ID, if you have one (usually application-defined) */
    onesdk_outgoingmessagetracer_set_correlation_id(tracer, onesdk_asciistr(mycorrelationid);

    /* set error information */
    if (something_went_wrong)
        onesdk_tracer_error(tracer, onesdk_asciistr("error type"), onesdk_asciistr("error message"));

    /* end and release tracer */
    onesdk_tracer_end(tracer);
```

For the other side, we distinguish two activities: Receiving the message an processing it. You can (optionally) use a tracer around
receiving one or multiple messages if you are interested in the time the actual receiving takes. For the processing of the message, i.e.,
any business logic that is executed in response to the message content, there is a different tracer. Note that only the tracer for
processing messages supports an incoming tag. This is because the incoming tag must be set before starting the tracer and this cannot be
done for the receive-tracer, as the tag is normally part of the message content or headers, thus available only after message receipt.

The following example shows using the message receive and process tracers in the recommended combination, i.e., starting the process tracer
inside the receive tracer:

```C
    onesdk_tracer_handle_t receive_tracer = onesdk_incomingmessagereceivetracer_create(messagingsysteminfo_handle);

    /* start receive_tracer */
    onesdk_tracer_start(receive_tracer);

    while (/* ... e.g., a message is available in the queue, or only once ... */) {
        /* ... actually receive a message, ... */

        unsigned char const* byte_tag = mymessage_get_header_optional(mymessage, ONESDK_DYNATRACE_MESSAGE_PROPERTY_NAME);
        onesdk_size_t byte_tag_size = ...;      /* size of the byte tag that we received (Note: byte_tag is not null-terminated) */

        /* create process_tracer */
        onesdk_tracer_handle_t const process_tracer = onesdk_incomingmessageprocesstracer_create(messagingsysteminfo_handle);

        /* set the tag that we got from the message */
        if (byte_tag_size != 0)
            onesdk_tracer_set_incoming_dynatrace_byte_tag(process_tracer, byte_tag, byte_tag_size);

        /* optional: set message ID, if provided by the messaging system */
        onesdk_outgoingmessagetracer_set_vendor_message_id(process_tracer, onesdk_asciistr(mymessage_get_id_str(mymessage)));

        /* optional: set correlation ID, if you have one (usually application-defined) */
        onesdk_outgoingmessagetracer_set_correlation_id(process_tracer, onesdk_asciistr(mycorrelationid));

        /* start process_tracer */
        onesdk_tracer_start(process_tracer);

        /* ... do the actual work: process the message content, do something in response, ... */

        /* set error information */
        if (something_went_wrong_with_processing)
            onesdk_tracer_error(process_tracer, onesdk_asciistr("error type"), onesdk_asciistr("error message"));

        /* end & release process_tracer */
        onesdk_tracer_end(process_tracer);
    }
    /* set error information */
    if (something_went_wrong_with_receiving)
        onesdk_tracer_error(receive_tracer, onesdk_asciistr("error type"), onesdk_asciistr("error message"));
    onesdk_tracer_end(receive_tracer);
```

Note that currently, the receive tracer will never create a new PurePath, so if it is not started inside another started tracer, nothing
will be traced by it (although the message process tracer will still work).


You should not forget to release the messaging system info object in your cleanup code (before shutting down the SDK):

```C
    onesdk_messagingsysteminfo_delete(messagingsysteminfo_handle);
    messagingsysteminfo_handle = ONESDK_INVALID_HANDLE;
```

> 📕 [Reference documentation for messaging tracers](https://dynatrace.github.io/OneAgent-SDK-for-C/group__messaging.html)  
> 📗 [Documentation on messaging tracers in the specification repository](https://github.com/Dynatrace/OneAgent-SDK#messaging)

<a name="using-the-dynatrace-oneagent-sdk-to-trace-custom-service-methods"></a>
<a name="trace-custom-service-methods"></a>

### Trace custom service methods

You can use the SDK to trace custom service methods. A custom service method is a meaningful part of your code that you want to trace but
that does not fit any other tracer.

```C
    /* create tracer */
    onesdk_tracer_handle_t const tracer = onesdk_customservicetracer_create(
        onesdk_asciistr("custom service method"),
        onesdk_asciistr("logical service name"));

    /* start tracer */
    onesdk_tracer_start(tracer);

    /* ... actually execute the custom service ... */

    /* set error information */
    if (something_went_wrong)
        onesdk_tracer_error(tracer, onesdk_asciistr("error type"), onesdk_asciistr("error message"));

    /* end and release tracer */
    onesdk_tracer_end(tracer);
```

> 📕 [Reference documentation for custom service tracers](https://dynatrace.github.io/OneAgent-SDK-for-C/group__customservice.html)

<a name="using-the-dynatrace-oneagent-sdk-to-add-custom-request-attributes"></a>
<a name="add-custom-request-attributes"></a>

### Add custom request attributes

You can add custom request attributes (key value pairs) to the currently traced service. Those attributes can then be used to e.g. search/filter requests in Dynatrace.
To add a custom request attribute, simply call one of the `onesdk_customrequestattribute_add_{type}` functions:

```C
    /* add simple values */
    onesdk_customrequestattribute_add_integer(onesdk_asciistr("account-id"), 42);
    onesdk_customrequestattribute_add_float(onesdk_asciistr("service-quality"), 0.707106);
    onesdk_customrequestattribute_add_string(onesdk_asciistr("region"), onesdk_asciistr("emea"));

    /* add multiple values with the same key to create a list */
    onesdk_customrequestattribute_add_integer(onesdk_asciistr("account-group"), 1);
    onesdk_customrequestattribute_add_integer(onesdk_asciistr("account-group"), 2);
    onesdk_customrequestattribute_add_integer(onesdk_asciistr("account-group"), 3);
```

This will add the custom request attributes to the currently traced service. If no tracer is active, the values will be discarded.

> 📕 [Reference documentation for custom request attributes](https://dynatrace.github.io/OneAgent-SDK-for-C/group__custom__request__attributes.html)

<a name="forking"></a>
<a name="using-the-dynatrace-oneagent-sdk-with-forked-child-processes-only-available-on-linux"></a>
<a name="using-the-dynatrace-oneagent-sdk-with-forked-child-processes-not-available-on-windows"></a>



<a name="retrieve-a-w3c-trace-context"></a>

### Retrieve a W3C trace context

This feature allows you to retrieve a W3C TraceContext trace ID and span ID referencing the current PurePath node,
as defined in <https://www.w3.org/TR/trace-context>.

This trace ID and span ID information is not intended for tagging and
context-propagation scenarios and primarily designed for log-enrichment use
cases. Use
[`onesdk_tracer_get_outgoing_dynatrace_string_tag`][refd_tracer_get_outgoing_dynatrace_string_tag],
[`onesdk_tracer_set_incoming_dynatrace_string_tag`][refd_tracer_set_incoming_dynatrace_string_tag],
[`onesdk_tracer_get_outgoing_dynatrace_byte_tag`][refd_tracer_get_outgoing_dynatrace_byte_tag],
[`onesdk_tracer_set_incoming_dynatrace_byte_tag`][refd_tracer_set_incoming_dynatrace_byte_tag]
for tagging traces (see the usage examples elsewhere in this document).

The following example shows how to print the current trace & span ID to stdout
in a format that works well with Dynatrace Log Monitoring
(see <https://www.dynatrace.com/support/help/shortlink/log-monitoring-log-enrichment> for more):

```c
/* The context of the active tracer will be printed, so one should be active.
   You can copy & paste from any other sample that starts a tracer. */
onesdk_tracer_start(tracer);

char trace_id[ONESDK_TRACE_ID_BUFFER_SIZE];
char span_id[ONESDK_SPAN_ID_BUFFER_SIZE];

/* A result code is returned, if and only if it is != ONESDK_SUCCESS, both IDs will consist of ASCII zeros only. */
onesdk_tracecontext_get_current(trace_id, sizeof(trace_id), span_id, sizeof(span_id));
fprintf(stderr, "[!dt dt.trace_id=%s,dt.span_id=%s] Some important log info.\n", trace_id, span_id);
```


> 📕 [Reference documentation for tracecontext][tcref]

[tcref]: https://dynatrace.github.io/OneAgent-SDK-for-C/group__tracecontext.html

[refd_tracer_get_outgoing_dynatrace_string_tag]: https://dynatrace.github.io/OneAgent-SDK-for-C/group__tracers.html#gad1a200fc7591163158458bb9938f4b29
[refd_tracer_get_outgoing_dynatrace_byte_tag]: https://dynatrace.github.io/OneAgent-SDK-for-C/group__tracers.html#ga7ae547cfdab36ab9464f82708fe9d414
[refd_tracer_set_incoming_dynatrace_string_tag]: https://dynatrace.github.io/OneAgent-SDK-for-C/group__tracers.html#ga332fb9c487786fb521f19d899079d0a6
[refd_tracer_set_incoming_dynatrace_byte_tag]: https://dynatrace.github.io/OneAgent-SDK-for-C/group__tracers.html#ga7a2c9b92ca453b575a1aa11cf5ab7ac8

## Using the Dynatrace OneAgent SDK with forked child processes (only available on Linux)

Some applications, especially web servers, use a concurrency model that is based on forked child processes. Typically a master process
is started which is responsible only for creating and managing child processes by means of forking. The child processes do the real work,
for example handling web requests.

The recommended way to use the SDK in such a scenario is as follows: You initialize the SDK in the master using the [`onesdk_initialize_2`
function][refd_initialize_2] passing the [`ONESDK_INIT_FLAG_FORKABLE` flag][refd_init_flag_forkable] in the flags argument. This way you
will not be able to use the SDK in the master process (attempts to do so will be ignored, if applicable with an error code), but all forked
child processes will share the same agent. This has a lower overhead, for example the startup of worker processes is not slowed down, and
the per-worker memory overhead is reduced.

![Diagram color legend](img/fork-legend.png)

Recommended, simple scenario:

![Diagram showing how to pre-initialize the SDK in the parent/master process](img/fork-simple.png)

Double/daemon forks (i.e., a process forks a single child and then terminates) are also supported, as long as the intermediate child does
not use the SDK. If possible, it is better to initialize the SDK only in the intermediate child, as illustrated in the diagram below, but
calling `onesdk_initialize_2(ONESDK_FORKABLE)` in the master's parent process is also supported (take especial care to not use any SDK
functions in the master then, not even `onesdk_agent_get_current_state`).

![Diagram showing how to pre-initialize the SDK in the parent/master process and double forking](img/double-fork.png)

There are some scenarios where you may not be able to use this feature:

* You have to use an older SDK or agent version which does not support the `onesdk_initialize_2` API, or
* You have no control over the master process, i.e. are unable to ensure that `onesdk_initialize_2` is called before the first fork in the
  master process.

In these cases, you can use the following workaround: Instead of initializing the SDK in the master process, you initialize it
(without the `ONESDK_INIT_FLAG_FORKABLE` flag) in each worker process. This has the consequence that each process is monitored
separately with its own agent. This is not recommended as each per-worker agent will have to establish and maintain its own
connection to Dynatrace, which results in higher overhead, especially at startup.

  ![Diagram showing how to initialize the SDK only in the child processes](img/init-child.png)

When you initialize the SDK using `onesdk_initialize` or without passing `ONESDK_INIT_FLAG_FORKABLE` to `onesdk_initialize_2`,
you may not use the SDK in forked child processes (attempts to do so will simply do nothing and may return the `ONESDK_ERROR_FORK_CHILD`
error code).

![Diagram showing how to pre-initialize the SDK in the parent/master process and double forking](img/fork-bad.png)

Example for SDK initialization with `ONESDK_INIT_FLAG_FORKABLE`:

```C
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <onesdk/onesdk.h>

onesdk_result_t g_onesdk_init_result = ONESDK_ERROR_NOT_INITIALIZED;

int worker_main();

int main(int argc, char** argv) {
    onesdk_stub_process_cmdline_args(argc, argv, 1);  /* optional: let the SDK process command line arguments   */
    onesdk_stub_strip_sdk_cmdline_args(&argc, argv);  /* optional: remove SDK command line arguments from argv  */

    /* Initialize SDK in forkable mode. */
    g_onesdk_init_result = onesdk_initialize_2(ONESDK_INIT_FLAG_FORKABLE);
    /* Assuming everything went well, the SDK is now in a _parent-initialized_ state. That means child processes
       that this process forks will be able to use the SDK with very little initialization overhead.
       (This process, the master, won't be able to use the SDK though.) */

    /* This process can now proceed and fork some workers, either all at once or on demand ... */

    static size_t const worker_count = 10;
    for (size_t i = 0; i < worker_count; ++i) {
        int const worker_pid = fork();
        if (worker_pid == -1) {
            perror("fork(2) failed");
            return 1;
        } else if (worker_pid == 0) {
            return worker_main();
        } else {
            /* ... remember worker PID/establish communication ... */
        }
    }

    /* ... maybe accept connections, dispatch work to the worker processes, wait(2) for them ... */

    /* ... unitl eventualls it's time to shut down. */

    /* Shut down SDK */
    if (g_onesdk_init_result == ONESDK_SUCCESS)
        onesdk_shutdown();

    return 0;
}

int worker_main() {
    /* Assuming everything went well in the parent process and `g_onesdk_init_result == ONESDK_SUCCESS`,
       this process has inherited the SDK in a _pre-initialized_ state.
       (The initialization state changes from _parent-initialized_ to _pre-initialized_ while forking.)
       That means the SDK initialization will automatically be completed when this process begins to use
       the SDK, e.g. when the first tracer is created... */

    onesdk_tracer_handle_t const tracer = onesdk_sometracer_create(
        onesdk_asciistr("some argument"),
        onesdk_asciistr("some other argument"));

    /* The SDK should now be fully initialized and the tracer should have been created successfully. */

    onesdk_tracer_start(tracer);
    /* ... do the actual work ... */
    onesdk_tracer_end(tracer);

    /* Since we inherited a _pre-initialized_ SDK state we have to call `onesdk_shutdown`. */
    if (g_onesdk_init_result == ONESDK_SUCCESS)
        onesdk_shutdown();
    return 0;
}
```

<a name="fork-trouble"></a>

As the behavior of the SDK is sometimes complicated to understand with forking,
the [`onesdk_agent_get_fork_state`][refd_agent_get_fork_state] function is provided
which can help you examine the state the SDK currently is in regarding forking.
Basic usage is shown in sample1. You may want to call it before and after an SDK operation
that behaves/fails unexpectly if you use [`ONESDK_INIT_FLAG_FORKABLE`][refd_init_flag_forkable].

> 📕 Reference documentation for:
> * [initialization and shutdown](https://dynatrace.github.io/OneAgent-SDK-for-C/group__init.html)
> * [`onesdk_agent_get_fork_state`][refd_agent_get_fork_state]

[refd_agent_get_fork_state]: https://dynatrace.github.io/OneAgent-SDK-for-C/group__misc.html#ga77260efaf63455969962e05b6b170135
[refd_initialize_2]: https://dynatrace.github.io/OneAgent-SDK-for-C/group__init.html#gac0681af704ba7e6404c3f67f582ee4db
[refd_init_flag_forkable]: https://dynatrace.github.io/OneAgent-SDK-for-C/group__init.html#ga732bf07f0e190264baf29f3a1c22cc4a

<a name="troubleshooting"></a>

## Troubleshooting

This section describes two broad categories of problems and how to debug them. The SDK has extensive
logging capabilites which will often point you quite directly at what the issue is.

<a name="troubleshooting-init"></a>

### Problems with initializing the SDK

Main symptom: [`onesdk_initialize`][refd_initialize] / [`onesdk_initalize_2`][refd_initialize_2] returning an error code.

If the SDK stub cannot load or initialize the agent module (see output of sample1), you can set the SDK stub's logging level to activate
logging of what happengs during initialization by either

- setting the environment variable `DT_LOGLEVELSDK=FINEST`; or
- calling [`onesdk_stub_set_logging_level(ONESDK_LOGGING_LEVEL_FINEST)`][refd_stub_set_logging_level]; or
- if your program passes command line arguments to the SDK (see [`onesdk_stub_process_cmdline_args`][refd_process_cmdline_args]), you can
  use the command line argument `--dt_loglevelsdk=FINEST`.

Whichever method you choose (usually setting the environment variable is the easiest), make sure to apply it _before_ calling [`onesdk_initialize`][refd_initialize] / [`onesdk_initalize_2`][refd_initialize_2].

Once you have enabled logging, log output of the stub will be written to `stderr` by default. Refer to the [documentation for
`onesdk_stub_set_logging_callback`][refd_stub_set_logging_callback] if you need to process stub log messages in another way.

If initialization fails, [`ONESDK_ERROR_LOAD_AGENT`][refd_error_load_agent] (numerical code 2952658951, -1342308345 or 0xaffe0007,
error message "Could not load agent.").
is the most common error code. These are the two most common causes we have observed for this issue:

1. The OneAgent is not installed on the host where the program runs. Install the OneAgent and restart the program.
2. The program being run is started with a debugger.
   The OneAgent will not inject in that case. Start the program without debugger.
   You may still attach the debugger later, once the program is running.

[refd_error_load_agent]: https://dynatrace.github.io/OneAgent-SDK-for-C/onesdk__common_8h.html#aa120990f128eb02bce1e88a489a7f398

<a name="troubleshooting-postinit"></a>

### Problems occuring after initialization

For any problems you encounter after successful initialization
(for example, no paths are shown in the UI, or you wonder why a function returns `ONESDK_INVALID_HANDLE` or another error code),
it is best to check for messages from the agent logging callbacks: see
[`onesdk_agent_set_warning_callback`][refd_agent_set_warning_callback] and
[`onesdk_agent_set_verbose_callback`][refd_agent_set_verbose_callback] in the reference documentation or in sample1.

You can also check the agent log files (see the Dynatrace documentation for where to find them, e.g., on
[Linux](https://www.dynatrace.com/support/help/shortlink/oneagent-files-linux#log-files) or
[Windows](https://www.dynatrace.com/support/help/shortlink/oneagent-files-windows#log-files)).
You can increase the agent log level by setting the environment variable `DT_LOGLEVELFILE={level}` or passing the command line argument `--dt_loglevelfile={level}` to the SDK.
This will provide additional debug information in agent log file. (Alternatively you can use `DT_LOGLEVELCON={level}` or `--dt_loglevelcon={level}` if you want to receive agent log output via `stderr`.)

Lastly, in some situations, the [`onesdk_agent_get_current_state`][refd_agent_get_current_state] function may provide additional insights. See sample1 for a usage example.

Special situations can arise when forking is involved, see [the section on forking](#forking).

[refd_stub_set_logging_level]: https://dynatrace.github.io/OneAgent-SDK-for-C/group__init.html#ga85b610bcd0d771fe641a1cd1ef03fd13
[refd_stub_set_logging_callback]: https://dynatrace.github.io/OneAgent-SDK-for-C/group__init.html#ga68fd905f95b1fdc05b7d45e5a419934d
[refd_agent_set_warning_callback]: https://dynatrace.github.io/OneAgent-SDK-for-C/group__misc.html#ga31c7f418f4b3515097434f8df6810cad
[refd_agent_set_verbose_callback]: https://dynatrace.github.io/OneAgent-SDK-for-C/group__misc.html#ga1324a8c95a407255e838641e8a8f03a9
[refd_agent_get_current_state]: https://dynatrace.github.io/OneAgent-SDK-for-C/group__misc.html#gab45441c798009a1bad93480e8476a1e4

<a name="requirements"></a>

## Requirements

- Dynatrace OneAgent needs to be installed on the system that is to be monitored (required versions see below)
- The supported environments are:
    + Windows on x86
    + Linux on x86. musl libc is currently not supported (e.g. used in Alpine Linux)
    + Solaris on SPARC (x86 is *not* supported).
      Support for this platform is new since version 1.3.2 of the SDK.

  Refer to https://www.dynatrace.com/support/help/shortlink/supported-technologies#operating-systems for the exact versions supported by
  OneAgent. Note that only the subset of operating systems and processor architectures explicitly listed both in this README and the link
  are supported by the SDK.

<a name="compatibility-of-dynatrace-oneagent-sdk-for-cc-releases-with-oneagent-releases"></a>
<a name="version-support-and-compatibility-table"></a>

### Version support and compatibility table

|OneAgent SDK for C/C++|Dynatrace OneAgent|Support status or EOL date|
|:---------------------|:-----------------|:-------------|
|1.7.1                 |>=1.251           |Supported     |
|1.6.1                 |>=1.179           |Supported     |
|1.5.1                 |>=1.179           |Deprecated with support ending 2023-07-01 |
|1.4.1                 |>=1.161           |Deprecated with support ending 2023-07-01 |
|1.3.2                 |>=1.159           |Deprecated with support ending 2023-07-01 |
|1.3.1                 |>=1.151           |Deprecated with support ending 2023-07-01 |
|1.2.0                 |>=1.147           |Deprecated with support ending 2023-07-01 |
|1.1.0                 |>=1.141           |Deprecated with support ending 2023-07-01 |
|1.0.0                 |>=1.133           |Deprecated with support ending 2023-07-01 |

Note that this table only states the support status of the mentioned OneAgent SDK for C/C++ version,
not the OneAgent itself.

You should always try to update to the latest version if possible, as it may contain reliability or security
improvements (see respective release notes). This is especially true if an end of support date for your version
is announced.

<a name="help"></a>
<a name="help--support"></a>

## Help & Support

The Dynatrace OneAgent SDK for C/C++ is fully supported by Dynatrace. For the support status of a particular version, refer to the [version
support and compatibility table](#version-support-and-compatibility-table). For detailed support policy see [Dynatrace OneAgent SDK
help](https://github.com/Dynatrace/OneAgent-SDK#help).


<a name="read-the-manual"></a>

### Read the manual

* The most recent version of the reference documentation can be viewed at https://dynatrace.github.io/OneAgent-SDK-for-C/
* A high level documentation/description of OneAgent SDK concepts is available at https://github.com/Dynatrace/OneAgent-SDK/.
* Of course, this README also contains lots of useful information.

<a name="let-us-help-you"></a>

### Let us help you

**Get Help**
* Ask a question in the [product forums](https://community.dynatrace.com/t5/Using-Dynatrace/ct-p/UsingDynatrace)
* Read the [product documentation](https://www.dynatrace.com/support/help/)

**Open a [GitHub issue](https://github.com/Dynatrace/OneAgent-SDK-for-C/issues) to:**
* Report minor defects like typos
* Ask any questions related to the community effort

SLAs don't apply for GitHub tickets.

**Customers can open a ticket on the [Dynatrace support portal](https://one.dynatrace.com/hc/) to:**
* Get support from the Dynatrace technical support engineering team
* Manage and resolve product related technical issues

SLAs apply according to the customer's support level.

<a name="release-notes"></a>

## Release Notes

See also <https://github.com/Dynatrace/OneAgent-SDK-for-C/releases>.

|Version|Description                                                                                                             |
|:------|:-----------------------------------------------------------------------------------------------------------------------|
|1.7.1  |Add W3C trace context support for log enrichment (not for tagging/linking). <br> Announce deprecation of versions < 1.6.1 |
|1.6.1  |Deprecate metrics-related APIs. <br> Don't look for agent module in `PATH/LD_LIBRARY_PATH/...`, disallow relative `DT_HOME` on Windows (prevent DLL hijacking)   |
|1.5.1  |Added metrics APIs (preview feature), improved logging callback APIs, new API to query fork state                       |
|1.4.1  |Added custom service tracers and messaging tracers                                                                      |
|1.3.2  |Support for Solaris SPARC                                                                                               |
|1.3.1  |Support for monitoring forked child processes, added new API to check agent version compatibility                       |
|1.2.0  |Added in-process linking, added custom request attributes, added outgoing web request tracers                           |
|1.1.0  |Added incoming web request tracers, added row count & round trip count for DB request tracers                           |
|1.0.0  |Initial version                                                                                                         |
