# Changelog

All notable changes to this project will be documented in this file.

The format is based on
[Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to
[Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## noise-implementation - 2023-10-01

Add fdtd solver for the modeling of quantum fluctuations.

### Added
- The preprocessor directive 'WITH_NOISE' is added: If set to 1,
  solver instances including the noise treatment are added as cmake targets.

### Changed
- Interface change in qm_description: We add an input argument 
  'num_carrier_cell' to the qm_description constructor, which sets the 
   number of particles in a volume presented by one grid cell of the system.

## Autosave/restart simulation - 2023-07-14

### Added
- Modern C++14 wrapper Highfive for libhdf5.
- Class sim_data, store field and density matrix values of last 
  simulation timestep.
- Method autosave in abstract writer class with implementation for HDF5.
- New abstract class reader and HDF5 implementation.
- Initial conditions for fields/density matrix,
  simulation data retrieved from the autosave file.
### Changed
- Scenario constructor extended by input argument for initial conditions 
  for polarization.
- Data concept in write method, simulation meta-informations are stored 
  in a group setup, divided into two datasets: scenario and device.

## [0.4.0] - 2020-xx-xx

Forth unstable version.

### Added
 - Flexible initial conditions for electric and magnetic fields.
 - Flexible boundary conditions for electric and magnetic fields.
 - Support for automatic "big picture" documentation generation.
 - Support for automatic unit tests.

### Changed
 - xxx TODO xxxx

## [0.3.0] - 2020-03-02

Third unstable version.

### Changed
 - Established new version scheme with major, minor and patch number
 - Major revision of project structure and development process according to
   the bertha project (CI support, contributing and coding style guidelines,
   preparation for deployment, ...)
 - Major refactoring of solver classes (including separation of treatment
   of electromagnetic field and quantum mechanical systems)

### Removed
 - Dropped support for Intel Xeon Phi KNC
 - Dropped Predictor-Corrector and Runge-Kutta methods
 - Dropped support for MATLAB data export

## [0.2] - 2018-09-27

Second unstable version.

### Added
 - General description of materials, in particular quantum mechanical
   properties
 - Alternative numerical methods such as those based on the Rodrigues approach
 - Writer for HDF5 data files

## [0.1] - 2017-07-18

First unstable version, served as proof of concept.

### Added
 - CMake build system, modular project structure
 - Base classes that describe the device and simulation scenario
 - Elementary numerical methods
 - Parallelization support for OpenMP and CUDA
 - Writer for MATLAB data files
