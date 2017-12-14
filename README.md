# Dynatrace OneAgent SDK for C/C++

SDK that allows Dynatrace customers to instrument native applications.

The SDK package includes
- The libraries and header files necessary for instrumenting applications
- A simple sample application
- Reference documentation

The most recent version of the reference documentation is also available online at https://dynatrace.github.io/OneAgent-SDK-for-C/

## Features

* Trace any remote call end to end across processes and different programming languages
The SDK is compatible with the respective Java and Node.js SDK
* Trace any SQL based database call

## Initializing the Dynatrace OneAgent SDK 

Paul: add a sample main here that just deals with the header files, and init and shutdown of the agent

## Building and linking against Dynatrace OneAgent SDK 

Compile and link info

## Using the Dynatrace OneAgent SDK to trace remote calls

You can use the SDK to trace propritary IPC communication from one process to the other. This will enable you to see full Service Flow, PurePath and Smartscape topology for remoting technologies that Dynatrace is not aware of.

Instrumenting an outbound remote call:

Outbound call code

Instrumenting an incoming remote call:

## Using the Dynatrace OneAgent SDK to trace SQL based database calls.


## Troubleshooting
As long as the ADK can't connect to agent (see output of sample), you might set the following system property to print debug information to standard out:
	
	-Dcom.dynatrace.oneagent.adk.debug=true

As soon as ADK is active, but no paths are shown in UI or AppMon Client, enable the agent debug flag:
	
	debugTaggingAdkJava=true

This will provide additional debug information in agent log.

Additionally ensure, that you have set an `LoggingCallback` in your application. For usage see class `StdErrLoggingCallback` in `remotecall-server` module.

## OneAgent ADK Requirements
- OneAgent (supported versions see below; AppMon classic agent isn't supported)

## Compatibility Dynatrace OneAgent SDK for C releases with OneAgent releases
|OneAgent SDK for C|Dynatrace OneAgent|AppMon Agent|
|:------|:--------|:--------|
|1.0.0  |>=1.133  |>=7.1    |

## Release Notes (OneAgent ADK sample applications)
|Version|Date|Description|
|:------|:----------|:--------------|
|1.0.0  |09.2017    |Initial version|
