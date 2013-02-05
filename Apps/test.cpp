#include "GetSet/GetSetSpecial.hxx"
#include "GetSet/GetSetXML.h"

#include "GetSetGui/GetSetSettingsWindow.h"

#include <iostream>

#include <QtGui/QApplication>

#include "runProcess.h"

void gui(const std::string& section, const std::string& key)
{
	if (key=="Run")
	{
		Process exe(GetSet<>("Run/Binary File"));
		exe.setCommanLineArgs(GetSet<>("Run/Command Line Arguments"));
		exe.setWorkingDirectory(GetSet<>("Run/Working Directory"));
		if (!exe.run(false))
			std::cout << "Failed to run command!" << std::endl;
		else
			std::cout << "Running" << GetSet<>("Run/Binary File").getString() << std::endl;

		if (GetSet<bool>("Run/Wait For Exit"))
		{
			int return_value=exe.waitForExit();
			std::cout << "Exit Code: " << return_value << std::endl;
		}
	}
}

int main(int argc, char **argv)
{
	QApplication app(argc,argv);

	GetSetGui::File("Run/Binary File").setExtensions("Executable File (*.exe)")="C:/Windows/notepad.exe";
	GetSetGui::Button("Run/Run")="Create Process...";
	GetSetGui::Directory("Run/Working Directory")=".";
	GetSet<>("Run/Command Line Arguments")="";
	GetSet<bool>("Run/Wait For Exit")=0;

	GetSetIO::save(GetSetIO::XmlFile("out.xml"));
	
	GetSetHandler callback(gui);

	GetSetSettingsWindow window;
	window.setWindowTitle("RCL launcher");
	window.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint);
	window.show();
	
	return app.exec();
}
