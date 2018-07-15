#ifndef _NRRD_IMAGE_HPP
#define _NRRD_IMAGE_HPP


py::class_<NRRD::Image<float>> self( m, "NrrdImage" );
self.def( py::init<const std::string&>() )
.def_static( "fromArray", [&]( py::array_t<float>& array )
{


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

} )
.def( "show", [&]( const NRRD::Image<float>& self, std::string title, bool blockingCall )
{
	// std::thread t( [&]() {
	// 	std::cout << "thread function\n";
	if ( !app ) {
		app = std::make_unique<QApplication>( ONE_ARGUMENT_FOR_ARGC, &qtApplicationName );
	}


	// } );
	// t.detach();
	// QMainWindow m;
	// m.show();

	UtilsQt::Figure( title, self );
	// app->exec();
	// app->processEvents();

	while ( UtilsQt::FigureWindow::exists( title ) && blockingCall ) {
		app->processEvents();
		std::this_thread::sleep_for( std::chrono::milliseconds( 20 ) );
	}

	if ( blockingCall ) {
		UtilsQt::FigureWindow::instance_delete_all();
	}
}, py::arg( "title" ) = "", py::arg( "blocking_call" ) = true );

#endif /* _NRRD_IMAGE_HPP */
