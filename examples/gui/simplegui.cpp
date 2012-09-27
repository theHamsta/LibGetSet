#include <GetSet/GetSet.hxx>
#include <GetSet/GetSetFile.h>

// only needed for Qt GUI:
#include <GetSetGui/GetSetGui.h>

#include <iostream>

std::string g_iniFile;

/// A callback function to handle events
void gui(const std::string& section, const std::string& key)
{
	// Button "Control" "Save Configuration" has been  clicked
	if (section=="Control" && key=="Save Configuration")
	{
		GetSetIO::save(GetSetIO::IniFile(g_iniFile));
		std::cout << "Configuration saved!\n";
		return;
	}

	// Button "Control" "Run Algorithm" has been  clicked
	if (section=="Control" && key=="Run Algorithm")
	{

		std::cout << "\nAt this point your algorithm would execule with the following parameters:\n\n";
		std::cout << "Input/Source Image   = " << GetSet<>("Input/Source Image").getString() << std::endl;
		std::cout << "Input/Target Image   = " << GetSet<>("Input/Target Image").getString() << std::endl;
		std::cout << "Input/Mask Image     = " << GetSet<>("Input/Mask Image").getString() << std::endl;

		std::cout << "Number of Iterations = " << GetSet<int>("Algorithm/Number of Iterations").getValue() << std::endl;
		std::cout << "Step Size            = " << GetSet<double>("Algorithm/Step Size").getValue() << std::endl;

		// Test if verbose output is enabled
		if (GetSet<bool>("Algorithm","Verbose Output"))
			std::cout << "With Verbose Output\n";
		else
			std::cout << "Without Verbose Output\n";
		return;
	}

	std::cout << "Key \"" << key << "\" in section \"" << section << "\" = " << (std::string)GetSet<>(section,key) << std::endl;

}

/// A typical main function using GetSet
int main(int argc, char** argv)
{
	// Get the directory where the executable is located
	std::string executablePath=argv[0];
	std::string executableName=splitNameFromPath(executablePath);
	g_iniFile=executablePath+"/config.ini";

	// Define types and standard values
	GetSetGui::File("Input","Source Image")="./file0.png";
	GetSetGui::File("Input","Target Image")="./file1.png";
	GetSetGui::File("Input","Mask Image")="./mask.png";
	GetSet<int>("Algorithm","Number of Iterations")=25;
	GetSet<double>("Algorithm","Step Size")=0.123;
	GetSet<bool>("Algorithm","Verbose Output")=false;

	// Add two buttons to start our algorithm and to save the current configuration
	GetSetGui::Button("Control/Run Algorithm")="Run...";
	GetSetGui::Button("Control/Save Configuration")="Save ini-File";

	// (try to) load configuration file
	GetSetIO::load(GetSetIO::IniFile(g_iniFile));

	// Tell GetSet which function to call when something changes
	GetSetHandler call_back(gui);

	// If we have no GUI other than tis configuration window:
	return GetSetGui::runQtApp("Settings",argc,argv);
}
