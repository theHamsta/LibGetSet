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
#include "GetSet/GetSetCmdLine.hxx"
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
	bool directRun=false;
	bool help=(argc==2 && (std::string(argv[1])=="--help" || std::string(argv[1])=="-h"));
	// Print help string
	if (argc==1 || help)
	{
		std::cout << "Usage:\n    AutoGUI [AutoGUI.ini]\n    AutoGUI --help\n    AutoGUI c.exe [-R] [-r] [-c c.ini] [-a/A cmdl_args] [-w work_dir] [-l c.log]\n";
		if (help)
		{
			std::cout	<< "\n\n"
						<< "Run a client executable (\"c.exe\") with the \"--xml\" command line flag and generate a GUI from its output. "
						<< "The client is expected to print a GetSet parameter description in XML format to stdout. "
						<< "This allows the user to change parameters via auto-generated GUI and finally to launch the client executable with an ini-File containing these settings.\n\n"
						<< "Client developers see also: https://sourceforge.net/projects/getset/ \n"
						<< "\n"
						<< " -r     Run client program right away. No configuration-GUI.\n"
						<< "        Status window and progress bar are shown on client's request.\n"
						<< "\n"
						<< " -R     Configure and run client program right away.\n"
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
						<< "Author: Andre Aichert - andre.aichert@cs.fau.de\n"
						<< "\n";
		}
		return 1;
	}

	// AutoGUI host parameters
	GetSetGui::File("Basic/Binary File")
		.setExtensions("Executable File (*.exe)")
		.setAttribute("CommandLineFlag","1;--executable-file");
	GetSetGui::File("Basic/Config File")
		.setExtensions("Ini-File (*.ini);;All Files (*)")
		.setCreateNew(true)
		.setAttribute("CommandLineFlag","-c;--config-file");
	GetSetGui::File("Basic/Log File")
		.setExtensions("Log-File (*.log);;All Files (*)")
		.setCreateNew(true)
		.setAttribute("CommandLineFlag","-l;--log-file");
	GetSetGui::Directory("Advanced/Working Directory").setAttribute("CommandLineFlag","-w");
	GetSet<>("Advanced/Command Line Args").setAttribute("CommandLineFlag","-a;--args");
	GetSet<>("Advanced/Command Line Args (config)").setAttribute("CommandLineFlag","-A;--config-args");

	// Find out if the first argument is an ini-File or an executable
	std::string extension,path=argv[1];
	extension=splitRight(path,".");
	if (extension.length()>5) // for example /bin/linux_noextension
		std::swap(path,extension);

	// Handle command line arguments
	if (argc==1 || (argc==2 && extension==".ini"))
	{
		if (argc==2)
			config_file=argv[1];
		GetSetIO::load<GetSetIO::IniFile>(config_file);
	}
	else
	{
		
		GetSetIO::CmdLineParser cmd;
		cmd.index("Basic/Binary File",1);
		cmd.declare(); // add all GetSet parameters and resp. flags
		if (!cmd.parse(argc,argv))
		{
			std::cerr << "Failed to parse command line arguments. Try:\n   AutoGUI --help\n";
			return 1;
		}
		if (cmd.getUnhandledArgs().size()>1)
		{
			// Handle -r flag
			if (cmd.getUnhandledArgs().size()==2 && (++(cmd.getUnhandledArgs().begin()))->second=="-R")
				showAutoGuiConfig=false;
			else if (cmd.getUnhandledArgs().size()==2 && (++(cmd.getUnhandledArgs().begin()))->second=="-r")
				directRun=true;
			else
			{
				std::cerr << "Unrecognized command line arguments. Try:\n   AutoGUI --help\n";
				return 1;
			}
		}
		if (GetSet<>("Basic/Config File").getString().empty())
		GetSet<>("Basic/Config File")=path+".ini";	// eg. ./bin/bla.ini
		if (GetSet<>("Basic/Log File").getString().empty())
			GetSet<>("Basic/Log File")=path+".log"; 	// eg. ./bin/bla.log
		splitRight(path,"/\\");
		if (GetSet<>("Advanced/Working Directory").getString().empty())
		GetSet<>("Advanced/Working Directory")=path;// eg. ./bin
	}

	// Tell GetSet to callback gui(...) for any GUI-input
	GetSetHandler callback(gui);

	// Run Qt GUI
	QApplication app(argc,argv);

	if (directRun)
	{
		// execute child process right away
		childProcess=new ConfigureProcess(
			GetSet<>("Basic/Binary File"),
			GetSet<>("Basic/Config File"),
			GetSet<>("Basic/Log File"),
			GetSet<>("Advanced/Command Line Args"),
			GetSet<>("Advanced/Command Line Args (config)")
			);
		childProcess->setWorkingDirectory(GetSet<>("Advanced/Working Directory"));
		int exit_code=childProcess->run();
		std::cout << "Exit code: " << exit_code << std::endl;
		exit(exit_code);
	}

	if (showAutoGuiConfig)
	{
		autogui_window=new GetSetSettingsWindow();
		autogui_window->setWindowTitle("AutoGUI");
		autogui_window->setButton("Ok",gui)->setDefault(true);
		autogui_window->show();
	}
	else gui("AutoGUI", "Ok"); // Pretend user already clicked the "Ok" button ("-r"-flag)

	return app.exec();
}
