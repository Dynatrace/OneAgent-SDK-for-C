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

cmake_policy(PUSH)
cmake_policy(VERSION 2.8.12)
cmake_minimum_required(VERSION 2.8.12)

include(CheckLibraryExists)
include(CheckSymbolExists)

include("${CMAKE_CURRENT_LIST_DIR}/onesdk-version.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/onesdk-platform.cmake")

get_filename_component(onesdk_path "${CMAKE_CURRENT_LIST_FILE}" PATH)

# define platform dependent strings
if (onesdk_platform MATCHES "windows")
    set(onesdk_lib_prefix "")
    set(onesdk_linklib_suffix ".lib")
    set(onesdk_shlib_suffix ".dll")
    set(onesdk_implib_suffix ".lib")
elseif (onesdk_platform MATCHES "linux" OR onesdk_platform MATCHES "sunos")
    set(onesdk_lib_prefix "lib")
    set(onesdk_linklib_suffix ".a")
    set(onesdk_shlib_suffix ".so")
    set(onesdk_implib_suffix OFF)
else ()
    message(FATAL_ERROR "ONESDK: Unsupported onesdk_platform (${onesdk_platform})")
endif ()

set(onesdk_shlib_name "${onesdk_lib_prefix}onesdk_shared${onesdk_shlib_suffix}")
if (UNIX)
    set(onesdk_shlib_soname "${onesdk_shlib_name}")
endif ()

set(onesdk_lib_path "${onesdk_path}/${onesdk_lib_subdir}")

# find out what we need to link for pthreads functions

if (UNIX)
    set(ONESDK_SAVE_CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS})
    set(ONESDK_SAVE_CMAKE_REQUIRED_DEFINITIONS ${CMAKE_REQUIRED_DEFINITIONS})
    set(ONESDK_SAVE_CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES})
    set(ONESDK_SAVE_CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES})

    # pthreads can be implemented in libc...
    if (NOT DEFINED onesdk_thread_libs)
        CHECK_SYMBOL_EXISTS(pthread_create pthread.h onesdk_libc_has_pthreads)
        if (onesdk_libc_has_pthreads)
            set(onesdk_thread_libs "")
        endif ()
    endif ()
    # ... or in (lib)pthreads ...
    if (NOT DEFINED onesdk_thread_libs)
        CHECK_LIBRARY_EXISTS(pthreads pthread_create "" onesdk_have_libpthreads)
        if (onesdk_have_libpthreads)
            set(onesdk_thread_libs "-lpthreads")
        endif ()
    endif ()
    # ... or in (lib)pthread.
    if (NOT DEFINED onesdk_thread_libs)
        CHECK_LIBRARY_EXISTS(pthread pthread_create "" onesdk_have_libpthread)
        if (onesdk_have_libpthread)
            set(onesdk_thread_libs "-lpthread")
        endif ()
    endif ()
    # otherwise we give up and leave it to the user to link whatever is required (or set `onesdk_thread_libs` before including this file).

    set(CMAKE_REQUIRED_FLAGS ${ONESDK_SAVE_CMAKE_REQUIRED_FLAGS})
    set(CMAKE_REQUIRED_DEFINITIONS ${ONESDK_SAVE_CMAKE_REQUIRED_DEFINITIONS})
    set(CMAKE_REQUIRED_INCLUDES ${ONESDK_SAVE_CMAKE_REQUIRED_INCLUDES})
    set(CMAKE_REQUIRED_LIBRARIES ${ONESDK_SAVE_CMAKE_REQUIRED_LIBRARIES})
    unset(ONESDK_SAVE_CMAKE_REQUIRED_FLAGS)
    unset(ONESDK_SAVE_CMAKE_REQUIRED_DEFINITIONS)
    unset(ONESDK_SAVE_CMAKE_REQUIRED_INCLUDES)
    unset(ONESDK_SAVE_CMAKE_REQUIRED_LIBRARIES)
endif ()

# TARGET onesdk_static

add_library(onesdk_static STATIC IMPORTED)
set_property(TARGET onesdk_static PROPERTY VERSION "${onesdk_VERSION}")
set_property(TARGET onesdk_static PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${onesdk_path}/include")
set_property(TARGET onesdk_static PROPERTY IMPORTED_LOCATION "${onesdk_lib_path}/${onesdk_lib_prefix}onesdk_static${onesdk_linklib_suffix}")
if (onesdk_static_extra_libs)
    set_property(TARGET onesdk_static APPEND PROPERTY INTERFACE_LINK_LIBRARIES
        ${onesdk_static_extra_libs})
endif ()
if (CMAKE_DL_LIBS)
    set_property(TARGET onesdk_static APPEND PROPERTY INTERFACE_LINK_LIBRARIES "${CMAKE_DL_LIBS}")
endif ()
if (onesdk_thread_libs)
    set_property(TARGET onesdk_static APPEND PROPERTY INTERFACE_LINK_LIBRARIES "${onesdk_thread_libs}")
endif ()

# TARGET onesdk_shared

add_library(onesdk_shared SHARED IMPORTED)
set_property(TARGET onesdk_shared PROPERTY VERSION "${onesdk_VERSION}")
set_property(TARGET onesdk_shared PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${onesdk_path}/include")
set_property(TARGET onesdk_shared PROPERTY IMPORTED_LOCATION "${onesdk_lib_path}/${onesdk_shlib_name}")
if (onesdk_shlib_soname)
    set_property(TARGET onesdk_shared PROPERTY IMPORTED_SONAME "${onesdk_shlib_soname}")
endif()
set_property(TARGET onesdk_shared APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS "ONESDK_SHARED")
if (onesdk_implib_suffix)
    set_property(TARGET onesdk_shared APPEND PROPERTY IMPORTED_IMPLIB "${onesdk_lib_path}/${onesdk_lib_prefix}onesdk_shared${onesdk_implib_suffix}")
endif ()

cmake_policy(POP)
