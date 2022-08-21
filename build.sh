#!/bin/bash

target=debug
verboseOutput=false

while getopts v:verbose: flag
do
    case "${flag}" in
        v) verboseOutput=true;;
        a) verboseOutput=true;;
    esac
done

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
cd build
echo $verboseOutput
if [[ $verboseOutput = "true" ]]
then
  make VERBOSE=1
else
  make
fi
