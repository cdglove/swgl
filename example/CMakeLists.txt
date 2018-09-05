#
# CMakeLists.txt
#
# Copyright (c) Chris Glover, 2017-2018
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#

##############################################################################
# Helper functions to add examples
##############################################################################
function(add_swgl_example_target target_name target_file)
    add_executable(${target_name} ${target_file})
    target_link_libraries(${target_name} PRIVATE swgl)
endfunction()

function(add_swgl_example target_file)
  string(REPLACE "/" "." target_name ${target_file}) 
  add_swgl_example_target(swgl.example.${target_name} "${target_file}.cpp")
endfunction()

##############################################################################
# Examples
##############################################################################
add_swgl_example(tinyrendertut)
