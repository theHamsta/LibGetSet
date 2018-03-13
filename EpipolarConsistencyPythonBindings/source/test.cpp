#include "LibProjectiveGeometry/ProjectionMatrix.h"


#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>

namespace py = pybind11;


PYBIND11_MODULE(_epipolar_consistency, m) {
    m.def("makeProjectionMatrix", &Geometry::makeProjectionMatrix, "Makes a projections matrix.");
    m.def("makeCalibrationMatrix", &Geometry::makeCalibrationMatrix, "Makes a calibration matrix.");
}


