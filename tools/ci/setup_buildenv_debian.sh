#!/bin/bash

# packages from distribution repositories
apt update && apt install --no-install-recommends -y ca-certificates \
  doxygen g++ git libhdf5-dev make python3-dev python3-pip \
  python3-setuptools python3-wheel swig wget

# distribution specialization
apt install --no-install-recommends -y lsb-release
codename=`lsb_release -cs`

# Clang compiler
if [ "$codename" == "buster" ]; then
    apt-get install --no-install-recommends -y clang-7 libomp-dev
fi

# clang-format
if [ "$codename" == "buster" ]; then
    apt-get install --no-install-recommends -y clang-format-6.0
fi

# CMake
if [ "$codename" == "jessie" ] || [ "$codename" == "stretch" ]; then
    # we need CMake >= 3.9
    wget https://github.com/Kitware/CMake/releases/download/v3.16.2/cmake-3.16.2-Linux-x86_64.sh
    chmod +x cmake-3.16.2-Linux-x86_64.sh
    ./cmake-3.16.2-Linux-x86_64.sh --skip-license --prefix=/
else
    apt-get install --no-install-recommends -y cmake
fi

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
  
# HighFive library
git clone https://github.com/BlueBrain/HighFive.git && cd HighFive && \
  git checkout v2.7.1 && cmake -Bbuild -H. -DHIGHFIVE_EXAMPLES=OFF \
   -DHIGHFIVE_UNIT_TESTS=Off -DHIGHFIVE_USE_BOOST=Off && \
  cmake --build build/ --target install && cd ..

# EigenRand library
git clone https://github.com/bab2min/EigenRand && cd EigenRand && \
  cmake -Bbuild -H. -DCMAKE_BUILD_TYPE=Release \
   -DEIGENRAND_BUILD_TEST=off && cmake --build build/ --target install && \
  cd ..