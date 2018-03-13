#include "LibProjectiveGeometry/ProjectionMatrix.h"


#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>


namespace py = pybind11;


// Passing bytes to C++
// 
// A Python bytes object will be passed to C++ functions that accept std::string or char* without conversion.



PYBIND11_MODULE(_epipolar_consistency, m) {
    m.def("makeProjectionMatrix", &Geometry::makeProjectionMatrix, "Makes a projections matrix.");
    m.def("makeCalibrationMatrix", &Geometry::makeCalibrationMatrix, "Makes a calibration matrix.");
}


