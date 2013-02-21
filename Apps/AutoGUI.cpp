#include "GetSet/GetSet.hxx"
#include "GetSet/GetSetXML.h"

#include "GetSetGui/GetSetSettingsWindow.h"

#include <iostream>
#include <functional>

#include <QtGui/QApplication>

#include "Process.h"

class ConfigureProcess : private Process {
public:
	ConfigureProcess(const std::string& executable_path="", const std::string& config_file_path="config.ini", const std::string& working_dir="")
		: Process(executable_path)
		, config_file(config_file_path)
		, window(0x0)
	{
		setWorkingDirectory(working_dir);
	}

	~ConfigureProcess()
	{
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

	bool configure()
	{
		closeWindow();
		if (!setCommanLineArgs("--xml").run())
			return false;
		waitForExit();
		std::istringstream xml(getConsoleOutput());
		GetSetIO::XmlFile xmlConfig(xml,std::cout);
		configuration.load(xmlConfig);
		if (configuration.empty())
			return false;
		window=new GetSetSettingsWindow("",configuration, binaryFile);
		window->show();
		return true;
	}

	int run()
	{
		window->close();
		GetSetIO::save<GetSetIO::IniFile>(config_file,configuration);
		setCommanLineArgs(config_file).run();
		return waitForExit(true);
	}

protected:
	bool good;
	std::string				config_file;
	GetSetDictionary		configuration;
	GetSetSettingsWindow*	window;
};

ConfigureProcess *childProcess=0x0;

void gui(const std::string& section, const std::string& key)
{
	if (key=="Binary File")
	{
		if (childProcess)
			delete childProcess;
		childProcess=new ConfigureProcess(
			GetSet<>("Run/Binary File"),
			GetSet<>("Run/Config File"),
			GetSet<>("Run/Working Directory"));
	}

	if (key=="Configure" && childProcess)
		childProcess->configure();

	if (key=="Run" && childProcess)
		std::cout << "Exit code: " << childProcess->run() << std::endl;

	GetSetIO::save<GetSetIO::IniFile>("config.ini");
}

int main(int argc, char **argv)
{
	QApplication app(argc,argv);

	GetSetGui::File("Run/Binary File").setExtensions("Executable File (*.exe)")="c:/windows/system32/ping.exe";
	GetSetGui::File("Run/Config File").setExtensions("Ini-File (*.ini)").setCreateNew(true)="config.ini";
	GetSetGui::Directory("Run/Working Directory");
	GetSetGui::Button("Run/Configure")="Configure...";
	GetSetGui::Button("Run/Run")="Run...";

	GetSetIO::load<GetSetIO::IniFile>("config.ini");

	GetSetHandler callback(gui);
	gui("Run","Binary File");

	GetSetSettingsWindow window;
	window.setWindowTitle("AutoGUI");
	window.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint);
	window.show();
	
	return app.exec();

}
