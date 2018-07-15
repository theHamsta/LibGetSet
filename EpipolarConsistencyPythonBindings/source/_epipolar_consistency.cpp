
#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <memory>

#include "LibProjectiveGeometry/ProjectionMatrix.h"

#include "NRRD/nrrd_image.hxx"
#include "LibUtilsQt/Figure.hxx"
#include <thread>



namespace py = pybind11;

std::unique_ptr<QApplication> app;
int ONE_ARGUMENT_FOR_ARGC = 1;
char* qtApplicationName = ( char* )( "" );


PYBIND11_MODULE( _epipolar_consistency, m )
{

#include "_nrrd_image.hpp"

	m.def( "make_projection_matrix", &Geometry::makeProjectionMatrix, "Makes a projections matrix." );
	m.def( "make_calibration_matrix", &Geometry::makeCalibrationMatrix, "Makes a calibration matrix." );



}


