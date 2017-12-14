# Dynatrace OneAgent SDK for C/C++

This SDK allows Dynatrace customers to instrument native applications.


## Package contents

The SDK package includes
- The libraries and header files necessary for instrumenting applications
- A simple sample application
- Reference documentation

The most recent version of the reference documentation is also available online at https://dynatrace.github.io/OneAgent-SDK-for-C/


## Features

- Trace any remote call end to end across processes and different programming languages.
  The SDK is compatible with the respective Java and Node.js SDK.
- Trace any SQL based database call.


## Initializing the Dynatrace OneAgent SDK 

```C
#include <onesdk/onesdk.h>

int main(int argc, char** argv) {
    onesdk_stub_process_cmdline_args(argc, argv, 1);  /* Optional: Let the SDK process command line arguments   */
    onesdk_stub_strip_sdk_cmdline_args(&argc, argv);  /* Optional: Remove SDK command line arguments from argv  */

    /* Initialize SDK */
    onesdk_result_t const onesdk_init_result = onesdk_initialize();

    /* ... use SDK ... */

    /* Shut down SDK */
    if (onesdk_init_result == ONESDK_SUCCESS)
        onesdk_shutdown();

    return 0;
}
```

## Building and linking against the Dynatrace OneAgent SDK 

The SDK doesn't have to be compiled, you only need to link your application to the SDK libraries.

### Using CMake

If you use CMake to generate build files for your application, you should be able to use the provided `onesdk-config.cmake` script ala

```CMake
include("path/to/sdk-package/onesdk-config.cmake")
target_link_libraries(your_application onesdk_static)
```

### Auto-linking with Visual Studio

If you use Visual Studio to build a Windows application, you can use the SDK's auto-linking feature. To do this, simply define the
preprocessor macro `ONESDK_AUTO_LINK` before including any SDK header file. Aside from that, you only have to add the appropriate "lib"
path.

### Other build systems

If you use another build system you have to configure it to
- add a "lib" path to the appropriate platform subdirectory under `lib` (e.g. `path/to/sdk-package/lib/linux-x86_64`)
- link the appropriate library (e.g. `libonesdk_static.a`)

The SDK contains code that dynamically loads the agent library (`.dll`/`.so`/...), so depending on your platform you might need to link
additional libraries (e.g. under Linux you would typically add `-ldl` to the linker command line).

On Windows, when using Visual Studio 2015 or later, you also have to link `legacy_stdio_definitions.lib`.


## Using the Dynatrace OneAgent SDK to trace remote calls

You can use the SDK to trace proprietary IPC communication from one process to the other. This will enable you to see full Service Flow,
PurePath and Smartscape topology for remoting technologies that Dynatrace is not aware of.

Instrumenting an outbound remote call:

```C
    /* create tracer */
    onesdk_tracer_handle_t const tracer = onesdk_outgoingremotecalltracer_create(
        onesdk_asciistr("service method"),
        onesdk_asciistr("service name"),
        onesdk_asciistr("service endpoint"),
        ONESDK_CHANNEL_TYPE_TCP_IP,           /* channel type     */
        onesdk_asciistr("localhost:12345")    /* channel endpoint */ );

    /* start tracer */
    onesdk_tracer_start(tracer);

    /* get byte representation of tag */
    onesdk_size_t byte_tag_size = 0;
    onesdk_tracer_get_outgoing_dynatrace_byte_tag(tracer, NULL, 0, &byte_tag_size);
    void* byte_tag = NULL;
    if (byte_tag_size != 0) {
        byte_tag = malloc(byte_tag_size);
        if (byte_tag != NULL)
            onesdk_tracer_get_outgoing_dynatrace_byte_tag(tracer, byte_tag, byte_tag_size, NULL);
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
    /* create tracer */
    onesdk_tracer_handle_t const tracer = onesdk_incomingremotecalltracer_create(
        onesdk_asciistr("service method"),
        onesdk_asciistr("service name"),
        onesdk_asciistr("service endpoint"));

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

## Using the Dynatrace OneAgent SDK to trace SQL based database calls.

To trace database requests you need a database info object which stores the information about your database which does not change between
individual requests. This will typically be created somewhere in your initialization code (after initializing the SDK):

```C
onesdk_databaseinfo_handle_t db_info_handle = ONESDK_INVALID_HANDLE;

/* ... */

    db_info_handle = onesdk_databaseinfo_create(
        onesdk_asciistr("database name"),
        onesdk_asciistr("database type"),
        ONESDK_CHANNEL_TYPE_TCP_IP,           /* channel type     */
        onesdk_asciistr("localhost:12345")    /* channel endpoint */ );
```

Then you can trace the SQL database requests:

```C
    /* create tracer */
    onesdk_tracer_handle_t const tracer = onesdk_databaserequesttracer_create_sql(
        db_info_handle,
        onesdk_asciistr("SELECT 42;"));

    /* start tracer */
    onesdk_tracer_start(tracer);

    /* ... perform the database request ... */

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

## Troubleshooting

As long as the SDK can't connect to the agent (see output of sample), you might set the following system property to print debug information
to standard out:

    -Dcom.dynatrace.oneagent.adk.debug=true

As soon as the SDK is active, but no paths are shown in the UI or AppMon Client, enable the agent debug flag:

    debugTaggingAdkJava=true

This will provide additional debug information in agent log.

Additionally ensure, that you have set an `LoggingCallback` in your application. For usage see class `StdErrLoggingCallback` in
`remotecall-server` module.


## OneAgent SDK Requirements

- OneAgent (supported versions see below; AppMon classic agent isn't supported)


## Compatibility Dynatrace OneAgent SDK for C releases with OneAgent releases

|OneAgent SDK for C|Dynatrace OneAgent|AppMon Agent|
|:------|:--------|:------------|
|1.0.0  |>=1.133  |not supported|


## Release Notes (OneAgent SDK sample applications)

|Version|Date|Description|
|:------|:----------|:--------------|
|1.0.0  |09.2017    |Initial version|
