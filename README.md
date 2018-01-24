**Disclaimer: This SDK is currently in early access and still work in progress.**

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
  The SDK is compatible with other OneAgent SDKs and OneAgents in general.
- Trace any SQL-based database call.


## Documentation

The reference documentation is included in this package. The most recent version is also available online at https://dynatrace.github.io/OneAgent-SDK-for-C/

A high level documentation/description of SDK concepts is available at https://github.com/Dynatrace/OneAgent-SDK/


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
        onesdk_asciistr("database name"),     /* the name of the database that you connect to */
        onesdk_asciistr("database type"),     /* the type of the database (e.g. "sqlite", "MySQL", "Oracle", "DB2") */
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

If the SDK stub cannot load or initialize the agent module (see output of sample1), you can set the SDK stub's logging level to activate logging by either
- calling `onesdk_stub_set_logging_level(ONESDK_LOGGING_LEVEL_{LEVEL})`
- setting the environment variable `DT_LOGLEVELSDK={level}`
- if your program passes command line arguments to the SDK (see `onesdk_stub_process_cmdline_args`), you can use the command line argument `--dt_loglevelsdk={level}`

Once you have enabled logging, log output of the stub will be written to `stderr` by default. See documentation for `onesdk_stub_set_logging_callback` if you need to process stub log messages in another way.

If the SDK agent is active, but no paths are shown in the UI, check the agent log files.
You can increase the agent log level by setting the environment variable `DT_LOGLEVELFILE={level}` or passing the command line argument `--dt_loglevelfile={level}` to the SDK.
This will provide additional debug information in agent log file. (Alternatively you can use `DT_LOGLEVELCON={level}` or `--dt_loglevelcon={level}` if you want to receive agent log output via `stdout`.)

To troubleshoot SDK issues you can also use the SDK's agent logging callback - see `onesdk_agent_set_logging_callback` in the reference documentation.


## OneAgent SDK Requirements

- Dynatrace OneAgent needs to be installed on the system that is to be monitored (supported versions see below) 
- Supported environments include all Windows or Linux x86 environments
- musl libc is currently not supported


## Compatibility Dynatrace OneAgent SDK for C releases with OneAgent releases

|OneAgent SDK for C|Dynatrace OneAgent|
|:-----------------|:-----------------|
|1.0.0             |>=1.133           |


## Support

The Dynatrace OneAgent SDK is currently in early access. Please report tickets via the [GitHub issue tracker](https://github.com/Dynatrace/OneAgent-SDK-for-C/issues).


## Release Notes

|Version|Date|Description|
|:------|:----------|:--------------|
|1.0.0  |01.2018    |Initial version|
