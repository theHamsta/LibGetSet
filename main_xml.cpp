#include <QtGui/qapplication>

#include "GetSet/GetSet.hxx"
#include "GetSetGui/GetSetSettingsWindow.h"

#include <iostream>

std::string g_iniFileName;
std::string g_xmlFileName;

// We have a separate GetSetDictionaries for controlling the program itself
GetSetDictionary control;

// And here we define what's in it.
void defineControlProperties()
{
	GetSetGui::Directory(control,"Input","Directory");
	GetSetGui::File(control,"Input","file.xml").setExtensions("xml-Files (*.xml);;All Files (*.*)");
	GetSetGui::File(control,"Input","file.ini").setExtensions("ini-Files (*.ini);;All Files (*.*)");
}

// Handles changes in the control window
void control_callback(const std::string& section, const std::string& key)
{

}

int main(int argc, char **argv)
{
	if (argc!=2)
	{
		std::cerr << "Usage:\n\t./GetSetConfig file.xml\n-or-\n./GetSetConfig file.ini\n";
		return 1;
	}

	// Input file
	std::string path=argv[1];

	// Decompose path
	std::string directory=path;
	std::string basename;
	std::string extension;
	basename=splitNameFromPath(directory);
	extension=splitRight(basename,".");

	// Define GetSetConfig's properties in the control window
	defineControlProperties();

	GetSet<std::string>(control,"Input","Directory")=directory;
	GetSet<std::string>(control,"Input","file.ini")=basename+".ini";
	GetSet<std::string>(control,"Input","file.xml")=basename+".xml";
	
	g_iniFileName=directory+"/"+basename+".ini";
	g_xmlFileName=directory+"/"+basename+".xml";

	GetSetDictionary::globalDictionary().parseIni(fileReadString(g_iniFileName));
	GetSetDictionary::globalDictionary().parseXML(fileReadString(g_xmlFileName));

	// Handle GUI input in control window
	GetSetHandler gui(control,control_callback);

	QApplication app(argc,argv);

	GetSetSettingsWindow setup;
	setup.setWindowTitle(GetSet<std::string>(control,"Input","file.ini").getString().c_str());
	setup.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint);
	setup.show();

	GetSetSettingsWindow controlWindow(control);
	controlWindow.setWindowTitle("GetSetConfig");
	controlWindow.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint);
	controlWindow.show();
	
	return app.exec();
}