#!/bin/bash

cmake -S . -B build
cmake --build build --target format
cd build
mv compile_commands.json ..
make -j`${nproc}`
