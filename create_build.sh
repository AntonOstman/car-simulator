#!/usr/bin/bash
if [ -d "./build" ]; then
    echo "-- Deleting existing build --"
    rm build -rf
fi

cmake . -B build -DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
# cmake . -B build -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake
