#include "GetSet/GetSet.hxx"
#include "GetSet/GetSetXML.h"

#include "GetSetGui/GetSetSettingsWindow.h"

#include <iostream>

#include <QtGui/QApplication>

#include "Process.h"

#include <QtGui/QScrollArea>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>


void gui(const std::string& section, const std::string& key)
{
	GetSetIO::save<GetSetIO::IniFile>("config.ini");
	if (section=="RCL launcher" && key=="Ok") // Ok button of window has been pressed
	{
	
	}
}

int main(int argc, char **argv)
{
	QApplication app(argc,argv);


	GetSetGui::File("Run/Binary File").setExtensions("Executable File (*.exe)").setDescription("This is a tooltip");

	GetSet<>("Run/abca");
	GetSet<>("Run/abcd");
//	GetSetIO::load<GetSetIO::IniFile>("config.ini");
	
	GetSetHandler callback(gui);

	GetSetSettingsWindow window;
	window.setWindowTitle("RCL launcher");
	window.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint);
	window.setButton("Ok",gui);
	window.show();
	
	return app.exec();

}
