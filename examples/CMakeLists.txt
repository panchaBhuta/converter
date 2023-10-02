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
add_example(ex001 ex001.cpp)
add_example(ex002 ex002.cpp)
add_example(ex003 ex003.cpp)
#]=========================================]


add_example(exConv001 exConv001.cpp)
