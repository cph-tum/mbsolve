%module readerhdf5
%{
#include <mbsolve/reader-hdf5/reader_hdf5.hpp>
%}

%init
%{
    mbsolve::reader_hdf5_loader hdf5_load;
%}

%import(module="mbsolve.lib") "../../mbsolve-lib/include/mbsolve/lib.hpp"

%include "exception.i"
%include "std_except.i"
%include "std_map.i"
%include "std_shared_ptr.i"
%include "std_string.i"
%include "std_vector.i"

%exception {
  try {
    $action
  } catch (const std::exception& e) {
    SWIG_exception(SWIG_RuntimeError, e.what());
  }
}
