#!/usr/bin/bash

rm bin/SimpleOpenGL
cd build && make
cd ..
bin/SimpleOpenGL
