#!/bin/bash

mkdir build
cd build
cmake ..
mv compile_commands.json ..
make -j`${nproc}`
