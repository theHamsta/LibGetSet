#include <QtGui/qapplication>

#include "GetSet/GetSet.hxx"
#include "GetSetGui/GetSetSettingsWindow.h"
#include "GetSet/GetSetCmdLineParser.h"

#include <iostream>

/// Predeclaration, definitiosn are platform dependent
std::string runShellCommand(const std::string& shellCommand, const std::string& arguments,
							const std::string& workingDirectory, const std::string& outputRedirectFile);

#ifdef _WIN32
    #define NOMINMAX
	#include <windows.h>

	std::string runShellCommand(const std::string& shellCommand, const std::string& arguments,
								const std::string& workingDirectory, const std::string& outputRedirectFile)
	{
		std::string call="cd " + workingDirectory + " & " +shellCommand + " " + arguments +" > "+outputRedirectFile;
		fileWriteString(outputRedirectFile,"");
		system(call.c_str());
		std::string output=fileReadString(outputRedirectFile);
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
		// Get info about executable in control dictioary
		std::string nameOfExe=section;
		std::string pathToExe=GetSet<std::string>(control,nameOfExe,"Path To Executable");
		std::string pwd=GetSet<std::string>(control,"Input","Working Directory");
		std::string xml=GetSet<std::string>(control,nameOfExe,"XML Dexcription");
		// parse the xml and compose the command line arguments from global dictionary
		GetSetCmdLineParser cmd(false);
		cmd.parseXML(xml);
		std::string arguments;
		// Create a container with all section/key pairs that can be set by a command line flag
		// If there is more than one command line flag, we choose the shorter flag
		typedef std::map<std::string,std::string> MapStrStr;
		typedef std::map<std::string,MapStrStr> MapStrMapStrStr;
		MapStrMapStrStr	parameters;
		GetSetCmdLineParser::MapStrStrPair flags=cmd.getFlags();
		for (GetSetCmdLineParser::MapStrStrPair::const_iterator it=flags.begin();it!=flags.end();++it)
		{
			std::string& par=parameters[it->second.first][it->second.second];
			if(par.empty()||parameters[it->second.first][it->second.second].length()>it->first.length())
				par=it->first;
		}
		// And store the values in yet another map by flag, which also takes care fo the sorting
		MapStrStr flagValue;
		for (MapStrMapStrStr::const_iterator sectionit=parameters.begin();sectionit!=parameters.end();++sectionit)
			for (MapStrStr::const_iterator keyit=sectionit->second.begin();keyit!=sectionit->second.end();++keyit)
			{
				const std::string& it_section=sectionit->first;
				const std::string& it_key=keyit->first;
				const std::string& it_flag=keyit->second;
				std::string it_type=GetSetDictionary::globalDictionary().getDatainterface(it_section,it_key)->getType();
				if (it_type=="Trigger")
					; // skip triggers
				else if (it_type=="Enum")
					flagValue[it_flag]=toString<int>(GetSet<int>(it_section,it_key));
				else
					flagValue[it_flag]=GetSet<std::string>(it_section,it_key).getString();
			}
		// finally 
		for (MapStrStr::const_iterator it=flagValue.begin();it!=flagValue.end();++it)
			if (it->first==toString(stringTo<int>(it->first)))
				arguments+="\""+it->second+"\" "; // indexed argument
			else
				arguments+=it->first + " \"" + it->second + "\" ";
		std::cout << runShellCommand(pathToExe,arguments,pwd,pwd+"/"+nameOfExe+".log");
	}
}

int main(int argc, char** argv)
{
	GetSetDictionary &dict(GetSetDictionary::globalDictionary());

	// Path where this program can be found (good only when caller has permission to write to this directory)
	std::string workingDir=argv[0];
	splitNameFromPath(workingDir);
	GetSetGui::Directory(control,"Input","Working Directory")=workingDir;
	// Declare the parameters this application has (configured thru control dictionary)
	GetSetGui::File inputFiles(control,"Input","XML Files");
	inputFiles.setExtensions("XML Parameter Descriptions (*.xml);;Executable Files (*.exe);;All Files (*.*)");
	inputFiles.setMultiple(true);
	GetSet<bool>(control,"Input","Seperate Dictionaries")=true;
	// Get values from the ini-File and from the command line
	control.parseIni(fileReadString("GetSetCli.ini"));
	if (argc>1)
	{
		GetSetCmdLineParser cmd(false,control);
		cmd.parse(argc, argv, false);
		GetSet<std::string>(control,"Input","XML Files")=vectorToString(cmd.getUnnamedArgs(),";");
	}
	fileWriteString("GetSetCli.ini",control.getIni());
	// Parse the xml files or run binaries with the "--xml" flag and parsing their output
	std::vector<std::string> files=stringToVector<std::string>(GetSet<std::string>(control,"Input","XML Files"));
	for (int i=0;i<(int)files.size();i++)
	{
		std::string extension=splitRight(files[i],".");
		if (extension=="xml")
		{
			// xml files we just parse
			dict.parseXML(fileReadString(files[i]));
		}
		else
		{
			// Other files, we assume are executables
			std::string pathToExe=files[i];
			std::string name=splitNameFromPath(files[i]);
			// And try to run them
			std::cout << "Trying to run " << name << "...\n";
			std::string output=runShellCommand(pathToExe," --xml",workingDir,workingDir+"/"+name+".xml");
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
			// If we got xml output, we parse it.
			dict.parseXML(output);
			GetSet<std::string>(control,name,"Path To Executable")=pathToExe;
			GetSetGui::ReadOnlyText(control,name,"XML Dexcription")=output;
			GetSetGui::Trigger(control,name,"Run Command Line Tool")="Run...";
		}
	}

	GetSetHandler gui(control,control_callback);

	QApplication app(argc,argv);

	GetSetSettingsWindow setup;
	setup.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint);
	setup.show();

	GetSetSettingsWindow controlWindow(control);
	controlWindow.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint);
	controlWindow.show();

	return app.exec();
}
