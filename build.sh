#!/bin/bash
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -S . -B bin
cmake --build bin
