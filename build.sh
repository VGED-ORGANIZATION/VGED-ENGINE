#!/bin/bash

target=debug

if [[ $1 = "release" ]]
then
  target=release
fi

# build the engine
if [[ $target = "release" ]]
then
  echo "Building release target. Validation layers will be disabled"
  cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
else
  echo "Building debug target"
  cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
fi
cd build && make VERBOSE=1