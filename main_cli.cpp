#include <QtGui/qapplication>

#include "GetSet/GetSet.hxx"
#include "GetSetGui/GetSetSettingsWindow.h"
#include "GetSet/GetSetCmdLineParser.h"

#include "GetSet/tinyxml2.h"

#include <iostream>

/// Run a command line aplication. If outputRedirectFile is provided, the output is returned (Definitiosn are platform dependent)
std::string runShellCommand(const std::string& shellCommand, const std::string& arguments,
							const std::string& workingDirectory, const std::string& outputRedirectFile);

#ifdef _WIN32
    #define NOMINMAX
	#include <windows.h>

	std::string runShellCommand(const std::string& shellCommand, const std::string& arguments,
								const std::string& workingDirectory, const std::string& outputRedirectFile="")
	{
		std::string call;
		fileWriteString(outputRedirectFile,"");
		if (outputRedirectFile.empty())
			call="cd "+workingDirectory+" & cmd /c "+shellCommand+" "+arguments;
		else
			call="cd "+workingDirectory+" & "+shellCommand+" "+arguments+" > "+outputRedirectFile;
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
GetSetDictionary &dict(GetSetDictionary::globalDictionary());

std::string g_workingDir; // actually the directory where this executable is located

void control_callback(const std::string& name, const std::string& key)
{
	if (key=="Input Files")
	{
		//// Clear everything
		//control.clear(section);
		//dict=GetSetDictionary();
		//// and start a new
	}

	if (key=="Run Command Line Tool")
	{
		// Write current values to ini file
		std::string pwd=GetSet<std::string>(control,name,"Working Directory");
		std::string iniFile=GetSet<std::string>(control,name,"Ini File");
		if (!iniFile.empty())
			fileWriteString(pwd+"/"+iniFile,GetSetDictionary::globalDictionary().getIni());
		// Parse the xml and compose the command line arguments from global dictionary
		GetSetCmdLineParser cmd(false);
		cmd.parseXML(GetSet<std::string>(control,name,"XML Dexcription"));
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
		std::cout << runShellCommand(GetSet<std::string>(control,name,"Path To Executable"),arguments,pwd,
									pwd+"/"+GetSet<std::string>(control,name,"Log File").getString());
	}
}

void initCLI()
{
	// Parse the ini files or run binaries with the "--xml" flag and parsing their output
	std::vector<std::string> files=stringToVector<std::string>(GetSet<std::string>(control,"Input","Input Files"));
	for (int i=0;i<(int)files.size();i++)
	{
		std::string directory=files[i];
		std::string pathToExe;
		std::string name=splitNameFromPath(directory);
		std::string extension=splitRight(name,".");
		std::string xml;
		std::string pwd=g_workingDir;
		GetSet<std::string>(control,name,"Log File")=name+".log";
		if (extension=="cli" || extension=="xml")
		{
			// files[i] is not an executable but an ini-file which can provide keys:
			// "Path To Executable" "XML Dexcription" "Log File" "Ini File"
			// If it does not provide these keys, we assume:
			pathToExe=directory+"/"+name;
			#ifdef _WIN32
				pathToExe+=".exe";
			#endif
			xml=directory+"/"+name+".xml";
			// Finally load data from cli file
			std::string ini="[" + name + "]\n\n" + fileReadString(files[i]);
			control.parseIni(ini);
			// And load xml description of the executable
			xml=fileReadString(xml);
			std::string iniFile=GetSet<std::string>(control,name,"Ini File");
			if (!iniFile.empty())
				GetSet<std::string>(control,name,"Ini File")=directory+"/"+iniFile;
		}
		else
		{
			// If the extension is not ".cli" or ".xml" we assume we are dealing with an executable
			pathToExe=files[i];
			// And try to run it
			std::cout << "Trying to run " << name << "...\n";
			xml=runShellCommand(pathToExe," --xml",g_workingDir,g_workingDir+"/"+name+".xml");
		}
		if (xml.empty() || xml[0]!='<')
		{
			std::cerr << "No valid XML provided!\n";
			std::cout <<"###" << xml << "###";
			dict.erase(name,"Log File");
			continue;
		}
		// If we got xml output, we parse it.
		dict.parseXML(xml);
		// If we have an ini-File, we parse it
		dict.parseIni(GetSet<std::string>(control,name,"Ini File"));
		// Finally complete the <name>-section in the control dictionary
		GetSet<std::string>(control,name,"Path To Executable")=pathToExe;
		GetSetGui::ReadOnlyText(control,name,"XML Dexcription")=xml;
		GetSetGui::Directory(control,name,"Working Directory")=pwd;
		GetSetGui::File(control,name,"Log File").setExtensions("Log File (.log)");
		GetSetGui::File(control,name,"Ini File").setExtensions("Log File (.ini)");
		GetSetGui::File(control,name,"Log File").setCreateNew(true);
		GetSetGui::File(control,name,"Ini File").setCreateNew(true);
		GetSetGui::Trigger(control,name,"Run Command Line Tool")="Run...";
	}
}

int main(int argc, char** argv)
{
	// Path where this program can be found (good only when caller has permission to write to this directory)
	g_workingDir=argv[0];
	splitNameFromPath(g_workingDir);

	// Declare the input parameter this application has (configured thru control dictionary)
	GetSetGui::File inputFiles(control,"Input","Input Files");
	inputFiles.setExtensions("XML Command Line Interface Description (*.xml *.cli);;Executable Files (*.exe);;All Files (*.*)");
	inputFiles.setMultiple(true);

	// Get values from the ini-File and from the command line
	if (argc>1)
	{
		GetSetCmdLineParser cmd(false,control);
		cmd.parse(argc, argv, false);
		GetSet<std::string>(control,"Input","Input Files")=vectorToString(cmd.getUnnamedArgs(),";");
	}
	else
		control.parseIni(fileReadString("GetSetCli.ini"));
	fileWriteString("GetSetCli.ini",control.getIni());

	initCLI();

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
