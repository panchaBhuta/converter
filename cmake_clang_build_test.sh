#!/bin/bash


mkdir -p ./build-clang-debug
#cd ./build-clang-debug && cmake -DCONVERTER_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=clang++ --trace ..  && cd ..
cd ./build-clang-debug && cmake -DCONVERTER_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=clang++ ..  && cd ..
#cd ./build-clang-debug && cmake  --build . --verbose    &&  cd ..
cd ./build-clang-debug && cmake  --build .     &&  cd ..
#cd ./build-clang-debug && ctest -C Debug --verbose      &&  cd ..
cd ./build-clang-debug && ctest -C Debug       &&  cd ..

#cmake -DCONVERTER_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=clang++ --trace ..

