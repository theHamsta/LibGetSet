
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
int NULL_INT = 1;
char* foo = ( char* )( "" );


PYBIND11_MODULE( _epipolar_consistency, m )
{

	auto fromArray = [&]( py::array_t<float>& array ) {


		if ( array.ndim() == 1 ) {
			// auto raw_array = array.mutable_unchecked<2>();
			return std::make_unique<NRRD::Image<float>> ( array.shape()[0], 1, 1, const_cast<float*>( array.data() ) );
		} else if ( array.ndim() == 2 ) {
			// auto raw_array = array.mutable_unchecked<2>();
			return std::make_unique<NRRD::Image<float >>( array.shape()[1], array.shape()[0], 1, const_cast<float*>( array.data() ) );
		}

		if ( array.ndim() == 3 ) {
			// auto raw_array = array.mutable_unchecked<3>();

			return std::make_unique<NRRD::Image<float>>( array.shape()[2], array.shape()[1],  array.shape()[0], const_cast<float*>( array.data() ) );
		}

	};

	auto nrrdImageShow = [&]( const NRRD::Image<float>& self, std::string title = "" ) {
		// std::thread t( [&]() {
		// 	std::cout << "thread function\n";
		if ( !app ) {
			app = std::make_unique<QApplication>( NULL_INT, &foo );
		}


		// } );
		// t.detach();
		// QMainWindow m;
		// m.show();

		UtilsQt::Figure( title, self );
		// app->exec();

		while ( UtilsQt::FigureWindow::exists( title ) ) {
			app->processEvents();
			std::this_thread::sleep_for( std::chrono::milliseconds( 20 ) );
		}

		UtilsQt::FigureWindow::instance_delete_all();
	};

	py::class_<NRRD::Image<float>> self( m, "NrrdImage" );
	self.def( py::init<const std::string&>() )
	.def_static( "fromArray", fromArray )
	.def( "show", nrrdImageShow, py::arg( "title" ) = "" );

	m.def( "make_projection_matrix", &Geometry::makeProjectionMatrix, "Makes a projections matrix." );
	m.def( "make_calibration_matrix", &Geometry::makeCalibrationMatrix, "Makes a calibration matrix." );
	m.def( "imshow", [&] ( py::array_t<float>& array, const std::string & title ) {
		auto nrrdImage = fromArray( array );
		nrrdImageShow( *nrrdImage, title );
	}, py::arg( "array" ), py::arg( "title" ) = "" );



}


