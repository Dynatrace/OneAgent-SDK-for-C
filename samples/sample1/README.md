# SDK sample application

This C++ sample application shows how to initialize the SDK and use it to trace various operations. It's written in C\+\+11, but it doesn't
use any fancy C\+\+ wrappers for SDK types/functions, and of course the general principles of using the SDK are the same in every language.
Meaning you should be able to apply what you see here to basically any language, as long as it can call native code.

## Building

This sample uses `CMake` as build file generator. Simply invoking `CMake` on this directory should be enough to create suitable build files.

## Where to find what

- For initialization of the SDK, see `main.cpp`
- For tracing outgoing web requests, see `web_client.h`
- For tracing incoming web requests, see `web_service.h`
- For tracing outgoing remote calls, see `transformer_service_client_proxy.h`
- For tracing incoming remote calls, see `transformer_service_dispatcher.h`
- For tracing database operations, see `config_database.h`
- For using in-process links, see `web_service_impl.h`
- For adding custom request attributes, see `transformer_service.h`
