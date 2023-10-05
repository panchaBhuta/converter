cmake_minimum_required(VERSION 3.24 FATAL_ERROR)
project(cmake-fetchcontent)
set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
  converter
  GIT_REPOSITORY "https://github.com/panchaBhuta/converter.git"
  GIT_TAG        "v1.2.8"
)
FetchContent_MakeAvailable(converter)

add_executable(exprog src/exprog.cpp)
target_link_libraries(exprog PUBLIC converter)
install(TARGETS exprog DESTINATION bin)

add_custom_target(uninstall
  COMMAND "${CMAKE_COMMAND}" -E remove "${CMAKE_INSTALL_PREFIX}/bin/exprog"
)
