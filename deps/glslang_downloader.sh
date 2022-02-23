#!/bin/bash

git clone https://github.com/KhronosGroup/glslang.git
cd glslang
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$(pwd)/install" ./../
make -j4 install


