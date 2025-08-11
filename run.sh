#!/usr/bin/bash

rm bin/SimpleOpenGL
cd build && make
cd ..
cp build/compile_commands.json .
bin/SimpleOpenGL
