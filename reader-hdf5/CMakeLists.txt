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
project(reader-hdf5)

add_library(reader-hdf5
  ${CMAKE_CURRENT_SOURCE_DIR}/include/mbsolve/reader-hdf5/reader_hdf5.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/reader_hdf5.cpp)
set_target_properties(reader-hdf5 PROPERTIES
  CXX_STANDARD 11
  VERSION ${VERSION_CORE}
  SOVERSION ${VERSION_MAJOR})
if(BUILD_SHARED_LIBS)
  target_compile_definitions(reader-hdf5 PRIVATE "-DH5_BUILT_AS_DYNAMIC_LIB")
else()
  target_compile_definitions(reader-hdf5 PRIVATE "-DH5_BUILT_AS_STATIC_LIB")
endif()
target_include_directories(reader-hdf5 PRIVATE ${HDF5_INCLUDE_DIRS})
target_include_directories(reader-hdf5 PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_link_libraries(reader-hdf5 PRIVATE mbsolve-lib ${HDF5_LIBRARIES} HighFive)

if(MBSOLVE_PYTHON)
  set_property(SOURCE swig/readerhdf5.i PROPERTY CPLUSPLUS ON)
  set(CMAKE_SWIG_OUTDIR ${PYTHON_IFACE_PATH})
  swig_add_library(readerhdf5
    LANGUAGE python
    SOURCES swig/readerhdf5.i)
  set_target_properties(${SWIG_MODULE_readerhdf5_REAL_NAME} PROPERTIES
    CXX_STANDARD 11
    ARCHIVE_OUTPUT_DIRECTORY $<1:${PYTHON_IFACE_PATH}>
    LIBRARY_OUTPUT_DIRECTORY $<1:${PYTHON_IFACE_PATH}>
    RUNTIME_OUTPUT_DIRECTORY $<1:${PYTHON_IFACE_PATH}>)
  target_include_directories(${SWIG_MODULE_readerhdf5_REAL_NAME} PRIVATE
    ${PYTHON_INCLUDE_DIRS})
  target_link_libraries(${SWIG_MODULE_readerhdf5_REAL_NAME} PRIVATE
    mbsolve-lib reader-hdf5)
  if(APPLE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
      target_link_libraries(${SWIG_MODULE_readerhdf5_REAL_NAME} PRIVATE
        "-undefined dynamic_lookup")
    endif()
  endif()
  if(WIN32)
    target_link_libraries(${SWIG_MODULE_readerhdf5_REAL_NAME} PRIVATE
      ${PYTHON_LIBRARIES})
  endif()
  set(PYTHON_MOD_STRING
    "${PYTHON_MOD_STRING}
            '$<TARGET_FILE_NAME:${SWIG_MODULE_readerhdf5_REAL_NAME}>',"
    PARENT_SCOPE)
endif()

# install library
install(TARGETS reader-hdf5
  EXPORT ${PROJECT}-reader-hdf5-targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# install CMake targets
install(EXPORT ${PROJECT}-reader-hdf5-targets
  NAMESPACE ${PROJECT}::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT})

# install public header files
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
