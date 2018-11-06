**Disclaimer: This SDK is currently in beta and still work in progress.**

# Dynatrace OneAgent SDK for C/C++

This SDK enables Dynatrace customers to extend request level visibility into any native process. The SDK is C based and thus can be used in any C or C++ application. It can also be used in other languages via language bindings.
In order to use the development kit you need to have access to the source code of the application in question.


## Package contents

The SDK package includes
- The libraries and header files necessary for instrumenting applications
- A simple sample application
- Reference documentation


## Features

- Trace any remote call end-to-end across processes and different programming languages.
- Trace any SQL-based database call.
- Trace incoming and outgoing web requests.
- Trace asynchronous processing within one process.
- Add custom request attributes to any currently traced service.

When tracing incoming or outgoing calls or requests, this SDK is compatible with other OneAgent SDKs and OneAgents in general.


## Documentation

The reference documentation is included in this package. The most recent version is also available online at https://dynatrace.github.io/OneAgent-SDK-for-C/

A high level documentation/description of OneAgent SDK concepts is available at https://github.com/Dynatrace/OneAgent-SDK/


## Getting started

To start using the Dynatrace OneAgent SDK for C/C++, simply download the latest source archive from [releases](https://github.com/Dynatrace/OneAgent-SDK-for-C/releases).
The source archive also includes all necessary artifacts (e.g. the static and dynamic library files), so this is all you need.
Extract the archive to a local folder on your machine and then add the appropriate "include" and "lib" paths to your build system.


## Building and linking against the Dynatrace OneAgent SDK

The SDK doesn't have to be compiled, you only need to link your application to the SDK libraries.

### Using CMake

If you use CMake to generate build files for your application, you should be able to use the provided `onesdk-config.cmake` script ala

```CMake
include("path/to/sdk-package/onesdk-config.cmake")
target_link_libraries(your_application onesdk_static)
```

### Auto-linking with Visual Studio

If you use Visual Studio to build a Windows application, you can use the SDK's auto-linking feature. To do this, simply define the preprocessor macro `ONESDK_AUTO_LINK` before including any SDK header file.
Aside from that, you only have to add the appropriate "include" and "lib" paths.

### Other build systems

If you use another build system you have to configure it to
- add an "include" path to `path/to/sdk-package/include`
- add a "lib" path to the appropriate platform subdirectory under `lib` (e.g. `path/to/sdk-package/lib/linux-x86_64`)
- link the appropriate library (e.g. `libonesdk_static.a`)

The SDK contains code that dynamically loads the agent library (`.dll`/`.so`/...), so depending on your platform you might need to link
additional libraries (e.g. under Linux you would typically add `-ldl` to the linker command line).

On Windows, when using Visual Studio 2015 or later, you also have to link `legacy_stdio_definitions.lib`.

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


## Initializing the Dynatrace OneAgent SDK

```C
#include <onesdk/onesdk.h>

int main(int argc, char** argv) {
    onesdk_stub_process_cmdline_args(argc, argv, 1);  /* optional: let the SDK process command line arguments   */
    onesdk_stub_strip_sdk_cmdline_args(&argc, argv);  /* optional: remove SDK command line arguments from argv  */

    /* Initialize SDK */
    onesdk_result_t const onesdk_init_result = onesdk_initialize();

    /* ... use SDK ... */

    /* Shut down SDK */
    if (onesdk_init_result == ONESDK_SUCCESS)
        onesdk_shutdown();

    return 0;
}
```


## Using the Dynatrace OneAgent SDK to trace remote calls

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


## Using the Dynatrace OneAgent SDK to trace SQL based database calls

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


## Using the Dynatrace OneAgent SDK to trace incoming web requests

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


## Using the Dynatrace OneAgent SDK to trace outgoing web requests

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


## Using the Dynatrace OneAgent SDK to trace asynchronous activities

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


## Using the Dynatrace OneAgent SDK to add custom request attributes

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


## Using the Dynatrace OneAgent SDK with forked child processes (not available on Windows)

Some applications, especially web servers, use a concurrency model that is based on forked child processes. Typically a master process
is started which is responsible only for creating and managing child processes by means of forking. The child processes do the real work,
for example handling web requests.

The recommended way to use the SDK in such a scenario is as follows: You initialize the SDK in the master using the `onesdk_initialize_2`
function passing the `ONESDK_INIT_FLAG_FORKABLE` flag in the flags argument. This way you will not be able to use the SDK in the
master process (attempts to do so will be ignored, if applicable with an error code), but all forked child processes will share the same
agent. This has a lower overhead, for example the startup of worker processes is not slowed down, and the per-worker memory overhead is
reduced.

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


## Troubleshooting

If the SDK stub cannot load or initialize the agent module (see output of sample1), you can set the SDK stub's logging level to activate logging by either
- calling `onesdk_stub_set_logging_level(ONESDK_LOGGING_LEVEL_{LEVEL})`
- setting the environment variable `DT_LOGLEVELSDK={level}`
- if your program passes command line arguments to the SDK (see `onesdk_stub_process_cmdline_args`), you can use the command line argument `--dt_loglevelsdk={level}`

Once you have enabled logging, log output of the stub will be written to `stderr` by default. See documentation for `onesdk_stub_set_logging_callback` if you need to process stub log messages in another way.

If the SDK agent is active, but no paths are shown in the UI, check the agent log files.
You can increase the agent log level by setting the environment variable `DT_LOGLEVELFILE={level}` or passing the command line argument `--dt_loglevelfile={level}` to the SDK.
This will provide additional debug information in agent log file. (Alternatively you can use `DT_LOGLEVELCON={level}` or `--dt_loglevelcon={level}` if you want to receive agent log output via `stderr`.)

To troubleshoot SDK issues you can also use the SDK's agent logging callback - see `onesdk_agent_set_logging_callback` in the reference documentation.


## Dynatrace OneAgent SDK for C/C++ Requirements

- Dynatrace OneAgent needs to be installed on the system that is to be monitored (supported versions see below)
- Supported environments include all Windows or Linux x86 environments
- musl libc is currently not supported (e.g. used in Alpine Linux)


## Compatibility of Dynatrace OneAgent SDK for C/C++ releases with OneAgent releases

|OneAgent SDK for C/C++|Dynatrace OneAgent|
|:---------------------|:-----------------|
|1.3.1                 |>=1.151           |
|1.2.0                 |>=1.147           |
|1.1.0                 |>=1.141           |
|1.0.0                 |>=1.133           |


## Support

The Dynatrace OneAgent SDK is currently in early access. Please report tickets via the [GitHub issue tracker](https://github.com/Dynatrace/OneAgent-SDK-for-C/issues).


## Release Notes

|Version|Description                                                                                                             |
|:------|:-----------------------------------------------------------------------------------------------------------------------|
|1.3.1  |Support for monitoring forked child processes                                                                           |
|1.2.0  |Added in-process linking, added custom request attributes, added outgoing web request tracers                           |
|1.1.0  |Added incoming web request tracers, added row count & round trip count for DB request tracers                           |
|1.0.0  |Initial version                                                                                                         |
