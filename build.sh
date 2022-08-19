#!/bin/bash
mkdir -p bin
cd bin
cmake -S ../ -B .
make && make Shaders && ./vged
cd ..
