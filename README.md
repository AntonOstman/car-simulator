# Install

## glfw

```
sudo apt-get install libx11-dev libglfw3-dev xorg-dev
```

## vcpkg and assimp

Install vcpkg in project root (or fix all paths to vcpkg)

```
git submodule update --init --recursive
cd vcpkg
./bootstrap-vcpkg.sh
./vcpkg install assimp
```

## Install dependencies, build and run

```
./install.sh
./create_build.sh
./run.sh
```
