#
# mbsolve: An open-source solver tool for the Maxwell-Bloch equations.
#
# Copyright (c) 2016, Computational Photonics Group, Technical University of
# Munich.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation,
# Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA

cmake_minimum_required(VERSION 3.9)

# project information
set(PROJECT "mbsolve")
set(PROJECT_AUTHOR "Michael Riesch")
set(PROJECT_EMAIL "michael.riesch@tum.de")
set(PROJECT_URL "https://www.github.com/mriesch-tum/mbsolve")
set(VERSION_MAJOR 0)
set(VERSION_MINOR 4)
set(VERSION_PATCH 0)
set(VERSION_PRRLS "rc1")
set(VERSION_CORE "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
set(VERSION_FULL "${VERSION_CORE}${VERSION_PRRLS}")
project(${PROJECT} VERSION ${VERSION_CORE})

# is python installation prefix set?
if(DEFINED ENV{PREFIX})
  set(PREFIX_SETUPPY "--prefix $ENV{PREFIX}")
endif()

# require CMake GNUInstallDirs package
include(GNUInstallDirs)

# require CMake helper package for CMake configuration files
include(CMakePackageConfigHelpers)

# CXX_STANDARD is required
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# export shared library symbols on Windows
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# specify architecture
set(ARCH "HOST" CACHE STRING "specify architecture")
set_property(CACHE ARCH PROPERTY STRINGS "HOST" "AVX2" "SSE3")

# set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'RelWithDebInfo' as none was "
    "specified.")
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build."
    FORCE)
  # set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()

# build static libraries or dynamic libraries?
option(BUILD_SHARED_LIBS "turn off to build static libraries" ON)

# specify components
option(WITH_HDF5 "with(out) HDF5 writer" ON)
option(WITH_CPU "with(out) CPU solver" ON)
option(WITH_CPU_NOISE "with(out) CPU solver noise" ON)
option(WITH_CUDA "with(out) CUDA solver" OFF)
option(WITH_DOXYGEN "with(out) Doxygen documentation" ON)
option(WITH_FORMAT "with(out) clang-format support" ON)
option(WITH_PYTHON "with(out) Python interface" ON)
option(WITH_TESTS "with(out) Catch2 unit tests" OFF)
option(WITH_CPPTOOL "with(out) C++ tool" ON)

# Eigen library
find_package(Eigen3 3.3.4 REQUIRED NO_MODULE)

# Python interface (enabled only when shared libraries are build)
if(WITH_PYTHON AND BUILD_SHARED_LIBS)
  find_package(SWIG 2.0.12)
  find_package(PythonInterp)
  find_package(PythonLibs)
  if(SWIG_FOUND AND PythonLibs_FOUND AND PythonInterp_FOUND)
    if(POLICY CMP0078)
      cmake_policy(SET CMP0078 OLD)
    endif()
    if(POLICY CMP0086)
      cmake_policy(SET CMP0086 OLD)
    endif()
    include(${SWIG_USE_FILE})
    set(MBSOLVE_PYTHON TRUE)
    set(PYTHON_IFACE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT}")
    set(PYTHON_MOD_STRING "")
  endif()
endif()

# Catch2 unit tests
if(WITH_TESTS)
  find_package(Catch2)
  if(Catch2_FOUND)
    set(MBSOLVE_TESTS TRUE)
    include(CTest)
    include(Catch)
    enable_testing()
  endif()
endif()

# mbsolve library
add_subdirectory(mbsolve-lib)
get_target_property(LIB_SOURCE_FILES mbsolve-lib SOURCES)
list(APPEND ALL_SOURCE_FILES ${LIB_SOURCE_FILES})

# HDF support?
if(WITH_HDF5)
  find_package(HDF5 COMPONENTS C CXX)
  find_package(HighFive REQUIRED)
  if(HDF5_FOUND)
    set(MBSOLVE_HDF5 TRUE)
    add_subdirectory(writer-hdf5)
    get_target_property(WRITER_HDF_SOURCE_FILES writer-hdf5 SOURCES)
    add_subdirectory(reader-hdf5)
    get_target_property(READER_HDF_SOURCE_FILES reader-hdf5 SOURCES)
    list(APPEND ALL_SOURCE_FILES ${WRITER_HDF_SOURCE_FILES} ${READER_HDF_SOURCE_FILES})
  endif()
endif()

# CUDA support?
if(WITH_CUDA)
  find_package(CUDA 7.0)
  if(CUDA_FOUND)
    set(MBSOLVE_CUDA TRUE)
    add_subdirectory(solver-cuda)
  endif()
endif()

# CPU support?
if(WITH_CPU)
  find_package(OpenMP COMPONENTS CXX)
  if(WITH_CPU_NOISE)
    find_package(EigenRand)
    if(NOT EigenRand_FOUND)
      add_library(EigenRand INTERFACE)
      add_library(EigenRand::EigenRand ALIAS EigenRand)
      target_include_directories(EigenRand INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/EigenRand)
    endif()
    set(ARCH "AVX2")
  endif()
  if(OPENMP_FOUND)
    set(MBSOLVE_CPU TRUE)
    add_subdirectory(solver-cpu)
    get_target_property(SOLVER_CPU_SOURCE_FILES solver-cpu SOURCES)
    list(APPEND ALL_SOURCE_FILES ${SOLVER_CPU_SOURCE_FILES})
  endif()
endif()

# C++ tool
if(WITH_CPPTOOL)
  find_package(cxxopts NO_MODULE)
  if(cxxopts_FOUND)
    add_subdirectory(mbsolve-tool)
    get_target_property(TOOL_SOURCE_FILES mbsolve-tool SOURCES)
    list(APPEND ALL_SOURCE_FILES ${TOOL_SOURCE_FILES})
  endif()
endif()

# Doxygen documentation
if(WITH_DOXYGEN)
  find_package(Doxygen)
  if(DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
      ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(doc
      ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generating API documentation with Doxygen" VERBATIM)
  endif()
endif()

# clang-format support
if(WITH_FORMAT)
  find_program(CLANG_FORMAT NAMES "clang-format" "clang-format-6.0")
  if(CLANG_FORMAT)
    set(MBSOLVE_FORMAT TRUE)
  endif()
endif()

# install Python interface
if(MBSOLVE_PYTHON)
  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/python/setup.py.in"
    "${CMAKE_CURRENT_BINARY_DIR}/setup.py.inc")
  file(GENERATE
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/setup.py"
    INPUT "${CMAKE_CURRENT_BINARY_DIR}/setup.py.inc")
  file(GENERATE
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT}/__init__.py"
    INPUT "${CMAKE_CURRENT_SOURCE_DIR}/python/__init__.py.in")
  install(CODE
    "execute_process(COMMAND ${PYTHON_EXECUTABLE} -m pip install . \
     ${PREFIX_SETUPPY} \
     WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})")
endif()

# install CMake configuration files
configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/CMakeConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT}-config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT})
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT}-config-version.cmake
  VERSION ${VERSION_CORE}
  COMPATIBILITY SameMajorVersion)
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT}-config.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT}-config-version.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT})

# code formatting check
if(MBSOLVE_FORMAT)
  add_custom_target(format
    COMMAND ${CLANG_FORMAT} -style=file -i ${ALL_SOURCE_FILES}
    COMMENT "Formatting all source files")
endif()
