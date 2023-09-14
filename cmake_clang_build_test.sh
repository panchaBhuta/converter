#!/bin/bash


mkdir -p ./_build-clang-debug
#cd ./_build-clang-debug && cmake -DCONVERTER_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=clang++ --trace ..  && cd ..
cd ./_build-clang-debug && cmake -DCONVERTER_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=clang++ ..  && cd ..
#cd ./_build-clang-debug && cmake  --build . --verbose    &&  cd ..
cd ./_build-clang-debug && cmake  --build .     &&  cd ..
#cd ./_build-clang-debug && ctest -C Debug --verbose      &&  cd ..
cd ./_build-clang-debug && ctest -C Debug       &&  cd ..

#cmake -DCONVERTER_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=clang++ --trace ..

