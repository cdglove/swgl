#
# CMakeLists.txt
#
# Copyright (c) Chris Glover, 2017-2018
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#

#find_package(Boost REQUIRED unit_test_framework)

##############################################################################
# Helper function to add tests
##############################################################################
function(add_swgl_test target_file)
	string(REPLACE "/" "." target_name ${target_file}) 
    set(swgl_test_name boost.swgl.test.${target_name})
    add_executable(${swgl_test_name} "${target_file}.cpp")
    target_link_libraries(${swgl_test_name} PRIVATE swgl Boost::unit_test_framework)
    target_compile_definitions(${swgl_test_name} PRIVATE BOOST_ERROR_CODE_HEADER_ONLY BOOST_ALL_NO_LIB)
    add_test(NAME ${swgl_test_name} COMMAND ${swgl_test_name})
endfunction()

# add_swgl_test(bitstreams)
# add_swgl_test(encode)
# add_swgl_test(decode)
# add_swgl_test(codec/rfc4648)
