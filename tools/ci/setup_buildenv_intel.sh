#!/bin/bash

# packages from distribution repositories
apt update && apt install --no-install-recommends -y doxygen \
git libhdf5-dev python3-dev python3-pip swig 

# Configuration of locales
apt install locales && locale-gen en_US.UTF-8

# Catch2 library
git clone https://github.com/catchorg/Catch2.git && cd Catch2 && \
  git checkout v2.13.4 && \
  cmake -Bbuild -H. -DBUILD_TESTING=OFF && \
  cmake --build build/ --target install && cd ..

# cxxopts library
git clone https://github.com/jarro2783/cxxopts.git && cd cxxopts && \
  cmake -Bbuild -H. -DCXXOPTS_BUILD_EXAMPLES=OFF -DCXXOPTS_BUILD_TESTS=OFF && \
  cmake --build build/ --target install && cd ..

# Eigen3 library
git clone https://gitlab.com/libeigen/eigen.git && cd eigen && \
  git checkout 3.3.7 && cmake -Bbuild -H. -DBUILD_TESTING=OFF && \
  cmake --build build/ --target install && cd ..
