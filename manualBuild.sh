#!/bin/bash

# ./manualBuild.sh tests test001   > err.log 2>&1
# ./manualBuild.sh tests testDefaultTemplateInstantiation -DENABLE_STD_TtoS
# ./manualBuild.sh cmake checkChrono_fromStream

#[=============================[    find which standards my GCC compiler supports?
#  https://stackoverflow.com/questions/34836775/compiler-standards-support-c11-c14-c17
#  If you want to list just non-deprecated C++:
#  g++ -v --help 2> /dev/null | grep -iv deprecated | grep "C++" | sed -n '/^ *-std=\([^<][^ ]\+\).*/ {s//\1/p}'
#
#  GCC version by g++ --version
#]=============================]    find which standards my GCC compiler supports?



echo "################### g++ compiler ##########################"
  mkdir -p ./build-tmp/gpp
  rm -f ./build-tmp/gpp/${2}
  g++ --verbose -g -Wall -Wextra -Wpedantic -Wshadow -Wpointer-arith \
                         -Wcast-qual -Wno-missing-braces -Wswitch-default -Wcast-align \
                         -Wunreachable-code -Wundef -Wuninitialized -Wold-style-cast \
                         -Wsign-conversion -Weffc++ \
     -DUSE_FILEPREFIXMAP=1 -fmacro-prefix-map="$(pwd)/"= ${3} \
     --std=gnu++2a -fconcepts \
     -I "$(pwd)/include" -I "$(pwd)/build-debug/_deps/date-src/include/" -I "$(pwd)/build-debug/include/" \
     -o ./build-tmp/gpp/${2}  "$(pwd)/${1}/${2}.cpp"
     #-lstdc++


echo "################### gcc compiler ##########################"
  mkdir -p ./build-tmp/gcc
  rm -f ./build-tmp/gcc/${2}
  gcc --verbose -g -Wall -Wextra -Wpedantic -Wshadow -Wpointer-arith \
                         -Wcast-qual -Wno-missing-braces -Wswitch-default -Wcast-align \
                         -Wunreachable-code -Wundef -Wuninitialized -Wold-style-cast \
                         -Wsign-conversion -Weffc++ \
     -DUSE_FILEPREFIXMAP=1 -fmacro-prefix-map="$(pwd)/"= ${3} \
    --std=gnu++2a -fconcepts \
     -I "$(pwd)/include" -I "$(pwd)/build-debug/_deps/date-src/include/" -I "$(pwd)/build-debug/include/" \
     -o ./build-tmp/gcc/${2}  "$(pwd)/${1}/${2}.cpp" \
     -lstdc++ -lm


echo "################### clang compiler ##########################"
  mkdir -p ./build-tmp/clang
  rm -f ./build-tmp/clang/${2}
  /usr/bin/clang++ --verbose -g -Wall -Wextra -Wpedantic -Wshadow -Wpointer-arith \
                             -Wcast-qual -Wno-missing-braces -Wswitch-default -Wcast-align \
                             -Wunreachable-code -Wundef -Wuninitialized -Wold-style-cast \
                             -Wsign-conversion -Weffc++ \
     -DUSE_FILEPREFIXMAP=1 -fmacro-prefix-map="$(pwd)/"= ${3} \
     --std=gnu++2a \
     -I "$(pwd)/include" -I "$(pwd)/build-debug/_deps/date-src/include/" -I "$(pwd)/build-debug/include/" \
     -o ./build-tmp/clang/${2}  "$(pwd)/${1}/${2}.cpp"
     #-lstdc++

