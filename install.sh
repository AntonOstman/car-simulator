#!/usr/bin/bash
sudo apt-get install libx11-dev libglfw3-dev xorg-dev apt-get install curl zip unzip tar
git submodule update --init --recursive
cd vcpkg && ./bootstrap-vcpkg.sh && ./vcpkg install assimp
