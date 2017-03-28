#include <iostream>
#include <functional>

#include <GetSet/GetSet.hxx>
#include <GetSetGui/GetSetGui.h>
#include <GetSetGui/GetSetTabWidget.h>

/// Application
GetSetGui::GetSetApplication g_app("Test");

// Managing saving/loading parametzers and automatic GUI
#include <GetSetGui/GetSetTabWidget.h>

/// Pre-processing of X-ray projection images.
struct PreProccess {
	
	/// Modifying intensities (first normalize, then apply bias and scale, then logarithm)
	struct Intensity {
		bool	normalize=false;
		double	bias=0.0;
		double	scale=1.0;
		bool	apply_log=false;
	} intensity;

	/// Low-pass filter. Applied after intensity changes.
	struct Lowpass {
		double gaussian_sigma=1.84;
		int    half_kernel_width=5;
	} lowpass;	
	
	/// Modifying geometry
	struct Geometry {
		bool	flip_u=false;
		bool	flip_v=false;
	} geometry;

	/// Declare default values.
	void declareGUI(GetSetSection& section)
	{
		section.key<bool>  ("Intensity/Per Pixel/Normalize")=intensity.normalize;
		section.key<double>("Intensity/Per Pixel/Bias")=intensity.bias;
		section.key<double>("Intensity/Per Pixel/Scale")=intensity.scale;
		section.key<bool>  ("Intensity/Per Pixel/Apply Minus Logarithm")=intensity.apply_log;
		section.subsection("Intensity/Per Pixel").setGrouped();
			
		section.key<double>("Intensity/Lowpass Filter/Gaussian Sigma")=lowpass.gaussian_sigma;
		section.key<int>   ("Intensity/Lowpass Filter/Half Kernel Width")=lowpass.half_kernel_width;
		section.subsection ("Intensity/Lowpass Filter").setGrouped();

		section.key<bool>  ("Geometry/Flip u-Axis")=geometry.flip_u;
		section.key<bool>  ("Geometry/Flip v-Axis")=geometry.flip_v;
		section.subsection("Geometry").setGrouped();

	}
	
	// Retreive current values from GUI
	void retreiveFromGUI(GetSetSection& section)
	{
		intensity.normalize=section.key<bool>("Intensity/Normalize");
		intensity.bias=section.key<double>("Intensity/Bias");
		intensity.scale=section.key<double>("Intensity/Scale");
		intensity.apply_log=section.key<bool>("Intensity/Apply Minus Logarithm");
		lowpass.gaussian_sigma=section.key<double>("Intensity/Lowpass Filter/Gaussian Sigma");
		lowpass.half_kernel_width=section.key<int>("Intensity/Lowpass Filter/Half Kernel Width");
		geometry.flip_u=section.key<bool>("Geometry/Flip u-Axis");
		geometry.flip_v=section.key<bool>("Geometry/Flip v-Axis");
	}
};


void gui(const std::string& section, const std::string& key)
{
	std::cout << key << std::endl;


//	g_app.saveSettings();
}

int main(int argc, char **argv)
{

	PreProccess().declareGUI(GetSetSection("Pre-Processing"));

	g_app.init(argc,argv,gui);
	g_app.window().addDefaultFileMenu();

	GetSetGui::GetSetTabWidget* intensity=new GetSetGui::GetSetTabWidget("Pre-Processing/Intensity");
	intensity->show();

	return g_app.exec();
}
