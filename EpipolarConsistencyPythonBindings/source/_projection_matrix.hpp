
m.def( "make_projection_matrix", &Geometry::makeProjectionMatrix, "Makes a projections matrix." );
m.def( "make_calibration_matrix", &Geometry::makeCalibrationMatrix, "Makes a calibration matrix." );

m.def( "normalize_projection_matrix", &Geometry::normalizeProjectionMatrix);
m.def( "pseudo_inverse", &Geometry::pseudoInverse);
m.def( "projection_matrix_decompostion", &Geometry::projectionMatrixDecomposition);
m.def( "get_camera_instrinsics", &Geometry::getCameraIntrinsics);
m.def( "get_camera_center", &Geometry::getCameraCenter);
m.def( "get_camera_principal_point", &Geometry::getCameraPrincipalPoint);
m.def( "get_camera_direction", &Geometry::getCameraPrincipalPoint);
m.def( "get_camera_axis_direction", &Geometry::getCameraPrincipalPoint);
m.def( "get_camera_focal_length_px", &Geometry::getCameraFocalLengthPx);
m.def( "get_camera_image_plane", &Geometry::getCameraImagePlane);
m.def( "compute_fundamental_matrix", &Geometry::computeFundamentalMatrix);

m.attr( "infinity2") = Geometry::infinity2;
m.attr( "infinity2") = Geometry::infinity3;
m.attr( "origin2") = Geometry::origin2;
m.attr( "origin3") = Geometry::origin3;

//m.def( "finitePoint2", py::overload_cast<Eigen::Vector2d>( &Geometry::finitePoint));
//m.def( "finitePoint3", py::overload_cast<Eigen::Vector3d>( &Geometry::finitePoint));
//m.def( "infinitePoint2", py::overload_cast<Eigen::Vector2d>( &Geometry::infinitePoint));
//m.def( "infinitePoint3", py::overload_cast<Eigen::Vector3d>( &Geometry::infinitePoint));

//m.def( "dehomogenize2", py::overload_cast<Geometry::RP2Point>(&Geometry::dehomogenize));
//m.def( "dehomogenize3", py::overload_cast<Geometry::RP3Plane>(&Geometry::dehomogenize));
m.def( "euclidian2", &Geometry::euclidian2);
//m.def( "allfinite", &Geometry::allfinite<Vector2d>);
//m.def( "allfinite", &Geometry::allfinite<Vector3d>);
m.def( "euclidian3", &Geometry::euclidian3);
//m.def( "normalize", py::overload_cast<Geometry::RP2Point>(&Geometry::normalize));
//l.def( "normalize", py::overload_cast<Geometry::RP2Line>(&Geometry::normalize));
//m.def( "normalize", py::overload_cast<RP2Plane>(&Geometry::normalize));
m.def( "normalized", py::overload_cast<Geometry::RP3Plane>(&Geometry::normalized));
//m.def( "normalized", py::overload_cast<Geometry::RP2Line>(&Geometry::normalized));
m.def( "meet", &Geometry::meet);
m.def( "join", &Geometry::join);
//m.def( "joinPluecker", &Geometry::join_pluecker);
m.def( "line", &Geometry::line);
m.def( "lineToAngleIntercept", &Geometry::lineToAngleIntercept);
m.def( "lineRelativeToCenter", &Geometry::lineRelativeToCenter);
//m.def( "meet_pluecker", &Geometry::meet_pluecker);
m.def("pluecker_closest_point_to_origin",&Geometry::pluecker_closest_point_to_origin);
	

	///// Distance of a line to the origin
	//inline double pluecker_distance_to_origin(const RP3Line& L)
	//{
		//return pluecker_moment(L).norm()/pluecker_direction(L).norm();
	//}

	///// Compute the closest point on the line L to a point X
	//inline RP3Point pluecker_closest_to_point(const RP3Line& L, RP3Point X)
	//{
		//auto direction=pluecker_direction(L);
		//auto plane_through_X_orthogonal_to_L=RP3Plane(direction[0],direction[1],direction[2],-direction.dot(euclidian3(X))).eval();
		//auto closest_point_to_X_on_L=meet_pluecker(L,plane_through_X_orthogonal_to_L);
		//return closest_point_to_X_on_L;

	//}

	//////////////////////
	//// Pluecker Matrices
	//////////////////////
	
	///// Anti-symmetric matrix for the join operation using dual Plücker coordinates
	//inline Eigen::Matrix4d plueckerMatrixDual(const RP3Line& L)
	//{
		//Eigen::Matrix4d B;
		//B << 
				//0 , + L(5), - L(4), + L(3),
			//- L(5),     0 , + L(2), - L(1),
			//+ L(4), - L(2),     0 , + L(0),
			//- L(3), + L(1), - L(0),     0;
		//return B;
	//}

	///// Anti-symmetric matrix for the meet operation dual Plücker coordinates
	//inline Eigen::Matrix4d plueckerMatrix(const RP3Line& L)
	//{
		//Eigen::Matrix4d B;
		//B << 
					 //0 , - L(0), - L(1), - L(2),
				 //+ L(0),     0 , - L(3), - L(4),
				 //+ L(1), + L(3),     0 , - L(5),
				 //+ L(2), + L(4), + L(5),     0;
		//return B;
	//}
	
	//////////////////////
	//// Projections
	//////////////////////

	///// Sturm-style projection matrix for Plücker lines. Projectoin from Plücker coordinates directly to 2D lines.
	//inline Eigen::Matrix<double,6,1> pluecker_projection_matrix(const ProjectionMatrix& P)
	//{
		//Eigen::Matrix<double,6,1> PL;
		//PL << 
			//P(1,0)*P(2,1)-P(1,1)*P(2,0),+P(1,0)*P(2,2)-P(1,2)*P(2,0),+P(1,0)*P(2,3)-P(1,3)*P(2,0),+P(1,1)*P(2,2)-P(1,2)*P(2,1),+P(1,1)*P(2,3)-P(1,3)*P(2,1),+P(1,2)*P(2,3)-P(1,3)*P(2,2),
			//P(0,1)*P(2,0)-P(0,0)*P(2,1),-P(0,0)*P(2,2)+P(0,2)*P(2,0),-P(0,0)*P(2,3)+P(0,3)*P(2,0),-P(0,1)*P(2,2)+P(0,2)*P(2,1),-P(0,1)*P(2,3)+P(0,3)*P(2,1),-P(0,2)*P(2,3)+P(0,3)*P(2,2),
			//P(0,0)*P(1,1)-P(0,1)*P(1,0),+P(0,0)*P(1,2)-P(0,2)*P(1,0),+P(0,0)*P(1,3)-P(0,3)*P(1,0),+P(0,1)*P(1,2)-P(0,2)*P(1,1),+P(0,1)*P(1,3)-P(0,3)*P(1,1),+P(0,2)*P(1,3)-P(0,3)*P(1,2);
		//return PL;
	//}
	
	///// Directly project 3D line in Plücker coordinates to 2D line.
	//inline RP2Line pluecker_project(const RP3Line& L, const ProjectionMatrix& P)
	//{
		//return RP2Line(
			//L[0]*(P(1,0)*P(2,1)-P(1,1)*P(2,0))+L[1]*(+P(1,0)*P(2,2)-P(1,2)*P(2,0))+L[2]*(+P(1,0)*P(2,3)-P(1,3)*P(2,0))+L[3]*(+P(1,1)*P(2,2)-P(1,2)*P(2,1))+L[4]*(+P(1,1)*P(2,3)-P(1,3)*P(2,1))+L[5]*(+P(1,2)*P(2,3)-P(1,3)*P(2,2)),
			//L[0]*(P(0,1)*P(2,0)-P(0,0)*P(2,1))+L[1]*(-P(0,0)*P(2,2)+P(0,2)*P(2,0))+L[2]*(-P(0,0)*P(2,3)+P(0,3)*P(2,0))+L[3]*(-P(0,1)*P(2,2)+P(0,2)*P(2,1))+L[4]*(-P(0,1)*P(2,3)+P(0,3)*P(2,1))+L[5]*(-P(0,2)*P(2,3)+P(0,3)*P(2,2)),
			//L[0]*(P(0,0)*P(1,1)-P(0,1)*P(1,0))+L[1]*(+P(0,0)*P(1,2)-P(0,2)*P(1,0))+L[2]*(+P(0,0)*P(1,3)-P(0,3)*P(1,0))+L[3]*(+P(0,1)*P(1,2)-P(0,2)*P(1,1))+L[4]*(+P(0,1)*P(1,3)-P(0,3)*P(1,1))+L[5]*(+P(0,2)*P(1,3)-P(0,3)*P(1,2)));
	//}

	///// A mapping T from a 3D point to a plane E via central projection from C. T*X=meet(join(C,X),E)
	//inline RP3Homography centralProjectionToPlane(const RP3Point& C, const RP3Point& E)
	//{
		//Eigen::Matrix4d P;
		//P << 
		 //+ C[1]*E[1] + C[2]*E[2] + C[3]*E[3] , - C[0]*E[1]                         , - C[0]*E[2]                         , - C[0]*E[3]                        ,
		 //- C[1]*E[0]                         , + C[0]*E[0] + C[2]*E[2] + C[3]*E[3] , - C[1]*E[2]                         , - C[1]*E[3]                        ,
		 //- C[2]*E[0]                         , - C[2]*E[1]                         , + C[0]*E[0] + C[3]*E[3] + C[1]*E[1] , - C[2]*E[3]                        ,
		 //- C[3]*E[0]                         , - C[3]*E[1]                         , - C[3]*E[2]                         , + C[0]*E[0] + C[1]*E[1] + C[2]*E[2];
		//return P;
	//}

	///////////////////////////
	//// Useful homographies
	///////////////////////////

	///// Homogeneous 2D rotation
	//inline RP2Homography Rotation(double alpha)
	//{
		//double ca=cos(alpha);
		//double sa=sin(alpha);
		//Eigen::Matrix3d R;
		//R << 
			  //ca, -sa, 0,
			  //sa,  ca, 0,
			   //0,   0, 1;
		//return R;
	//}

	///// Homogeneous 2D rigid transformaion
	//inline RP2Homography Rigid(double alpha, double tu, double tv)
	//{
		//Eigen::Matrix3d R;
		//double ca=cos(alpha);
		//double sa=sin(alpha);
		//R << 
			  //ca, -sa, tu,
			  //sa,  ca, tv,
			   //0,   0, 1;
		//return R;
	//}

	///// Homogeneous 2D translation
	//inline RP2Homography Translation(double tu, double tv)
	//{
		//Eigen::Matrix3d T;
		//T << 
			  //1, 0, tu,
			  //0, 1, tv,
			  //0, 0, 1;
		//return T;
	//}

	///// Homogeneous 2D scaling
	//inline RP2Homography Scale(double su, double sv)
	//{
		//Eigen::Matrix3d S;
		//S << 
			  //su, 0,  0,
			  //0,  sv, 0,
			  //0,  0,  1;
		//return S;
	//}

	///// Homogeneous rotation about X-axis
	//inline RP3Homography RotationX(double alpha)
	//{
		//double ca=cos(alpha);
		//double sa=sin(alpha);
		//Eigen::Matrix4d R;
		//R << 
			 //1,  0,   0, 0,
			 //0, ca, -sa, 0,
			 //0, sa,  ca, 0,
			 //0,  0,   0, 1;
		//return R;
	//}

	///// Homogeneous rotation about Y-axis
	//inline RP3Homography RotationY(double alpha)
	//{
		//double ca=cos(alpha);
		//double sa=sin(alpha);
		//Eigen::Matrix4d R;
		//R << 
			 //ca,  0, sa, 0,
			  //0,  1,  0, 0,
			//-sa,  0, ca, 0,
			  //0,  0,  0, 1;
		//return R;
	//}
	
	///// Homogeneous rotation about Z-axis
	//inline RP3Homography RotationZ(double alpha)
	//{
		//double ca=cos(alpha);
		//double sa=sin(alpha);
		//Eigen::Matrix4d R;
		//R << 
			  //ca, -sa, 0, 0,	
			 //sa,  ca, 0, 0,
			  //0,   0, 1, 0,
			  //0,   0, 0, 1;
		//return R;
	//}

	///// Homogeneous translation
	//inline RP3Homography Translation(double tX,double tY,double tZ)
	//{
		//Eigen::Matrix4d T;
		//T << 
			  //1, 0, 0, tX,
			 //0, 1, 0, tY,
			 //0, 0, 1, tZ,
			 //0, 0, 0, 1;
		//return T;
	//}
	
	///// Homogeneous translation
	//inline RP3Homography Translation(const Eigen::Vector3d& t)
	//{
		//Eigen::Matrix4d T;
		//T << 
			  //1, 0, 0, t[0],
			 //0, 1, 0, t[1],
			 //0, 0, 1, t[2],
			 //0, 0, 0, 1;
		//return T;
	//}

	///// Homogeneous 3D scaling
	//inline RP3Homography Scale(double sx, double sy, double sz)
	//{
		//Eigen::Matrix4d S;
		//S << 
			  //sx, 0,  0,  0,
			 //0,  sy, 0,  0,
			 //0,  0,  sz, 0,
			 //0,  0,  0,  1;
		//return S;
	//}

	///// Homogeneous 3D scaling
	//inline RP3Homography Scale(const Eigen::Vector3d& s)
	//{
		//Eigen::Matrix4d S;
		//S << 
			  //s(0), 0,    0,    0,
			 //0,    s(1), 0,    0,
			 //0,    0,    s[2], 0,
			 //0,    0,    0,    1;
		//return S;
	//}

	///////////////////////////
	//// Utilities (metric)
	///////////////////////////

	///// Compute angle relative to zero-plane E0 and 90° plane E1. Input assumed to be in HNF.
	//inline double plane_angle_in_pencil(const RP3Plane& E, const RP3Plane& E0, const RP3Plane& E90)
	//{
		//double sina=E.head(3).dot(E90.head(3));
		//double cosa=E.head(3).dot(E0.head(3));
		//return std::atan2(sina,cosa);
	//}


//} // namespace Geometry
 
//#endif // __projective_geometry_hxx
