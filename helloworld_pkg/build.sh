#!/bin/bash

# exit on error and print each command
set -e

if [ -d ./build/install ]; then
    rm -rf ./build/install
fi

# cmake
cmake -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DAIMRT_SAMPLES_INSTALL=ON \
    -DCMAKE_INSTALL_PREFIX=./build/install \
    $@

cmake --build build --config Release --target install --parallel $(nproc)
