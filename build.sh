#!/bin/bash

target=debug
verboseOutput=false

while getopts v flag
do
    case "${flag}" in
        v) verboseOutput=true;;
        verbose) verboseOutput=true;;
    esac
done

if [[ $1 = "release" ]]
then
  target=release
fi
if [[ $2 = "release" ]]
then
  target=release
fi

# build the engine
if [[ $target = "release" ]]
then
  echo "Building release target. Validation layers will be disabled"
  cmake -S . -B build/Release -DCMAKE_BUILD_TYPE=Release
  cd build/Release
else
  echo "Building debug target"
  cmake -S . -B build/Debug -DCMAKE_BUILD_TYPE=Debug
  cd build/Debug
fi

if [[ $verboseOutput = "true" ]]
then
  make VERBOSE=1
else
  make
fi

if [[ $target = "release" ]]
then
  cd ../..
  strip bin/Release/editor bin/Release/example_game
fi