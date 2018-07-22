
auto submodule_projective_geometry = m.def_submodule( "projective_geometry" );

submodule_projective_geometry.def( "make_projection_matrix", &Geometry::makeProjectionMatrix, "Makes a projections matrix." );
submodule_projective_geometry.def( "make_calibration_matrix", &Geometry::makeCalibrationMatrix, "Makes a calibration matrix." );

submodule_projective_geometry.def( "normalize_projection_matrix", &Geometry::normalizeProjectionMatrix, py::arg().noconvert() );
// submodule_projective_geometry.def( "normalize_projection_matrix", [&]( Eigen::Ref<Geometry::ProjectionMatrix> matrix )
// {
// 	Geometry::normalizeProjectionMatrix( matrix );
// 	return matrix;
// } );
// submodule_projective_geometry.def( "pseudo_inverse", py::overload_cast<const Geometry>(&Geometry::pseudoInverse ));
submodule_projective_geometry.def( "projection_matrix_decompostion", &Geometry::projectionMatrixDecomposition );
submodule_projective_geometry.def( "get_camera_instrinsics", &Geometry::getCameraIntrinsics );
submodule_projective_geometry.def( "get_camera_center", &Geometry::getCameraCenter );
submodule_projective_geometry.def( "get_camera_principal_point", &Geometry::getCameraPrincipalPoint );
submodule_projective_geometry.def( "get_camera_direction", &Geometry::getCameraPrincipalPoint );
submodule_projective_geometry.def( "get_camera_axis_direction", &Geometry::getCameraPrincipalPoint );
submodule_projective_geometry.def( "get_camera_focal_length_px", &Geometry::getCameraFocalLengthPx );
submodule_projective_geometry.def( "get_camera_image_plane", &Geometry::getCameraImagePlane );
submodule_projective_geometry.def( "compute_fundamental_matrix", &Geometry::computeFundamentalMatrix );
//#endif // __projection_matrix


submodule_projective_geometry.attr( "infinity2" ) = Geometry::infinity2;
submodule_projective_geometry.attr( "infinity2" ) = Geometry::infinity3;
submodule_projective_geometry.attr( "origin2" ) = Geometry::origin2;
submodule_projective_geometry.attr( "origin3" ) = Geometry::origin3;

submodule_projective_geometry.def( "finitePoint2", py::overload_cast<const Eigen::Vector2d&>( &Geometry::finitePoint ) );
submodule_projective_geometry.def( "finitePoint3", py::overload_cast<const Eigen::Vector3d&>( &Geometry::finitePoint ) );
submodule_projective_geometry.def( "infinitePoint2", py::overload_cast<const Eigen::Vector2d&>( &Geometry::infinitePoint ) );
submodule_projective_geometry.def( "infinitePoint3", py::overload_cast<const Eigen::Vector3d&>( &Geometry::infinitePoint ) );

submodule_projective_geometry.def( "dehomogenized", py::overload_cast<Geometry::RP2Point>( &Geometry::dehomogenized ) );
submodule_projective_geometry.def( "dehomogenized", py::overload_cast<Geometry::RP3Plane>( &Geometry::dehomogenized ) );
submodule_projective_geometry.def( "euclidian2", &Geometry::euclidian2 );
// submodule_projective_geometry.def( "allfinite", &Geometry::allfinite<const Vector2d> );
// submodule_projective_geometry.def( "allfinite", &Geometry::allfinite<const Vector3d> );
submodule_projective_geometry.def( "euclidian3", &Geometry::euclidian3 );
//submodule_projective_geometry.def( "normalize", py::overload_cast<Geometry::RP2Point>(&Geometry::normalize));
//l.def( "normalize", py::overload_cast<Geometry::RP2Line>(&Geometry::normalize));
//submodule_projective_geometry.def( "normalize", py::overload_cast<RP2Plane>(&Geometry::normalize));
submodule_projective_geometry.def( "normalized", py::overload_cast<Geometry::RP3Plane>( &Geometry::normalized ) );
submodule_projective_geometry.def( "normalized", py::overload_cast<Geometry::RP2Line>( &Geometry::normalized ) );
submodule_projective_geometry.def( "meet", &Geometry::meet );
submodule_projective_geometry.def( "join", &Geometry::join );
submodule_projective_geometry.def( "join_pluecker", py::overload_cast<const Geometry::RP3Point&, const Geometry::RP3Point& >( &Geometry::join_pluecker ) );
submodule_projective_geometry.def( "join_pluecker", py::overload_cast<const Geometry::RP3Line&, const Geometry::RP3Point& >( &Geometry::join_pluecker ) );
submodule_projective_geometry.def( "meet_pluecker", py::overload_cast<const Geometry::RP3Plane&, const Geometry::RP3Plane& >( &Geometry::meet_pluecker ) );
submodule_projective_geometry.def( "meet_pluecker", py::overload_cast<const Geometry::RP3Line&, const Geometry::RP3Plane& >( &Geometry::meet_pluecker ) );
submodule_projective_geometry.def( "line", &Geometry::line );
submodule_projective_geometry.def( "line_to_angle_intercept", &Geometry::lineToAngleIntercept );
submodule_projective_geometry.def( "line_relative_to_center", &Geometry::lineRelativeToCenter );
submodule_projective_geometry.def( "pluecker_closest_point_to_origin", &Geometry::pluecker_closest_point_to_origin );
submodule_projective_geometry.def( "pluecker_distance_to_origin", &Geometry::pluecker_distance_to_origin );
submodule_projective_geometry.def( "pluecker_closest_to_point", &Geometry::pluecker_closest_to_point );
submodule_projective_geometry.def( "pluecker_matrix_dual", &Geometry::plueckerMatrixDual );
submodule_projective_geometry.def( "pluecker_matrix", &Geometry::plueckerMatrix );
submodule_projective_geometry.def( "pluecker_projection_matrix", &Geometry::pluecker_projection_matrix );
submodule_projective_geometry.def( "pluecker_project", &Geometry::pluecker_project );
submodule_projective_geometry.def( "central_projection_to_plane", &Geometry::centralProjectionToPlane );
submodule_projective_geometry.def( "rotation", &Geometry::Rotation );
submodule_projective_geometry.def( "rotation_x", &Geometry::RotationX );
submodule_projective_geometry.def( "rotation_y", &Geometry::RotationY );
submodule_projective_geometry.def( "rotation_z", &Geometry::RotationZ );
submodule_projective_geometry.def( "rigid", &Geometry::Rigid );
submodule_projective_geometry.def( "scale", py::overload_cast<double, double>( &Geometry::Scale ) );
submodule_projective_geometry.def( "scale", py::overload_cast<double, double, double>( &Geometry::Scale ) );
submodule_projective_geometry.def( "scale", py::overload_cast<const Eigen::Vector3d&>( &Geometry::Scale ) );

submodule_projective_geometry.def( "translation", py::overload_cast<double, double>( &Geometry::Translation ) );
submodule_projective_geometry.def( "translation", py::overload_cast<double, double, double>( &Geometry::Translation ) );
submodule_projective_geometry.def( "translation", py::overload_cast<const Eigen::Vector3d&>( &Geometry::Translation ) );
submodule_projective_geometry.def( "plane_angle_in_pencil",  &Geometry::plane_angle_in_pencil );
//#endif // __projective_geometry_hxx


submodule_projective_geometry.def( "pseudo_inverse",  py::overload_cast<const Eigen::MatrixXd&>( &Geometry::pseudoInverse ) );
submodule_projective_geometry.def( "pseudo_inverse_and_nullspace", [&]( const Eigen::Matrix<double, 3, 4>& matrix )
{
	Eigen::Matrix<double, 4, 3> Pinv;
	Eigen::Vector4d C;
	Geometry::pseudoInverseAndNullspace( matrix, Pinv, C );
	return std::make_pair( Pinv, C );

} );
submodule_projective_geometry.def( "make_rank_deficient",  &Geometry::makeRankDeficient );
submodule_projective_geometry.def( "nullspace_left",  &Geometry::nullspace_left );

// SingularValueDecompositon.h

submodule_projective_geometry.def( "box",  &Geometry::box );
// submodule_projective_geometry.def( "camera_frustrum",  &Geometry::cameraFrustum );
submodule_projective_geometry.def( "intersect_line_with_convex",  &Geometry::intersectLineWithConvex );
submodule_projective_geometry.def( "intersect_line_with_rect",  &Geometry::intersectLineWithRect );
// // GeometryVisualisation.h



py::class_<Geometry::SourceDetectorGeometry> sourceDetectorGeometry( submodule_projective_geometry, "SourceDetectorGeometry" );
sourceDetectorGeometry.def_readwrite( "C", &Geometry::SourceDetectorGeometry::C );
sourceDetectorGeometry.def_readwrite( "O", &Geometry::SourceDetectorGeometry::O );
sourceDetectorGeometry.def_readwrite( "U", &Geometry::SourceDetectorGeometry::U );
sourceDetectorGeometry.def_readwrite( "V", &Geometry::SourceDetectorGeometry::V );
sourceDetectorGeometry.def_readwrite( "principal_point_3d", &Geometry::SourceDetectorGeometry::principal_point_3d );
sourceDetectorGeometry.def_readwrite( "principal_plane", &Geometry::SourceDetectorGeometry::principal_plane );
sourceDetectorGeometry.def_readwrite( "image_plane", &Geometry::SourceDetectorGeometry::image_plane );
sourceDetectorGeometry.def_static( "make_projection_matrix", &Geometry::SourceDetectorGeometry::makeProjectionMatrix );
sourceDetectorGeometry.def( py::init<const Geometry::ProjectionMatrix&, double>() );
sourceDetectorGeometry.def( py::init<	const Geometry::RP3Point&, const Geometry::RP3Point&,
							const Eigen::Vector3d&, const Eigen::Vector3d& >() );
sourceDetectorGeometry.def( "point_on_detector", py::overload_cast<double, double>( &Geometry::SourceDetectorGeometry::point_on_detector, py::const_ ) );
sourceDetectorGeometry.def( "point_on_detector", py::overload_cast<const Geometry::RP2Point&>( &Geometry::SourceDetectorGeometry::point_on_detector, py::const_ ) );
sourceDetectorGeometry.def( "project_to_detector", py::overload_cast<const Geometry::RP3Point&>( &Geometry::SourceDetectorGeometry::project_to_detector, py::const_ ) );
sourceDetectorGeometry.def( "project", &Geometry::SourceDetectorGeometry::project );
sourceDetectorGeometry.def_property_readonly( "projection_matrix", &Geometry::SourceDetectorGeometry::projectionMatrix );

// SourceDetectorGeometry.h