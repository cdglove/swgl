#
# CMakeLists.txt
#
# Copyright (c) Chris Glover, 2017-2018
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
cmake_minimum_required(VERSION 3.8)
project(swgl VERSION 0.1 LANGUAGES CXX)

##############################################################################
# Global Settings
##############################################################################
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

##############################################################################
# Options
##############################################################################
option (SWGL_BUILD_EXAMPLES "Build examples" ON)
option (SWGL_BUILD_TESTS "Build tests" ON)
option (SWGL_BUILD_BENCHMARKS "Build benchmarks" OFF)
option (SWGL_BUILD_DOCS "Build benchmarks" OFF)

##############################################################################
# Look for the rest of Boost
##############################################################################
#set(Boost_USE_STATIC_LIBS ON)
#find_package(Boost REQUIRED)

##############################################################################
# Setup the 'radix' header-only library target, along with its install target
# and exports.
##############################################################################
add_library(swgl)
target_include_directories(
    swgl PUBLIC 
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    $<INSTALL_INTERFACE:include/*.hpp>)

# target_link_libraries(
# 	radix INTERFACE Boost::boost)

file(GLOB_RECURSE SWGL_HEADERS ${CMAKE_CURRENT_SOURCE_DIR} include/*.hpp)
set(SWGL_SOURCES
	src/image.cpp
	src/model.cpp
	src/pipeline.cpp
)
target_sources(swgl PUBLIC ${SWGL_HEADERS} PRIVATE ${SWGL_SOURCES})

#
#  HACK: have the files showing in the IDE, under the name 'moduleA_ide'
#
add_custom_target(swgl.ide SOURCES ${SWGL_HEADERS})

##############################################################################
# Setup subdirectories and testing
##############################################################################
enable_testing()

if(SWGL_BUILD_DOCS)
	add_subdirectory(doc)
endif()

if(SWGL_BUILD_EXAMPLES)
	##############################################################################
	# Dependencies
	##############################################################################
	set(GLFW_BUILD_EXAMPLES OFF)
	set(GLFW_BUILD_TESTS OFF)
	set(GLFW_BUILD_DOCS OFF)
	set(GLFW_INSTALL OFF)
	add_subdirectory(${PROJECT_SOURCE_DIR}/contrib/glfw)
	add_subdirectory(example)
endif()

if(SWGL_BUILD_TESTS)
	add_subdirectory(test)
endif()

if(SWGL_BUILD_BENCHMARKS)
	add_subdirectory(benchmark)
endif()
