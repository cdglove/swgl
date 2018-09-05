#
# CMakeLists.txt
#
# Copyright (c) Chris Glover, 2017-2018
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#

include(ExternalProject)
set(BENCHMARK_DOWNLOAD_DEPENDENCIES ON)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "")
set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "")
ExternalProject_Add(google_benchmark 
	GIT_REPOSITORY https://github.com/google/benchmark.git
	GIT_TAG v1.3.0
	PREFIX "google_benchmark"
	INSTALL_COMMAND ""
	CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  	UPDATE_DISCONNECTED 1
  	UPDATE_COMMAND "")

##############################################################################
# Helper function to add benchmarks
##############################################################################
function(add_radix_bench_target target_name target_file)
    add_executable(${target_name} ${target_file})
    target_link_libraries(${target_name} PRIVATE radix benchmark)
    if(WIN32)
   		target_link_libraries(${target_name} PRIVATE Shlwapi)
   	else()
   		target_link_libraries(${target_name} PRIVATE pthread)
   	endif()
    target_include_directories(${target_name} PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/google_benchmark/src/google_benchmark/include")
    target_compile_definitions(${target_name} PRIVATE BOOST_ERROR_CODE_HEADER_ONLY BOOST_ALL_NO_LIB)
    add_test(NAME ${target_name} COMMAND ${target_name})
    add_dependencies(${target_name} google_benchmark)
endfunction()

function(add_radix_bench target_file)
  string(REPLACE "/" "." target_name ${target_file}) 
  add_radix_bench_target(boost.radix.bench.${target_name} "${target_file}.cpp")
endfunction()

link_directories("${CMAKE_CURRENT_BINARY_DIR}/google_benchmark/src/google_benchmark-build/src")
add_radix_bench(codec/rfc4648)
add_radix_bench_target(boost.radix.bench.codec.rfc4648_novalidation codec/rfc4648.cpp)
target_compile_definitions(boost.radix.bench.codec.rfc4648_novalidation PRIVATE RADIXBENCH_DECODE_NOVALIDATION=1)
add_radix_bench(codec/base64_reference)
add_radix_bench(codec/base64_beast)
