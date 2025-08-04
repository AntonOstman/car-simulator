# Install

## glfw

```
sudo apt-get install libx11-dev libglfw3-dev xorg-dev
```

## vcpkg and assimp

Install vcpkg in project root (or fix all paths to vcpkg)

```
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
./vcpkg integrate install
./vcpkg install assimp
```

## Run and build program

VCPKG handles some dependencies, supply correct toolchain file

```
cmake . -B build -DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake
./run.sh
```
