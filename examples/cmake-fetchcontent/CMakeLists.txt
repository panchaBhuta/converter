cmake_minimum_required(VERSION 3.24 FATAL_ERROR)
project(cmake-fetchcontent)
set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
  converter
  GIT_REPOSITORY "https://github.com/panchaBhuta/converter.git"
  GIT_TAG        "v1.2.17"
)
FetchContent_MakeAvailable(converter)

add_executable(exprog src/exprog.cpp)
target_link_libraries(exprog converter) # donot specify PUBLIC here,
    # executable 'exprog' has to have transitive association to all its dependencies(aka library chain)
    # refer   https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html#target-usage-requirements
    # refer   https://cmake.org/cmake/help/latest/command/target_link_libraries.html#libraries-for-a-target-and-or-its-dependents
    # refer   https://cmake.org/cmake/help/latest/command/target_link_libraries.html#libraries-for-a-target-and-or-its-dependents
install(TARGETS exprog DESTINATION bin)

add_custom_target(uninstall
  COMMAND "${CMAKE_COMMAND}" -E remove "${CMAKE_INSTALL_PREFIX}/bin/exprog"
)
