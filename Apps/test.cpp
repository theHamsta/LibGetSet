#include <iostream>
#include <functional>

#include <GetSet/GetSet.hxx>
#include <GetSetGui/GetSetGui.h>
#include <GetSetGui/GetSetTabWidget.h>

/// Application
GetSetGui::GetSetApplication g_app("Test");

void gui(const std::string& section, const std::string& key)
{
	std::cout << key << std::endl;

	if (key=="Show")
	{
		GetSetSection("Visualization/Trajectory").setDisabled(!GetSet<bool>("Visualization/Images/Show"));
	}

	if (key=="Distort Images")
	{
		GetSetSection("Visualization/Trajectory").setCollapsible(!GetSet<bool>("Visualization/Images/Distort Images"));
	}

	g_app.saveSettings();
}

int main(int argc, char **argv)
{
	// Visualization
	GetSet<int>("Visualization/Trajectory/Selection 0")=0;
	GetSet<int>("Visualization/Trajectory/Selection 1")=1;
	GetSet<int>("Visualization/Trajectory/Skip Projections")=0;
	// Visualization/Images
	GetSet<bool>("Visualization/Images/Show")=1;
	GetSet<bool>("Visualization/Images/Distort Images")=1;
	GetSetGui::RangedDouble("Visualization/Images/Scale").setMin(0.1).setMax(2).setStep(0.05)=0.2;
	GetSetSection("Visualization/Images").setGrouped();
	// Visualization/Trajectory
	GetSetGui::File("Trajectory/Projection Matrices").setMultiple(true).setExtensions("One Projection Matrix Per Line (*.ompl);;All Files (*)");
	GetSetGui::File("Trajectory/Projection Images").setMultiple(true).setExtensions("2D NRRD Images (*.nrrd);;All Files (*)");
	GetSet<double>("Trajectory/Pixel Spacing")=.308;
	GetSetSection("Visualization/Trajectory").setCollapsible();

	// Projection Matrix
	GetSet<>("Projection/Projection Matrix");
	GetSet<double>("Projection/FOV")=20;
	GetSet<>("Projection/Rotation").setString("0 0 -1.5708 0");
	GetSet<double>("Projection/Distance")=2000;

	g_app.init(argc,argv,gui);
	g_app.window().addDefaultFileMenu();
	return g_app.exec();
}
