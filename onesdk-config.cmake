#
# Copyright 2017 Dynatrace LLC
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

cmake_policy(PUSH)
cmake_policy(VERSION 2.8.12)
cmake_minimum_required(VERSION 2.8.12)

project(onesdk)

get_filename_component(onesdk_path "${CMAKE_CURRENT_LIST_FILE}" PATH)

# determine CPU architecture
if (CMAKE_C_COMPILER_ARCHITECTURE_ID MATCHES "X86" OR CMAKE_C_COMPILER_ARCHITECTURE_ID MATCHES "x64")
    set(onesdk_arch "x86")
elseif (CMAKE_CXX_COMPILER_ARCHITECTURE_ID MATCHES "X86" OR CMAKE_CXX_COMPILER_ARCHITECTURE_ID MATCHES "x64")
    set(onesdk_arch "x86")
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "x86" OR CMAKE_SYSTEM_PROCESSOR MATCHES "AMD64")
    set(onesdk_arch "x86")
else ()
    message(FATAL_ERROR "ONESDK: Unsupported architecture (${CMAKE_C_COMPILER_ARCHITECTURE_ID},${CMAKE_CXX_COMPILER_ARCHITECTURE_ID},${CMAKE_SYSTEM_PROCESSOR})")
endif ()

# determine bitness
if (CMAKE_C_SIZEOF_DATA_PTR EQUAL 4)
    set(onesdk_bits 32)
elseif (CMAKE_C_SIZEOF_DATA_PTR EQUAL 8)
    set(onesdk_bits 64)
elseif (CMAKE_CXX_SIZEOF_DATA_PTR EQUAL 4)
    set(onesdk_bits 32)
elseif (CMAKE_CXX_SIZEOF_DATA_PTR EQUAL 8)
    set(onesdk_bits 64)
else ()
    message(FATAL_ERROR "ONESDK: Unknown/unsupported bitness (${CMAKE_C_SIZEOF_DATA_PTR,CMAKE_CXX_SIZEOF_DATA_PTR})")
endif ()

# determine platform
if (WIN32)
    set(onesdk_platform "windows")
elseif (CMAKE_C_PLATFORM_ID MATCHES "Linux")
    set(onesdk_platform "linux")
elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(onesdk_platform "linux")
else ()
    message(FATAL_ERROR "ONESDK: Unsupported platform (${CMAKE_C_PLATFORM_ID},${CMAKE_SYSTEM_NAME})")
endif ()

# define platform dependent strings
if (onesdk_platform MATCHES "windows")
    set(onesdk_lib_prefix "")
    set(onesdk_linklib_suffix ".lib")
    set(onesdk_shlib_suffix ".dll")
    set(onesdk_implib_suffix ".lib")
elseif (onesdk_platform MATCHES "linux")
    set(onesdk_lib_prefix "lib")
    set(onesdk_linklib_suffix ".a")
    set(onesdk_shlib_suffix ".so")
    set(onesdk_implib_suffix OFF)
else ()
    message(FATAL_ERROR "ONESDK: Unsupported onesdk_platform (${onesdk_platform})")
endif ()

set(onesdk_cfg_name "${onesdk_platform}-${onesdk_arch}_${onesdk_bits}")
set(onesdk_lib_path "${onesdk_path}/lib/${onesdk_cfg_name}")

# TARGET onesdk_static

add_library(onesdk_static STATIC IMPORTED)
set_property(TARGET onesdk_static PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${onesdk_path}/include")
set_property(TARGET onesdk_static PROPERTY IMPORTED_LOCATION "${onesdk_lib_path}/${onesdk_lib_prefix}onesdk_static${onesdk_linklib_suffix}")
if (MSVC_VERSION AND NOT MSVC_VERSION LESS 1900)
    set_property(TARGET onesdk_static APPEND PROPERTY INTERFACE_LINK_LIBRARIES "legacy_stdio_definitions.lib")
endif ()
if (CMAKE_DL_LIBS)
    set_property(TARGET onesdk_static APPEND PROPERTY INTERFACE_LINK_LIBRARIES "${CMAKE_DL_LIBS}")
endif ()

# TARGET onesdk_shared

add_library(onesdk_shared SHARED IMPORTED)
set_property(TARGET onesdk_shared PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${onesdk_path}/include")
set_property(TARGET onesdk_shared PROPERTY IMPORTED_LOCATION "${onesdk_lib_path}/${onesdk_lib_prefix}onesdk_shared${onesdk_shlib_suffix}")
set_property(TARGET onesdk_shared APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS "ONESDK_SHARED")
if (onesdk_implib_suffix)
    set_property(TARGET onesdk_shared APPEND PROPERTY IMPORTED_IMPLIB "${onesdk_lib_path}/${onesdk_lib_prefix}onesdk_shared${onesdk_implib_suffix}")
endif ()

cmake_policy(POP)
