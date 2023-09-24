#
# URL:      https://github.com/panchaBhuta/converter
#
# Copyright (c) 2023-2023 Gautam Dhar
# All rights reserved.
#
# converter is distributed under the BSD 3-Clause license, see LICENSE for details.
#


# https://stackoverflow.com/questions/14446495/cmake-project-structure-with-unit-tests


# Test macro add_example
macro(add_example testname)
  if(CMAKE_BUILD_TYPE MATCHES "Debug")
    add_executable(${testname} ${testname}.cpp)
    target_link_libraries(${testname} PUBLIC converter)
  endif()
endmacro(add_example)



#[=========================================[
# Examples
add_example(ex001 examples/ex001.cpp)
add_example(ex002 examples/ex002.cpp)
add_example(ex003 examples/ex003.cpp)
add_example(ex004 examples/ex004.cpp)
add_example(ex005 examples/ex005.cpp)
add_example(ex006 examples/ex006.cpp)
add_example(ex007 examples/ex007.cpp)
add_example(ex008 examples/ex008.cpp)
add_example(ex009 examples/ex009.cpp)
#]=========================================]


add_example(exConv001 examples/exConv001.cpp)
