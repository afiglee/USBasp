#!/bin/sh
if test -d "build"; then
  echo "Removing old build directory..."
  rm -rf build
fi
mkdir build
echo "Building the project..."
cd build
#cmake -DCMAKE_PREFIX_PATH=/opt/homebrew ..
cmake ..
make
