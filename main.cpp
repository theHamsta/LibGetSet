#include <QtGui/qapplication>

#include "GetSet/GetSet.hxx"
#include "GetSetGui/GetSetSettingsWindow.h"
#include "GetSet/GetSetCmdLineParser.h"

#include <iostream>

/// Predeclaration, definitiosn are platform dependent
std::string runShellCommand(const std::string& shellCommand, const std::string& outputRedirectFile);

#ifdef _WIN32
    #define NOMINMAX
	#include <windows.h>

	std::string runShellCommand(const std::string& shellCommand, const std::string& outputRedirectFile)
	{
		std::cout << "Running \"" << shellCommand << "\" ..." << std::endl;
		system((shellCommand+" > "+outputRedirectFile).c_str());
		std::string output=fileReadString(outputRedirectFile);
		std::cout << output << std::endl;
		return output;
	}

#endif

#if defined(__APPLE__)||defined(__linux__)
	// 2do posix version here
#endif

// We have a separate GetSetDictionaries for controlling the program itself
GetSetDictionary control;

void control_callback(const std::string& section, const std::string& key)
{
	if (key=="Run Command Line Tool")
	{
		std::string nameOfExe=section;
		std::string pathToExe=GetSet<std::string>(control,nameOfExe,"Path To Executable");
		std::cout << "Launching " << nameOfExe << "...\n";
		std::cout << "Path: " << pathToExe << "\n";
	}
}

int main(int argc, char** argv)
{
	GetSetDictionary &dict(GetSetDictionary::globalDictionary());

	// Path where this program can be found (good only when called has permission to wrote to this directory)
	std::string workingDir=argv[0];
	splitNameFromPath(workingDir);
	GetSetGui::Directory(control,"Input","Working Directory")=workingDir;
	// XML files or executable files
	GetSetGui::File inputFiles(control,"Input","XML Files");
	inputFiles.setExtensions("XML Parameter Descriptions (*.xml);;Executable Files (*.exe);;All Files (*.*)");
	inputFiles.setMultiple(true);
	GetSet<bool>(control,"Input","Seperate Dictionaries")=true;

	control.parseIni(fileReadString("GetSetCli.ini"));
	if (argc>1)
	{
		GetSetCmdLineParser cmd(false,control);
		cmd.parse(argc, argv, false);
		GetSet<std::string>(control,"Input","XML Files")=vectorToString(cmd.getUnnamedArgs(),";");
	}
	fileWriteString("GetSetCli.ini",control.getIni());

	std::vector<std::string> files=stringToVector<std::string>(GetSet<std::string>("input","XML Files"));
	for (int i=0;i<(int)files.size();i++)
	{
		std::string extension=splitRight(files[i],".");
		if (extension=="xml")
		{
			dict.parseXML(fileReadString(files[i]));
		}
		else
		{
			// we assume file[i] is path to an executable
			std::string pathToExe=files[i];
			std::string name=splitNameFromPath(files[i]);
			// And try to run it
			std::cout << "Trying to run " << name << "...\n";
			std::string output=runShellCommand(pathToExe+" --xml",workingDir);
			if (output.empty())
			{
				std::cerr << "Failed to run the command.\n";
				continue;
			}
			else if (output[0]!='<')
			{
				std::cerr << "It appears the executable " << files[i] << " has not understood the --xml flag!\n";
				continue;
			}
			dict.parseXML(output);
			GetSet<std::string>(control,name,"Path To Executable")=pathToExe;
			GetSetGui::ReadOnlyText(control,name,"XML Dexcription")=output;
			GetSetGui::Trigger(control,name,"Launch With Parameters")="Run...";
		}
	}

	GetSetHandler gui(control_callback);

	QApplication app(argc,argv);

	GetSetSettingsWindow setup;
	setup.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint);
	setup.show();

	GetSetSettingsWindow controlWindow(control);
	controlWindow.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint);
	controlWindow.show();

	return app.exec();
}
