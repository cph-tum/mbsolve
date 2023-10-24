# mbsolve

An open-source solver tool for the full wave 1D Maxwell-Bloch equations.

[![Build Status](https://travis-ci.org/mriesch-tum/mbsolve.svg?branch=master)](https://travis-ci.org/mriesch-tum/mbsolve)

## Overview

![Overview of the mbsolve project.](doc/mbsolve_overview.png)

The project consists of the following components:

 - mbsolve-lib: Contains the common base classes for solvers and writers
   (which are subsequently derived and implemented in the other subprojects).
   As the name suggests, the solver class is responsible for solving the
   Maxwell-Bloch equations while the writer class writes the results to a
   file of the respective format. It also provides classes to describe a
   device setup (materials, dimensions, ...) and a simulation scenario
   (simulation settings, sources, ...). Furtermore, a 
   class reader ist included to load autosaved simulation data to restart the simulation from a 
   specific timestep.

 - mbsolve-tool: A C++ test application that implements various simulation
   setups.

 - solver-cpu: Uses the OpenMP standard to efficiently solve the equation
   system on multi-core CPUs or Intel's Xeon Phi.

 - solver-cuda: Uses NVIDIA's CUDA framework to efficiently solve the equation
   system on graphics processing units (GPUs). Different solvers are
   implemented in this subproject.

 - tools: Various scripts in different scripting languages.

 - writer-hdf5: Writes the results into the HDF5 format.

 - reader-hdf5: Loads simulation data from the HDF5 format.


All libraries feature a Python interface and can be used conveniently from
Python scripts, Jupyter notebooks etc. See tools/python for examples.

## Install via conda

If you have a conda distribution installed, you can install mbsolve using

    $ conda install -c conda-forge mbsolve

from the `conda-forge` channel.

## Get the code

Make sure to clone using

    git clone --recurse-submodules [...]

in order to clone third-party libraries recursively. Alternatively, issue

    git submodule update --init --recursive

in the repository if it already exists.

## Build instructions

The project is built using the CMake build system. The typical commands under
Linux are

    $ cd mbsolve
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make -j 8
    $ ./mbsolve [--options]

By default, the build type is set to release with debug information, which
enables debugging but features the optimization necessary for reasonable
performance. Adjust the -j command line parameter of the make command in
order to speed up the compilation.

In order to enable/disable a component, an extra option has to be passed to
CMake:

    $ cmake -DWITH_CUDA=ON/OFF ..

The project has been successfully compiled with the GNU compiler collection
(gcc) and the Intel compiler (icc). gcc is used as default, the Intel compiler
may be used with the following command:

    $ CC=path_to_intel/icc CXX=path_to_intel/icpc cmake -DWITH_CUDA=OFF ..

Note that CUDA does not support all versions of the Intel compiler, hence the
CUDA support is disabled.

The target architecture may be set using the ARCH option. By default, compiler
optimizations suitable for the host machine are applied. However, when
building for another machine the architecture can be set with e.g.

    $ cmake -DARCH=AVX2 ..

The Doxygen documentation can be build with

    $ make doc

If you want to use Eclipse, use the appropriate generator flag -G and (if
desired) set the build type to Debug. (Note that Eclipse yields a warning if
the build directory is a child of the source directory. Adjust path_src.)

    $ cmake -DCMAKE_BUILD_TYPE=Debug -G"Eclipse CDT4 - Unix Makefiles" path_src

## Tools

 - Python scripts: Example scripts that define simulation setups and run the
   solver. Execute from build directory. See tools/python.

 - MATLAB scripts: The examples read the simulation results and plot them.
   See tools/matlab.

 - Slurm scripts: Example scripts for the Slurm Workload Manager.
   See tools/slurm.

## Dependencies

The following software packages are required to build the project:

 - C++ compiler
    - GNU Compiler Collection (gcc) >= 4.9.2
    - Intel C++ Compiler, any recent version
    - Clang >= v7.0.0 (Note: AppleClang lacks OpenMP support)
    - Microsoft Visual C++ (MSVC), any recent version (Note: MSVC only
      supports OpenMP 2.0)
 - cxxopts (any recent version) for mbsolve-tool
 - CMake >= 3.9
 - Eigen library >= 3.3.4
 - EigenRand >= 0.5.0
 - HDF5 (any recent version) for the writer-hdf and reader-hdf
 - Highfive >= 2.7.1 as C++11 friendly interface for libhdf5
 - Python >= 2.7 and SWIG >= 2.0.12 for Python support
 - CUDA >= 7.0 for solver-cuda
 - OpenMP (see supported compilers) for solver-cpu
 - Doxygen (any recent version) for the documentation

If the required dependencies are not met, the components may be disabled by
the build system. Consider the minimum version given in the brackets.

The runtime dependencies are a reduced set of the list above. Namely, those
are:

 - C/C++ standard libraries including OpenMP support
 - Python runtime
 - HDF5

Naturally, the same versions as used during compilation must be available.

## Contributing to mbsolve

Feel free to use the code and do not hesitate to contact me or to create an
issue on GitHub. Any contributions are welcome.

If you found the code helpful and used it in your research, you can cite the
following paper:<br />
Riesch, Michael and Jirauschek, Christian,
"mbsolve: An open-source solver tool for the Maxwell-Bloch equations,"
Comp. Phys. Comm. <b>268</b>, 108097 (2021).
[![arXiv:2005.05412](https://img.shields.io/badge/arXiv-2005.05412-informational)](https://arxiv.org/abs/2005.05412)
[![doi:10.1016/j.cpc.2021.108097](https://img.shields.io/badge/DOI-10.1016/j.cpc.2021.108097-important)](https://doi.org/10.1016/j.cpc.2021.108097)

If desired, you can cite the project web page directly:<br />
Riesch, Michael and Jirauschek, Christian, "mbsolve: An open-source solver
tool for the Maxwell-Bloch equations,"
[https://github.com/mriesch-tum/mbsolve](https://github.com/mriesch-tum/mbsolve)
[Online], (2017).
