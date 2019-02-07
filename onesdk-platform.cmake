#
# Copyright 2017-2018 Dynatrace LLC
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
# http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# Determine CPU architecture
if (   CMAKE_C_COMPILER_ARCHITECTURE_ID   MATCHES "(x86|X86|x64|X64|amd64|AMD64|i386|i686)"
    OR CMAKE_CXX_COMPILER_ARCHITECTURE_ID MATCHES "(x86|X86|x64|X64|amd64|AMD64|i386|i686)"
    OR CMAKE_SYSTEM_PROCESSOR             MATCHES "(x86|X86|x64|X64|amd64|AMD64|i386|i686)"
    )
    set(onesdk_arch "x86")
elseif (   CMAKE_C_COMPILER_ARCHITECTURE_ID   MATCHES "sparc"
        OR CMAKE_CXX_COMPILER_ARCHITECTURE_ID MATCHES "sparc"
        OR CMAKE_SYSTEM_PROCESSOR MATCHES "sparc")
    set(onesdk_arch "sparc")
else ()
    message(SEND_ERROR "CMAKE_C_COMPILER_ARCHITECTURE_ID = ${CMAKE_C_COMPILER_ARCHITECTURE_ID}")
    message(SEND_ERROR "CMAKE_CXX_COMPILER_ARCHITECTURE_ID = ${CMAKE_CXX_COMPILER_ARCHITECTURE_ID}")
    message(SEND_ERROR "CMAKE_SYSTEM_PROCESSOR = ${CMAKE_SYSTEM_PROCESSOR}")
    message(FATAL_ERROR "ONESDK: Unsupported architecture (see above).")
endif ()

# Determine bitness
if (CMAKE_C_SIZEOF_DATA_PTR EQUAL 4 OR CMAKE_CXX_SIZEOF_DATA_PTR EQUAL 4)
    set(onesdk_bits 32)
elseif (CMAKE_C_SIZEOF_DATA_PTR EQUAL 8 OR CMAKE_CXX_SIZEOF_DATA_PTR EQUAL 8)
    set(onesdk_bits 64)
else ()
    message(SEND_ERROR "CMAKE_C_SIZEOF_DATA_PTR = ${CMAKE_C_SIZEOF_DATA_PTR}")
    message(SEND_ERROR "CMAKE_CXX_SIZEOF_DATA_PTR = ${CMAKE_CXX_SIZEOF_DATA_PTR}")
    message(FATAL_ERROR "ONESDK: Unknown/unsupported bitness (see above).")
endif ()

# Determine platform
if (WIN32)
    set(onesdk_platform "windows")
elseif (CMAKE_C_PLATFORM_ID STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(onesdk_platform "linux")
elseif (CMAKE_C_PLATFORM_ID STREQUAL "SunOS" OR CMAKE_SYSTEM_NAME STREQUAL "SunOS")
    set(onesdk_platform "sunos")
else ()
    message(SEND_ERROR "CMAKE_C_PLATFORM_ID = ${CMAKE_C_PLATFORM_ID}")
    message(SEND_ERROR "CMAKE_SYSTEM_NAME = ${CMAKE_SYSTEM_NAME}")
    message(FATAL_ERROR "ONESDK: Unsupported platform (see above).")
endif ()

set(onesdk_static_extra_libs "")
if (MSVC_VERSION AND NOT MSVC_VERSION LESS 1900)
    list(APPEND onesdk_static_extra_libs "legacy_stdio_definitions.lib")
endif()

set(onesdk_cfg_name "${onesdk_platform}-${onesdk_arch}_${onesdk_bits}")
set(onesdk_lib_subdir "lib/${onesdk_cfg_name}")

