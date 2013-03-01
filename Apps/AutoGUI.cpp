//
//  Library: GetSet
//  c++ library for load/saving *typed* and *named* properties and automatic GUI.
//  
//  Copyright (c) by Andre Aichert (aaichert@gmail.com)
//    
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//  
//    http://www.apache.org/licenses/LICENSE-2.0
//    
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//

#include <iostream>
#include <sstream>

#include <cstdlib>

#include <QtGui/QApplication>
#include <QtGui/QPushButton>
#include <QtGui/QProgressBar>

#include "GetSet/GetSet.hxx"
#include "GetSet/GetSetXML.h"
#include "GetSetGui/GetSetSettingsWindow.h"

#include "Process.h"
#include "ConfigureProcess.h"

GetSetSettingsWindow *autogui_window;
ConfigureProcess *childProcess=0x0;
std::string config_file="AutoGUI.ini";

// Handle GUI-events
void gui(const std::string& section, const std::string& key)
{
	GetSetIO::save<GetSetIO::IniFile>(config_file);

	if (section=="AutoGUI" && key=="Ok")
	{
		if (childProcess)
			delete childProcess;
		childProcess=new ConfigureProcess(
			GetSet<>("Basic/Binary File"),
			GetSet<>("Basic/Config File"),
			GetSet<>("Basic/Log File"),
			GetSet<>("Advanced/Command Line Args"),
			GetSet<>("Advanced/Command Line Args (config)")
			);
		childProcess->setWorkingDirectory(GetSet<>("Advanced/Working Directory"));
		GetSetSettingsWindow *w=childProcess->configure();
		if (w)
		{
			if (autogui_window)
				autogui_window->close();
			w->setWindowTitle("Client Program");
			w->setButton("Run...",gui)->setDefault(true);
		}
		else
			std::cerr << "Failed to launch process or process does not support the --xml flag.\n";
	}

	if (section=="Client Program" && key=="Run...")
	{
		int exit_code=childProcess->run();
		std::cout << "Exit code: " << exit_code << std::endl;
		// If client successfully executes, we quit.
		if (exit_code==0) exit(0);
	}
}

void killChild()
{
	if (childProcess)
		childProcess->kill();
}

int main(int argc, char **argv)
{
	atexit(killChild);
	bool showAutoGuiConfig=true;
	bool help=(argc==2 && (std::string(argv[1])=="--help" || std::string(argv[1])=="-h"));
	// Print help string
	if (argc==1 || help)
	{
		std::cout << "Usage:\n    AutoGUI [AutoGUI.ini]\n    AutoGUI --help\n    AutoGUI c.exe [-r] [-c c.ini] [-a/A cmdl_args] [-w work_dir] [-l c.log]\n";
		if (help)
		{
			std::cout	<< "\n\n"
						<< "Run a client executable (\"c.exe\") with the \"--xml\" command line flag and generate a GUI from its output. "
						<< "The client is expected to print a GetSet parameter description in XML format to stdout. "
						<< "This allows the user to change parameters via auto-generated GUI and finally to launch the client executable with an ini-File containing these settings.\n\n"
						<< "Client developers see also: https://sourceforge.net/projects/getset/ \n"
						<< "\n\n"
						<< " -r     Configure and run client program right away.\n"
						<< "        This will hide AutoGUI's own configuration window.\n"
						<< "\n"
						<< " -c     Specify an ini-file with client Configuration.\n"
						<< "        This will be the first command line argument supplied to client.\n"
						<< "\n"
						<< " -a     Specify additional command line arguments for running client.\n"
						<< "\n"
						<< " -A     Specify command line args for configuring client via \"--xml\"\n"
						<< "\n"
						<< " -w     Specify working directory for client.\n"
						<< "\n"
						<< " -l     Specify a log file. Client output will be piped to stdout and log file.\n"
						<< "\n"
						<< " -help  Print this text.\n"
						<< "\n"
						<< "Author: Andre Aichert - andre.aichert@cs.fau.de\n"
						<< "\n";
		}

		return 1;
	}

	// AutoGUI host parameters
	GetSetGui::File("Basic/Binary File").setExtensions("Executable File (*.exe)")="./bin/client.exe";
	GetSetGui::File("Basic/Config File").setExtensions("Ini-File (*.ini);;All Files (*)").setCreateNew(true)="./bin/client.ini";
	GetSetGui::File("Basic/Log File").setExtensions("Log-File (*.log);;All Files (*)").setCreateNew(true)="./bin/client.ini";
	GetSetGui::Directory("Advanced/Working Directory");
	GetSet<>("Advanced/Command Line Args");
	GetSet<>("Advanced/Command Line Args (config)");

	// Find out if the first argument is an ini-File or an executable
	std::string extension,path=argv[1];
	extension=splitRight(path,".");

	// Handle command line arguments
	if (argc==1 || (argc==2 && extension==".ini"))
	{
		if (argc==2)
			config_file=argv[1];
		GetSetIO::load<GetSetIO::IniFile>(config_file);
	}
	else
	{
		GetSet<>("Basic/Binary File")=argv[1];		// eg. ./bin/bla.exe (or just ./bin/bla for linux)
		GetSet<>("Basic/Config File")=path+".ini";	// eg. ./bin/bla.ini
		GetSet<>("Basic/Log File")=path+".log"; 	// eg. ./bin/bla.log
		splitRight(path,"/\\");
		GetSet<>("Advanced/Working Directory")=path;// eg. ./bin
		// Parse additional command line arguments.
		int i=2;
		for (;i<argc;i++)
		{
			std::string flag=argv[i];
			if (flag=="-r")
			{
				showAutoGuiConfig=false;
				continue;
			}
			if (i==argc-1)
			{
				std::cerr << "Missing value for command line flag. Try:\n   AutoGUI --help\n";
				return 1;
			}
			if (flag=="-c")
			{
				GetSet<>("Basic/Config File")=argv[++i];
				continue;
			}
			if (flag=="-a")
			{
				GetSet<>("Advanced/Command Line Args")=argv[++i];
				continue;
			}
			if (flag=="-A")
			{
				GetSet<>("Advanced/Command Line Args (config)")=argv[++i];
				continue;
			}
			if (flag=="-w")
			{
				GetSet<>("Advanced/Working Directory")=argv[++i];
				continue;
			}
			if (flag=="-l")
			{
				GetSet<>("Basic/Log File")=argv[++i];
				continue;
			}
			std::cerr << "Unrecognized command line arguments. Try:\n   AutoGUI --help\n";
			return 1;
		}
	}

	// Tell GetSet to callback gui(...) for any GUI-input
	GetSetHandler callback(gui);

	// Run Qt GUI
	QApplication app(argc,argv);

	if (showAutoGuiConfig)
	{
		autogui_window=new GetSetSettingsWindow();
		autogui_window->setWindowTitle("AutoGUI");
	//	autogui_window->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint);
		autogui_window->setButton("Ok",gui)->setDefault(true);
		autogui_window->show();
	}
	else gui("AutoGUI", "Ok"); // Pretend user already clicked the "Ok" button ("-r"-flag)

	return app.exec();
}
