#include "GetSet/GetSetSpecial.hxx"
#include "GetSet/GetSetXML.h"

#include "GetSetGui/GetSetSettingsWindow.h"

#include <iostream>

#include <QtGui/QApplication>

#include "Process.h"


void gui(const std::string& section, const std::string& key)
{
	GetSetIO::save<GetSetIO::IniFile>("config.ini");
}

int main(int argc, char **argv)
{
	QApplication app(argc,argv);

	GetSetGui::File("Run/Binary File").setExtensions("Executable File (*.exe)")="c:/windows/system32/ping.exe";
	GetSetGui::Button("Run/Run")="Create Process...";
	GetSetGui::Directory("Run/Working Directory");

	// if (argc==2 && argv[1]=="--xml")
	{
		GetSetIO::XmlFile xml(std::cin,std::cout);
		GetSetDictionary::global().save(xml);
		return 0;
	}

	GetSetIO::load<GetSetIO::IniFile>("config.ini");
	
	GetSetHandler callback(gui);

	GetSetSettingsWindow window;
	window.setWindowTitle("RCL launcher");
	window.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint);
	window.show();
	
	return app.exec();

}
