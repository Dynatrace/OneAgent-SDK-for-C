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

#ifndef ONESDK_H_INCLUDED
#define ONESDK_H_INCLUDED

/** @file
    @brief Main include file for the SDK - includes everything.
*/

/** @mainpage

    If you are new to the OneAgent SDK for C/C++ check out the README on GitHub: https://github.com/Dynatrace/OneAgent-SDK-for-C.

    See @ref init for information about initializing the SDK.

    See @ref onesdk_agent.h and the various <a href="modules.html">modules</a> for information about how to instrument an application.

    A high level documentation/description of programming language-independent SDK concepts is available at
    https://github.com/Dynatrace/OneAgent-SDK/
*/

/*========================================================================================================================================*/

/* IWYU pragma: begin_exports */
#include "onesdk/onesdk_init.h"
#include "onesdk/onesdk_agent.h"
#include "onesdk/onesdk_string.h"
#include "onesdk/onesdk_metrics.h"
/* IWYU pragma: end_exports */

#endif /* ONESDK_H_INCLUDED */
