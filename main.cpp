#include <QtGui/qapplication>

#include "GetSet/GetSet.hxx"
#include "GetSetGui/GetSetSettingsWindow.h"
#include "GetSet/GetSetCmdLineParser.h"

#include <iostream>

void gui(const std::string& section, const std::string& key)
{
	if (key=="XML")
		std::cout << GetSetDictionary::globalDictionary().getXML();

	std::cout << section << " : " << key << "=" << GetSet<std::string>(section,key).getString() << std::endl;
}

int main(int argc, char** argv)
{
	// Basic GetSet usage:
	GetSet<int>("Test 1", "Number of Iterations")=13;
	int nIterations=GetSet<int>("Test 1", "Number of Iterations"); // == 13
	

	// Creating a button
	GetSetGui::Trigger("Test 1", "XML")="Print to cout";


	GetSet<std::string>("Test 2", "Some Directory")="./out"; // Value is preserved when type changes to Directory

	// Files and directories
	GetSetGui::Directory("Test 2", "Some Directory");


	//
	// Listing to Events
	//
	GetSetHandler callback(gui);

	//
	// Command Line Interface
	//
	
	GetSetCmdLineParser().parse(argc,argv);

	//GetSetCmdLineParser cmd(false);

	//cmd.flagAuto("Test 1","Number of Iterations");
	//cmd.flag("-o","Test 2","Some Directory");
	//cmd.require("Test 2","Some Directory");
	//cmd.flagIndexed(0,"Test 2", "Input File");

	//cmd.parse(argc,argv);

	//
	// Automatically generate a GUI (in this case using Qt)
	//
	
	QApplication app(argc,argv);

	GetSetSettingsWindow setup;
	setup.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint);
	setup.show();

	return app.exec();
}
