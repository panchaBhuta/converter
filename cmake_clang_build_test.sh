#!/bin/bash


mkdir -p ./_build-clang-debug
cd ./_build-clang-debug && cmake -DCONVERTER_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=clang++ --trace ..
cd ./_build-clang-debug && cmake  --build . --verbose
cd ./_build-clang-debug && ctest -C Debug --verbose

#cmake -DCONVERTER_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=clang++ --trace ..

