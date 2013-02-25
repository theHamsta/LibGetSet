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

#include <QtGui/QApplication>
#include <QtGui/QPushButton>
#include <QtGui/QProgressBar>

#include "GetSet/GetSet.hxx"
#include "GetSet/GetSetXML.h"
#include "GetSetGui/GetSetSettingsWindow.h"

#include "Process.h"

#include <windows.h>

/// Class representing a command line tool that understands the "--xml" argument (FIXME clean up)
class ConfigureProcess : public Process {
public:
	ConfigureProcess(const std::string& executable_path="", const std::string& config_file_path="client.ini", const std::string& cmdlinearg="")
		: Process(executable_path)
		, config_file(config_file_path)
		, config_cmdline(cmdlinearg)
		, window(0x0)
	{}

	~ConfigureProcess() {
		closeWindow();
	}
	
	void closeWindow()
	{
		if (window)
		{
			window->close();
			delete window;
		}	
	}

	/// Open Configuration Window
	GetSetSettingsWindow* configure()
	{
		closeWindow();
		if (!setCommanLineArgs("--xml").run())
			return 0x0;
		waitForExit();
		std::istringstream xml(getConsoleOutput());
		GetSetIO::XmlFile xmlConfig(xml,std::cout);
		configuration.load(xmlConfig);
		if (configuration.empty())
			return 0x0;
		window=new GetSetSettingsWindow("",configuration, binaryFile);
		window->show();
		return window;
	}

	bool handleControlCommand(const std::string& command)
	{
		std::istringstream str(command);
		std::string type, action;
		getline(str,type,'-');
		getline(str,action,':');
		trim(type);
		trim(action);
		if (type=="Progress")
		{
			std::string name;
			if (action=="start")
			{
				std::string title;
				getline(str,title,'-');
				getline(str,name,'\0');
				trim(title);
				trim(name);
				if (progress_bars.find(name)!=progress_bars.end() && progress_bars[name]!=0x0)
					delete progress_bars[name];
				progress_bars[name]=new QProgressBar();
				progress_bars[name]->setWindowTitle(title.c_str());
				progress_bars[name]->setMinimum(0);
				progress_bars[name]->show();
			}
			if (action=="status")
			{
				int now,total;
				str >> now;
				str.ignore(100,'/');
				str >> total;
				getline(str,name,'\0');
				trim(name);
				if (progress_bars.find(name)!=progress_bars.end())
				{
					progress_bars[name]->setMaximum(total);
					progress_bars[name]->setValue(now);
				}
			}
			if (action=="done")
			{
				getline(str,name,'\0');
				trim(name);
				if (progress_bars.find(name)!=progress_bars.end())
				{
					if (progress_bars[name]!=0x0)
						delete progress_bars[name];
					progress_bars.erase(progress_bars.find(name));
				}
			}
			QApplication::processEvents(); // 2do extra thread
			return true;
		}
		return false;
	}

	/// This overload always blocks until termination of child. requires windows.h... FIXME not a very smart implementation either
	int run()
	{
		window->hide();
		GetSetIO::save<GetSetIO::IniFile>(config_file,configuration);
		if (!setCommanLineArgs(std::string("\"")+config_file+"\" "+config_cmdline).run())
			std::cout << "Failed to run process!\n";
		else if (stdoutReadHandle)
		{
			stdOutput.clear();
			DWORD bytes_read;
			char tBuf[257];
			std::string line;
			int nfound=0;
			while (ReadFile(stdoutReadHandle, tBuf, 256, &bytes_read, NULL) && bytes_read > 0)
			{
				tBuf[bytes_read]=0;
				for (int i=0;i<(int)bytes_read;i++)
				{
					line.push_back(tBuf[i]);
					if (nfound==-1)
					{
						if (tBuf[i]=='\n')
						{
							handleControlCommand(line);
							line.clear();
							nfound=0;
						}
					}
					else
					{
						if (tBuf[i]=='#') nfound++;
						if (nfound==3)
						{
							// "###" indicates a control command
							line.clear();
							nfound=-1;
						}
						if (tBuf[i]=='\n')
						{
							std::cout << line;
							line.clear();
						}
					}
				}

			}
			stdoutReadHandle=0x0;
		}
		int ret=waitForExit();
		progress_bars.clear();
		window->show();
		return ret;
	}

protected:
	bool good;
	std::string				config_file;
	std::string				config_cmdline;
	GetSetDictionary		configuration;
	GetSetSettingsWindow*	window;
	std::map<std::string,QProgressBar*> progress_bars;
};


GetSetSettingsWindow *autogui_window;
ConfigureProcess *childProcess=0x0;
std::string config_file="config.ini";

// Handle GUI-events
void gui(const std::string& section, const std::string& key)
{
	if (section=="AutoGUI" && key=="Ok")
	{
		if (childProcess)
			delete childProcess;
		childProcess=new ConfigureProcess(
			GetSet<>("Basic/Binary File"),
			GetSet<>("Basic/Config File"),
			GetSet<>("Advanced/Command Line Arguments")
			);
		childProcess->setWorkingDirectory(GetSet<>("Advanced/Working Directory"));
		GetSetSettingsWindow *w=childProcess->configure();
		if (w)
		{
			autogui_window->close();
			w->setWindowTitle("Client Program");
			w->setButton("Run...",gui)->setDefault(true);
		}
		else
			std::cerr << "Failed to launch process or process does not support the --xml flag.\n";
	}

	if (section=="Client Program" && key=="Run...")
		std::cout << "Exit code: " << childProcess->run() << std::endl;

	GetSetIO::save<GetSetIO::IniFile>(config_file);
}

int main(int argc, char **argv)
{
	// print help string
	if (argc==2 && (argv[1]=="--help" || argv[1]=="-h"))
	{
		std::cout << "Usage:\n    AutoGUI [config.ini]\n    AutoGUI client.exe\n    AutoGUI client.exe \"command line args\"\n";
		return 1;
	}

	// AutoGUI host parameters
	GetSetGui::File("Basic/Binary File").setExtensions("Executable File (*.exe)")="./bin/client.exe";
	GetSetGui::File("Basic/Config File").setExtensions("Ini-File (*.ini);;All Files (*)").setCreateNew(true)="./bin/client.ini";
	GetSetGui::Directory("Advanced/Working Directory");
	GetSet<>("Advanced/Command Line Arguments");
	// Handle command line arguments
	if (argc==2 || argc==3)
	{
		// Find out if the argument is an ini-File or an executable
		std::string extension,path=argv[1];
		extension=splitRight(path,".");
		if (extension==".ini")
		{
			config_file=argv[1];
			GetSetIO::load<GetSetIO::IniFile>(config_file);
		}
		else
		{
			GetSet<>("Basic/Binary File")=argv[1];			// eg. ./bin/bla.exe (or just ./bin/bla for linux)
			if (argc>2)
				GetSet<>("Basic/Config File")=argv[2];		// Config file was specified via command line
			if (argc>3) GetSet<>("Advanced/Command Line Arguments")=argv[2];

			else
				GetSet<>("Basic/Config File")=path+".ini";	// eg. ./bin/bla.ini
			splitRight(path,"/\\");
			GetSet<>("Advanced/Working Directory")=path;	// eg. ./bin
		}
	}
	else GetSetIO::load<GetSetIO::IniFile>(config_file); // no cmd line args -> use default config_file

	// Tell GetSet to callback gui(...) for any GUI-input
	GetSetHandler callback(gui);

	// Run Qt GUI
	QApplication app(argc,argv);
	autogui_window=new GetSetSettingsWindow();
	autogui_window->setWindowTitle("AutoGUI");
	autogui_window->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint);
	autogui_window->setButton("Ok",gui)->setDefault(true);
	autogui_window->show();	
	return app.exec();
}
